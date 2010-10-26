/* File: purple.c
   Time-stamp: <2010-10-26 15:54:38 gawen>

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
/* this file contains common functions for libpurple and pidgin */

#include "common.h"

#include "pbar.h"
#include "purple.h"

/* callbacks */
static void cb_global_moods_for_each(gpointer key, gpointer value,
                                     gpointer user_data);
static void cb_set_alias_failure(PurpleAccount *account, const char *error);
static void cb_dummy();

/* check if default gtk blist is created */
gboolean is_gtk_blist_created()
{
  const PidginBuddyList *blist;

  blist = pidgin_blist_get_default_gtk_blist();

  if(!blist ||
     !blist->vbox ||
     !gtk_widget_get_visible(blist->vbox))
    return FALSE;
  return TRUE;
}

/* get buddy icon from statusbox widget */
GdkPixbuf * get_buddy_icon()
{
  const PidginBuddyList *blist;
  const PidginStatusBox *statusbox;

  blist = pidgin_blist_get_default_gtk_blist();
  statusbox = PIDGIN_STATUS_BOX(blist->statusbox);
  return statusbox->buddy_icon;
}

/* get buddy icon hovered from statusbox widget */
GdkPixbuf * get_buddy_icon_hover()
{
  const PidginBuddyList *blist;
  const PidginStatusBox *statusbox;

  blist = pidgin_blist_get_default_gtk_blist();
  statusbox = PIDGIN_STATUS_BOX(blist->statusbox);
  return statusbox->buddy_icon_hover;
}

/* get current status stock id */
const gchar * get_status_stock_id()
{
  const PurpleSavedStatus *status;
  PurpleStatusPrimitive prim;

  status = purple_savedstatus_get_current();
  prim   = purple_savedstatus_get_type(status);
  return pidgin_stock_id_from_status_primitive(prim);
}

/* get mood icon path */
gchar * get_mood_icon_path(const gchar *mood)
{
  gchar *path;

  /* FIXME: choose something else for "None" mood */
  if(!mood)
    path = g_build_filename(DATADIR, "pixmaps", "pidgin",
                            "toolbar", "16", "emote-select.png", NULL);
  else if(!strcmp(mood, "busy"))
    path = g_build_filename(DATADIR, "pixmaps", "pidgin",
                            "status", "16", "busy.png", NULL);
  else if(!strcmp(mood, "hiptop"))
    path = g_build_filename(DATADIR, "pixmaps", "pidgin",
                            "emblems", "16", "hiptop.png", NULL);
  else {
    gchar *filename = g_strdup_printf("%s.png", mood);
    path = g_build_filename(DATADIR, "pixmaps", "pidgin",
                            "emotes", "small", filename, NULL);
    g_free(filename);
  }

  return path;
}

/* get current set mood for all mood-supporting accounts, or NULL if not set or
   not set to the same on all */
const gchar * get_global_mood_status()
{
  GList *accounts = purple_accounts_get_all_active();
  const gchar *found_mood = NULL;

  for(; accounts ; accounts = g_list_delete_link(accounts, accounts)) {
    PurpleAccount *account = (PurpleAccount *)accounts->data;

    if(purple_account_is_connected(account) &&
       (purple_account_get_connection(account)->flags &
        PURPLE_CONNECTION_SUPPORT_MOODS)) {
      const gchar *curr_mood;
      PurplePresence *presence;
      PurpleStatus *status;
      presence  = purple_account_get_presence(account);
      status    = purple_presence_get_status(presence, "mood");
      curr_mood = purple_status_get_attr_string(status, PURPLE_MOOD_NAME);

      if(found_mood && !purple_strequal(curr_mood, found_mood)) {
        /* found a different mood */
        found_mood = NULL;
        break;
      }
      else
        found_mood = curr_mood;
    }
  }

  return found_mood;
}

