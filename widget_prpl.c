/* File: widget_prpl.c
   Time-stamp: <2012-04-05 17:22:08 gawen>

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
#include "widget_prpl.h"
#include "purple.h"

static GList * append_attr(GList *list, const gchar *id, const gchar *value)
{
  list = g_list_append(list, (gpointer)id);
  return g_list_append(list, (gpointer)value);
}

void cb_status(PurpleAccount *account, PurpleStatus *old, PurpleStatus *new)
{
  g_return_if_fail(bar->installed);

  PurpleStatusPrimitive prim;
  const gchar *stock, *pm;

  PurpleSavedStatus *status = purple_savedstatus_get_current();
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

  /* we should at least check for the current song,
     if it's different we just apply the new one */
  if(!is_same_song(old, new)) {
    GList *a_tune = NULL;
    const gchar *value;

    value = purple_status_get_attr_string(new, PURPLE_TUNE_TITLE);
    purple_prefs_set_string(PREF "/tune-title", value);
    append_attr(a_tune, PURPLE_TUNE_TITLE, value);

    value = purple_status_get_attr_string(new, PURPLE_TUNE_ALBUM);
    purple_prefs_set_string(PREF "/tune-album", value);
    append_attr(a_tune, PURPLE_TUNE_ALBUM, value);

    value = purple_status_get_attr_string(new, PURPLE_TUNE_ARTIST);
    purple_prefs_set_string(PREF "/tune-artist", value);
    append_attr(a_tune, PURPLE_TUNE_ARTIST, value);

    set_status_all("tune", a_tune);
    g_list_free(a_tune);
  }

  prim = purple_savedstatus_get_type(status);
  stock = pidgin_stock_id_from_status_primitive(prim);
  set_widget_status(stock);
}

void cb_signed_off(PurpleConnection *gc)
{
  PurpleAccount *acct = purple_connection_get_account(gc);
  update_available_features(acct, FALSE);
  update_available_widgets();
}

void cb_signed_on(PurpleConnection *gc)
{
  const gchar *name = purple_prefs_get_string(PREF "/nickname");
  PurpleAccount *account = purple_connection_get_account(gc);
  set_display_name(account, name);
  update_available_features(account, TRUE);
  update_available_widgets();

  purple_debug_info(NAME, "nickname changed to \"%s\" by signed-on account\n",
                    name);

  if(gc && gc->flags & PURPLE_CONNECTION_SUPPORT_MOODS) {
    const gchar *mood = purple_prefs_get_string(PREF "/mood");
    set_status_with_mood(account, mood);

    purple_debug_info(NAME, "mood changed to \"%s\" by signed-on account\n",
                      mood);
  }

  /* load tune and stuff */
  GList *a_tune = NULL;
  GList *a_mood = NULL;

  const struct attrs {
    const gchar *pref;
    const gchar *attr;
    GList **list;
  } attrs[] = {
    { PREF "/mood-message", PURPLE_MOOD_COMMENT, &a_mood },
    { PREF "/tune-title", PURPLE_TUNE_TITLE, &a_tune },
    { PREF "/tune-artist", PURPLE_TUNE_ARTIST, &a_tune },
    { PREF "/tune-album", PURPLE_TUNE_ALBUM, &a_tune },
    { PREF "/game-message", "game", &a_tune },
    { PREF "/office-message", "office", &a_tune },
    { NULL, NULL, NULL }
  }; const register struct attrs *a = attrs;

  for(; a->pref ; a++) {
    const gchar *value;

    if(purple_prefs_get_bool(PREF "/reset-attrs"))
      value = NULL;
    else
      value = purple_prefs_get_string(a->pref);

    if(value && !strcmp(value, ""))
      value = NULL;
    else
      purple_debug_info(NAME, "%s message changed to \"%s\" by signed-on account\n",
                        a->attr, value);

    *(a->list) = g_list_append(*(a->list), (gpointer)a->attr);
    *(a->list) = g_list_append(*(a->list), (gpointer)value);
  }

  const struct status_list {
    const gchar *status_id;
    GList *list;
    gboolean cont;
  } status[] = {
    { "tune", a_tune, TRUE },
    { "mood", a_mood, TRUE },
    { NULL, NULL, FALSE}
  }; register const struct status_list *s = status;

  for(; s->cont ; s++) {
    purple_account_set_status_list(account, s->status_id, TRUE, s->list);
    g_list_free(s->list);
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
  GList *a_tune = NULL;
  GList *a_mood = NULL;

  /* just to update widget */
  const gchar *pm = purple_request_fields_get_string(fields, PREF "/personal-message");
  const gchar *markup = purple_prefs_get_string(PREF "/personal-message-markup");
  set_status_message(pm);
  set_widget_pm(markup, pm);
  purple_debug_info(NAME, "personal message changed to \"%s\" by user\n", pm);

  const struct r_field {
    const gchar *id;
    const gchar *attr;
    GList **list;
  } r_fields[] = {
    { PREF "/mood-message", PURPLE_MOOD_COMMENT, &a_mood },
    { PREF "/tune-title", PURPLE_TUNE_TITLE, &a_tune },
    { PREF "/tune-artist", PURPLE_TUNE_ARTIST, &a_tune },
    { PREF "/tune-album", PURPLE_TUNE_ALBUM, &a_tune },
    { PREF "/game-message", "game", &a_tune },
    { PREF "/office-message", "office", &a_tune },
    { NULL, NULL, NULL }
  }; const register struct r_field *rf = r_fields;

  for(; rf->id ; rf++) {
    const gchar *value = purple_request_fields_get_string(fields, rf->id);

    if(!purple_prefs_get_bool(PREF "/reset-attrs"))
      purple_prefs_set_string(rf->id, value);

    if(value && !strcmp(value, ""))
      value = NULL;
    else
      purple_debug_info(NAME, "%s message changed to \"%s\" by user\n",
                        rf->attr, value);

    *(rf->list) = append_attr(*(rf->list), rf->attr, value);
  }

  const struct status_list {
    const gchar *status_id;
    GList *list;
    gboolean cont;
  } status[] = {
    { "tune", a_tune, TRUE },
    { "mood", a_mood, TRUE },
    { NULL, NULL, FALSE}
  }; register const struct status_list *s = status;

  for(; s->cont ; s++) {
    set_status_all(s->status_id, s->list);
    g_list_free(s->list);
  }

  bar->pm_dialog = FALSE;
}

void cb_pm_cancel(gpointer data, PurpleRequestFields *fields)
{
  g_return_if_fail(bar->installed);

  bar->pm_dialog = FALSE;
}
