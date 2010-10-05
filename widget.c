/* File: widget.c
   Time-stamp: <2010-10-05 20:04:49 gawen>

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

#include "common.h"

#include "pbar.h"
#include "widget.h"
#include "widget_gtk.h"
#include "widget_prpl.h"
#include "preferences.h"

struct widget *bar = NULL;

void create_widget()
{
  if(!bar)
    bar = g_malloc(sizeof(struct widget));

  /* widgets that can possibly be modified along plugin's execution */
  bar->icon        = gtk_image_new();
  bar->status      = gtk_button_new_from_stock(NULL);
  bar->name_label  = gtk_label_new(NULL);
  bar->name_button = gtk_button_new();
  bar->name_entry  = gtk_entry_new();
  bar->pm_label    = gtk_label_new(NULL);
  bar->pm_button   = gtk_button_new();
  bar->pm_entry    = gtk_entry_new();
  bar->hbox        = gtk_hbox_new(FALSE, 2);
  bar->event_box   = gtk_event_box_new();


  /* widgets that are not modified */
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  GtkWidget *hbox1 = gtk_hbox_new(FALSE, 2);

  /* setup widgets */
  gtk_label_set_justify(GTK_LABEL(bar->name_label), GTK_JUSTIFY_LEFT);
  gtk_label_set_justify(GTK_LABEL(bar->pm_label), GTK_JUSTIFY_LEFT);
  gtk_button_set_relief(GTK_BUTTON(bar->name_button), GTK_RELIEF_NONE);
  gtk_button_set_relief(GTK_BUTTON(bar->pm_button), GTK_RELIEF_NONE);
  gtk_button_set_relief(GTK_BUTTON(bar->status), GTK_RELIEF_NONE);
  gtk_button_set_alignment(GTK_BUTTON(bar->name_button), .01, .5);
  gtk_button_set_alignment(GTK_BUTTON(bar->pm_button), .01, .5);
  gtk_widget_set_can_focus(bar->name_button, FALSE);
  gtk_widget_set_can_focus(bar->pm_button, FALSE);
  gtk_widget_set_can_focus(bar->status, FALSE);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(bar->event_box), FALSE);

  /* setup widgets according preferences */
  /* TODO: justify and relief from prefs */

  /* pack widgets */
  gtk_container_add(GTK_CONTAINER(bar->name_button), bar->name_label);
  gtk_container_add(GTK_CONTAINER(bar->pm_button), bar->pm_label);
  gtk_container_add(GTK_CONTAINER(bar->event_box),bar->icon);
  gtk_box_pack_start(GTK_BOX(hbox1), bar->name_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox1), bar->name_entry, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox1), bar->status, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), bar->pm_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), bar->pm_entry, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(bar->hbox), bar->event_box, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(bar->hbox), vbox, TRUE, TRUE, 0);

  /* pack into buddy list */
  PidginBuddyList *blist = pidgin_blist_get_default_gtk_blist();
  gtk_box_pack_start(GTK_BOX(blist->vbox), bar->hbox, FALSE, TRUE, 2);
  gtk_box_reorder_child(GTK_BOX(blist->vbox), bar->hbox, 0);

  /* setup initial states */
  bar->hover_name = FALSE;
  bar->hover_pm   = FALSE;

  /* connect gtk and purple signals */
  const struct g_signal {
    GtkWidget *widget;
    const gchar *signal;
    void (*callback)(GtkWidget *, gpointer);
  } g_signal_connections[] = {
    { bar->event_box, "button-press-event", cb_buddy_icon },
    { bar->event_box, "enter-notify-event", cb_buddy_icon_enter },
    { bar->event_box, "leave-notify-event", cb_buddy_icon_leave },
    { bar->name_button, "clicked", cb_name_button },
    { bar->name_button, "enter", cb_name_button_enter },
    { bar->name_button, "leave", cb_name_button_leave },
    { bar->name_entry, "activate", cb_name_entry },
    { bar->name_entry, "focus-out-event", cb_name_entry },
    { bar->pm_button, "clicked", cb_pm_button },
    { bar->pm_button, "enter", cb_pm_button_enter },
    { bar->pm_button, "leave", cb_pm_button_leave },
    { bar->pm_entry, "activate", cb_pm_entry },
    { bar->pm_entry, "focus-out-event", cb_pm_entry },
    { NULL, NULL, NULL }
  }; register const struct g_signal *g_sig = g_signal_connections;

  const struct p_signal {
    void * instance;
    const char *signal;
    void * callback;
  } p_signal_connections[] = {
    { purple_accounts_get_handle(), "account-status-changed", cb_status },
    { NULL, NULL, NULL }
  }; register const struct p_signal *p_sig = p_signal_connections;

  for(; g_sig->widget ; g_sig++)
    g_signal_connect(G_OBJECT(g_sig->widget),
                     g_sig->signal,
                     G_CALLBACK(g_sig->callback),
                     NULL);
  for(; p_sig->instance ; p_sig++)
    purple_signal_connect(p_sig->instance,
                          p_sig->signal,
                          thisplugin,
                          PURPLE_CALLBACK(p_sig->callback),
                          NULL);

  /* show everything */
  gtk_widget_show_all(bar->hbox);
  gtk_widget_hide(bar->name_entry);
  gtk_widget_hide(bar->pm_entry);

  /* inform that the bar is installed */
  bar->installed = TRUE;
}

