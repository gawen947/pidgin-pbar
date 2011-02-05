/* File: gtk.h
   Time-stamp: <2011-02-05 02:56:44 gawen>

   Copyright (C) 2011 David Hauweele <david.hauweele@gmail.com>

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

#ifndef _GTK_H_
#define _GTK_H_

#include "common.h"

#if !GTK_CHECK_VERSION(2,18,0)
gboolean gtk_widget_get_visible(GtkWidget *widget);
void gtk_widget_set_visible(GtkWidget *widget, gboolean visible);
void gtk_widget_set_can_focus(GtkWidget *widget, gboolean can_focus);
#endif /* GTK < 2.18 */

#endif /* _GTK_H_ */
