/* File: actions.c
   Time-stamp: <2011-02-04 14:40:47 gawen>

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

#include "common.h"

#include "pbar.h"
#include "purple.h"
#include "protocol_features.h"
#include "acct_features.h"

static void protocol_features(PurplePluginAction *act)
{
  struct protocol_features_dialog *f_diag = create_protocol_features_dialog();
  init_protocol_features_dialog(f_diag);
}

static void acct_features(PurplePluginAction *act)
{
  struct acct_features_dialog *f_diag = create_acct_features_dialog();
  init_acct_features_dialog(f_diag);
}

GList * create_actions(PurplePlugin *plugin, gpointer ctx)
{
  GList *l = NULL;

  const struct action {
    const gchar *title;
    void (*action)(PurplePluginAction *);
  } actions[] = {
    { N_("Protocol features"), protocol_features },
    { N_("Account features"), acct_features },
    { NULL, NULL }
  }; const struct action *acts = actions;

  for(; acts->title ; acts++) {
    PurplePluginAction *act = purple_plugin_action_new(acts->title,
                                                       acts->action);
    l = g_list_append(l, act);
  }

  return l;
}
