/* File: prefs.c
   Time-stamp: <2011-06-17 05:03:03 gawen>

   Copyright (C) 2010 David Hauweele <david@hauweele.net>
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
#include "widget.h"

/* callback for preferences setting which set preferences and
   update interface when needed */
static void cb_nickname_markup(GtkWidget *widget, gpointer data);
static void cb_nickname_markup_hover(GtkWidget *widget, gpointer data);
static void cb_personal_message_markup(GtkWidget *widget, gpointer data);
static void cb_personal_message_markup_hover(GtkWidget *widget, gpointer data);
static void cb_hide_statusbox(GtkWidget *widget, gpointer data);
static void cb_override_status(GtkWidget *widget, gpointer data);
static void cb_nickname_justify(GtkWidget *widget, gpointer data);
static void cb_personal_message_justify(GtkWidget *widget, gpointer data);
static void cb_compact(GtkWidget *widget, gpointer data);
static void cb_frame_entry(GtkWidget *widget, gpointer data);
static void cb_swap_click(GtkWidget *widget, gpointer data);
static void cb_reset_attrs(GtkWidget *widget, gpointer data);
static void cb_widget_position(GtkWidget *widget, gpointer data);

/* the following structures are defined here as global
   since they are needed in combobox callback */

/* alias we will use for combobox */
static const struct i_alias {
  const char *alias;
  int value;
} alias_justify[] = {
  { N_("Left"),   JUSTIFY_LEFT },
  { N_("Center"), JUSTIFY_CENTER },
  { N_("Right"),  JUSTIFY_RIGHT },
  { NULL, 0 }
};

/* widget position in the buddy list */
static const struct i_alias alias_position[] = {
  { N_("Top"),     POSITION_TOP },
  { N_("Bottom"),  POSITION_BOTTOM },
  { NULL, 0 }
};

/* init preferences and set default values */
void init_prefs()
{
  /* string preferences and default value */
  const struct prefs_string {
    const char *name;
    const char *value;
  } prefs_add_string[] = {
    { PREF "/personal-message-markup-hover", "<span color=\"DarkOliveGreen4\"><small><i>%p</i></small></span>" },
    { PREF "/personal-message-markup", "<small>%p</small>" },
    { PREF "/nickname-markup-hover", "<span color=\"DarkOliveGreen4\"><b>%n</b></span>" },
    { PREF "/nickname-markup", "<b>%n</b>" },
    { PREF "/personal-message", EMPTY_PM },
    { PREF "/tune-title", "" },
    { PREF "/tune-artist", "" },
    { PREF "/tune-album", "" },
    { PREF "/game-message", "" },
    { PREF "/office-message", "" },
    { PREF "/nickname", EMPTY_NAME },
    { PREF "/mood-message", "" },
    { PREF "/mood", "" },
    { NULL, NULL }
  }; const struct prefs_string *s = prefs_add_string;

  /* boolean preferences and default value */
  const struct prefs_bool {
    const char *name;
    gboolean value;
  } prefs_add_bool[] = {
    { PREF "/hide-statusbox", TRUE },
    { PREF "/override-status", FALSE },
    { PREF "/frame-entry", TRUE },
    { PREF "/swap-click", FALSE },
    { PREF "/reset-attrs", FALSE },
    { PREF "/compact", FALSE },
    { NULL, FALSE }
  }; const struct prefs_bool *b = prefs_add_bool;

  /* integer preferences and default value */
  const struct prefs_int {
    const char *name;
    int value;
  } prefs_add_int[] = {
    { PREF "/nickname-justify", JUSTIFY_LEFT },
    { PREF "/personal-message-justify", JUSTIFY_LEFT },
    { PREF "/widget-position", POSITION_TOP },
    { NULL, 0 }
  }; const struct prefs_int *i = prefs_add_int;

  /* add preferences */
  purple_prefs_add_none(PREF);
  for(; s->name ; s++)
    purple_prefs_add_string(s->name, s->value);
  for(; b->name ; b++)
    purple_prefs_add_bool(b->name, b->value);
  for(; i->name ; i++)
    purple_prefs_add_int(i->name, i->value);
}

