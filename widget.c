/* File: widget.c
   Time-stamp: <2011-08-26 18:48:52 gawen>

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

#include "common.h"

#include "pbar.h"
#include "widget.h"
#include "widget_gtk.h"
#include "widget_prpl.h"
#include "preferences.h"
#include "purple.h"
#include "gtk.h"

/* we only have one widget per plugin
   but this might change in the future */
struct widget *bar;

void create_widget()
{
  /* we need this when the bar is reset for example
     using the preferences dialog */
  if(!bar) {
    bar = g_malloc(sizeof(struct widget));
    memset(bar, 0, sizeof(struct widget));
    check_available_features();
  }

#ifdef _WIN32
  /* force gtk-button-images to 1 on Windows,
     otherwise, mood and status images won't
     be displayed */
  GtkSettings *settings = gtk_settings_get_default();
  gtk_settings_set_long_property(settings, "gtk-button-images", 1, "gtkrc:21");
#endif /* _WIN32 */

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

  /* add main widgets */
  gtk_add_main_widget(PBAR_WIDGET(bar), bar->hbox);
  gtk_add_main_widget(PBAR_WIDGET(bar), bar->status_menu);
  gtk_add_main_widget(PBAR_WIDGET(bar), bar->mood_menu);

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
  const struct pbar_gtk_signal g_signal_connections[] = {
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
  };

  /* purple signals and callback */
  const struct pbar_prpl_signal p_signal_connections[] = {
    { purple_accounts_get_handle(), "account-status-changed", cb_status },
    { purple_connections_get_handle(), "signed-on", cb_signed_on },
    { purple_connections_get_handle(), "signed-off", cb_signed_off },
    { NULL, NULL, NULL }
  };

  /* purple preferences signals and callback */
  const struct pbar_prpl_signal pp_signal_connections[] = {
    { bar, PIDGIN_PREFS_ROOT "/accounts/buddyicon", cb_buddy_icon_update },
    { NULL, NULL, NULL }
  };

  /* connect signals and save handlers and instance when needed
     to disconnect those signals when the widget is destroyed */
  gtk_connect_signals(PBAR_WIDGET(bar), g_signal_connections, NULL);
  prpl_connect_signals(PBAR_WIDGET(bar), p_signal_connections, NULL);
  prpl_prefs_connect_signals(PBAR_WIDGET(bar), pp_signal_connections, NULL);

  /* show everything */
  gtk_widget_show_all(bar->hbox);
  gtk_widget_hide(bar->name_entry);
  gtk_widget_hide(bar->pm_entry);

  /* inform that the bar is installed */
  widget_set_all_sensitive(FALSE);
  update_available_widgets();
  bar->installed = TRUE;
}

static void free_inner_widget()
{
  g_return_if_fail(bar->installed);

  bar->installed = FALSE;

  /* disconnect purple and prefs signals */
  prpl_disconnect_signals(PBAR_WIDGET(bar));
  prpl_prefs_disconnect_signals(PBAR_WIDGET(bar));

  gtk_destroy(PBAR_WIDGET(bar)); /* destroy widgets */
}

void reset_widget()
{
  free_inner_widget();

  /* create widget but do not free the structure itself
     to save references for available features */
  create_widget();
}

void destroy_widget()
{
  free_inner_widget();

  g_free(bar);                   /* free widget */
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

    for(; types ; types = types->next) {
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

      status_name  = purple_status_type_get_name(type);
      menu_item    = gtk_image_menu_item_new_with_label(status_name);
      icon         = gtk_image_new_from_stock(stock, GTK_ICON_SIZE_MENU);

      gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item), icon);
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

/* create a change name dialog */
void create_name_dialog()
{
  const gchar *name = purple_prefs_get_string(PREF "/nickname");
  if(!name || !strcmp(name, EMPTY_NAME))
    name = "";

  purple_request_input(thisplugin,
                       _("Change nickname"),
                       _("Enter your nickname here..."),
                       _("This will change your nickname "
                         "for every account which supports it."),
                       name,
                       FALSE,
                       FALSE,
                       NULL,
                       _("OK"),
                       G_CALLBACK(cb_name_apply),
                       _("Cancel"),
                       G_CALLBACK(cb_name_cancel),
                       NULL,
                       NULL,
                       NULL,
                       NULL);
  bar->name_dialog = TRUE;
}

