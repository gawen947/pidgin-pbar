/* File: prefs.h
   Time-stamp: <2010-11-09 21:47:25 gawen>

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
#define EMPTY_NAME    N_("<Enter nickname here>")
#define EMPTY_PM      N_("<Enter personal message here>")
#define EMPTY_TITLE   N_("<Enter song title here>")
#define EMPTY_ARTIST  N_("<Enter song artist here>")
#define EMPTY_ALBUM   N_("<Enter song album here>")
#define EMPTY_GENRE   N_("<Enter song genre here>")
#define EMPTY_COMMENT N_("<Enter song comment here>")
#define EMPTY_TRACK   N_("<Enter song track here>")
#define EMPTY_TIME    N_("<Enter song time here>")
#define EMPTY_YEAR    N_("<Enter song year here>")
#define EMPTY_URL     N_("<Enter song URL here>")
#define EMPTY_FULL    N_("<Enter full song here>")
#define EMPTY_GAME    N_("<Enter game name here>")
#define EMPTY_OFFICE  N_("<Enter office app. here")
#define EMPTY_MOOD    N_("<Enter mood message here>")

/* justification */
enum { JUSTIFY_LEFT, JUSTIFY_CENTER, JUSTIFY_RIGHT };

/* pm type */
enum { PM_NONE,
       PM_STATUS,
       PM_MOOD,
       PM_SONG,
       PM_ALL = (PM_STATUS & PM_MOOD & PM_SONG) };

void init_prefs();
GtkWidget * get_config_frame(PurplePlugin *plugin);


#endif /* _PREFERENCES_H_ */
