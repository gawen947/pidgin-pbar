 /* File: prefs.c
   Time-stamp: <2010-10-05 01:17:58 gawen>

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
#include "widget.h"

static void cb_nickname_markup(GtkWidget *widget, gpointer data);
static void cb_nickname_markup_hover(GtkWidget *widget, gpointer data);
static void cb_personal_message_markup(GtkWidget *widget, gpointer data);
static void cb_personal_message_markup_hover(GtkWidget *widget, gpointer data);
static void cb_hide_statusbox(GtkWidget *widget, gpointer data);

void init_prefs()
{
  purple_prefs_add_none(PREF);
  purple_prefs_add_string(PREF "/personal-message-markup-hover",
                          "<span color=\"darkgreen\"><small><i>%p</i></small></span>");

  purple_prefs_add_string(PREF "/personal-message-markup",
                          "<small><i>%p</i></small>");

  purple_prefs_add_string(PREF "/nickname-markup-hover",
                          "<span color=\"darkgreen\"><b>%n</b></span>");

  purple_prefs_add_string(PREF "/nickname-markup",
                          "<b>%n</b>");

  purple_prefs_add_string(PREF "/personal-message",
                          EMPTY_PM);

  purple_prefs_add_string(PREF "/nickname",
                          EMPTY_NAME);

  purple_prefs_add_bool(PREF "/hide-statusbox", TRUE);
}

GtkWidget * get_config_frame(PurplePlugin *plugin)
{
  /* create widgets */
  GtkWidget *table = gtk_table_new(5, 2, FALSE);
  GtkWidget *l_n_m  = gtk_label_new("Nickname markup");
  GtkWidget *l_n_mh = gtk_label_new("Nickname markup hover");
  GtkWidget *l_p_m  = gtk_label_new("Personal message markup");
  GtkWidget *l_p_mh = gtk_label_new("Personal message markup hover");
  GtkWidget *n_m    = gtk_entry_new();
  GtkWidget *n_mh   = gtk_entry_new();
  GtkWidget *p_m    = gtk_entry_new();
  GtkWidget *p_mh   = gtk_entry_new();
  GtkWidget *sb     = gtk_check_button_new_with_label("Hide status box");

  /* load configuration */
  const gchar *s_n_m  = purple_prefs_get_string(PREF "/nickname-markup");
  const gchar *s_n_mh = purple_prefs_get_string(PREF "/nickname-markup-hover");
  const gchar *s_p_m  = purple_prefs_get_string(PREF "/personal-message-markup");
  const gchar *s_p_mh = purple_prefs_get_string(PREF "/personal-message-markup-hover");
  gboolean s_sb = purple_prefs_get_bool(PREF "/hide-statusbox");

  /* set configuration */
  gtk_entry_set_text(GTK_ENTRY(n_m),s_n_m);
  gtk_entry_set_text(GTK_ENTRY(n_mh),s_n_mh);
  gtk_entry_set_text(GTK_ENTRY(p_m),s_p_m);
  gtk_entry_set_text(GTK_ENTRY(p_mh),s_p_mh);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sb),s_sb);

  /* pack widgets */
  gtk_table_attach_defaults(GTK_TABLE(table),l_n_m,0,1,0,1);
  gtk_table_attach_defaults(GTK_TABLE(table),l_n_mh,0,1,1,2);
  gtk_table_attach_defaults(GTK_TABLE(table),l_p_m,0,1,2,3);
  gtk_table_attach_defaults(GTK_TABLE(table),l_p_mh,0,1,3,4);
  gtk_table_attach_defaults(GTK_TABLE(table),n_m,1,2,0,1);
  gtk_table_attach_defaults(GTK_TABLE(table),n_mh,1,2,1,2);
  gtk_table_attach_defaults(GTK_TABLE(table),p_m,1,2,2,3);
  gtk_table_attach_defaults(GTK_TABLE(table),p_mh,1,2,3,4);
  gtk_table_attach_defaults(GTK_TABLE(table),sb,1,2,4,5);

  /* connect signals */
  g_signal_connect(G_OBJECT(n_m),
                   "changed",
                   G_CALLBACK(cb_nickname_markup),
                   NULL);

  g_signal_connect(G_OBJECT(n_mh),
                   "changed",
                   G_CALLBACK(cb_nickname_markup_hover),
                   NULL);

  g_signal_connect(G_OBJECT(p_m),
                   "changed",
                   G_CALLBACK(cb_personal_message_markup),
                   NULL);

  g_signal_connect(G_OBJECT(p_mh),
                   "changed",
                   G_CALLBACK(cb_personal_message_markup_hover),
                   NULL);

  g_signal_connect(G_OBJECT(sb),
                   "toggled",
                   G_CALLBACK(cb_hide_statusbox),
                   NULL);

  return table;
}

static void cb_nickname_markup(GtkWidget *widget, gpointer data)
{
  const gchar *value = gtk_entry_get_text(GTK_ENTRY(widget));
  purple_prefs_set_string(PREF "/nickname-markup", value);

  if(!get_widget_name_hover_state()) {
    const gchar *name = purple_prefs_get_string(PREF "/nickname");
    set_widget_name(value, name);
  }

  purple_debug_info(NAME, "nickname markup changed\n");
}

static void cb_nickname_markup_hover(GtkWidget *widget, gpointer data)
{
  const gchar *value = gtk_entry_get_text(GTK_ENTRY(widget));
  purple_prefs_set_string(PREF "/nickname-markup-hover", value);

  if(get_widget_name_hover_state()) {
    const gchar *name = purple_prefs_get_string(PREF "/nickname");
    set_widget_name(value, name);
  }

  purple_debug_info(NAME, "nickname markup hover changed\n");
}

static void cb_personal_message_markup(GtkWidget *widget, gpointer data)
{
  const gchar *value = gtk_entry_get_text(GTK_ENTRY(widget));
  purple_prefs_set_string(PREF "/personal-message-markup", value);

  if(!get_widget_pm_hover_state()) {
    const gchar *pm = purple_prefs_get_string(PREF "/personal-message");
    set_widget_pm(value, pm);
  }

  purple_debug_info(NAME, "personal message markup hover changed\n");
}

static void cb_personal_message_markup_hover(GtkWidget *widget, gpointer data)
{
  const gchar *value = gtk_entry_get_text(GTK_ENTRY(widget));
  purple_prefs_set_string(PREF "/personal-message-markup-hover", value);

  if(get_widget_pm_hover_state()) {
    const gchar *pm = purple_prefs_get_string(PREF "/personal-message");
    set_widget_pm(value, pm);
  }

  purple_debug_info(NAME, "personal message markup hover changed\n");
}

static void cb_hide_statusbox(GtkWidget *widget, gpointer data)
{
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  purple_prefs_set_bool(PREF "/hide-statusbox", state);
  set_statusbox_visible(!state);

  purple_debug_info(NAME, "status box state changed");
}
