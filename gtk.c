/* File: gtk.c
   Time-stamp: <2011-02-05 04:26:05 gawen>

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

#include "common.h"

#include "gtk.h"

#if !GTK_CHECK_VERSION(2,18,0)
gboolean gtk_widget_get_visible(GtkWidget *widget)
{
  return GTK_WIDGET_FLAGS(widget) & GTK_VISIBLE;
}

void gtk_widget_set_visible(GtkWidget *widget, gboolean visible)
{
  if(visible)
    gtk_widget_show(widget);
  else
    gtk_widget_hide(widget);
}

void gtk_widget_set_can_focus(GtkWidget *widget, gboolean can_focus)
{
  if(can_focus)
    GTK_WIDGET_SET_FLAGS(widget, GTK_CAN_FOCUS);
  else
    GTK_WIDGET_UNSET_FLAGS(widget, GTK_CAN_FOCUS);
}
#endif /* GTK < 2.18 */

void gtk_connect_signals(struct pbar_widget *w,
                         const struct pbar_gtk_signal *sig_list,
                         gpointer data)
{
  const struct pbar_gtk_signal *s = sig_list;
  for(; s->signal ; s++) {
    gulong handler_id = g_signal_connect(G_OBJECT(s->widget),
                                         s->signal,
                                         G_CALLBACK(s->callback),
                                         data);
    w->gtk_hnd  = g_list_append(w->gtk_hnd, GINT_TO_POINTER(handler_id));
    w->gtk_inst = g_list_append(w->gtk_inst, s->widget);
  }
}
