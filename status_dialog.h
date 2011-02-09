/* File: status_dialog.h
   Time-stamp: <2011-02-09 20:32:23 gawen>

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

#ifndef _STATUS_DIALOG_H_
#define _STATUS_DIALOG_H_

#include "common.h"

#include "gtk.h"

struct status_dialog {
  BEGIN_PBAR_WIDGET;

  /* window and list storage */
  GtkWidget *window;
  GtkListStore *list_store;

  /* global status for selection */
  GHashTable *global_status;
};

struct status_dialog * create_status_dialog();
void destroy_status_dialog(struct status_dialog *s_diag);
void init_status_dialog(struct status_dialog *s_diag);

#endif /* _STATUS_DIALOG_H_ */
