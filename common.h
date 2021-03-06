/* File: common.h
   Time-stamp: <2012-04-05 13:00:29 gawen>

   Copyright (C) 2010 David Hauweele <david@hauweele.net>
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

#ifndef _COMMON_H_
#define _COMMON_H_

/* define for NLS */
#define GETTEXT_PACKAGE "pidgin-pbar"

/* common include for pidgin and gtk */

#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include "pidginstock.h"
#include "plugin.h"
#include "signals.h"
#include "gtkblist.h"
#include "gtkstatusbox.h"
#include "gtkprefs.h"
#include "prpl.h"
#include "request.h"
#include "debug.h"
#include "gtkutils.h"
#include "buddyicon.h"
#include "version.h"
#include "gtkplugin.h"

#ifdef _WIN32
# include "win32dep.h"
#endif /* _WIN32 */

#endif /* _COMMON_H_ */