/* get global moods */
PurpleMood * get_global_moods()
{
  GHashTable *global_moods = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                   NULL, NULL);
  GHashTable *mood_counts  = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                   NULL, NULL);

  GList *accounts = purple_accounts_get_all_active();
  PurpleMood *result = NULL;
  GList *out_moods = NULL;
  int i = 0;
  int num_accounts = 0;

  for(; accounts ; accounts = g_list_delete_link(accounts, accounts)) {
    PurpleAccount *account = (PurpleAccount *)accounts->data;
    if(purple_account_is_connected(account)) {
      PurpleConnection *gc = purple_account_get_connection(account);
      if(gc->flags & PURPLE_CONNECTION_SUPPORT_MOODS) {
        PurplePluginProtocolInfo *prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
        PurpleMood *mood = NULL;

        for(mood = prpl_info->get_moods(account) ;
            mood->mood ; mood++) {
          int mood_count = GPOINTER_TO_INT(g_hash_table_lookup(mood_counts,
                                                               mood->mood));

          if(!g_hash_table_lookup(global_moods, mood->mood)) {
            g_hash_table_insert(global_moods, (gpointer)mood->mood, mood);
          }
          g_hash_table_insert(mood_counts, (gpointer)mood->mood, GINT_TO_POINTER(mood_count + 1));
        }

        num_accounts++;
      }
    }
  }

  g_hash_table_foreach(global_moods, cb_global_moods_for_each, &out_moods);
  result = g_new0(PurpleMood, g_hash_table_size(global_moods) + 1);

  while(out_moods) {
    PurpleMood *mood = (PurpleMood *)out_moods->data;
    int in_num_accounts = GPOINTER_TO_INT(g_hash_table_lookup(mood_counts,
                                                              mood->mood));

    if(in_num_accounts == num_accounts) {
      /* mood is present in all accounts supporting moods */
      result[i].mood = mood->mood;
      result[i].description = mood->description;
      i++;
    }
    out_moods = g_list_delete_link(out_moods, out_moods);
  }

  g_hash_table_destroy(global_moods);
  g_hash_table_destroy(mood_counts);

  return result;
}

/* set display name for account */
void set_display_name(PurpleAccount *account, const gchar *name)
{
  const gchar *id;

  id = purple_account_get_protocol_id(account);

  /* exception for set_public_alias */
  if(!strcmp(id, "prpl-jabber")) {
    PurpleConnection *gc;
    gchar *iq_id;
    xmlnode *iq, *pubsub, *publish, *nicknode;

    gc = account->gc;
    iq_id = g_strdup_printf("purple%x", g_random_int());
    iq = xmlnode_new("iq");
    xmlnode_set_attrib(iq, "type", "set");
    xmlnode_set_attrib(iq, "id", iq_id);

    pubsub = xmlnode_new("pubsub");
    xmlnode_set_attrib(pubsub, "xmlns", "http://jabber.org/protocol/pubsub");
    publish = xmlnode_new("publish");
    xmlnode_set_attrib(publish,"node","http://jabber.org/protocol/nick");
    nicknode = xmlnode_new_child(xmlnode_new_child(publish, "item"), "nick");
    xmlnode_set_namespace(nicknode, "http://jabber.org/protocol/nick");
    xmlnode_insert_data(nicknode, name, -1);
    xmlnode_insert_child(pubsub, publish);
    xmlnode_insert_child(iq, pubsub);

    purple_signal_emit(purple_connection_get_prpl(gc),
                       "jabber-sending-xmlnode", gc, &iq);
    g_free(iq_id);
  }
  else
    /* provide dummy callback since some
       protocols don't check before calling */
    purple_account_set_public_alias(account, name, cb_dummy,
                                    cb_set_alias_failure);
}

static void cb_global_moods_for_each(gpointer key, gpointer value,
                                     gpointer user_data)
{
  GList **out_moods = (GList **)user_data;
  PurpleMood *mood = (PurpleMood *)value;

  *out_moods = g_list_append(*out_moods, mood);
}

static void cb_set_alias_failure(PurpleAccount *account, const char *error)
{
  const gchar *id = purple_account_get_protocol_id(account);
  purple_debug_info(NAME, "aliases not supported by \"%s\"\n", id);
}

static void cb_dummy() {}
