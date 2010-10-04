/* File: prefs.h
   Time-stamp: <2010-10-05 01:18:50 gawen>

   Copyright (C) 2010 David Hauweele <david.hauweele@gmail.com>

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

#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include "common.h"

#include "pbar.h"
#include "widget.h"

/* preferences */
#define PREF "/plugins/gtk/" NAME

/* empty messages */
#define EMPTY_PM   N_("<Enter personal message here>")
#define EMPTY_NAME N_("<Enter nickname here>")

void init_prefs();
GtkWidget * get_config_frame(PurplePlugin *plugin);


#endif /* _PREFERENCES_H_ */
