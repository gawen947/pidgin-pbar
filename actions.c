/* File: actions.c
   Time-stamp: <2011-02-09 20:11:28 gawen>

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
#include "widget.h"
#include "purple.h"
#include "protocol_features.h"
#include "acct_features.h"
#include "status_dialog.h"

static void cb_icon_choose(const gchar *path, gpointer data)
{
  g_return_if_fail(path);

  purple_prefs_set_path(PIDGIN_PREFS_ROOT "/accounts/buddyicon", path);
}

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

static void change_nickname(PurplePluginAction *act)
{ create_name_dialog(); }

static void change_pm(PurplePluginAction *act)
{ create_pm_dialog(); }

static void change_status(PurplePluginAction *act)
{
  struct status_dialog *s_diag = create_status_dialog();
  init_status_dialog(s_diag);
}

static void change_mood(PurplePluginAction *act)
{
}

static void change_icon(PurplePluginAction *act)
{
  g_return_if_fail(bar->installed);

  PidginBuddyList *blist = pidgin_blist_get_default_gtk_blist();
  GtkWidget *chooser = pidgin_buddy_icon_chooser_new(GTK_WINDOW
                                                     (gtk_widget_get_toplevel
                                                      (GTK_WIDGET(blist))),
                                                     cb_icon_choose,
                                                     NULL);
  gtk_widget_show(chooser);
}

GList * create_actions(PurplePlugin *plugin, gpointer ctx)
{
  GList *l = NULL;

  const struct action {
    const gchar *title;
    void (*action)(PurplePluginAction *);
  } actions[] = {
    { N_("Change nickname"), change_nickname },
    { N_("Change personal message"), change_pm },
    { N_("Change status"), change_status },
    { N_("Change mood"), change_mood },
    { N_("Change icon"), change_icon },
    { N_("Protocol features"), protocol_features },
    { N_("Account features"), acct_features },
    { NULL, NULL }
  }; const struct action *acts = actions;

  for(; acts->title ; acts++) {
    PurplePluginAction *act = purple_plugin_action_new(_(acts->title),
                                                       acts->action);
    l = g_list_append(l, act);
  }

  return l;
}
