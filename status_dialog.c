/* File: status_dialog.c
   Time-stamp: <2011-02-09 20:09:53 gawen>

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
#include "status_dialog.h"

static void cb_destroy_win(GtkWidget *widget, gpointer data);
static void cb_close_button(GtkWidget *widget, gpointer data);
static void cb_apply_button(GtkWidget *widget, gpointer date);
static void cb_refresh_button(GtkWidget *widget, gpointer data);

enum {
  STATUSICON_COLUMN,
  STATUS_COLUMN,
  N_COLUMN
};

static void cb_destroy_win(GtkWidget *widget, gpointer data)
{ destroy_status_dialog((struct status_dialog *)data); }

static void cb_apply_button(GtkWidget *widget, gpointer data)
{
}

static void cb_close_button(GtkWidget *widget, gpointer data)
{ destroy_status_dialog((struct status_dialog *)data); }

static void cb_refresh_button(GtkWidget *widget, gpointer data)
{
  struct status_dialog *s_diag = (struct status_dialog *)data;
  gtk_list_store_clear(s_diag->list_store);
  init_status_dialog(s_diag);
}

struct status_dialog * create_status_dialog()
{
  struct status_dialog *s_diag = g_malloc(sizeof(struct status_dialog));
  memset(s_diag, 0, sizeof(struct status_dialog));

  /* widgets that can be modifier along dialog lifetime */
  s_diag->window = pidgin_create_dialog(_("Status selection"),
                                        PIDGIN_HIG_BORDER,
                                        "status-select",
                                        TRUE);
  s_diag->list_store = gtk_list_store_new(N_COLUMN,
                                          GDK_TYPE_PIXBUF, /* STATUSICON */
                                          G_TYPE_STRING   /* STATUS */ );

  /* add main widgets */
  gtk_add_main_widget(PBAR_WIDGET(s_diag), s_diag->window);

  /* widgets that are not modified */
  GtkWidget *vbox = pidgin_dialog_get_vbox_with_properties(GTK_DIALOG(s_diag->window),
                                                           FALSE,
                                                           PIDGIN_HIG_BORDER);
  GtkWidget *hbox = pidgin_dialog_get_action_area(GTK_DIALOG(s_diag->window));
  GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(s_diag->list_store));
  GtkWidget *refresh_button = gtk_button_new_from_stock(GTK_STOCK_REFRESH);
  GtkWidget *close_button   = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  GtkWidget *apply_button   = gtk_button_new_from_stock(GTK_STOCK_APPLY);

  /* create view and model */
  GtkTreeViewColumn *p_col = gtk_tree_view_column_new();
  GtkCellRenderer *p_renderer;
  gtk_tree_view_column_set_title(p_col, _("Status"));
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), p_col);
  p_renderer = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_column_pack_start(p_col, p_renderer, FALSE);
  gtk_tree_view_column_add_attribute(p_col, p_renderer,
                                     "pixbuf", STATUSICON_COLUMN);
  p_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(p_col, p_renderer, TRUE);
  gtk_tree_view_column_add_attribute(p_col, p_renderer,
                                     "text", STATUS_COLUMN);

  /* pack widgets */
  gtk_box_pack_start(GTK_BOX(hbox), refresh_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), close_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), apply_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), view, TRUE, TRUE, 0);

  /* gtk signals and callback */
  const struct pbar_gtk_signal g_signal_connections[] = {
    { s_diag->window, "destroy", cb_destroy_win },
    { refresh_button, "clicked", cb_refresh_button },
    { close_button, "clicked", cb_close_button },
    { NULL, NULL, NULL }
  };

  /* connect signals and save handlers and instance when needed
     to disconnect those signals when the widget is destroyed */
  gtk_connect_signals(PBAR_WIDGET(s_diag), g_signal_connections, s_diag);

  /* show everything */
  gtk_widget_show_all(s_diag->window);
  gtk_window_present(GTK_WINDOW(s_diag->window));

  return s_diag;
}

void destroy_status_dialog(struct status_dialog *s_diag)
{
  gtk_destroy(PBAR_WIDGET(s_diag)); /* destroy widgets */
  g_free(s_diag);                   /* free dialog */
}

void init_status_dialog(struct status_dialog *s_diag)
{
  GList *a = purple_accounts_get_all_active();
  GHashTable *global_status = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                    NULL, NULL);

  for(; a ; a = g_list_delete_link(a, a)) {
    PurpleAccount *acct  = (PurpleAccount *)a->data;
    GList *types = purple_account_get_status_types(acct);

    for(; types ; types = types->next) {
      GtkTreeIter iter;
      GdkPixbuf *icon;
      PurpleStatusType *type = (PurpleStatusType *)types->data;
      PurpleStatusPrimitive prim;
      const gchar *stock, *status_name;

      if(!purple_status_type_is_user_settable(type) ||
         purple_status_type_is_independent(type))
        continue;

      prim  = purple_status_type_get_primitive(type);
      stock = pidgin_stock_id_from_status_primitive(prim);

      if(g_hash_table_lookup(global_status, stock))
        continue;
      else
        g_hash_table_insert(global_status, (gpointer)stock,
                            GINT_TO_POINTER(TRUE));
      status_name = purple_status_type_get_name(type);
      icon        = gtk_widget_render_icon(s_diag->window, stock,
                                           GTK_ICON_SIZE_MENU, NULL);
      gtk_list_store_append(s_diag->list_store, &iter);
      gtk_list_store_set(s_diag->list_store, &iter,
                         STATUSICON_COLUMN, icon,
                         STATUS_COLUMN, status_name,
                         -1);
    }
  }
}
