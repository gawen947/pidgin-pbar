/* File: purple.c
   Time-stamp: <2011-06-17 13:57:28 gawen>

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
/* this file contains common functions for libpurple and pidgin */

#include "common.h"

#include "pbar.h"
#include "gtk.h"
#include "purple.h"

/* callbacks */
static void cb_global_moods_for_each(gpointer key, gpointer value,
                                     gpointer user_data);
static void cb_set_alias_failure(PurpleAccount *account, const char *error);
static void cb_dummy();

/* check if default gtk blist is created */
gboolean is_gtk_blist_created()
{
  const PidginBuddyList *blist = pidgin_blist_get_default_gtk_blist();

  if(!blist ||
     !blist->vbox ||
     !gtk_widget_get_visible(blist->vbox))
    return FALSE;
  return TRUE;
}

/* get buddy icon from statusbox widget */
GdkPixbuf * get_buddy_icon()
{
  const PidginBuddyList *blist = pidgin_blist_get_default_gtk_blist();
  const PidginStatusBox *statusbox = PIDGIN_STATUS_BOX(blist->statusbox);

  return statusbox->buddy_icon;
}

/* get buddy icon hovered from statusbox widget */
GdkPixbuf * get_buddy_icon_hover()
{
  const PidginBuddyList *blist = pidgin_blist_get_default_gtk_blist();
  const PidginStatusBox *statusbox = PIDGIN_STATUS_BOX(blist->statusbox);

  return statusbox->buddy_icon_hover;
}

/* create purple protocol icon from protocol info */
GdkPixbuf * create_prpl_icon_from_info(PurplePluginProtocolInfo *prpl_info,
                                       PidginPrplIconSize size)
{
  const char *protoname = NULL;
  char *tmp;
  char *filename = NULL;
  GdkPixbuf *pixbuf;

  if (prpl_info->list_icon == NULL)
    return NULL;

  protoname = prpl_info->list_icon(NULL, NULL);
  if (protoname == NULL)
    return NULL;

  /*
   * Status icons will be themeable too, and then it will look up
   * protoname from the theme
   */
  tmp = g_strconcat(protoname, ".png", NULL);

  filename = g_build_filename(DATADIR, "pixmaps", "pidgin", "protocols",
                              size == PIDGIN_PRPL_ICON_SMALL ? "16" :
                              size == PIDGIN_PRPL_ICON_MEDIUM ? "22" : "48",
                              tmp, NULL);
  g_free(tmp);

  pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
  g_free(filename);

  return pixbuf;
}

/* get current status stock id */
const gchar * get_status_stock_id()
{
  const PurpleSavedStatus *status = purple_savedstatus_get_current();
  PurpleStatusPrimitive prim = purple_savedstatus_get_type(status);

  return pidgin_stock_id_from_status_primitive(prim);
}

/* get mood icon path */
gchar * get_mood_icon_path(const gchar *mood)
{
  gchar *path;

  if(!mood || !strcmp(mood, ""))
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

/* get available attributes for a protocol
   returned hashtable should be freed manually */
GHashTable * get_protocol_attrs(PurplePluginProtocolInfo *protocol)
{
  if(!protocol->status_types)
    return NULL;

  GHashTable *attrs = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);
  GList *l = protocol->status_types(NULL);
  for(; l ; l = l->next) {
    PurpleStatusType *type = (PurpleStatusType *)l->data;
    GList *k = purple_status_type_get_attrs(type);
    for(; k ; k = k->next) {
      struct _PurpleStatusAttr {
        char *id;
        char *name;
        PurpleValue *value_type;
      };
      struct _PurpleStatusAttr *attr = (struct _PurpleStatusAttr *)k->data;
      if(g_hash_table_lookup(attrs, attr->id))
        continue;
      else
        g_hash_table_insert(attrs, (gpointer)attr->id,
                            GINT_TO_POINTER(TRUE));
    }
  }

  return attrs;
}

/* get available attributes for an account
   returned hashtable should be freed manally */