void destroy_widget()
{
  g_return_if_fail(bar->installed);

  GList *l, *i;

  bar->installed = FALSE;
  l = gtk_container_get_children(GTK_CONTAINER(bar->hbox));
  for(i = l ; i ; i = i->next) {
    gtk_widget_destroy(i->data);
    i->data = NULL;
  }
  gtk_widget_destroy(bar->hbox);

  g_free(bar);
  bar = NULL;
}

void init_widget()
{
  g_return_if_fail(bar->installed);

  /* for buddy icon */
  const PidginBuddyList *blist;
  const PidginStatusBox *statusbox;
  GdkPixbuf *icon;

  /* for nickname, personal message and status box */
  const gchar *markup, *value;
  gboolean state;

  /* for status */
  const PurpleSavedStatus *status;
  PurpleStatusPrimitive prim;
  const gchar *stock;

  /* nickname */
  markup = purple_prefs_get_string(PREF "/nickname-markup");
  value  = purple_prefs_get_string(PREF "/nickname");
  set_widget_name(markup, value);

  /* personal message */
  markup = purple_prefs_get_string(PREF "/personal-message-markup");
  value  = purple_prefs_get_string(PREF "/personal-message");
  set_widget_pm(markup, value);

  /* buddy icon */
  blist = pidgin_blist_get_default_gtk_blist();
  statusbox = PIDGIN_STATUS_BOX(blist->statusbox);
  icon = statusbox->buddy_icon;
  if(icon)
    gtk_image_set_from_pixbuf(GTK_IMAGE(bar->icon), icon);
  else
    gtk_image_set_from_stock(GTK_IMAGE(bar->icon), GTK_STOCK_MISSING_IMAGE, 48);

  /* status image */
  status = purple_savedstatus_get_current();
  prim   = purple_savedstatus_get_type(status);
  stock  = pidgin_stock_id_from_status_primitive(prim);
  set_widget_status(stock);

  /* statusbox hidding */
  state = purple_prefs_get_bool(PREF "/hide-statusbox");
  set_statusbox_visible(!state);
}

static gchar * g_strreplace(const gchar *string, const gchar *old,
                            const gchar *new)
{
  gchar ** split, * ret;

  split = g_strsplit(string, old, -1);
  ret = g_strjoinv(new, split);
  g_strfreev(split);

  /* should be freed with g_free */
  return ret;
}

void set_widget_name(const gchar *markup, const gchar *name)
{
  g_return_if_fail(bar->installed);

  gchar *escaped_name, *new;

  escaped_name = g_markup_printf_escaped("%s", name);
  new = g_strreplace(markup, "%n", escaped_name);
  g_free(escaped_name);

  gtk_label_set_markup(GTK_LABEL(bar->name_label), new);
  g_free(new);
}

void set_widget_pm(const gchar *markup, const gchar *pm)
{
  g_return_if_fail(bar->installed);

  gchar *escaped_pm, *new;

  escaped_pm = g_markup_printf_escaped("%s", pm);
  new = g_strreplace(markup, "%p", escaped_pm);
  g_free(escaped_pm);

  gtk_label_set_markup(GTK_LABEL(bar->pm_label), new);
  g_free(new);
}

void set_widget_status(const gchar *stock)
{
  g_return_if_fail(bar->installed);

  GtkWidget *icon = gtk_image_new_from_stock(stock, GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(bar->status), icon);
  gtk_button_set_label(GTK_BUTTON(bar->status), "");
  /* TODO: use the given stock message instead */
}

void set_widget_icon(GdkPixbuf *icon)
{
  g_return_if_fail(bar->installed);

  if(icon)
    gtk_image_set_from_pixbuf(GTK_IMAGE(bar->icon), icon);
  else
    gtk_image_set_from_stock(GTK_IMAGE(bar->icon), GTK_STOCK_MISSING_IMAGE, 48);
}

void set_statusbox_visible(gboolean visible)
{
  const PidginBuddyList *blist;
  GtkWidget *statusbox;

  blist = pidgin_blist_get_default_gtk_blist();
  statusbox = blist->statusbox;
  if(statusbox)
    gtk_widget_set_visible(statusbox, visible);
}

gboolean get_widget_name_hover_state()
{
  return bar->hover_name;
}

gboolean get_widget_pm_hover_state()
{
  return bar->hover_pm;
}
