/* File: acct_features.h
   Time-stamp: <2011-02-07 17:31:32 gawen>

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

#ifndef _ACCT_FEATURES_H_
#define _ACCT_FEATURES_H_

#include "common.h"

#include "gtk.h"

struct acct_features_dialog {
  BEGIN_PBAR_WIDGET;

  /* window and list storage */
  GtkWidget *window;
  GtkListStore *list_store;
};

struct acct_features_dialog * create_acct_features_dialog();
void destroy_acct_features_dialog(struct acct_features_dialog *f_diag);
void init_acct_features_dialog(struct acct_features_dialog *f_diag);

#endif /* _ACCT_FEATURES_H_ */
