/* File: pbar.h
   Time-stamp: <2010-11-15 23:55:36 gawen>

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

#ifndef _PBAR_H_
#define _PBAR_H_

#include "common.h"

#ifndef G_GNUC_NULL_TERMINATED
#  if __GNUC__ >= 4
#    define G_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#  else
#    define G_GNUC_NULL_TERMINATED
#  endif /* __GNUC__ >= 4 */
#endif /* G_GNUC_NULL_TERMINATED */

/* name */
#define NAME               "pbar"     /* internal name */
#define DISP_NAME          N_("PBar") /* displayed name */
#define VERSION            "0.2-git"  /* current version */

/* plugin information */
#define PLUGIN_ID          "gtk-" NAME   /* gtk plugin id */
#define PLUGIN_NAME        DISP_NAME
#ifndef COMMIT
# define PLUGIN_VERSION    VERSION
#else
# define PLUGIN_VERSION    VERSION " (commit:" COMMIT ")" /* add git commit
                                                             when available */
#endif
#define PLUGIN_SUMMARY     N_("A toolbar to update some account settings "    \
                              "globally.")
#define PLUGIN_DESCRIPTION N_("Adds a toolbar to the buddy list to quickly "  \
                              "update nickname, personal message, icon, "     \
                              "status and mood. It also allows updating of "  \
                              "the current song and other parameters which "  \
                              "are updated globally on all accounts that "    \
                              "support them.")
#define PLUGIN_AUTHOR      "David Hauweele <david.hauweele@gmail.com>"
#define PLUGIN_WEBSITE     "http://www.atlantysse.prout.be/~gawen/pidgin-pbar.html"

extern PurplePlugin *thisplugin;

#endif /* _PBAR_H_ */
