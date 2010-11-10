/* File: widget_prpl.c
   Time-stamp: <2010-11-10 02:58:14 gawen>

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
#include "widget.h"
#include "widget_prpl.h"
#include "purple.h"

void cb_status(PurpleAccount *account, PurpleStatus *old, PurpleStatus *new)
{
  g_return_if_fail(bar->installed);

  PurpleSavedStatus *status;
  PurpleStatusPrimitive prim;
  const gchar *stock, *pm;

  status = purple_savedstatus_get_current();
  if(purple_prefs_get_bool(PREF "/override-status")) {
    pm = purple_prefs_get_string(PREF "/personal-message");
    purple_savedstatus_set_message(status,pm);
    purple_savedstatus_activate(status);
  }
  else {
    const gchar *markup;
    markup = purple_prefs_get_string(PREF "/personal-message-markup");
    pm = purple_savedstatus_get_message(status);
    if(!pm)
      pm = "";
    set_widget_pm(markup, pm);
    purple_prefs_set_string(PREF "/personal-message", pm);
  }

  prim = purple_savedstatus_get_type(status);
  stock = pidgin_stock_id_from_status_primitive(prim);
  set_widget_status(stock);
}

void cb_signed_on(PurpleConnection *gc)
{
  const gchar *name;
  PurpleAccount *account;

  name = purple_prefs_get_string(PREF "/nickname");
  account = purple_connection_get_account(gc);
  set_display_name(account, name);

  purple_debug_info(NAME, "nickname changed to \"%s\" by signed-on account\n",
                    name);

  if(gc && gc->flags & PURPLE_CONNECTION_SUPPORT_MOODS) {
    const gchar *mood = purple_prefs_get_string(PREF "/mood");
    set_status_with_mood(account, mood);

    purple_debug_info(NAME, "mood changed to \"%s\" by signed-on account\n",
                      mood);
  }
}

void cb_buddy_icon_update(const char *name, PurplePrefType type,
                          gconstpointer val, gpointer data)
{
  g_return_if_fail(bar->installed);

  GdkPixbuf *icon;

  icon = get_buddy_icon();
  set_widget_icon(icon);
}

void cb_name_apply(PurpleAccount *account, const char *user_info)
{
  g_return_if_fail(bar->installed);

  const gchar *markup, *name;

  name = user_info;
  markup = purple_prefs_get_string(PREF "/nickname-markup");
  set_widget_name(markup, name);

  purple_prefs_set_string(PREF "/nickname", name);
  set_display_name_all(name);

  bar->name_dialog = FALSE;

  purple_debug_info(NAME, "nickname changed to \"%s\" by user\n", name);
}

void cb_name_cancel(PurpleAccount *account, const char *user_info)
{
  g_return_if_fail(bar->installed);

  bar->name_dialog = FALSE;
}

void cb_pm_apply(gpointer data, PurpleRequestFields *fields)
{
  g_return_if_fail(bar->installed);

  /* attrs */
  GList *a_pm = NULL;
  GList *a_tune = NULL;
  GList *a_mood = NULL;

  /* just to update widget */
  const gchar *pm = purple_request_fields_get_string(fields, PREF "/personal-message");
  const gchar *markup = purple_prefs_get_string(PREF "/personal-message-markup");
  set_widget_pm(markup, pm);

  const struct r_field {
    const gchar *id;
    const gchar *attr;
    const gchar *empty;
    GList **list;
  } r_fields[] = {
    { PREF "/personal-message", NULL, EMPTY_PM, &a_pm },
    { PREF "/mood-message", PURPLE_MOOD_COMMENT, EMPTY_MOOD, &a_mood },
    { PREF "/tune-title", PURPLE_TUNE_TITLE, EMPTY_TITLE, &a_tune },
    { PREF "/tune-artist", PURPLE_TUNE_ARTIST, EMPTY_ARTIST, &a_tune },
    { PREF "/tune-album", PURPLE_TUNE_ALBUM, EMPTY_ALBUM, &a_tune },
    { PREF "/tune-genre", PURPLE_TUNE_GENRE, EMPTY_GENRE, &a_tune },
    { PREF "/tune-comment", PURPLE_TUNE_COMMENT, EMPTY_COMMENT, &a_tune },
    { PREF "/tune-track", PURPLE_TUNE_TRACK, EMPTY_TRACK, &a_tune },
    { PREF "/tune-time", PURPLE_TUNE_TIME, EMPTY_TIME, &a_tune },
    { PREF "/tune-year", PURPLE_TUNE_YEAR, EMPTY_YEAR, &a_tune },
    { PREF "/tune-url", PURPLE_TUNE_URL, EMPTY_URL, &a_tune },
    { PREF "/tune-full", PURPLE_TUNE_FULL, EMPTY_FULL, &a_tune },
    /* from msn-pecan */
    { PREF "/game-message", "game", EMPTY_GAME, &a_tune },
    { PREF "/office-message", "office", EMPTY_OFFICE, &a_tune },
    { NULL, NULL, NULL, NULL }
  }; const register struct r_field *rf = r_fields;

  for(; rf->id ; rf++) {
    const gchar *value = purple_request_fields_get_string(fields, rf->id);

    if(!strcmp(value, _(rf->empty)))
      value = "";
    else
      purple_prefs_set_string(rf->id, value);

    *(rf->list) = g_list_append(*(rf->list), (gpointer)rf->attr);
    *(rf->list) = g_list_append(*(rf->list), (gpointer)value);
  }

  const struct status_list {
    const gchar *status_id;
    GList *list;
  } status[] = {
    { NULL, a_pm },
    { "tune", a_tune },
    { "mood", a_mood },
    { NULL, NULL }
  }; register const struct status_list *s = status;

  for(; s->list ; s++) {
    set_status_all(s->status_id, s->list);
    g_list_free(s->list);
  }

  bar->pm_dialog = FALSE;

  /* FIXME: change that ! */
  purple_debug_info(NAME, "personal message changed to \"%s\" by user\n", pm);
}

void cb_pm_cancel(gpointer data, PurpleRequestFields *fields)
{
  g_return_if_fail(bar->installed);

  bar->pm_dialog = FALSE;
}
