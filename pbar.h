/* File: pbar.h
   Time-stamp: <2010-10-10 16:22:27 gawen>

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
#define NAME               "pbar"    /* internal name */
#define DISP_NAME          "PBar"    /* displayed name */
#define VERSION            "0.1-git" /* current version */

/* plugin information */
#define PLUGIN_ID          "gtk-" NAME   /* gtk plugin id */
#define PLUGIN_NAME        N_(DISP_NAME)
#ifndef COMMIT
# define PLUGIN_VERSION    VERSION
#else
# define PLUGIN_VERSION    VERSION " (commit:" COMMIT ")" /* add git commit
                                                             when available */
#endif
#define PLUGIN_SUMMARY     N_("Set alias and status globally")
#define PLUGIN_DESCRIPTION N_("Display a toolbar to set alias " \
                              "and status globally")
#define PLUGIN_AUTHOR      "David Hauweele <david.hauweele@gmail.com>"
#define PLUGIN_WEBSITE     "http://www.atlantysse.prout.be/~gawen/pidgin-pbar.html"

extern PurplePlugin *thisplugin;

#endif /* _PBAR_H_ */
