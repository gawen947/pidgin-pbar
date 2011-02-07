/* File: protocol_features.h
   Time-stamp: <2011-02-07 17:31:27 gawen>

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

#include "common.h"

#include "gtk.h"

struct protocol_features_dialog {
  BEGIN_PBAR_WIDGET;

  /* window and list storage */
  GtkWidget *window;
  GtkListStore *list_store;
};

struct protocol_features_dialog * create_protocol_features_dialog();
void destroy_protocol_features_dialog(struct protocol_features_dialog *f_diag);
void init_protocol_features_dialog(struct protocol_features_dialog *f_diag);

#endif /* _PROTOCOL_FEATURES_H_ */
