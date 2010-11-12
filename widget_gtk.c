/* File: widget_gtk.c
   Time-stamp: <2010-11-13 00:28:24 gawen>

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
#include "widget_prpl.h"
#include "widget_gtk.h"
#include "widget.h"
#include "purple.h"

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
  pidgin_set_cursor(bar->icon_eventbox, GDK_HAND2);
}

void cb_buddy_icon_leave(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkPixbuf *icon;

  icon = get_buddy_icon();
  set_widget_icon(icon);
  pidgin_set_cursor(bar->icon_eventbox, GDK_ARROW);
}

void cb_name(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkEventButton *event;
  gboolean left_click;
  const gchar *name = purple_prefs_get_string(PREF "/nickname");

  if(!name || !strcmp(name, EMPTY_NAME))
    name = "";

  event = (GdkEventButton *)gtk_get_current_event();

  left_click = (event->button == 1);
  left_click = purple_prefs_get_bool(PREF "/left-entry") ? left_click : !left_click;

  if(left_click && !bar->name_dialog) {
    gtk_entry_set_text(GTK_ENTRY(bar->name_entry), name);

    if(purple_prefs_get_bool(PREF "/compact"))
      gtk_widget_hide(bar->pm_eventbox);
    gtk_widget_hide(bar->name_eventbox);
    gtk_widget_show(bar->name_entry);

    gtk_widget_grab_focus(bar->name_entry);
  }
  else if(!bar->name_dialog) {
    purple_request_input(thisplugin,
                         _("Change nickname"),
                         _("You may change your nickname here"),
                         _("Press apply when you're ok or cancel ortherwize"),
                         name,
                         FALSE,
                         FALSE,
                         NULL,
                         _("OK"),
                         G_CALLBACK(cb_name_apply),
                         _("Cancel"),
                         G_CALLBACK(cb_name_cancel),
                         NULL,
                         NULL,
                         NULL,
                         NULL);
    bar->name_dialog = TRUE;
  }
}

void cb_name_enter(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *markup, *name;

  markup = purple_prefs_get_string(PREF "/nickname-markup-hover");
  name   = purple_prefs_get_string(PREF "/nickname");

  bar->hover_name = TRUE;
  set_widget_name(markup, name);
}

void cb_name_leave(GtkWidget *widget, gpointer data)
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

  const gchar *name, *markup;

  name = gtk_entry_get_text(GTK_ENTRY(widget));
  markup = purple_prefs_get_string(PREF "/nickname-markup");
  set_widget_name(markup, name);

  purple_prefs_set_string(PREF "/nickname", name);
  set_display_name_all(name);

  bar->name_entry_activated = TRUE;

  if(purple_prefs_get_bool(PREF "/compact"))
    gtk_widget_show(bar->pm_eventbox);
  gtk_widget_hide(bar->name_entry);
  gtk_widget_show(bar->name_eventbox);

  purple_debug_info(NAME, "nickname changed to \"%s\" by user\n", name);
}

void cb_name_entry_focus_out(GtkWidget *widget, gpointer data)
{
  if(!bar->name_entry_activated)
    cb_name_entry(widget, data);
  bar->name_entry_activated = FALSE;
}

void cb_pm(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkEventButton *event;
  gboolean left_click;
  const gchar *pm = purple_prefs_get_string(PREF "/personal-message");

  event = (GdkEventButton *)gtk_get_current_event();

  left_click = (event->button == 1);
  left_click = purple_prefs_get_bool(PREF "/left-entry") ? left_click : !left_click;


  if(left_click && !bar->pm_dialog) {

    if(!pm || !strcmp(pm, EMPTY_PM))
      pm = "";

    gtk_entry_set_text(GTK_ENTRY(bar->pm_entry), pm);

    if(purple_prefs_get_bool(PREF "/compact"))
      gtk_widget_hide(bar->name_eventbox);
    gtk_widget_hide(bar->pm_eventbox);
    gtk_widget_show(bar->pm_entry);

    gtk_widget_grab_focus(bar->pm_entry);
  }
  else if(!bar->pm_dialog) {
    PurpleRequestFields *fields;
    PurpleRequestFieldGroup *group;
    PurpleRequestField *field;

    const struct s_field {
      const gchar *text;
      const gchar *pref;
    } s_fields[] = {
      { N_("Mood message"), PREF "/mood-message" },
      { N_("Song title"), PREF "/tune-title" },
      { N_("Song artist"), PREF "/tune-artist" },
      { N_("Song album"), PREF "/tune-album" },
      { N_("Game name"), PREF "/game-message" },
      { N_("Office app name"), PREF "/office-message" },
      { NULL, NULL }
    }; register const struct s_field *sf = s_fields;

    fields = purple_request_fields_new();
    group = purple_request_field_group_new(NULL);
    purple_request_fields_add_group(fields, group);

    field = purple_request_field_string_new(PREF "/personal-message",
                                            _("Personal message"),
                                            pm,
                                            FALSE);
    purple_request_field_set_required(field, FALSE);
    purple_request_field_group_add_field(group, field);
    for(; sf->text ; sf++) {
      const gchar *message = purple_prefs_get_string(sf->pref);

      field = purple_request_field_string_new(sf->pref,
                                              sf->text,
                                              message,
                                              FALSE);
      purple_request_field_set_required(field, FALSE);
      purple_request_field_group_add_field(group, field);
    }

    purple_request_fields(thisplugin,
                          _("Change status message"),
                          _("Test string here"),
                          _("Please enter your personal message here"),
                          fields,
                          _("OK"),
                          G_CALLBACK(cb_pm_apply),
                          _("Cancel"),
                          G_CALLBACK(cb_pm_cancel),
                          NULL, NULL, NULL, NULL);
    bar->pm_dialog = TRUE;
  }
}

void cb_pm_enter(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *markup, *pm;

  markup = purple_prefs_get_string(PREF "/personal-message-markup-hover");
  pm     = purple_prefs_get_string(PREF "/personal-message");

  bar->hover_pm = TRUE;
  set_widget_pm(markup, pm);
}

void cb_pm_leave(GtkWidget *widget, gpointer data)
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

  pm = gtk_entry_get_text(GTK_ENTRY(widget));
  purple_prefs_set_string(PREF "/personal-message", pm);

  /* set personal message for all protocols */
  set_status_message(pm);

  markup = purple_prefs_get_string(PREF "/personal-message-markup");
  set_widget_pm(markup, pm);
  bar->pm_entry_activated = TRUE;

  if(purple_prefs_get_bool(PREF "/compact"))
    gtk_widget_show(bar->name_eventbox);
  gtk_widget_hide(bar->pm_entry);
  gtk_widget_show(bar->pm_eventbox);

  purple_debug_info(NAME, "personal message changed to \"%s\" by user\n", pm);
}

