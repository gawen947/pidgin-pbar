/* File: mood_dialog.c
   Time-stamp: <2011-02-11 01:46:37 gawen>

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
#include "purple.h"
#include "preferences.h"
#include "mood_dialog.h"

static void cb_destroy_win(GtkWidget *widget, gpointer data);
static void cb_close_button(GtkWidget *widget, gpointer data);
static void cb_apply_button(GtkWidget *widget, gpointer date);
static void cb_refresh_button(GtkWidget *widget, gpointer data);
static void cb_row_activated(GtkWidget *widget,
                             GtkTreePath *path,
                             GtkTreeViewColumn *column,
                             gpointer data);

enum {
  MOODICON_COLUMN,
  MOOD_COLUMN,
  N_COLUMN
};

static void cb_destroy_win(GtkWidget *widget, gpointer data)
{ destroy_mood_dialog((struct mood_dialog *)data); }

static void cb_row_activated(GtkWidget *widget,
                             GtkTreePath *path,
                             GtkTreeViewColumn *column,
                             gpointer data)
{ cb_apply_button(widget, data); }

static void cb_apply_button(GtkWidget *widget, gpointer data)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *sel;
  struct mood_dialog *s_diag = (struct mood_dialog *)data;

  sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(s_diag->list_view));
  if(gtk_tree_selection_get_selected(sel, &model, &iter)) {
    const gchar *mood;
    gchar *name;

    gtk_tree_model_get(model, &iter, MOOD_COLUMN, &name, -1);
    mood = g_hash_table_lookup(s_diag->global_moods, name);

    if(mood) { /* mood found */
      gchar *path;

      destroy_mood_dialog(s_diag); /* destroy dialog first */

      set_status_with_mood_all(mood);
      purple_prefs_set_string(PREF "/mood", mood);
      path = get_mood_icon_path(mood);
      set_widget_mood(path);
      g_free(path);

      purple_debug_info(NAME, "mood changed to \"%s\" by user\n", mood);
    } else
      purple_debug_info(NAME, "selected mood \"%s\" doesn't exists\n", name);
    g_free(name);
  } else
    purple_debug_info(NAME, "no row selected\n");
}

static void cb_close_button(GtkWidget *widget, gpointer data)

{ destroy_mood_dialog((struct mood_dialog *)data); }

static void cb_refresh_button(GtkWidget *widget, gpointer data)
{
  struct mood_dialog *s_diag = (struct mood_dialog *)data;
  gtk_list_store_clear(s_diag->list_store);
  init_mood_dialog(s_diag);
}

struct mood_dialog * create_mood_dialog()
{
  struct mood_dialog *s_diag = g_malloc(sizeof(struct mood_dialog));
  memset(s_diag, 0, sizeof(struct mood_dialog));

  /* widgets that can be modifier along dialog lifetime */
  s_diag->window = pidgin_create_dialog(_("Mood selection"),
                                        PIDGIN_HIG_BORDER,
                                        "mood-select",
                                        TRUE);
  s_diag->list_store = gtk_list_store_new(N_COLUMN,
                                          GDK_TYPE_PIXBUF, /* MOODICON */
                                          G_TYPE_STRING    /* MOOD */ );
  s_diag->list_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(
                                                     s_diag->list_store));


  /* add main widgets */
  gtk_add_main_widget(PBAR_WIDGET(s_diag), s_diag->window);

  /* widgets that are not modified */
  GtkWidget *pbox = gtk_pidgin_dialog_box_new(_("Select your mood..."),
                                              _("This will change your mood for"
                                                " every account which supports"
                                                " it."),
                                              PIDGIN_STOCK_DIALOG_QUESTION);
  GtkWidget *vbox = pidgin_dialog_get_vbox_with_properties(GTK_DIALOG(
                                                             s_diag->window),
                                                           FALSE,
                                                           PIDGIN_HIG_BORDER);
  GtkWidget *hbox = pidgin_dialog_get_action_area(GTK_DIALOG(s_diag->window));
  GtkWidget *refresh_button = gtk_button_new_from_stock(GTK_STOCK_REFRESH);
  GtkWidget *close_button   = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  GtkWidget *apply_button   = gtk_button_new_from_stock(GTK_STOCK_APPLY);

  /* create view and model */
  GtkTreeViewColumn *p_col = gtk_tree_view_column_new();
  GtkCellRenderer *p_renderer;
  gtk_tree_view_column_set_title(p_col, _("Mood"));
  gtk_tree_view_append_column(GTK_TREE_VIEW(s_diag->list_view), p_col);
  p_renderer = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_column_pack_start(p_col, p_renderer, FALSE);
  gtk_tree_view_column_add_attribute(p_col, p_renderer,
                                     "pixbuf", MOODICON_COLUMN);
  p_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(p_col, p_renderer, TRUE);
  gtk_tree_view_column_add_attribute(p_col, p_renderer,
                                     "text", MOOD_COLUMN);

  /* pack widgets */
  gtk_box_pack_start(GTK_BOX(hbox), refresh_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), close_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), apply_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), pbox, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), s_diag->list_view, TRUE, TRUE, 0);

  /* gtk signals and callback */
  const struct pbar_gtk_signal g_signal_connections[] = {
    { s_diag->window, "destroy", cb_destroy_win },
    { s_diag->list_view, "row-activated", PBAR_GTK_CALLBACK(cb_row_activated) },
    { refresh_button, "clicked", cb_refresh_button },
    { apply_button, "clicked", cb_apply_button },
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

void destroy_mood_dialog(struct mood_dialog *s_diag)
{
  gtk_destroy(PBAR_WIDGET(s_diag));            /* destroy widgets */
  g_hash_table_destroy(s_diag->global_moods);  /* free global moods */
  g_free(s_diag);                              /* free dialog */
}

void init_mood_dialog(struct mood_dialog *s_diag)
{
  GtkTreeIter empty_iter;
  GdkPixbuf *empty_pixbuf;
  GtkWidget *empty_image;
  const gchar *empty_mood;
  gchar *empty_path;
  PurpleMood *mood = get_global_moods();

  if(!s_diag->global_moods)
    s_diag->global_moods = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                 NULL, NULL);

  /* add empty mood to mood list */
  empty_mood   = "";
  empty_path   = get_mood_icon_path(empty_mood);
  empty_image  = gtk_image_new_from_file(empty_path);
  empty_pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(empty_image));
  g_free(empty_path);
  gtk_list_store_append(s_diag->list_store, &empty_iter);
  gtk_list_store_set(s_diag->list_store, &empty_iter,
                     MOODICON_COLUMN, empty_pixbuf,
                     MOOD_COLUMN, _("None"),
                     -1);
  g_hash_table_insert(s_diag->global_moods, (gpointer)_("None"),
                      (gpointer)empty_mood);

  for(; mood->mood ; mood++) {
    if(!mood->mood || !mood->description)
      continue;
    GtkTreeIter iter;
    GtkWidget *image;
    GdkPixbuf *pixbuf;
    gchar *path = get_mood_icon_path(mood->mood);

    g_hash_table_insert(s_diag->global_moods, (gpointer)_(mood->description),
                        (gpointer)mood->mood);
    image  = gtk_image_new_from_file(path);
    pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
    g_free(path);

    gtk_list_store_append(s_diag->list_store, &iter);
    gtk_list_store_set(s_diag->list_store, &iter,
                       MOODICON_COLUMN, pixbuf,
                       MOOD_COLUMN, _(mood->description),
                       -1);
  }
}
