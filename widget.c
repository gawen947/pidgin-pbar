/* File: widget.c
   Time-stamp: <2010-11-15 16:46:20 gawen>

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

#include "common.h"

#include "pbar.h"
#include "widget.h"
#include "widget_gtk.h"
#include "widget_prpl.h"
#include "preferences.h"
#include "purple.h"

/* we only have one widget per plugin
   but this might change in the future */
struct widget *bar;

void create_widget()
{
  /* this should occurs only once but
     this way way we avoid memory leaks */
  if(!bar)
    bar = g_malloc(sizeof(struct widget));

  /* widgets that can possibly be modified along plugin's execution */
  bar->icon          = gtk_image_new();
  bar->status        = gtk_button_new_from_stock(NULL);
  bar->mood          = gtk_button_new_from_stock(NULL);
  bar->name_label    = gtk_label_new(NULL);
  bar->name_eventbox = gtk_event_box_new();
  bar->name_entry    = gtk_entry_new();
  bar->pm_label      = gtk_label_new(NULL);
  bar->pm_eventbox   = gtk_event_box_new();
  bar->pm_entry      = gtk_entry_new();
  bar->hbox          = gtk_hbox_new(FALSE, 2);
  bar->icon_eventbox = gtk_event_box_new();
  bar->status_menu   = gtk_menu_new();
  bar->mood_menu     = gtk_menu_new();

  /* widgets that are not modified */
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  GtkWidget *hbox1 = gtk_hbox_new(FALSE, 2);
  GtkWidget *hbox2 = gtk_hbox_new(FALSE, 2);

  /* setup widgets */
  gtk_label_set_ellipsize(GTK_LABEL(bar->name_label), PANGO_ELLIPSIZE_END);
  gtk_label_set_ellipsize(GTK_LABEL(bar->pm_label), PANGO_ELLIPSIZE_END);
  gtk_label_set_max_width_chars(GTK_LABEL(bar->name_label), 50);
  gtk_button_set_relief(GTK_BUTTON(bar->status), GTK_RELIEF_NONE);
  gtk_button_set_relief(GTK_BUTTON(bar->mood), GTK_RELIEF_NONE);
  gtk_widget_set_can_focus(bar->name_eventbox, FALSE);
  gtk_widget_set_can_focus(bar->pm_eventbox, FALSE);
  gtk_widget_set_can_focus(bar->status, FALSE);
  gtk_widget_set_can_focus(bar->mood, FALSE);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(bar->icon_eventbox), FALSE);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(bar->name_eventbox), FALSE);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(bar->pm_eventbox), FALSE);

  /* pack widgets */
  gtk_container_add(GTK_CONTAINER(bar->name_eventbox), bar->name_label);
  gtk_container_add(GTK_CONTAINER(bar->pm_eventbox), bar->pm_label);
  gtk_container_add(GTK_CONTAINER(bar->icon_eventbox), bar->icon);
  if(purple_prefs_get_bool(PREF "/compact")) {
    gtk_box_pack_start(GTK_BOX(hbox1), bar->name_eventbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), bar->name_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), bar->pm_eventbox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), bar->pm_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), bar->mood, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), bar->status, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, 0);
  }
  else {
    gtk_box_pack_start(GTK_BOX(hbox1), bar->name_eventbox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), bar->name_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), bar->status, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), bar->pm_eventbox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), bar->pm_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), bar->mood, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, 0);
  }
  gtk_box_pack_start(GTK_BOX(bar->hbox), bar->icon_eventbox, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(bar->hbox), vbox, TRUE, TRUE, 0);

  /* pack at top of the buddy list and fallback to top
     if the position option is unknown */
  void (*gtk_box_pack)(GtkBox *, GtkWidget *, gboolean, gboolean, guint) = gtk_box_pack_start;
  const PidginBuddyList *blist = pidgin_blist_get_default_gtk_blist();
  switch(purple_prefs_get_int(PREF "/widget-position")) {
  case(POSITION_TOP):
    gtk_box_pack = gtk_box_pack_start;
    break;
  case(POSITION_BOTTOM):
    gtk_box_pack = gtk_box_pack_end;
    break;
  };
  gtk_box_pack(GTK_BOX(blist->vbox), bar->hbox, FALSE, TRUE, 2);
  gtk_box_reorder_child(GTK_BOX(blist->vbox), bar->hbox, 0);

  /* setup initial states */
  bar->gtk_hnd    = NULL;
  bar->gtk_inst   = NULL;
  bar->hover_name = FALSE;
  bar->hover_pm   = FALSE;
  bar->name_entry_activated = FALSE;
  bar->pm_entry_activated   = FALSE;
  bar->name_dialog = FALSE;
  bar->pm_dialog   = FALSE;

  /* gtk signals and callback */
  const struct g_signal {
    GtkWidget *widget;
    const gchar *signal;
    void (*callback)(GtkWidget *, gpointer);
  } g_signal_connections[] = {
    { bar->icon_eventbox, "button-press-event", cb_buddy_icon },
    { bar->icon_eventbox, "enter-notify-event", cb_buddy_icon_enter },
    { bar->icon_eventbox, "leave-notify-event", cb_buddy_icon_leave },
    { bar->name_eventbox, "button-press-event", cb_name },
    { bar->name_eventbox, "enter-notify-event", cb_name_enter },
    { bar->name_eventbox, "leave-notify-event", cb_name_leave },
    { bar->name_entry, "activate", cb_name_entry },
    { bar->name_entry, "focus-out-event", cb_name_entry_focus_out },
    { bar->pm_eventbox, "button-press-event", cb_pm },
    { bar->pm_eventbox, "enter-notify-event", cb_pm_enter },
    { bar->pm_eventbox, "leave-notify-event", cb_pm_leave },
    { bar->pm_entry, "activate", cb_pm_entry },
    { bar->pm_entry, "focus-out-event", cb_pm_entry_focus_out },
    { bar->status, "clicked", cb_status_button },
    { bar->mood, "clicked", cb_mood_button },
    { NULL, NULL, NULL }
  }; const struct g_signal *g_sig = g_signal_connections;

  /* purple signals and callback */
  const struct p_signal {
    void *instance;
    const char *signal;
    void *callback;
  } purple_signal_connections[] = {
    { purple_accounts_get_handle(), "account-status-changed", cb_status },
    { purple_connections_get_handle(), "signed-on", cb_signed_on },
    { NULL, NULL, NULL }
  }; const struct p_signal *purple_sig = purple_signal_connections;

  /* purple preferences signals and callback */
  const struct p_signal purple_prefs_signal_connections[] = {
    { bar, PIDGIN_PREFS_ROOT "/accounts/buddyicon", cb_buddy_icon_update },
    { NULL, NULL, NULL }
  }; const struct p_signal *purple_prefs_sig = purple_prefs_signal_connections;

  /* connect signals and save handlers and instance when needed
     to disconnect those signals when the widget is destroyed */
  for(; g_sig->widget ; g_sig++) {
    gulong handler_id = g_signal_connect(G_OBJECT(g_sig->widget),
                                         g_sig->signal,
                                         G_CALLBACK(g_sig->callback),
                                         NULL);
    bar->gtk_hnd  = g_list_append(bar->gtk_hnd, GINT_TO_POINTER(handler_id));
    bar->gtk_inst = g_list_append(bar->gtk_inst, g_sig->widget);
  }
  for(; purple_sig->instance ; purple_sig++)
    purple_signal_connect(purple_sig->instance,
                          purple_sig->signal,
                          bar,
                          PURPLE_CALLBACK(purple_sig->callback),
                          NULL);
  for(; purple_prefs_sig->instance ; purple_prefs_sig++)
    purple_prefs_connect_callback(purple_prefs_sig->instance,
                                  purple_prefs_sig->signal,
                                  PURPLE_PREFS_CALLBACK(purple_prefs_sig->callback),
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

  GList *l, *i, *j;

  bar->installed = FALSE;

  /* disconnect purple and prefs signals */
  purple_signals_disconnect_by_handle(bar);
  purple_prefs_disconnect_by_handle(bar);

  /* disconnect gtk signals */
  for(i = bar->gtk_hnd, j = bar->gtk_inst ; i && j ; i = i->next, j = j->next)
    g_signal_handler_disconnect(j->data, GPOINTER_TO_INT(i->data));
  g_list_free(bar->gtk_hnd);
  g_list_free(bar->gtk_inst);

  /* destroy drop down status menu */
  l = gtk_container_get_children(GTK_CONTAINER(bar->status_menu));
  for(i = l ; i ; i = i->next) {
    gtk_widget_destroy(i->data);
    i->data = NULL;
  }
  gtk_widget_destroy(bar->status_menu);

  /* destroy drop down mood menu */
  l = gtk_container_get_children(GTK_CONTAINER(bar->mood_menu));
  for(i = l ; i ; i = i->next) {
    gtk_widget_destroy(i->data);
    i->data = NULL;
  }
  gtk_widget_destroy(bar->mood_menu);

  /* destroy widget */
  l = gtk_container_get_children(GTK_CONTAINER(bar->hbox));
  for(i = l ; i ; i = i->next) {
    gtk_widget_destroy(i->data);
    i->data = NULL;
  }
  gtk_widget_destroy(bar->hbox);

  g_free(bar);
  bar = NULL;
}