void cb_pm_entry_focus_out(GtkWidget *widget, gpointer data)
{
  if(!bar->pm_entry_activated)
    cb_pm_entry(widget, data);
  bar->pm_entry_activated = FALSE;
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

  purple_debug_info(NAME, "status changed to \"%s\" by user\n",
                    purple_status_type_get_name(status_type));
}

void cb_mood_button(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);
  const gchar *empty_mood;
  GtkWidget *menu_item, *icon;
  PurpleMood *mood = get_global_moods();
  GdkEventButton *event;
  GList *l, *i;
  gchar *path;

  /* destroy dop down mood menu and create a new one */
  l = gtk_container_get_children(GTK_CONTAINER(bar->mood_menu));
  for(i = l ; i ; i = i->next) {
    gtk_widget_destroy(l->data);
    l->data = NULL;
  }
  gtk_widget_destroy(bar->mood_menu);
  bar->mood_menu = gtk_menu_new();

  /* add empty mood to mood menu */
  empty_mood = "";
  path       = get_mood_icon_path(empty_mood);
  icon       = gtk_image_new_from_file(path);
  menu_item  = gtk_image_menu_item_new();
  g_free(path);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item), icon);
  gtk_menu_item_set_label(GTK_MENU_ITEM(menu_item), _("None"));
  gtk_menu_shell_append(GTK_MENU_SHELL(bar->mood_menu), menu_item);
  g_signal_connect_swapped(menu_item, "activate",
                           G_CALLBACK(cb_mood_menu),
                           (gpointer)empty_mood);
  gtk_widget_show(menu_item);

  /* fill mood menu */
  for( ; mood->mood ; mood++) {
    if(!mood->mood || !mood->description)
      continue;

    path = get_mood_icon_path(mood->mood);
    icon = gtk_image_new_from_file(path);
    menu_item = gtk_image_menu_item_new();
    g_free(path);

    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item), icon);
    gtk_menu_item_set_label(GTK_MENU_ITEM(menu_item), _(mood->description));
    gtk_menu_shell_append(GTK_MENU_SHELL(bar->mood_menu), menu_item);

    g_signal_connect_swapped(menu_item, "activate",
                             G_CALLBACK(cb_mood_menu),
                             (gpointer)mood->mood);

    gtk_widget_show(menu_item);
  }

  event = (GdkEventButton *)gtk_get_current_event();
  gtk_menu_popup(GTK_MENU(bar->mood_menu), NULL, NULL, NULL, NULL,
                 event->button, event->time);
}

void cb_mood_menu(gpointer data)
{
  g_return_if_fail(bar->installed);

  const gchar *mood = (const gchar *)data;
  GList *accounts = purple_accounts_get_all_active();
  gchar *path;

  for(; accounts ; accounts = g_list_delete_link(accounts, accounts)) {
    PurpleAccount *account = (PurpleAccount *)accounts->data;
    PurpleConnection *gc = purple_account_get_connection(account);

    if(gc && gc->flags & PURPLE_CONNECTION_SUPPORT_MOODS)
      set_status_with_mood(account, mood);
  }

  purple_prefs_set_string(PREF "/mood", mood);
  path = get_mood_icon_path(mood);
  set_widget_mood(path);
  g_free(path);

  purple_debug_info(NAME, "mood changed to \"%s\" by user\n", mood);
}
