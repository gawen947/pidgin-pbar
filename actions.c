/* File: actions.c
   Time-stamp: <2011-02-01 01:55:17 gawen>

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
  GtkWidget *features;
} *f_diag;

static void create_features_dialog()
{
  /* this should occurs each time but
     this way we avoid memory leaks */
  if(!f_diag)
    f_diag = g_malloc(sizeof(struct features_dialog));

  /* widgets that can possibly be modified along dialog lifetime */

  /* widgets that are not modified */
  GtkWidget *refresh_button = gtk_button_new_from_stock(GTK_STOCK_REFRESH);
  GtkWidget *close_button   = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  GtkWidget *hbox           = gtk_hbox_new(FALSE, 10);
  GtkWidget *vbox           = gtk_vbox_new(FALSE, 10);

  /* setup widgets */

  /* pack widgets */
  gtk_box_pack_start(GTK_BOX(hbox), refresh_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), close_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), f_diag->features, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
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


