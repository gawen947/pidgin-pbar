/* File: widget_prpl.c
   Time-stamp: <2010-10-27 01:42:00 gawen>

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
    purple_debug_info(NAME, "personal message changed to \"%s\" by pidgin\n",
                      pm);
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

