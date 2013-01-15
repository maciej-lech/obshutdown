/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * This file is part of obshutdown (Openbox Shutdown Dialog).
 *
 * Copyright (C) 2011 panjandrum  <panjandrummail@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "main.h"
#include "callbacks.h"
#include "varibles.h"
#include "parsers.h"
#include "misc.h"
#include "actions.h"
#include "messages.h"

int main(int argc, char **argv)
{
	exitCode = 0;

	initGtk(&argc, &argv);

	migrateConfigToXdgDir();
	migrateThemesToXdgDir();
	initDataDefault();

	if (parseCommandline(&argc, &argv)) {
		if(initLockFile()) {
			printMessage(MSG_INFO, "Startup %s %s (-h to print usage)\n", OBS_NAME, OBS_VERSION);
			if (initConfigs()) {
				initWidgets();
				gtk_widget_show_all(winWidget);
				gtk_main();
			}
			else
				exitCode = 1;
			deleteLockFile();
		}
		else
			exitCode = 1;
	}
	else
		exitCode = 1;

	freeData();

	exit(exitCode);
}

gboolean initLockFile(void)
{
	gboolean ret = TRUE;
	GError *error = NULL;
	GFileOutputStream *stream = NULL;
	gchar *tmpfile = NULL;

	myLockFile = NULL;

	tmpfile = g_build_filename(g_get_tmp_dir(), "obshutdown.lock", NULL);

	if(g_file_test(tmpfile, G_FILE_TEST_EXISTS)) {
		printMessage(MSG_ERR, "Obshutdown alredy running! Lockfile \"%s\" exist.\n", tmpfile);
		ret = FALSE;
	}
	else {
		myLockFile = g_file_new_for_path(tmpfile);
		stream = g_file_create(myLockFile, G_FILE_CREATE_NONE, NULL, &error);
		if (!stream) {
			printMessage(MSG_ERR, "Lockfile creating failed: %s\n", error->message);
			ret = FALSE;
		}
		else
			g_object_unref(stream);
	}

	g_free(tmpfile);

	return ret;
}

inline void deleteLockFile(void)
{
	GError *error = NULL;

	if(!g_file_delete(myLockFile, NULL, &error)) {
		printMessage(MSG_ERR, "Lockfile deleting failed: %s\n", error->message);
		exitCode = 1;
	}

	g_object_unref(myLockFile);
}

