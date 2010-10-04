/* File: pbar.c
   Time-stamp: <2010-10-04 20:24:49 gawen>

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

#define PURPLE_PLUGINS

#include "common.h"

#include "pbar.h"
#include "preferences.h"
#include "widget.h"

static gboolean plugin_load(PurplePlugin *plugin);
static gboolean plugin_unload(PurplePlugin *plugin);

static PidginPluginUiInfo ui_info = { get_config_frame };

static PurplePluginInfo info = {
  PURPLE_PLUGIN_MAGIC,
  PURPLE_MAJOR_VERSION,
  PURPLE_MINOR_VERSION,
  PURPLE_PLUGIN_STANDARD,
  PIDGIN_PLUGIN_TYPE,
  0,
  NULL,
  PURPLE_PRIORITY_DEFAULT,
  PLUGIN_ID,
  PLUGIN_NAME,
  PLUGIN_VERSION,
  PLUGIN_SUMMARY,
  PLUGIN_DESCRIPTION,
  PLUGIN_AUTHOR,
  PLUGIN_WEBSITE,
  plugin_load,
  plugin_unload,
  NULL,
  &ui_info,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

PurplePlugin *thisplugin;

static gboolean is_gtk_blist_created()
{
  const PidginBuddyList *blist = pidgin_blist_get_default_gtk_blist();

  if(!blist ||
     !blist->vbox ||
     !gtk_widget_get_visible(blist->vbox))
    return FALSE;
  return TRUE;
}

static void cb_blist_created()
{
  /* create widget and
     load preferences */
  create_widget();
  init_widget();
}

static gboolean plugin_load(PurplePlugin *plugin)
{
  if(is_gtk_blist_created()) {
    /* create widget and
       load preferences */
    create_widget();
    init_widget();
  }
  else
    /* connect construction signal */
    purple_signal_connect(pidgin_blist_get_handle(),
                          "gtkblist-created",
                          plugin,
                          PURPLE_CALLBACK(cb_blist_created),
                          NULL);

  purple_debug_info(NAME,"plugin initialized...\n");

  return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin)
{
  destroy_widget();

  /* restore statusbox */
  set_statusbox_visible(TRUE);

  purple_debug_info(NAME,"plugin destroyed...\n");

  return TRUE;
}

static void init_plugin(PurplePlugin *plugin)
{
  thisplugin = plugin;

  /* load or create preferences */
  init_prefs();
}

PURPLE_INIT_PLUGIN(pbar, init_plugin, info)