GtkWidget * get_config_frame(PurplePlugin *plugin)
{
  /* entry widgets label, associated preference and callback */
  const struct widget {
    const char *name;
    const char *prefs;
    void (*callback)(GtkWidget *, gpointer);
    const char *tooltip;
  } entry[] = {
    { N_("_Nickname markup"), PREF "/nickname-markup", cb_nickname_markup,
      N_("Change the markup used to display the nickname using the "
         "Pango Markup Language where %n is replaced with your nickname.") },

    { N_("Nickname markup _hovered"), PREF "/nickname-markup-hover",
      cb_nickname_markup_hover,
      N_("Change the markup used to display the nickname when hovered "
         "by the mouse using the Pango Markup Language where %n is replaced "
         "with your nickname.") },

    { N_("Personal _message markup"), PREF "/personal-message-markup",
      cb_personal_message_markup,
      N_("Change the markup used to display the personal message using the "
         "Pango Markup Language where %p is replaced with your personal "
         "message.") },

     { N_("Personal message markup _hovered"), PREF "/personal-message-markup-hover",
       cb_personal_message_markup_hover,
       N_("Change the markup used to display the personal message when hovered "
          "by the mouse using the Pango Markup Language where %p is replaced "
          "with your personal message.") },

    { NULL, NULL, NULL }
  }; const struct widget *e = entry;

  /* combobox widgets label, associated preference, alias and callback */
  const struct i_widget {
    const char *name;
    const char *prefs;
    const struct i_alias *alias;
    void (*callback)(GtkWidget *, gpointer);
    const char *tooltip;
  } combobox[] = {
    { N_("Align _nickname"), PREF "/nickname-justify", alias_justify,
      cb_nickname_justify,
      N_("Alignment of the nickname into the bar.") },

    { N_("Align personal _message"), PREF "/personal-message-justify",
      alias_justify, cb_personal_message_justify,
      N_("Alignment of the personal message into the bar.") },

    { N_("Widget _position in the buddy list"), PREF "/widget-position",
      alias_position, cb_widget_position,
      N_("Position of the widget into pidgin's buddy list.") },

    { NULL, NULL, NULL, NULL }
  }; const struct i_widget *cbx = combobox;

  /* check button widgets label, associated preference and callback */
  const struct widget check_button[] = {
    { N_("Hide _statusbox"), PREF "/hide-statusbox", cb_hide_statusbox,
      N_("Show or hide the Pidgin's default statusbox.") },

    { N_("_Ignore status changes"), PREF "/override-status", cb_override_status,
      N_("Ignore changes made to status from outside the widget. "
         "This include changes made in Pidgin's default statusbox "
         "and other plugins.") },

    { N_("Use a frame for _entry"), PREF "/frame-entry", cb_frame_entry,
      N_("Enable or disable the use of a frame around the entries when "
         "editing the nickname or personal message from the bar.") },

    { N_("_Swap left and right click"), PREF "/swap-click", cb_swap_click,
      N_("Swap the role of left and right click to edit the nickname or "
         "personal message in the bar.") },

    { N_("Use a _compact bar"), PREF "/compact", cb_compact,
      N_("Reduce the size of the widget putting the nickname and personal "
         "message on one line.") },

    { N_("_Reset status messages"), PREF "/reset-attrs", cb_reset_attrs,
      N_("Clear the status messages when Pidgin restart (except for personal "
         "message). By default these messages are saved in the preferences and "
         "reactivated when pidgin restart.") },

    { NULL, NULL, NULL }
  }; const struct widget *cb = check_button;

  /* create table */
  GtkWidget *table = gtk_table_new(((sizeof(entry) - 2) +
                                    sizeof(check_button) / 2 - 1) /
                                   sizeof(struct widget),
                                   2, FALSE);

  /* load table and connect signals */
  int x = 0, y = 0;
  for(; e->name ; e++, y++) {
    /* entry widgets */
    GtkWidget *widget_label  = gtk_label_new_with_mnemonic(_(e->name));
    GtkWidget *widget_entry  = gtk_entry_new();
    const gchar *prefs_value = purple_prefs_get_string(e->prefs);

    gtk_label_set_mnemonic_widget(GTK_LABEL(widget_label), widget_entry);
    gtk_entry_set_text(GTK_ENTRY(widget_entry), prefs_value);
    gtk_widget_set_tooltip_text(widget_entry, _(e->tooltip));
    gtk_misc_set_alignment(GTK_MISC(widget_label), 0., .5);
    gtk_table_attach(GTK_TABLE(table), widget_label, 0, 1, y, y+1, GTK_FILL, GTK_FILL, 5, 5);
    gtk_table_attach(GTK_TABLE(table), widget_entry, 1, 2, y, y+1, GTK_FILL, GTK_FILL, 5, 5);
    g_signal_connect(G_OBJECT(widget_entry), "activate", G_CALLBACK(e->callback), NULL);
    g_signal_connect(G_OBJECT(widget_entry), "focus-out-event", G_CALLBACK(e->callback), NULL);
  }
  for(; cbx->name ; cbx++, y++) {
    /* combobox widgets */
    const struct i_alias *j;
    GtkWidget *widget_label = gtk_label_new_with_mnemonic(_(cbx->name));
    GtkWidget *widget_combo = gtk_combo_box_new_text();
    int prefs_value         = purple_prefs_get_int(cbx->prefs);
    int i;

    gtk_label_set_mnemonic_widget(GTK_LABEL(widget_label), widget_combo);
    gtk_misc_set_alignment(GTK_MISC(widget_label), 0., .5);
    gtk_widget_set_tooltip_text(widget_label, _(cbx->tooltip));
    gtk_table_attach(GTK_TABLE(table), widget_label, 0, 1, y, y+1, GTK_FILL, GTK_FILL, 5, 5);
    gtk_table_attach(GTK_TABLE(table), widget_combo, 1, 2, y, y+1, GTK_FILL, GTK_FILL, 5, 5);
    for(i = 0, j = cbx->alias ; j->alias ; j++, i++) {
      gtk_combo_box_append_text(GTK_COMBO_BOX(widget_combo), _(j->alias));
      if(j->value == prefs_value)
        gtk_combo_box_set_active(GTK_COMBO_BOX(widget_combo), i);
    }
    g_signal_connect(G_OBJECT(widget_combo), "changed", G_CALLBACK(cbx->callback), (gpointer)cbx->alias);
  }
  for(; cb->name ; cb++, x = (x + 1) % 2) {
    /* check button widgets */
    GtkWidget *widget_cb = gtk_check_button_new_with_mnemonic(_(cb->name));
    gboolean prefs_value = purple_prefs_get_bool(cb->prefs);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget_cb), prefs_value);
    gtk_widget_set_tooltip_text(widget_cb, _(cb->tooltip));
    gtk_table_attach(GTK_TABLE(table), widget_cb, x, x+1, y, y+1, GTK_FILL, GTK_FILL, 5, 5);
    g_signal_connect(G_OBJECT(widget_cb), "toggled", G_CALLBACK(cb->callback),NULL);
    if(x % 2)
      y++;
  }

  return table; /* pidgin destroy this when closed */
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

  purple_debug_info(NAME, "status box state changed\n");
}

