/* File: widget_prpl.c
   Time-stamp: <2010-10-10 21:10:42 gawen>

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
#include "widget_prpl.h"
#include "purple.h"

void cb_status(PurpleAccount *account, PurpleStatus *old, PurpleStatus *new)
{
  g_return_if_fail(bar->installed);

  PurpleSavedStatus *status;
  PurpleStatusPrimitive prim;
  const gchar *stock;
  const gchar *pm;

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
    set_widget_pm(markup, pm);
    purple_prefs_set_string(PREF "/personal-message", pm);
  }

  prim = purple_savedstatus_get_type(status);
  stock = pidgin_stock_id_from_status_primitive(prim);
  set_widget_status(stock);
}