/* disable every features of the widget */
void widget_set_all_sensitive(gboolean sensitive)
{
  GtkWidget *widgets[] = {
    bar->icon,
    bar->icon_eventbox,
    bar->status,
    bar->status_menu,
    bar->mood,
    bar->mood_menu,
    bar->name_label,
    bar->name_eventbox,
    bar->name_entry,
    bar->pm_label,
    bar->pm_eventbox,
    bar->pm_entry,
    NULL
  }; GtkWidget **w = widgets;

  gboolean *attrs[] = {
    &bar->mood_message,
    &bar->current_song,
    &bar->song_title,
    &bar->song_album,
    &bar->game_name,
    &bar->office_app,
    NULL
  }; gboolean **a = attrs;

  for(; *w ; w++)
    gtk_widget_set_sensitive(*w, sensitive);
  for(; *a ; a++)
    **a = sensitive;
}

#define assert_ref(count, inc) (bar->count > 0 || inc > 0)

void check_available_features()
{
  GList *a = purple_accounts_get_all_active();

  for(; a ; a = a->next) {
    PurpleAccount *acct = (PurpleAccount *)a->data;

    if(purple_account_is_connected(acct))
      update_available_features(acct, TRUE);
  }
}

/* enable/disable features when an account changes */
void update_available_features(PurpleAccount *acct, gboolean enable)
{
  int inc = 1;
  PurplePlugin *plugin = purple_find_prpl(acct->protocol_id);
  PurplePluginProtocolInfo *protocol = PURPLE_PLUGIN_PROTOCOL_INFO(plugin);
  GHashTable *attrs    = get_account_attrs(acct);

  if(!enable)
    inc = -1;

  /* update references */
  if(g_hash_table_lookup(attrs, "mood") &&
     assert_ref(mood_ref, inc))
      bar->mood_ref += inc;
  if(g_hash_table_lookup(attrs, "moodtext") &&
     assert_ref(mood_message_ref, inc))
    bar->mood_message_ref += inc;
  if(g_hash_table_lookup(attrs, "game") && assert_ref(game_name_ref, inc))
    bar->game_name_ref += inc;
  if(g_hash_table_lookup(attrs, "office") && assert_ref(office_app_ref, inc))
    bar->office_app_ref += inc;
  if(g_hash_table_lookup(attrs, "tune_title")  &&
     g_hash_table_lookup(attrs, "tune_artist") &&
     g_hash_table_lookup(attrs, "tune_album") &&
     assert_ref(current_song_ref, inc))
    bar->current_song_ref += inc;
  if(protocol->set_status && assert_ref(pm_ref, inc))
    bar->pm_ref += inc;
  if(protocol->set_buddy_icon && assert_ref(icon_ref, inc))
    bar->icon_ref += inc;
  if((!strcmp(acct->protocol_id, "prpl-jabber") || protocol->set_public_alias)&&
     assert_ref(name_ref, inc))
    bar->name_ref += inc;
  if(assert_ref(status_ref, inc))
    bar->status_ref += inc;
}

void update_available_widgets()
{
  PurpleMood *mood  = get_global_moods();

  const struct s_widget {
    GtkWidget *widget;
    gboolean sensitive;
  } widgets[] = {
    { bar->icon, bar->icon_ref },
    { bar->icon_eventbox, bar->icon_ref },
    { bar->status, bar->status_ref },
    { bar->status_menu, bar->status_ref },
    { bar->mood, bar->mood_ref && mood->mood },
    { bar->mood_menu, bar->mood_ref && mood->mood },
    { bar->name_label, bar->name_ref },
    { bar->name_eventbox, bar->name_ref },
    { bar->name_entry, bar->name_ref },
    { bar->pm_label, bar->pm_ref + bar->mood_message_ref + bar->game_name_ref +\
      bar->office_app_ref + bar->current_song_ref },
    { bar->pm_eventbox, bar->pm_ref + bar->mood_message_ref+bar->game_name_ref+\
      bar->office_app_ref + bar->current_song_ref },
    { bar->pm_entry, bar->pm_ref + bar->mood_message_ref + bar->game_name_ref +\
      bar->office_app_ref + bar->current_song_ref },
    { NULL, FALSE }
  }; const struct s_widget *w = widgets;

  const struct s_attr {
    gboolean *attr;
    gboolean sensitive;
  } attrs[] = {
    { &bar->pm_message, bar->pm_ref },
    { &bar->mood_message, bar->mood_message_ref && mood->mood },
    { &bar->current_song, bar->current_song_ref },
    { &bar->song_title, bar->current_song_ref },
    { &bar->song_album, bar->current_song_ref },
    { &bar->game_name, bar->game_name },
    { &bar->office_app, bar->office_app },
    { NULL, FALSE }
  }; const struct s_attr *a = attrs;

  for(; w->widget ; w++)
    gtk_widget_set_sensitive(w->widget, w->sensitive);
  for(; a->attr ; a++)
    *a->attr = a->sensitive;
}

