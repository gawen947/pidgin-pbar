/* File: actions.c
   Time-stamp: <2011-02-01 17:41:03 gawen>

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

static struct features_dialog {
  /* window and list storage */
  GtkWidget *window;
  GtkWidget *view;

  /* signals handlers and instance for disconnection */
  GList *gtk_hnd;
  GList *gtk_inst;
} *f_diag;

static void cb_destroy_win()
{
  destroy_features_dialog();
}

static void cb_close_button()
{
  destroy_features_dialog();
}

static void create_features_dialog()
{
  /* this should occurs each time but
     this way we avoid memory leaks */
  if(!f_diag)
    f_diag = g_malloc(sizeof(struct features_dialog));

  /* widgets that can possibly be modified along dialog lifetime */
  f_diag->window = pidgin_create_dialog(_("Supported features"),
                                        PIDGIN_HIG_BORDER,
                                        "supported features",
                                        TRUE);
  f_diag->view   = gtk_tree_view_new();

  /* widgets that are not modified */
  GtkWidget *refresh_button = gtk_button_new_from_stock(GTK_STOCK_REFRESH);
  GtkWidget *close_button   = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  GtkWidget *hbox           = gtk_hbox_new(FALSE, 10);
  GtkWidget *vbox           = gtk_vbox_new(FALSE, 10);

  /* create view and model */
  GtkCellRenderer *renderer;
  GtkTreeModel *model;

  const char *column_title[] = {
    N_("Protocol"),
    N_("Nickname"),
    N_("Buddy icon"),
    N_("Mood"),
    N_("Mood message"),
    N_("Song"),
    N_("Game"),
    N_("App."),
    NULL
  }; const char **ctitle = column_title;

  /* create columns */
  for(; ctitle ; ctitle++) {
    GtkTreeViewColumn *col = gtk_tree_view_column_new();

    gtk_tree_view_column_set_title(col, ctitle);
    gtk_tree_view_append_column(GTK_TREE_VIEW(f_diag->view), col);
  }

  /* pack widgets */
  gtk_box_pack_start(GTK_BOX(hbox), refresh_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), close_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), f_diag->features, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

  /* gtk signals and callback */
  const struct g_signal {
    GtkWidget *widget;
    const gchar *signal;
    void (*callback)(GtkWidget *, gpointer);
  } g_signal_connections[] = {
    { f_diag->window, "destroy", cb_destroy_win },
    { refresh_button, "clicked", cb_refresh_button },
    { close_button, "clicked", cb_close_button },
    { NULL, NULL, NULL }
  }; const struct g_signal *g_sig = g_signal_connections;

  /* connect signals and save handlers and instance when needed
     to disconnect those signals when the widget is destroyed */
  for(; g_sig->widget ; g_sig++) {
    gulong handler_id = g_signal_connect(G_OBJECT(g_sig->widget),
                                         g_sig->ignal,
                                         G_CALLBACK(g_sig->callback),
                                         NULL);
    f_diag->gtk_hnd = g_list_append(f_diag->gtk_hnd,GINT_TO_POINTER(handler_id));
    f_diag->gtk_inst = g_list_append(f_diag->gtk_inst, g_sig->widget);
  }

  /* show everything */
  gtk_widget_show(f_diag->window);
}

static void destroy_features_dialog()
{
  /* disconnect gtk signals */
  for(i = f_diag->gtk_hnd, j = f_diag->gtk_inst ; i && i = i->next ; j = j->next)
    g_signal_handler_disconnect(j->data, GPOINTER_TO_INT(i->data));
  g_list_free(f_diag->gtk_hnd);
  g_list_free(f_diag->gtk_inst);

  /* destroy widgets */
  l = gtk_container_get_children(GTK_CONTAINER(f_diag->window));
  for(i = l ; i ; i = i->next) {
    gtk_widget_destroy(i->data);
    i->data = NULL;
  }
  gtk_widget_destroy(f_diag->window);

  /* free dialog */
  g_free(f_diag);
  f_diag = NULL;
}

static void init_features_dialog()
{

}

static void action_features(PurplePluginAction *act)
{
}

GList * create_actions(PurplePlugin *plugin, gpointer ctx)
{
  GList *l = NULL;
  PurplePluginAction *act = NULL;

  act = purple_plugin_action_new(_("Supported features"), action_features);
  l = g_list_append(l, act);

  return l;
}


