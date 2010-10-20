/* File: widget_gtk.c
   Time-stamp: <2010-10-20 18:13:41 gawen>

   Copyright (C) 2010 David Hauweele <david.hauweele@gmail.com>
   Copyright (C) 2008,2009 Craig Harding <craigwharding@gmail.com>
                           Wolter Hellmund <wolterh@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include "common.h"

#include "preferences.h"
#include "widget_gtk.h"
#include "widget.h"
#include "purple.h"

static void cb_dummy() {}

static void cb_set_alias_failure(PurpleAccount *account, const char *error)
{
  const gchar *id = purple_account_get_protocol_id(account);

  purple_debug_info(NAME, "aliases not supported by \"%s\"\n", id);
}


static void cb_icon_choose(const gchar *path, gpointer data)
{
  g_return_if_fail(path);

  purple_prefs_set_path(PIDGIN_PREFS_ROOT "/accounts/buddyicon", path);
}

void cb_buddy_icon(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  PidginBuddyList *blist;
  GtkWidget *chooser;

  blist = pidgin_blist_get_default_gtk_blist();
  chooser = pidgin_buddy_icon_chooser_new(GTK_WINDOW
                                          (gtk_widget_get_toplevel
                                           (GTK_WIDGET(blist))),
                                          cb_icon_choose,
                                          NULL);
  gtk_widget_show(chooser);
}

void cb_buddy_icon_enter(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkPixbuf *icon;

  icon = get_buddy_icon_hover();
  set_widget_icon(icon);
  pidgin_set_cursor(bar->event_box, GDK_HAND2);
}

void cb_buddy_icon_leave(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkPixbuf *icon;

  icon = get_buddy_icon();
  set_widget_icon(icon);
  pidgin_set_cursor(bar->event_box, GDK_ARROW);
}

void cb_name_button(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *name = purple_prefs_get_string(PREF "/nickname");

  gtk_entry_set_text(GTK_ENTRY(bar->name_entry),
                     strcmp(name, EMPTY_NAME) ? name : "");

  gtk_widget_hide(bar->name_button);
  gtk_widget_show(bar->name_entry);

  gtk_widget_grab_focus(bar->name_entry);
}

void cb_name_button_enter(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *markup, *name;

  markup = purple_prefs_get_string(PREF "/nickname-markup-hover");
  name   = purple_prefs_get_string(PREF "/nickname");

  bar->hover_name = TRUE;
  set_widget_name(markup, name);
}

void cb_name_button_leave(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *markup, *name;

  markup = purple_prefs_get_string(PREF "/nickname-markup");
  name   = purple_prefs_get_string(PREF "/nickname");

  bar->hover_name = FALSE;
  set_widget_name(markup, name);
}

void cb_name_entry(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *name, *markup, *id;
  PurpleAccount *account;
  GList *accts;

  name = gtk_entry_get_text(GTK_ENTRY(widget));
  purple_prefs_set_string(PREF "/nickname", name);

  for(accts = purple_accounts_get_all_active(); accts ; accts = accts->next) {
    account = accts->data;
    if(!purple_account_is_connected(account))
      continue;
    id = purple_account_get_protocol_id(account);
    /* exception for set_public_alias */
    if(!strcmp(id, "prpl-jabber")) {
      PurpleConnection *gc;
      PurplePluginProtocolInfo *prpl_info;
      const gchar *raw;
      gchar *iq_id;
      xmlnode *iq, *pubsub, *publish, *nicknode;

      gc = account->gc;
      prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
      iq_id = g_strdup_printf("purple%x", g_random_int());
      iq = xmlnode_new("iq");
      xmlnode_set_attrib(iq, "type", "set");
      xmlnode_set_attrib(iq, "id", iq_id);

      pubsub = xmlnode_new("pubsub");
      xmlnode_set_attrib(pubsub, "xmlns", "http://jabber.org/protocol/pubsub");
      publish = xmlnode_new("publish");
      xmlnode_set_attrib(publish,"node","http://jabber.org/protocol/nick");
      nicknode = xmlnode_new_child(xmlnode_new_child(publish, "item"), "nick");
      xmlnode_set_namespace(nicknode, "http://jabber.org/protocol/nick");
      xmlnode_insert_data(nicknode, name, -1);
      xmlnode_insert_child(pubsub, publish);
      xmlnode_insert_child(iq, pubsub);

      raw = xmlnode_to_formatted_str(iq, NULL);
      if(prpl_info && prpl_info->send_raw)
        prpl_info->send_raw(gc, raw, strlen(raw));
      g_free(iq_id);
    }
    else
      /* provide dummy callback since some
         protocols don't check before calling */
      purple_account_set_public_alias(account, name, cb_dummy,
                                      cb_set_alias_failure);
  }

  markup = purple_prefs_get_string(PREF "/nickname-markup");
  set_widget_name(markup, name);

  gtk_widget_hide(bar->name_entry);
  gtk_widget_show(bar->name_button);

  purple_debug_info(NAME, "nickname set to \"%s\"\n", name);
}

