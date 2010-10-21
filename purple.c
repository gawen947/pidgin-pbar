/* File: purple.c
   Time-stamp: <2010-10-21 18:42:04 gawen>

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

static void cb_set_alias_failure(PurpleAccount *account, const char *error)
{
  const gchar *id = purple_account_get_protocol_id(account);
  purple_debug_info(NAME, "aliases not supported by \"%s\"\n", id);
}

static void cb_dummy() {}
