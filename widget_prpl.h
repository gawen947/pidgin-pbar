/* File: widget_prpl.h
   Time-stamp: <2010-10-28 20:22:13 gawen>

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

#ifndef _WIDGET_PRPL_H_
#define _WIDGET_PRPL_H_

#include "common.h"

void cb_name_apply(PurpleAccount *account, const char *user_info);
void cb_name_cancel(PurpleAccount *account, const char *user_info);
void cb_status(PurpleAccount *account, PurpleStatus *old, PurpleStatus *new);
void cb_signed_on(PurpleConnection *gc);
void cb_buddy_icon_update(const char *name, PurplePrefType type,
                          gconstpointer val, gpointer data);

#endif /* _WIDGET_PRPL_H_ */
