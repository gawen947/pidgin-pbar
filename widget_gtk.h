/* File: widget_gtk.h
   Time-stamp: <2010-09-26 01:11:22 gawen>

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

#ifndef _WIDGET_GTK_H_
#define _WIDGET_GTK_H_

#include "common.h"

void cb_buddy_icon(GtkWidget *widget, gpointer data);
void cb_buddy_icon_enter(GtkWidget *widget, gpointer data);
void cb_buddy_icon_leave(GtkWidget *widget, gpointer data);

void cb_name_button(GtkWidget *widget, gpointer data);
void cb_name_button_enter(GtkWidget *widget, gpointer data);
void cb_name_button_leave(GtkWidget *widget, gpointer data);
void cb_name_entry(GtkWidget *widget, gpointer data);

void cb_pm_button(GtkWidget *widget, gpointer data);
void cb_pm_button_enter(GtkWidget *widget, gpointer data);
void cb_pm_button_leave(GtkWidget *widget, gpointer data);
void cb_pm_entry(GtkWidget *widget, gpointer data);

#endif /* _WIDGET_GTK_H_ */