/* load preferences into our widget */
void init_widget()
{
  g_return_if_fail(bar->installed);

  /* entry frame */
  gboolean state = purple_prefs_get_bool(PREF "/frame-entry");
  set_widget_entry_frame(state);

  /* nickname */
  const gchar *markup = purple_prefs_get_string(PREF "/nickname-markup");
  const gchar *value  = purple_prefs_get_string(PREF "/nickname");
  int jtype  = purple_prefs_get_int(PREF "/nickname-justify");
  set_widget_name(markup, value);
  set_widget_name_justify(jtype);

  /* personal message */
  markup = purple_prefs_get_string(PREF "/personal-message-markup");
  value  = purple_prefs_get_string(PREF "/personal-message");
  jtype  = purple_prefs_get_int(PREF "/personal-message-justify");
  set_widget_pm(markup, value);
  set_widget_pm_justify(jtype);

  /* buddy icon */
  GdkPixbuf *icon = get_buddy_icon();
  set_widget_icon(icon);

  /* mood image */
  const gchar *current_mood = purple_prefs_get_string(PREF "/mood");
  gchar *path = get_mood_icon_path(current_mood);
  set_widget_mood(path);
  g_free(path);

  /* status image */
  const gchar *stock = get_status_stock_id();
  set_widget_status(stock);

  /* fill status menu */
  GList *accts = purple_accounts_get_all_active();
  GHashTable *global_status = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                    NULL, NULL);
  for(; accts ; accts = g_list_delete_link(accts, accts)) {
    PurpleAccount *account = (PurpleAccount *)accts->data;
    GList *types = purple_account_get_status_types(account);

    for(; types ; types = g_list_delete_link(types, types)) {
      PurpleStatusType *type = (PurpleStatusType *)types->data;
      PurpleStatusPrimitive prim;
      const gchar *stock, *status_name;
      GtkWidget *menu_item, *icon;

      if(!purple_status_type_is_user_settable(type) ||
         purple_status_type_is_independent(type))
        continue;

      prim  = purple_status_type_get_primitive(type);
      stock = pidgin_stock_id_from_status_primitive(prim);

      if(g_hash_table_lookup(global_status, stock))
        continue;
      else
        g_hash_table_insert(global_status, (gpointer)stock,
                            GINT_TO_POINTER(TRUE));

      menu_item    = gtk_image_menu_item_new_from_stock(stock, NULL);
      icon         = gtk_image_new_from_stock(stock, GTK_ICON_SIZE_MENU);
      status_name  = purple_status_type_get_name(type);

      gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item), icon);
      gtk_menu_item_set_label(GTK_MENU_ITEM(menu_item), status_name);

      gtk_menu_shell_append(GTK_MENU_SHELL(bar->status_menu), menu_item);

      g_signal_connect_swapped(menu_item, "activate",
                               G_CALLBACK(cb_status_menu),
                               (gpointer)type);

      gtk_widget_show(menu_item);
    }
  }
  g_hash_table_destroy(global_status);


  /* statusbox hiding */
  state = purple_prefs_get_bool(PREF "/hide-statusbox");
  set_statusbox_visible(!state);
}