/* TODO: review this, now it does the same as get_protocol_attrs... */
GHashTable * get_account_attrs(PurpleAccount *account)
{
  GHashTable *attrs = g_hash_table_new_full(g_str_hash, g_str_equal,
                                            NULL, NULL);
  GList *l = purple_account_get_status_types(account);
  for(; l ; l = l->next) {
    PurpleStatusType *type = (PurpleStatusType *)l->data;
    GList *k = purple_status_type_get_attrs(type);
    for(; k ; k = k->next) {
      struct _PurpleStatusAttr {
        char *id;
        char *name;
        PurpleValue *value_type;
      };
      struct _PurpleStatusAttr *attr = (struct _PurpleStatusAttr *)k->data;
      if(g_hash_table_lookup(attrs, attr->id))
        continue;
      else
        g_hash_table_insert(attrs, (gpointer)attr->id,
                            GINT_TO_POINTER(TRUE));
    }
  }

  return attrs;
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
        PurplePluginProtocolInfo *prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(
          gc->prpl);
        PurpleMood *mood = NULL;

        for(mood = prpl_info->get_moods(account) ;
            mood->mood ; mood++) {
          int mood_count = GPOINTER_TO_INT(g_hash_table_lookup(mood_counts,
                                                               mood->mood));

          if(!g_hash_table_lookup(global_moods, mood->mood))
            g_hash_table_insert(global_moods, (gpointer)mood->mood, mood);
          g_hash_table_insert(mood_counts, (gpointer)mood->mood,
                              GINT_TO_POINTER(mood_count + 1));
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

/* set status to the specified mood */
void set_status_with_mood(PurpleAccount *account, const gchar *mood)
{
  purple_account_set_status(account, "mood", TRUE, PURPLE_MOOD_NAME, mood,
                            NULL);
}

/* set status to the specified mood for all accounts */
void set_status_with_mood_all(const gchar *mood)
{
  GList *accts = purple_accounts_get_all_active();
  for(; accts ; accts = g_list_delete_link(accts, accts)) {
    PurpleAccount *account = (PurpleAccount *)accts->data;
    PurpleConnection *gc = purple_account_get_connection(account);

    if(gc && gc->flags & PURPLE_CONNECTION_SUPPORT_MOODS)
      set_status_with_mood(account, mood);
  }
}

/* set exclusive status for all accounts */
void set_status_all(const gchar *status_id, GList *attrs)
{
  GList *accts = purple_accounts_get_all_active();

  /* empty list means we have nothing to do */
  if(!attrs)
    return;

  for(; accts ; accts = g_list_delete_link(accts, accts)) {
    PurpleAccount *account = accts->data;

    if(!purple_account_is_connected(account))
      continue;
    purple_account_set_status_list(account, status_id, TRUE, attrs);
  }
}

/* set display name for account */
void set_display_name(PurpleAccount *account, const gchar *name)
{
  const gchar *id = purple_account_get_protocol_id(account);

  /* exception for set_public_alias */
  if(!strcmp(id, "prpl-jabber")) {
    PurpleConnection *gc = account->gc;
    gchar *iq_id = g_strdup_printf("purple%x", g_random_int());
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

/* set display name for all connected accounts */
void set_display_name_all(const char *name)
{
  GList *accts = purple_accounts_get_all_active();

  for(; accts ; accts = g_list_delete_link(accts, accts)) {
    PurpleAccount *account = accts->data;

    if(!purple_account_is_connected(account))
      continue;
    set_display_name(account, name);
  }
}

/* set status message (personal message) */
void set_status_message(const gchar *pm)
{
  PurpleSavedStatus *status = purple_savedstatus_get_current();

  purple_savedstatus_set_message(status, pm);
  purple_savedstatus_activate(status);
}

void prpl_connect_signals(struct pbar_widget *w,
                          const struct pbar_prpl_signal *sig_list,
                          gpointer data)
{
  const struct pbar_prpl_signal *s = sig_list;
  for(; s->signal ; s++)
    purple_signal_connect(s->instance,
                          s->signal,
                          w,
                          PURPLE_CALLBACK(s->callback),
                          data);
}

void prpl_prefs_connect_signals(struct pbar_widget *w,
                                const struct pbar_prpl_signal *sig_list,
                                gpointer data)
{
  const struct pbar_prpl_signal *s = sig_list;
  for(; s->signal ; s++)
    purple_prefs_connect_callback(s->instance,
                                  s->signal,
                                  PURPLE_PREFS_CALLBACK(s->callback),
                                  data);
}

void prpl_disconnect_signals(struct pbar_widget *w)
{ purple_signals_disconnect_by_handle(w); }

void prpl_prefs_disconnect_signals(struct pbar_widget *w)
{ purple_prefs_disconnect_by_handle(w); }

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