void migrateConfigToXdgDir(void)
{
	gchar *oldFilename, *newFilename, *xdgPath;
	GFile *oldFile, *newFile;


	xdgPath = g_build_filename(g_get_user_config_dir(), OBS_NAME, NULL);
	if (g_file_test(xdgPath, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
		g_free(xdgPath);
		return;
	}

	oldFilename = g_build_filename(g_get_home_dir(), ".obshutdown.rc", NULL);
       	if (!g_file_test(oldFilename, G_FILE_TEST_EXISTS)) {
		g_free(xdgPath);
		g_free(oldFilename);
		return;
	}

	if (g_mkdir_with_parents(xdgPath, (S_IRUSR | S_IWUSR | S_IXUSR)) == -1) {
		printMessage(MSG_WARN,"Failed to create %s\n", xdgPath);
		g_free(xdgPath);
		g_free(oldFilename);
		return;
	}

	newFilename = g_build_filename(xdgPath, "obshutdown.rc", NULL);
	oldFile = g_file_new_for_path(oldFilename);
	newFile = g_file_new_for_path(newFilename);
	if (!g_file_move(oldFile, newFile, G_FILE_COPY_NONE, NULL, NULL, NULL, NULL)) {
		printMessage(MSG_WARN,"Failed to migrate configuration file\n");
	}
	g_free(xdgPath);
	g_free(oldFilename);
	g_free(newFilename);
	g_object_unref(oldFile);
	g_object_unref(newFile);
}

void migrateThemesToXdgDir(void)
{
	gchar *oldPath, *xdgPath;	

	xdgPath = g_build_filename(g_get_user_data_dir(), OBS_NAME, "themes", NULL);
	if (g_file_test(xdgPath, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
		g_free(xdgPath);
		return;
	}

	oldPath = g_build_filename(g_get_home_dir(), ".themes", OBS_NAME, NULL);
	if (!g_file_test(oldPath, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
		g_free(xdgPath);
		g_free(oldPath);
		return;
	}

	if( g_mkdir_with_parents(xdgPath, (S_IRUSR | S_IWUSR | S_IXUSR)) == -1) {
		printMessage(MSG_WARN,"Failed to create %s\n", xdgPath);
		g_free(xdgPath);
		g_free(oldPath);
		return;
	}

	if (!copyThemesDir("")) {
		printMessage(MSG_WARN,"Failed to migrate themes files\n");
		g_free(xdgPath);
		g_free(oldPath);
		return;
	}

	g_rmdir(oldPath);
	g_free(xdgPath);
	g_free(oldPath);
}

gboolean copyThemesDir(const gchar *path)
{
	const gchar *file;
	gchar *fullPath, *childPath,  *oldFilename, *newFilename;
	GDir *dir;
	GFile *oldFile, *newFile;

	fullPath = g_build_filename(g_get_home_dir(), ".themes", OBS_NAME, path, NULL);
	dir = g_dir_open(fullPath, 0, NULL);
	file = g_dir_read_name(dir);
	while (file != NULL){
		if (g_file_test(file, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {			
			childPath = g_build_filename(fullPath, file, NULL);
			if (g_mkdir_with_parents(childPath, (S_IRUSR | S_IWUSR | S_IXUSR)) == -1) {
				g_free(fullPath);
				g_free(childPath);
				g_dir_close(dir);
				return FALSE;
			}

			childPath = g_build_filename(path, file, NULL);
			if (!copyThemesDir(childPath)) {
				g_free(fullPath);
				g_free(childPath);
				g_dir_close(dir);
				return FALSE;
			}
			g_free(childPath);
		}
		else {
			oldFilename = g_build_filename(fullPath, file, NULL);
			oldFile = g_file_new_for_path(oldFilename);
			newFilename = g_build_filename(g_get_user_data_dir(), OBS_NAME, "themes", path, file, NULL);
			newFile = g_file_new_for_path(newFilename);
			if (!g_file_move(oldFile, newFile, G_FILE_COPY_NONE, NULL, NULL, NULL, NULL)) {
				g_free(fullPath);
				g_dir_close(dir);
				g_free(oldFilename);
				g_free(newFilename);
				g_object_unref(oldFile);
				g_object_unref(newFile);
				return FALSE;
			}
			g_free(oldFilename);
			g_free(newFilename);
			g_object_unref(oldFile);
			g_object_unref(newFile);
		}
		file = g_dir_read_name(dir);
	}
	g_free(fullPath);
	g_dir_close(dir);
	return TRUE;
}

void initDataDefault(void)
{
	guint i;

	/* Options from rc file */
	myOptions.version = g_strdup(OBS_VERSION);
	myOptions.theme = g_strdup("foom");
	myOptions.verbose = FALSE;
	myOptions.composite = TRUE;
	myOptions.default_action = ACTION_NONE;
	for (i = 0; i <= 8; i++) {
		myOptions.buttons[i] = ACTION_NONE;
		myOptions.shortcuts[i] = 0;
		myOptions.commands[i] = NULL;
		myOptions.labels[i] = g_strdup(getActionStr(i));
	}

	/* Options from theme rc file */
	myTheme.name = NULL;
	myTheme.author = NULL;
	myTheme.version = NULL;
	myTheme.version_req = NULL;
	myTheme.win_decor = FALSE;
	myTheme.win_size = T_WIN_SIZE_FULLSCREEN;
	myTheme.win_size_width = 620;
	myTheme.win_size_heigh = 200;
	myTheme.win_pos = T_WIN_POS_CENTER;
	myTheme.win_pos_x = 0;
	myTheme.win_pos_y = 0;
	myTheme.win_bg = T_WIN_BACK_COLOR;
	myTheme.win_bg_file = NULL;
	myTheme.win_bg_trans = TRUE;
	gdk_color_parse("black", &myTheme.win_bg_color);
	myTheme.win_bg_alpha = 0.5;
	myTheme.bt_relief = T_BT_RELIEF_NONE;
	myTheme.bt_spacing = 10;
	myTheme.bt_img = TRUE;
	myTheme.bt_img_dir = g_strdup("foom");
	myTheme.bt_labels = TRUE;
	myTheme.bt_labels_style = g_strdup("<span fgcolor=\"white\">%s</span>");

	/* Options from commandline */
	myCmdOptions.rcfile = NULL;
	myCmdOptions.theme = NULL;
	myCmdOptions.verbose = FALSE;
	myCmdOptions.run_action = ACTION_NONE;
	myCmdOptions.timeout = 0;

	/* Configs paths */
	myOptions.rcfile = getRcPath();
	myOptions.trcfile = NULL;

	/* Configuration files */
	myConfigFile = NULL;
	myThemeConfigFile = NULL;

	/* Other */
	isFullscreen = FALSE;
}

void freeData(void)
{
	guint i;

	/* Options from rc */
	g_free(myOptions.version);
	g_free(myOptions.theme);
	for (i = 0; i <= 8; i++) {
		if (myOptions.commands[i])
			g_free(myOptions.commands[i]);
		if (myOptions.labels[i])
			g_free(myOptions.labels[i]);
	}

	/* Options from theme rc */
	if (myTheme.name)
		g_free(myTheme.name);
	if (myTheme.author)
		g_free(myTheme.author);
	if (myTheme.version)
		g_free(myTheme.version);
	if (myTheme.version_req)
		g_free(myTheme.version_req);
	if (myTheme.win_bg_file)
		g_free(myTheme.win_bg_file);
	g_free(myTheme.bt_img_dir);
	g_free(myTheme.bt_labels_style);

	/* Options from commandline */
	if (myCmdOptions.rcfile)
		g_free(myCmdOptions.rcfile);
	if (myCmdOptions.theme)
		g_free(myCmdOptions.theme);

	/* Configs paths */
	if (myOptions.rcfile)
		g_free(myOptions.rcfile);
	if (myOptions.theme)
		g_free(myOptions.trcfile);

}

inline void initGtk(int *argc, char ***argv)
{
	g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
	gtk_init(argc, argv);
	// TODO: Logi gtk
	//g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);
}

void initWidgets(void)
{
	guint i;
	GtkWidget *hboxWidget = NULL;
	GdkColor color = myTheme.win_bg_color;

	// TODO: Typ okna
	winWidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(winWidget), 8);
	gtk_window_set_title(GTK_WINDOW(winWidget), OBS_TITLE);
	gtk_widget_set_size_request(winWidget, myTheme.win_size_width, myTheme.win_size_heigh);
	if (myOptions.composite)
		gdk_color_parse("black", &color);
	gtk_widget_modify_bg(winWidget, GTK_STATE_NORMAL, &color);
	gtk_window_set_decorated(GTK_WINDOW(winWidget), myTheme.win_decor);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(winWidget), TRUE);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(winWidget), TRUE);
	GTK_WIDGET_SET_FLAGS(winWidget, GTK_CAN_FOCUS);

	gtk_widget_set_app_paintable(winWidget, TRUE);
	if (myTheme.win_pos == T_WIN_POS_CENTER) /* Window position - center */
		gtk_window_set_position(GTK_WINDOW(winWidget), GTK_WIN_POS_CENTER);
	else if (myTheme.win_pos == T_WIN_POS_CUSTOM) { /* Window position - custom */
		gtk_window_set_position(GTK_WINDOW(winWidget), GTK_WIN_POS_NONE);
		gtk_window_move(GTK_WINDOW(winWidget), myTheme.win_pos_x, myTheme.win_pos_y);
	}

	if (myTheme.win_size == T_WIN_SIZE_FULLSCREEN) /* Window size - fullscreen */
		gtk_window_resize(GTK_WINDOW(winWidget), gdk_screen_get_width(gdk_screen_get_default()), gdk_screen_get_height(gdk_screen_get_default()));
	else if (myTheme.win_size == T_WIN_SIZE_CUSTOM) /* Window size - custom */
		gtk_window_resize(GTK_WINDOW(winWidget), myTheme.win_size_width, myTheme.win_size_heigh);

	g_signal_connect(winWidget, "destroy", G_CALLBACK(onDestroy), NULL);
	g_signal_connect(winWidget, "key_press_event", G_CALLBACK(onKeyPress), NULL);
	g_signal_connect(winWidget, "window_state_event", G_CALLBACK(onWindowStateChange), NULL);
	g_signal_connect(winWidget, "show", G_CALLBACK(onWindowShow), NULL);

	if (myOptions.composite) {
		if (gtk_widget_is_composited(winWidget)) {
			printMessage(MSG_VERB, "Compositing enabled.\n");
			g_signal_connect(winWidget, "expose_event", G_CALLBACK(onExpose), NULL);
			g_signal_connect(winWidget, "screen-changed", G_CALLBACK(onScreenChanged), NULL);
			onScreenChanged(winWidget, NULL, NULL);
		} else {
			printMessage(MSG_WARN, "No compositing, enabling rendered effects!\n");
			myOptions.composite = FALSE;
		}

	}
	else {
		printMessage(MSG_VERB,  "Compositing disabled.\n");
	}

	hboxWidget = gtk_hbox_new(FALSE, 0);
	hboxButtonWidget = gtk_hbutton_box_new();
	/*gtk_hbutton_box_set_spacing_default(10);*/

	gtk_box_pack_start(GTK_BOX(hboxWidget), gtk_vbox_new(TRUE, 0), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hboxWidget), hboxButtonWidget, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hboxWidget), gtk_vbox_new(TRUE, 0), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(winWidget), hboxWidget);

	if (myOptions.buttons[0] != ACTION_NONE) {
		for (i = 0; i <= 8; i++)
			if (myOptions.buttons[i] != ACTION_NONE)
				addButton(myOptions.buttons[i]);
	}
	else {
		addButton(ACTION_CANCEL);
		addButton(ACTION_LOGOUT);
		addButton(ACTION_RESTART);
		addButton(ACTION_SHUTDOWN);
	}
}

