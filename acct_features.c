/* File: acct_features.c
   Time-stamp: <2011-02-07 17:29:51 gawen>

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

#include "pbar.h"
#include "purple.h"
#include "gtk.h"
#include "acct_features.h"

static void cb_destroy_win(GtkWidget *widget, gpointer data);
static void cb_close_button(GtkWidget *widget, gpointer data);
static void cb_refresh_button(GtkWidget *widget, gpointer data);

enum {
  ACCTICON_COLUMN,
  ACCT_COLUMN,
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

static void cb_destroy_win(GtkWidget *widget, gpointer data)
{ destroy_acct_features_dialog((struct acct_features_dialog *)data); }

static void cb_close_button(GtkWidget *widget, gpointer data)
{ destroy_acct_features_dialog((struct acct_features_dialog *)data); }

static void cb_refresh_button(GtkWidget *widget, gpointer data)
{
  struct acct_features_dialog *f_diag = (struct acct_features_dialog *)data;
  gtk_list_store_clear(f_diag->list_store);
  init_acct_features_dialog(f_diag);
}

struct acct_features_dialog * create_acct_features_dialog()
{
  struct acct_features_dialog *f_diag = g_malloc(sizeof(struct acct_features_dialog));
  memset(f_diag, 0, sizeof(struct acct_features_dialog));

  /* widgets that can possibly be modified along dialog lifetime */
  f_diag->window = pidgin_create_dialog(_("Account features"),
                                        PIDGIN_HIG_BORDER,
                                        "acct-features",
                                        TRUE);
  f_diag->list_store = gtk_list_store_new(N_COLUMN,
                                          GDK_TYPE_PIXBUF, /* ACCTICON */
                                          G_TYPE_STRING,   /* ACCT */
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
  GtkTreeViewColumn *p_col = gtk_tree_view_column_new();
  GtkCellRenderer *p_renderer;
  gtk_tree_view_column_set_title(p_col, _("Account"));
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), p_col);
  p_renderer = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_column_pack_start(p_col, p_renderer, FALSE);
  gtk_tree_view_column_add_attribute(p_col, p_renderer,
                                     "pixbuf", ACCTICON_COLUMN);
  p_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(p_col, p_renderer, TRUE);
  gtk_tree_view_column_add_attribute(p_col, p_renderer,
                                     "text", ACCT_COLUMN);
  const struct g_column {
    const gchar *title;                          /* column title */
    const gchar *attr_type;                      /* column type attribute */
    GtkCellRenderer *(*gtk_cell_renderer_new)(); /* gtk cell renderer creation */
    guint position;                              /* column position */
  } columns[] = {
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
  const struct pbar_gtk_signal g_signal_connections[] = {
    { f_diag->window, "destroy", cb_destroy_win },
    { refresh_button, "clicked", cb_refresh_button },
    { close_button, "clicked", cb_close_button },
    { NULL, NULL, NULL }
  };

  /* connect signals and save handlers and instance when needed
     to disconnect those signals when the widget is destroyed */
  gtk_connect_signals(PBAR_WIDGET(f_diag), g_signal_connections, f_diag);

  /* show everything */
  gtk_widget_show_all(f_diag->window);
  gtk_window_present(GTK_WINDOW(f_diag->window));

  return f_diag;
}

void destroy_acct_features_dialog(struct acct_features_dialog *f_diag)
{
  GList *l, *i, *j;

  /* disconnect gtk signals */
  for(i = f_diag->gtk_hnd, j = f_diag->gtk_inst ; i && j ;
      i = i->next, j = j->next)
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
}

