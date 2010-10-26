/* File: purple.h
   Time-stamp: <2010-10-10 16:40:42 gawen>

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

#ifndef _PURPLE_H_
#define _PURPLE_H_

#include "common.h"

/* not sure purple define that */
#ifndef PURPLE_PREFS_CALLBACK
# define PURPLE_PREFS_CALLBACK(func) ((PurplePrefCallback)func)
#endif

gboolean is_gtk_blist_created();
GdkPixbuf * get_buddy_icon();
GdkPixbuf * get_buddy_icon_hover();
const gchar * get_status_stock_id();
gchar * get_mood_icon_path();
const gchar * get_global_mood_status();
void set_display_name(PurpleAccount *account, const gchar *name);

#endif /* _PURPLE_H_ */
