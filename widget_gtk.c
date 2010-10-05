/* File: widget_gtk.c
   Time-stamp: <2010-10-05 20:55:16 gawen>

   Copyright (C) 2010 David Hauweele <david.hauweele@gmail.com>

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

static void colorshift(GdkPixbuf *dest, GdkPixbuf *src, int shift)
{
  /* TODO: redo */
  gint i,j;
  gint width, height, has_alpha, srcrowstride, destrowstride;
  guchar *target_pixels;
  guchar *orig_pixels;
  guchar *pixsrc;
  guchar *pixdest;
  int val;
  guchar r,g,b;

  has_alpha = gdk_pixbuf_get_has_alpha(src);
  width = gdk_pixbuf_get_width(src);
  height = gdk_pixbuf_get_height(src);
  srcrowstride = gdk_pixbuf_get_rowstride(src);
  destrowstride = gdk_pixbuf_get_rowstride(dest);
  target_pixels = gdk_pixbuf_get_pixels(dest);
  orig_pixels = gdk_pixbuf_get_pixels(src);

  for(i = 0 ; i < height ; i++) {
    pixdest = target_pixels + i * destrowstride;
    pixsrc  = orig_pixels + i * srcrowstride;
    for(j = 0 ; j < width ; j++) {
      r = *(pixsrc++);
      g = *(pixsrc++);
      b = *(pixsrc++);

      val = r + shift;
      *(pixdest++) = CLAMP(val, 0, 255);
      val = g + shift;
      *(pixdest++) = CLAMP(val, 0, 255);
      val = b + shift;
      *(pixdest++) = CLAMP(val, 0, 255);
      if(has_alpha)
        *(pixdest++) = *(pixsrc++);
    }
  }
}

static void set_buddy_icon_path(const gchar *path, gpointer data)
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
                                          set_buddy_icon_path,
                                          NULL);
  gtk_widget_show(chooser);
}

void cb_buddy_icon_enter(GtkWidget *widget, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkPixbuf *icon;

  icon = get_buddy_icon();
  colorshift(icon,icon,32);
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

  const gchar *name, *markup;
  PurpleAccount *account;
  PurplePlugin *protocol;
  PurpleConnection *connection;
  PurplePluginProtocolInfo *info;
  GList *pa, *pp;
  GList *protocols, *accounts;
  const gchar *account_id, *protocol_id;

  name = gtk_entry_get_text(GTK_ENTRY(widget));
  purple_prefs_set_string(PREF "/nickname", name);

  /* get all supported protocols and active accounts */
  protocols = purple_plugins_get_protocols();
  accounts  = purple_accounts_get_all_active();
  if(!(protocols && accounts)) {
    purple_debug_error(NAME, "cannot get protocol list or active accounts list");
    return;
  }

  /* ok! now set nickname for all connection! */
  for(pa = accounts ; pa ; pa = pa->next) {
    account = pa->data;
    account_id = g_strdup(purple_account_get_protocol_id(account));

    /* set nickname per protocol */
    if(purple_account_is_connected(account)) {
      connection = purple_account_get_connection(account);

      /* check every protocols and plugins */
      for(pp = protocols ; pp ; pp = pp->next) {
        protocol = pp->data;
        protocol_id = g_strdup(protocol->info->id);
        info = PURPLE_PLUGIN_PROTOCOL_INFO(protocol);

        /* set public alias accorded to protocol info */
        if(!strcmp(account_id, protocol_id)
           && info->set_public_alias)
          info->set_public_alias(connection, name, NULL, NULL);
      }
    }
  }

  markup = purple_prefs_get_string(PREF "/nickname-markup");
  set_widget_name(markup, name);

  /* TODO: check if we need to take care of hover state */
  gtk_widget_hide(bar->name_entry);
  gtk_widget_show(bar->name_button);

  purple_debug_info(NAME, "nickname set to \"%s\".", name);
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

  markup = purple_prefs_get_string(PREF "/nickname-markup");
  set_widget_pm(markup, pm);

  /* TODO: check if we need to take care of hover state */
  gtk_widget_hide(bar->pm_entry);
  gtk_widget_show(bar->pm_button);

  purple_debug_info(NAME, "personal message set to \"%s\".", pm);
}