static gchar * g_strreplace(const gchar *string, const gchar *old,
                            const gchar *new)
{
  gchar **split = g_strsplit(string, old, -1);
  gchar  *ret = g_strjoinv(new, split);

  g_strfreev(split);
  return ret; /* should be freed with g_free */
}

void set_widget_name(const gchar *markup, const gchar *name)
{
  g_return_if_fail(markup && name && bar->installed);

  gchar *escaped_name, *new;

  /* translate name if needed */
  if(!strcmp(name, EMPTY_NAME))
    name = _(EMPTY_NAME);

  escaped_name = g_markup_printf_escaped("%s", name);
  new = g_strreplace(markup, "%n", escaped_name);
  g_free(escaped_name);

  gtk_label_set_markup(GTK_LABEL(bar->name_label), new);
  g_free(new);
}

void set_widget_pm(const gchar *markup, const gchar *pm)
{
  g_return_if_fail(markup && pm && bar->installed);

  gchar *escaped_pm, *new;

  /* translate name if needed */
  if(!strcmp(pm, EMPTY_PM))
    pm = _(EMPTY_PM);

  escaped_pm = g_markup_printf_escaped("%s", pm);
  new = g_strreplace(markup, "%p", escaped_pm);
  g_free(escaped_pm);

  gtk_label_set_markup(GTK_LABEL(bar->pm_label), new);
  g_free(new);
}

