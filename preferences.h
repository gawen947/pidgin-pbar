/* File: prefs.h
   Time-stamp: <2010-11-06 03:08:51 gawen>

   Copyright (C) 2010 David Hauweele <david.hauweele@gmail.com>
   Copyright (C) 2008,2009 Craig Harding <craigwharding@gmail.com>
                           Wolter Hellmund <wolterh@gmail.com>

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
#define PREF "/plugins/gtk/" NAME /* preference root */

/* empty messages to display at first installation */
#define EMPTY_PM   N_("<Enter personal message here>")
#define EMPTY_SM   N_("<Enter status message here>")
#define EMPTY_MM   N_("<Enter mood message here>")
#define EMPTY_SONG N_("<Enter song here>")
#define EMPTY_NAME N_("<Enter nickname here>")

/* justification */
enum { JUSTIFY_LEFT, JUSTIFY_CENTER, JUSTIFY_RIGHT };

/* pm type */
enum { PM_NONE,          /* 000 */
       PM_STATUS,        /* 001 */
       PM_MOOD,          /* 010 */
       PM_STATUSANDMOOD, /* 011 */
       PM_SONG,          /* 100 */
       PM_STATUSANDSONG, /* 101 */
       PM_MOODANDSONG,   /* 110 */
       PM_ALL,           /* 111 */
};

void init_prefs();
GtkWidget * get_config_frame(PurplePlugin *plugin);


#endif /* _PREFERENCES_H_ */
