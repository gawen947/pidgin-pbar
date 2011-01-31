/* File: actions.c
   Time-stamp: <2011-01-31 19:03:32 gawen>

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

void create_features_diag()
{}

GList * create_actions(PurplePlugin *plugin, gpointer ctx)
{
  GList *l = NULL;
  PurplePluginAction *act = NULL;

  act = purple_plugin_action_new(_("Supported features"), create_features_diag);
  l = g_list_append(l, act);

  return l;
}