void addButton(Action action)
{
	GtkWidget *vboxWidget = NULL;
	GtkWidget *buttonWidget = NULL;
	GtkWidget *imageWidget = NULL;
	GtkWidget *labelWidget = NULL;
	gchar *imgpath = NULL;
	gchar *markup = NULL;

	g_assert(action != ACTION_NONE);

	printMessage(MSG_VERB, "Add button: %s\n", getActionStr(action));

	if (myTheme.bt_img) {
		imgpath = getButtonImagePath(action);
		if (imgpath) {
			imageWidget = gtk_image_new_from_file(imgpath);
			g_free(imgpath);
		}
		else
			imageWidget = gtk_image_new_from_stock(GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_LARGE_TOOLBAR);
	}

	vboxWidget = gtk_vbox_new(FALSE, 0);

	buttonWidget = gtk_button_new();

	if (myTheme.bt_relief == T_BT_RELIEF_NONE)
		gtk_button_set_relief(GTK_BUTTON(buttonWidget), GTK_RELIEF_NONE);
	if (myTheme.bt_relief == T_BT_RELIEF_NORMAL)
		gtk_button_set_relief(GTK_BUTTON(buttonWidget), GTK_RELIEF_NORMAL);

	gtk_widget_modify_bg(buttonWidget, GTK_STATE_NORMAL, &myTheme.win_bg_color);
	gtk_widget_set_can_focus(buttonWidget, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(buttonWidget), myTheme.bt_spacing / 2);
	/*gtk_container_add(GTK_CONTAINER(buttonWidget), imageWidget);*/
	gtk_button_set_image(GTK_BUTTON(buttonWidget), imageWidget);

	gtk_box_pack_start(GTK_BOX(vboxWidget), buttonWidget, TRUE, TRUE, 0);
	g_signal_connect(buttonWidget, "clicked", G_CALLBACK(onButtonClick), (gpointer) action);

	if (myTheme.bt_labels) {
		markup = g_markup_printf_escaped(myTheme.bt_labels_style, myOptions.labels[action]);
		labelWidget = gtk_label_new(markup);
		gtk_label_set_markup(GTK_LABEL(labelWidget), markup);
		g_free(markup);
		gtk_box_pack_end(GTK_BOX(vboxWidget), labelWidget, TRUE, TRUE, 0);
	}

	/*gdk_color_parse("white", &color);
	gtk_widget_modify_fg(labelWidget, GTK_STATE_NORMAL, &color);*/

	gtk_box_pack_start(GTK_BOX(hboxButtonWidget), vboxWidget, TRUE, TRUE, 0);

}
