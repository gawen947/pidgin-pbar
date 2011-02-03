/* File: actions.c
   Time-stamp: <2011-02-03 03:04:44 gawen>

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

static void cb_destroy_win();
static void cb_close_button();
static void destroy_features_dialog();
static void init_features_dialog();
static void init_features_dialog();
static void action_features(PurplePluginAction *act);

enum {
  PROTOCOL_COLUMN,
  NICKNAME_COLUMN,
  PM_COLUMN,
  ICON_COLUMN,
  MOOD_COLUMN,
  MOODMSG_COLUMN,
  TUNE_COLUMN,
  GAME_COLUMN,
  APP_COLUMN,
  N_COLUMN
};

static struct features_dialog {
  /* window and list storage */
  GtkWidget *window;
  GtkListStore *list_store;

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

static void cb_refresh_button() {}

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
  f_diag->list_store = gtk_list_store_new(N_COLUMN,
                                          G_TYPE_STRING,   /* PROTOCOL */
                                          GDK_TYPE_PIXBUF, /* NICKNAME */
                                          GDK_TYPE_PIXBUF, /* PM */
                                          GDK_TYPE_PIXBUF, /* ICON */
                                          GDK_TYPE_PIXBUF, /* MOOD */
                                          GDK_TYPE_PIXBUF, /* MOODMSG */
                                          GDK_TYPE_PIXBUF, /* TUNE */
                                          GDK_TYPE_PIXBUF, /* GAME */
                                          GDK_TYPE_PIXBUF  /* APP */ );

  /* widgets that are not modified */
  GtkWidget *vbox = pidgin_dialog_get_vbox_with_properties(GTK_DIALOG(f_diag->window),
                                                           FALSE,
                                                           PIDGIN_HIG_BORDER);
  GtkWidget *hbox = pidgin_dialog_get_action_area(GTK_DIALOG(f_diag->window));
  GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(f_diag->list_store));
  GtkWidget *refresh_button = gtk_button_new_from_stock(GTK_STOCK_REFRESH);
  GtkWidget *close_button   = gtk_button_new_from_stock(GTK_STOCK_CLOSE);

  /* create view and model */
  const struct g_column {
    const gchar *title;                          /* column title */
    const gchar *attr_type;                      /* column type attribute */
    GtkCellRenderer *(*gtk_cell_renderer_new)(); /* gtk cell renderer creation */
    guint position;                              /* column position */
  } columns[] = {
    { N_("Protocol"), "text", gtk_cell_renderer_text_new, PROTOCOL_COLUMN },
    { N_("Nickname"), "pixbuf", gtk_cell_renderer_pixbuf_new, NICKNAME_COLUMN },
    { N_("Status message"), "pixbuf", gtk_cell_renderer_pixbuf_new, PM_COLUMN },
    { N_("Buddy icon"), "pixbuf", gtk_cell_renderer_pixbuf_new, ICON_COLUMN },
    { N_("Mood"), "pixbuf", gtk_cell_renderer_pixbuf_new, MOOD_COLUMN },
    { N_("Mood message"), "pixbuf", gtk_cell_renderer_pixbuf_new, MOODMSG_COLUMN },
    { N_("Tune"), "pixbuf", gtk_cell_renderer_pixbuf_new, TUNE_COLUMN },
    { N_("Game"), "pixbuf", gtk_cell_renderer_pixbuf_new, GAME_COLUMN },
    { N_("App."), "pixbuf", gtk_cell_renderer_pixbuf_new, APP_COLUMN },
    { NULL, NULL, NULL, 0 }
  }; const struct g_column *col = columns;

  /* create columns */
  for(; col->title ; col++) {
    GtkCellRenderer *renderer = col->gtk_cell_renderer_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(_(col->title),
                                                                         renderer,
                                                                         col->attr_type,
                                                                         col->position,
                                                                         NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
  }

  /* pack widgets */
  gtk_box_pack_start(GTK_BOX(hbox), refresh_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), close_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), view, TRUE, TRUE, 0);

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
                                         g_sig->signal,
                                         G_CALLBACK(g_sig->callback),
                                         NULL);
    f_diag->gtk_hnd = g_list_append(f_diag->gtk_hnd,GINT_TO_POINTER(handler_id));
    f_diag->gtk_inst = g_list_append(f_diag->gtk_inst, g_sig->widget);
  }

  /* show everything */
  gtk_widget_show_all(f_diag->window);
  gtk_window_present(GTK_WINDOW(f_diag->window));
}

static void destroy_features_dialog()
{
  GList *l, *i, *j;

  /* disconnect gtk signals */
  for(i = f_diag->gtk_hnd, j = f_diag->gtk_inst ; i && j ; i = i->next, j = j->next)
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
  GList *p = purple_plugins_get_protocols();
  GtkWidget *w_yes = gtk_image_new_from_stock(GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
  GtkWidget *w_no  = gtk_image_new_from_stock(GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
  GdkPixbuf *yes = gtk_widget_render_icon(f_diag->window, GTK_STOCK_YES,
                                          GTK_ICON_SIZE_MENU, NULL);
  GdkPixbuf *no  = gtk_widget_render_icon(f_diag->window, GTK_STOCK_NO,
                                          GTK_ICON_SIZE_MENU, NULL);

  for(; p ; p = p->next) {
    PurplePlugin *plugin = p->data;
    PurplePluginInfo *info = plugin->info;
    PurplePluginProtocolInfo *protocol = PURPLE_PLUGIN_PROTOCOL_INFO(plugin);

    if(info && info->name) {
      GtkTreeIter iter;
      /* TODO: exception for XMPP */
      gtk_list_store_append(f_diag->list_store, &iter);
      gtk_list_store_set(f_diag->list_store, &iter,
                         PROTOCOL_COLUMN, info->name,
                         NICKNAME_COLUMN, protocol->set_public_alias ? yes : no,
                         PM_COLUMN, protocol->set_status ? yes : no,
                         ICON_COLUMN, yes,
                         MOOD_COLUMN, yes,
                         MOODMSG_COLUMN, yes,
                         TUNE_COLUMN, yes,
                         GAME_COLUMN, yes,
                         APP_COLUMN, yes,
                         -1);
    }
  }
}

static void action_features(PurplePluginAction *act)
{
  create_features_dialog();
  init_features_dialog();
}

GList * create_actions(PurplePlugin *plugin, gpointer ctx)
{
  GList *l = NULL;
  PurplePluginAction *act = NULL;

  act = purple_plugin_action_new(_("Supported features"), action_features);
  l = g_list_append(l, act);

  return l;
}


