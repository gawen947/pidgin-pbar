/* File: purple.c
   Time-stamp: <2010-10-05 20:43:05 gawen>

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

#include "purple.h"

gboolean is_gtk_blist_created()
{
  const PidginBuddyList *blist;

  blist = pidgin_blist_get_default_gtk_blist();

  if(!blist ||
     !blist->vbox ||
     !gtk_widget_get_visible(blist->vbox))
    return FALSE;
  return TRUE;
}

GdkPixbuf * get_buddy_icon()
{
  const PidginBuddyList *blist;
  const PidginStatusBox *statusbox;

  blist = pidgin_blist_get_default_gtk_blist();
  statusbox = PIDGIN_STATUS_BOX(blist->statusbox);
  return statusbox->buddy_icon;
}

const gchar * get_status_stock_id()
{
  const PurpleSavedStatus *status;
  PurpleStatusPrimitive prim;

  status = purple_savedstatus_get_current();
  prim   = purple_savedstatus_get_type(status);
  return pidgin_stock_id_from_status_primitive(prim);
}