static void cb_override_status(GtkWidget *widget, gpointer data)
{
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  purple_prefs_set_bool(PREF "/override-status", state);

  purple_debug_info(NAME, "override status state changed\n");
}

static void cb_nickname_justify(GtkWidget *widget, gpointer data)
{
  const gchar *value = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
  const struct i_alias *j;

  for(j = (struct i_alias *)data ; j->alias ; j++) {
    if(!strcmp(value, _(j->alias))) {
      purple_prefs_set_int(PREF "/nickname-justify", j->value);
      set_widget_name_justify(j->value);
      break;
    }
  }

  purple_debug_info(NAME, "nickname justification changed\n");
}

static void cb_personal_message_justify(GtkWidget *widget, gpointer data)
{
  const gchar *value = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
  const struct i_alias *j;

  for(j = (struct i_alias *)data ; j->alias ; j++) {
    if(!strcmp(value, _(j->alias))) {
      purple_prefs_set_int(PREF "/personal-message-justify", j->value);
      set_widget_pm_justify(j->value);
      break;
    }
  }

  purple_debug_info(NAME, "personal message justification changed\n");
}

static void cb_compact(GtkWidget *widget, gpointer data)
{
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

  purple_prefs_set_bool(PREF "/compact", state);
  /* recreate bar since we need to repack everything */
  reset_widget();
  init_widget();

  purple_debug_info(NAME, "compact state changed\n");
}

static void cb_frame_entry(GtkWidget *widget, gpointer data)
{
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

  purple_prefs_set_bool(PREF "/frame-entry", state);
  set_widget_entry_frame(state);

  purple_debug_info(NAME, "frame entry state changed\n");
}

static void cb_swap_click(GtkWidget *widget, gpointer data)
{
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  purple_prefs_set_bool(PREF "/swap-click", state);

  purple_debug_info(NAME, "swap click state changed\n");
}

static void cb_reset_attrs(GtkWidget *widget, gpointer data)
{
  gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

  purple_prefs_set_bool(PREF "/reset-attrs", state);
  set_widget_entry_frame(state);

  purple_debug_info(NAME, "reset attributes state changed\n");
}

static void cb_widget_position(GtkWidget *widget, gpointer data)
{
  const gchar *value = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
  const struct i_alias *j;

  for(j = (struct i_alias *)data ; j->alias ; j++) {
    if(!strcmp(value, _(j->alias))) {
      purple_prefs_set_int(PREF "/widget-position", j->value);
      /* recreate bar since we need to repack everything */
      reset_widget();
      init_widget();
      break;
    }
  }

  purple_debug_info(NAME, "personal message justification changed\n");
}