void init_acct_features_dialog(struct acct_features_dialog *f_diag)
{
  GList *a = purple_accounts_get_all_active();
  /* TODO: should be freed ? */
  GdkPixbuf *yes = gtk_widget_render_icon(f_diag->window, GTK_STOCK_YES,
                                          GTK_ICON_SIZE_MENU, NULL);
  GdkPixbuf *no  = gtk_widget_render_icon(f_diag->window, GTK_STOCK_NO,
                                          GTK_ICON_SIZE_MENU, NULL);

  /* last line summarize all available features */
  GtkTreeIter e_iter;
  GdkPixbuf *e_icon     = gtk_widget_render_icon(f_diag->window,
                                                 PIDGIN_STOCK_INFO,
                                                 GTK_ICON_SIZE_MENU,
                                                 NULL);
  GdkPixbuf *e_pm        = no;
  GdkPixbuf *e_buddyicon = no;
  GdkPixbuf *e_nickname  = no;
  GdkPixbuf *e_mood      = no;
  GdkPixbuf *e_moodmsg   = no;
  GdkPixbuf *e_game      = no;
  GdkPixbuf *e_app       = no;
  GdkPixbuf *e_tune      = no;

  for(; a ; a = a->next) {
    PurpleAccount *acct  = (PurpleAccount *)a->data;
    PurplePlugin *plugin = purple_find_prpl(acct->protocol_id);
    PurplePluginProtocolInfo *protocol = PURPLE_PLUGIN_PROTOCOL_INFO(plugin);

    GtkTreeIter iter;
    gchar *username = g_strdup_printf("%s (%s)",
                                           purple_account_get_username(acct),
                                           purple_account_get_protocol_name(acct));
    GdkPixbuf *a_icon = pidgin_create_prpl_icon(acct, PIDGIN_PRPL_ICON_MEDIUM);
    GHashTable *attrs  = get_account_attrs(acct);

    GdkPixbuf *nickname, *mood, *moodmsg, *game, *app, *tune, *pm, *buddyicon;

    if(g_hash_table_lookup(attrs, "mood"))
      e_mood = mood = yes;
    else
      mood = no;
    if(g_hash_table_lookup(attrs, "moodtext"))
      e_moodmsg = moodmsg = yes;
    else
      moodmsg = no;
    if(g_hash_table_lookup(attrs, "game"))
      e_game = game = yes;
    else
      game = no;
    if(g_hash_table_lookup(attrs, "office"))
      e_app = app = yes;
    else
      app = no;
    if((g_hash_table_lookup(attrs, "tune_title") &&
        g_hash_table_lookup(attrs, "tune_artist") &&
        g_hash_table_lookup(attrs, "tune_album")))
      e_tune = tune = yes;
    else
      tune = no;
    g_hash_table_destroy(attrs);

    if(protocol->set_status)
      e_pm = pm = yes;
    else
      pm = no;
    if(protocol->set_buddy_icon)
      e_buddyicon = buddyicon = yes;
    else
      buddyicon = no;
    /* exception for XMPP
       nickname supported
       manually
       FIXME: however some XMPP account don't support nickname extension */
    if(!strcmp(acct->protocol_id, "prpl-jabber") || protocol->set_public_alias)
      e_nickname = nickname = yes;
    else
      nickname = no;

    gtk_list_store_append(f_diag->list_store, &iter);
    gtk_list_store_set(f_diag->list_store, &iter,
                       ACCT_COLUMN, username,
                       ACCTICON_COLUMN, a_icon,
                       NICKNAME_COLUMN, nickname,
                       PM_COLUMN, pm,
                       ICON_COLUMN, buddyicon,
                       MOOD_COLUMN, mood,
                       MOODMSG_COLUMN, moodmsg,
                       TUNE_COLUMN, tune,
                       GAME_COLUMN, game,
                       APP_COLUMN, app,
                       -1);
    g_free(username);
  }

  /* last line summarize all available features */
  gtk_list_store_append(f_diag->list_store, &e_iter);
  gtk_list_store_set(f_diag->list_store, &e_iter,
                     ACCT_COLUMN, "Available features",
                     ACCTICON_COLUMN, e_icon,
                     NICKNAME_COLUMN, e_nickname,
                     PM_COLUMN, e_pm,
                     ICON_COLUMN, e_buddyicon,
                     MOOD_COLUMN, e_mood,
                     MOODMSG_COLUMN, e_moodmsg,
                     TUNE_COLUMN, e_tune,
                     GAME_COLUMN, e_game,
                     APP_COLUMN, e_app,
                     -1);
}
