/* File: widget.h
   Time-stamp: <2010-10-27 17:59:34 gawen>

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

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "common.h"

struct widget {
  /* icon and status */
  GtkWidget *icon;
  GtkWidget *icon_eventbox;
  GtkWidget *status;
  GtkWidget *status_menu;

  /* mood */
  GtkWidget *mood;
  GtkWidget *mood_menu;

  /* nickname */
  GtkWidget *name_label;
  GtkWidget *name_eventbox;
  GtkWidget *name_entry;

  /* personal message */
  GtkWidget *pm_label;
  GtkWidget *pm_eventbox;
  GtkWidget *pm_entry;

  GtkWidget *hbox;       /* contains the widget */

  gboolean installed;    /* widget installed or not */
  gboolean hover_name;   /* name hovered or not */
  gboolean hover_pm;     /* pm hovered or not */

  /* avoid setting status and name twice
     with focus-out-event */
  gboolean name_entry_activated;
  gboolean pm_entry_activated;

  /* signals handlers and instance for disconnection */
  GList *gtk_hnd;
  GList *gtk_inst;
};

extern struct widget *bar;

void create_widget();
void destroy_widget();
void init_widget();
void set_widget_name(const gchar *markup, const gchar *name);
void set_widget_pm(const gchar *markup, const gchar *pm);
void set_widget_status(const gchar *stock);
void set_widget_mood(const gchar *path);
void set_widget_icon(GdkPixbuf *icon);
void set_widget_name_justify(int justify);
void set_widget_pm_justify(int justify);
void set_widget_entry_frame(gboolean use_frame);
void set_statusbox_visible(gboolean visible);
gboolean get_widget_name_hover_state();
gboolean get_widget_pm_hover_state();

#endif /* _WIDGET_H_ */
