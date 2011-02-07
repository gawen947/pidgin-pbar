/* File: purple.h
   Time-stamp: <2011-02-07 20:12:06 gawen>

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

#ifndef _PURPLE_H_
#define _PURPLE_H_

#include "common.h"

#include "gtk.h"

struct pbar_prpl_signal {
  void *instance;
  const char *signal;
  void *callback;
};

/* not sure purple define that */
#ifndef PURPLE_PREFS_CALLBACK
# define PURPLE_PREFS_CALLBACK(func) ((PurplePrefCallback)func)
#endif

gboolean is_gtk_blist_created();
GdkPixbuf * create_prpl_icon_from_info(PurplePluginProtocolInfo *prpl_info,
                                       PidginPrplIconSize size);
GdkPixbuf * get_buddy_icon();
GdkPixbuf * get_buddy_icon_hover();
GHashTable * get_protocol_attrs(PurplePluginProtocolInfo *protocol);
GHashTable * get_account_attrs(PurpleAccount *account);
const gchar * get_status_stock_id();
gchar * get_mood_icon_path();
PurpleMood * get_global_moods();

void set_status_message(const gchar *pm);
void set_status_all(const gchar *status_id, GList *attrs);
void set_status_with_mood(PurpleAccount *account, const gchar *mood);
void set_display_name(PurpleAccount *account, const gchar *name);
void set_display_name_all(const gchar *name);
void prpl_disconnect_signals(struct pbar_widget *w);
void prpl_prefs_disconnect_signals(struct pbar_widget *w);
void prpl_connect_signals(struct pbar_widget *w,
                          const struct pbar_prpl_signal *sig_list,
                          gpointer data);
void prpl_prefs_connect_signals(struct pbar_widget *w,
                                const struct pbar_prpl_signal *sig_list,
                                gpointer data);

#endif /* _PURPLE_H_ */