void cb_pm_button(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *pm = purple_prefs_get_string(PREF "/personal-message");

  gtk_entry_set_text(GTK_ENTRY(bar->pm_entry), strcmp(pm,EMPTY_PM) ? pm : "");

  gtk_widget_hide(bar->pm_button);
  gtk_widget_show(bar->pm_entry);

  gtk_widget_grab_focus(bar->pm_entry);
}

void cb_pm_button_enter(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *markup, *pm;

  markup = purple_prefs_get_string(PREF "/personal-message-markup-hover");
  pm     = purple_prefs_get_string(PREF "/personal-message");

  bar->hover_pm = TRUE;
  set_widget_pm(markup, pm);
}

void cb_pm_button_leave(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *markup, *pm;

  markup = purple_prefs_get_string(PREF "/personal-message-markup");
  pm     = purple_prefs_get_string(PREF "/personal-message");

  bar->hover_pm = FALSE;
  set_widget_pm(markup, pm);
}

void cb_pm_entry(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *pm, *markup;
  PurpleSavedStatus *status;

  pm = gtk_entry_get_text(GTK_ENTRY(widget));
  purple_prefs_set_string(PREF "/personal-message", pm);

  /* set personal message for all protocols */
  status = purple_savedstatus_get_current();
  purple_savedstatus_set_message(status, pm);
  purple_savedstatus_activate(status);

  markup = purple_prefs_get_string(PREF "/personal-message-markup");
  set_widget_pm(markup, pm);

  gtk_widget_hide(bar->pm_entry);
  gtk_widget_show(bar->pm_button);

  purple_debug_info(NAME, "personal message set to \"%s\"\n", pm);
}

void cb_status_button(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkEventButton *event = (GdkEventButton *)gtk_get_current_event();

  gtk_menu_popup(GTK_MENU(bar->status_menu), NULL, NULL, NULL, NULL,
                 event->button, event->time);
}

void cb_status_menu(gpointer data)
{
  g_return_if_fail(bar->installed);

  PurpleSavedStatus *status;
  PurpleStatusType *status_type = (PurpleStatusType *)data;
  PurpleStatusPrimitive type_prim = purple_status_type_get_primitive(status_type);
  const gchar *pm;

  pm = purple_prefs_get_string(PREF "/personal-message");
  status = purple_savedstatus_get_current();
  purple_savedstatus_set_type(status, type_prim);
  purple_savedstatus_set_message(status, pm);
  purple_savedstatus_activate(status);

  purple_debug_info(NAME, "status set to \"%s\"\n",
                    purple_status_type_get_name(status_type));
}

void cb_buddy_icon_update(const char *name, PurplePrefType type,
                          gconstpointer val, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkPixbuf *icon;

  icon = get_buddy_icon();
  set_widget_icon(icon);
}
