/* File: prefs.c
   Time-stamp: <2010-10-05 19:16:24 gawen>

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
  const struct prefs_string {
    const char *name;
    const char *value;
  } prefs_add_string[] = {
    { PREF "/personal-message-markup-hover",
      "<span color=\"darkgreen\"><small><i>%p</i></small></span>" },
    { PREF "/personal-message-markup",
      "<small><i>%p</i></small>" },
    { PREF "/nickname-markup-hover",
      "<span color=\"darkgreen\"><b>%n</b></span>" },
    { PREF "/nickname-markup",
      "<b>%n</b>" },
    { PREF "/personal-message",
      EMPTY_PM },
    { PREF "/nickname",
      EMPTY_NAME},
    { NULL, NULL }
  }; register const struct prefs_string *s = prefs_add_string;

  const struct prefs_bool {
    const char *name;
    gboolean value;
  } prefs_add_bool[] = {
    { PREF "/hide-statusbox", TRUE },
    { NULL, FALSE }
  }; register const struct prefs_bool *b = prefs_add_bool;

  purple_prefs_add_none(PREF);
  for(; s->name ; s++)
    purple_prefs_add_string(s->name, s->value);
  for(; b->name ; b++)
    purple_prefs_add_bool(b->name, b->value);
}

GtkWidget * get_config_frame(PurplePlugin *plugin)
{
  struct widget {
    const char *name;
    const char *prefs;
    void (*callback)(GtkWidget *, gpointer);
  };

  const struct widget entry[] = {
    { "Nickname markup",
      PREF "/nickname-markup",
      cb_nickname_markup },
    { "Nickname markup hover",
      PREF "/nickname-markup-hover",
      cb_nickname_markup_hover },
    { "Personal message markup",
      PREF "/personal-message-markup",
      cb_personal_message_markup },
    { "Personal message markup hover",
      PREF "/personal-message-markup-hover",
      cb_personal_message_markup_hover },
    { NULL, NULL, NULL }
  }; register const struct widget *e = entry;

  const struct widget check_button[] = {
    { "Hide status box",
      PREF "/hide-statusbox",
      cb_hide_statusbox },
    { NULL, NULL, NULL }
  }; register const struct widget *cb = check_button;

  GtkWidget *table = gtk_table_new(((sizeof(entry) - 2) +
                                    sizeof(check_button) / 2 - 1) /
                                   sizeof(struct widget),
                                   2, FALSE);
  int x = 0, y = 0;
  for(; e->name ; e++, y++) {
    GtkWidget *widget_label  = gtk_label_new(e->name);
    GtkWidget *widget_entry  = gtk_entry_new();
    const gchar *prefs_value = purple_prefs_get_string(e->prefs);

    gtk_entry_set_text(GTK_ENTRY(widget_entry), prefs_value);
    gtk_misc_set_alignment(GTK_MISC(widget_label), 0., .5);
    gtk_table_attach(GTK_TABLE(table), widget_label, 0, 1, y, y+1, GTK_FILL, GTK_FILL, 5, 5);
    gtk_table_attach(GTK_TABLE(table), widget_entry, 1, 2, y, y+1, GTK_FILL, GTK_FILL, 5, 5);
    g_signal_connect(G_OBJECT(widget_entry), "changed", G_CALLBACK(e->callback),NULL);
  }
  for(; cb->name ; cb++, x = (x + 1) % 2, y++) {
    GtkWidget *widget_cb = gtk_check_button_new_with_label(cb->name);
    gboolean prefs_value = purple_prefs_get_bool(cb->prefs);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget_cb), prefs_value);
    gtk_table_attach(GTK_TABLE(table), widget_cb, x, x+1, y, y+1, GTK_FILL, GTK_FILL, 5, 5);
    g_signal_connect(G_OBJECT(widget_cb), "toggled", G_CALLBACK(cb->callback),NULL);
  }

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