/* create a personal message dialog */
void create_pm_dialog()
{
  PurpleRequestFields *fields;
  PurpleRequestFieldGroup *group;
  PurpleRequestField *field;
  const gchar *pm = purple_prefs_get_string(PREF "/personal-message");
  if(!pm || !strcmp(pm, EMPTY_PM))
    pm = "";

  const struct s_field {
    const gchar *text;
    const gchar *pref;
    gboolean enable;
  } groups[] = {
    { N_("_Mood message"), PREF "/mood-message", bar->mood_message },
    { N_("Current song"), NULL, bar->current_song },
    { N_("Song _title"), PREF "/tune-title", bar->current_song },
    { N_("Song _artist"), PREF "/tune-artist", bar->song_title },
    { N_("Song al_bum"), PREF "/tune-album", bar->song_album },
    { N_("MSN pecan extra attributes"), NULL, bar->game_name && bar->office_app },
    { N_("_Game name"), PREF "/game-message", bar->game_name },
    { N_("_Office app name"), PREF "/office-message", bar->office_app },
    { NULL, NULL },
    { NULL, NULL }
  }; const struct s_field *g = groups;

  fields = purple_request_fields_new();
  group = purple_request_field_group_new(_("Status and mood message"));
  purple_request_fields_add_group(fields, group);

  if(bar->pm_message) {
    field = purple_request_field_string_new(PREF "/personal-message",
                                            _("_Personal message"),
                                            pm,
                                            FALSE);
    purple_request_field_set_required(field, FALSE);
    purple_request_field_group_add_field(group, field);
  }

  for(; g->pref ; g++) {
    for(; g->pref ; g++) {
      const gchar *message;

      if(!g->enable)
        continue;
      else if(purple_prefs_get_bool(PREF "/reset-attrs"))
        message = "";
      else
        message = purple_prefs_get_string(g->pref);
      field = purple_request_field_string_new(g->pref,
                                              _(g->text),
                                              message,
                                              FALSE);
      purple_request_field_set_required(field, FALSE);
      purple_request_field_group_add_field(group, field);
    }
    if(g->enable) {
      group = purple_request_field_group_new(_(g->text));
      purple_request_fields_add_group(fields, group);
    }
  }

  purple_request_fields(thisplugin,
                        _("Change status messages"),
                        _("Enter status message..."),
                        _("This will change some status messages for every "
                          "account which supports it, please be advised "
                          "that some are inconsistent between each other."),
                        fields,
                        _("OK"),
                        G_CALLBACK(cb_pm_apply),
                        _("Cancel"),
                        G_CALLBACK(cb_pm_cancel),
                        NULL, NULL, NULL, NULL);
  bar->pm_dialog = TRUE;
}

/* replace format character <c><r> with <n> string and escape with <c><c> */
static gchar * g_strreplacefmt(const gchar *s, gchar c, gchar r, const gchar *n)
{
  int sn = strlen(n);
  int sr = strlen(s);
  int index = 0;
  gchar *ret = g_malloc(sr);

  for(; *s != '\0' ; s++) {
    if(*s == c) {
      s++;
      if(*s == r) {
        const gchar *i = n;

        sr += sn;
        ret = g_realloc(ret, sr);
        for(; *i != '\0' ; i++, index++)
          ret[index] = *i;
        continue;
      }
      else if(*s != c || *s == '\0')
        s--;
    }

    ret[index] = *s;
    index++;
  }

  ret[index] = '\0';

  return ret;
}

void set_widget_name(const gchar *markup, const gchar *name)
{
  g_return_if_fail(markup && name && bar->installed);

  gchar *escaped_name, *new;

  /* translate name if needed */
  if(!strcmp(name, EMPTY_NAME))
    name = _(EMPTY_NAME);

  escaped_name = g_markup_printf_escaped("%s", name);
  new = g_strreplacefmt(markup, '%', 'n', escaped_name);
  g_free(escaped_name);

  gtk_label_set_markup(GTK_LABEL(bar->name_label), new);
  g_free(new);
}

void set_widget_pm(const gchar *markup, const gchar *pm)
{
  g_return_if_fail(markup && pm && bar->installed);

  gchar *escaped_pm, *new;

  /* translate pm if needed */
  if(!strcmp(pm, EMPTY_PM))
    pm = _(EMPTY_PM);

  escaped_pm = g_markup_printf_escaped("%s", pm);
  new = g_strreplacefmt(markup, '%', 'p', escaped_pm);
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
