/* File: gtk.h
   Time-stamp: <2011-02-11 01:23:06 gawen>

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

#define PBAR_GTK_CALLBACK(func) ((void (*)(GtkWidget *, gpointer))func)
#define PBAR_WIDGET(widget) ((struct pbar_widget *)widget)
#define BEGIN_PBAR_WIDGET GList *gtk_hnd;     \
                          GList *gtk_inst;    \
                          GList *main_widgets

struct pbar_gtk_signal {
  GtkWidget *widget;
  const gchar *signal;
  void (*callback)(GtkWidget *, gpointer);
};

struct pbar_widget {
  BEGIN_PBAR_WIDGET; /* signals handlers and instance for disconnection */
};

#if !GTK_CHECK_VERSION(2,18,0)
gboolean gtk_widget_get_visible(GtkWidget *widget);
void gtk_widget_set_visible(GtkWidget *widget, gboolean visible);
void gtk_widget_set_can_focus(GtkWidget *widget, gboolean can_focus);
#endif /* GTK < 2.18 */

void gtk_destroy(struct pbar_widget *w);
void gtk_add_main_widget(struct pbar_widget *w, GtkWidget *widget);
void gtk_connect_signals(struct pbar_widget *w,
                         const struct pbar_gtk_signal *sig_list,
                         gpointer data);
GtkWidget * gtk_pidgin_dialog_box_new(const gchar *primary,
                                      const gchar *secondary,
                                      const gchar *icon);

#endif /* _GTK_H_ */
