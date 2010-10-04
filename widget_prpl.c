/* File: widget_prpl.c
   Time-stamp: <2010-10-05 01:26:47 gawen>

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

void cb_status(PurpleAccount *account, PurpleStatus *old, PurpleStatus *new)
{
  /* TODO: override pidgin status as an option */
  g_return_if_fail(bar->installed);

  PurpleSavedStatus *status;
  PurpleStatusPrimitive prim;
  const gchar *stock;
  const gchar *pm;

  pm = purple_prefs_get_string(PREF "/personal-message");
  status = purple_savedstatus_get_current();
  purple_savedstatus_set_message(status,pm);
  purple_savedstatus_activate(status);

  prim = purple_savedstatus_get_type(status);
  stock = pidgin_stock_id_from_status_primitive(prim);
  set_widget_status(stock);
}

