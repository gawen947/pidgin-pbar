/* File: gtk.c
   Time-stamp: <2011-02-11 01:29:58 gawen>

   Copyright (C) 2011 David Hauweele <david@hauweele.net>

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

void gtk_destroy(struct pbar_widget *w)
{
  GList *l, *i, *j;

  /* disconnect gtk signals */
  for(i = w->gtk_hnd, j = w->gtk_inst ; i && j ;
      i = i->next, j = j->next)
    g_signal_handler_disconnect(j->data, GPOINTER_TO_INT(i->data));
  g_list_free(w->gtk_hnd);
  g_list_free(w->gtk_inst);

  /* destroy widgets */
  for(j = w->main_widgets ; j ; j = j->next) {
    GtkWidget *main_widget = (GtkWidget *)j->data;

    l = gtk_container_get_children(GTK_CONTAINER(main_widget));
    for(i = l ; i ; i = i->next) {
      gtk_widget_destroy(i->data);
      i->data = NULL;
    }
    gtk_widget_destroy(main_widget);
  }
}

void gtk_add_main_widget(struct pbar_widget *w, GtkWidget *widget)
{ w->main_widgets = g_list_append(w->main_widgets, widget); }

GtkWidget * gtk_pidgin_dialog_box_new(const gchar *primary,
                                      const gchar *secondary,
                                      const gchar *icon)
{
  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *img;

  /* create initial pidgin box */
  hbox = gtk_hbox_new(FALSE, PIDGIN_HIG_BORDER);
  vbox = gtk_vbox_new(FALSE, PIDGIN_HIG_BORDER);

  /* create dialog icon */
  img = gtk_image_new_from_stock(icon,
                                 gtk_icon_size_from_name(
                                   PIDGIN_ICON_SIZE_TANGO_HUGE));
  gtk_misc_set_alignment(GTK_MISC(img), 0, 0);

  if(primary) { /* create and setup primary */
    GtkWidget *primary_label;
    gchar *primary_esc = g_markup_escape_text(primary, -1);
    gchar *label_text  = g_strdup_printf(
      "<span weight=\"bold\" size=\"larger\">%s</span>", primary_esc);
    primary_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(primary_label), label_text);
    gtk_label_set_line_wrap(GTK_LABEL(primary_label), TRUE);
    gtk_misc_set_alignment(GTK_MISC(primary_label), 0, 0);
    g_free(label_text);
    gtk_box_pack_start(GTK_BOX(vbox), primary_label, FALSE, FALSE, 0);
  }
  if(secondary) { /* create and setup secondary */
    GtkWidget *secondary_label;
    gchar *secondary_esc = g_markup_escape_text(secondary, -1);
    secondary_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(secondary_label), secondary_esc);
    g_free(secondary_esc);
    gtk_label_set_line_wrap(GTK_LABEL(secondary_label), TRUE);
    gtk_misc_set_alignment(GTK_MISC(secondary_label), 0, 0);
    gtk_box_pack_start(GTK_BOX(vbox), secondary_label, FALSE, FALSE, 0);
  }

  /* pack widgets into pidgin box */
  gtk_box_pack_start(GTK_BOX(hbox), img, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

  return hbox;
}
