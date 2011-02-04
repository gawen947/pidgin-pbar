/* File: protocol-features.h
   Time-stamp: <2011-02-04 03:29:44 gawen>

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

#ifndef _PROTOCOL_FEATURES_H_
#define _PROTOCOL_FEATURES_H_

struct features_dialog {
  /* window and list storage */
  GtkWidget *window;
  GtkListStore *list_store;

  /* signals handlers and instance for disconnection */
  GList *gtk_hnd;
  GList *gtk_inst;
};

struct features_dialog * create_features_dialog();
void destroy_features_dialog(struct features_dialog *f_diag);
void init_features_dialog(struct features_dialog *f_diag);

#endif /* _PROTOCOL_FEATURES_H_ */
