/* File: purple.c
   Time-stamp: <2010-10-20 16:13:51 gawen>

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
/* this file contains common functions for libpurple and pidgin */

#include "common.h"

#include "purple.h"

/* check if default gtk blist is created */
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

/* get buddy icon from statusbox widget */
GdkPixbuf * get_buddy_icon()
{
  const PidginBuddyList *blist;
  const PidginStatusBox *statusbox;

  blist = pidgin_blist_get_default_gtk_blist();
  statusbox = PIDGIN_STATUS_BOX(blist->statusbox);
  return statusbox->buddy_icon;
}

/* get buddy icon hovered from statusbox widget */
GdkPixbuf * get_buddy_icon_hover()
{
  const PidginBuddyList *blist;
  const PidginStatusBox *statusbox;

  blist = pidgin_blist_get_default_gtk_blist();
  statusbox = PIDGIN_STATUS_BOX(blist->statusbox);
  return statusbox->buddy_icon_hover;
}

/* get current status stock id */
const gchar * get_status_stock_id()
{
  const PurpleSavedStatus *status;
  PurpleStatusPrimitive prim;

  status = purple_savedstatus_get_current();
  prim   = purple_savedstatus_get_type(status);
  return pidgin_stock_id_from_status_primitive(prim);
}