void set_widget_status(const gchar *stock)
{
  g_return_if_fail(stock && bar->installed);

  GtkWidget *icon = gtk_image_new_from_stock(stock, GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(bar->status), icon);
  gtk_button_set_label(GTK_BUTTON(bar->status), "");
}

void set_widget_mood(const gchar *path)
{
  g_return_if_fail(path && bar->installed);

  GtkWidget *mood = gtk_image_new_from_file(path);
  gtk_button_set_image(GTK_BUTTON(bar->mood), mood);
  gtk_button_set_label(GTK_BUTTON(bar->mood), "");
}

void set_widget_icon(GdkPixbuf *icon)
{
  g_return_if_fail(bar->installed);

  if(icon)
    gtk_image_set_from_pixbuf(GTK_IMAGE(bar->icon), icon);
  else
    gtk_image_set_from_stock(GTK_IMAGE(bar->icon), GTK_STOCK_MISSING_IMAGE, 48);
}

/* convertion of int justification
   to float justification for
   gtk_misc_set_alignment */
static float int_jtype_to_float_jtype(int justify)
{
  float ret = 0.; /* default to left */

  switch(justify) {
  case(JUSTIFY_LEFT):
    ret = 0.;
    break;
  case(JUSTIFY_CENTER):
    ret = .5;
    break;
  case(JUSTIFY_RIGHT):
    ret = 1.;
    break;
  }

  return ret;
}

void set_widget_name_justify(int justify)
{
  g_return_if_fail(bar->installed);

  float jtype = int_jtype_to_float_jtype(justify);
  gtk_misc_set_alignment(GTK_MISC(bar->name_label), jtype, .5);
}

void set_widget_pm_justify(int justify)
{
  g_return_if_fail(bar->installed);

  float jtype = int_jtype_to_float_jtype(justify);
  gtk_misc_set_alignment(GTK_MISC(bar->pm_label), jtype, .5);
}

void set_widget_entry_frame(gboolean use_frame)
{
  g_return_if_fail(bar->installed);

  gtk_entry_set_has_frame(GTK_ENTRY(bar->name_entry), use_frame);
  gtk_entry_set_has_frame(GTK_ENTRY(bar->pm_entry), use_frame);
}

void set_statusbox_visible(gboolean visible)
{
  const PidginBuddyList *blist = pidgin_blist_get_default_gtk_blist();
  GtkWidget *statusbox = blist->statusbox;

  if(statusbox)
    gtk_widget_set_visible(statusbox, visible);
}

gboolean get_widget_name_hover_state() { return bar->hover_name; }
gboolean get_widget_pm_hover_state() { return bar->hover_pm; }
