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

#include <gtk/gtk.h>
#include "parsers.h"
#include "varibles.h"
#include "misc.h"
#include "actions.h"
#include "messages.h"

gboolean openRcFile(GKeyFile **keyfile, const gchar* rcfile)
{
	GError *error = NULL;

	*keyfile = g_key_file_new();
	if (!g_key_file_load_from_file(*keyfile, rcfile, G_KEY_FILE_NONE, &error)) {
		printMessage(MSG_ERR, "Error opening rc file: %s\n", error->message);
		return FALSE;
	}
	g_key_file_set_list_separator(*keyfile, ' ');

	return TRUE;
}

inline void closeRcFile(GKeyFile **keyfile)
{
	if (*keyfile)
		g_key_file_free(*keyfile);
}

gboolean initConfigs(void)
{

	if (myOptions.rcfile) {
		printMessage(MSG_VERB, "Loading configuration file: %s\n", myOptions.rcfile);
		if(openRcFile(&myConfigFile, myOptions.rcfile)) {
			parseConfigRcFile();
			closeRcFile(&myConfigFile);
		}
	}

	if (myCmdOptions.run_action != ACTION_NONE) {
		if (myCmdOptions.timeout) {
			runActionCommandTimeout(myCmdOptions.run_action);
			return FALSE;
		}
		else {
			runActionCommand(myCmdOptions.run_action);
			return FALSE;
		}
	}

	if (myOptions.trcfile) {
		printMessage(MSG_VERB, "Loading theme \"%s\": \"%s\".\n", myOptions.theme, myOptions.trcfile);
		if(openRcFile(&myThemeConfigFile, myOptions.trcfile)) {
			parseThemeRcFile();
			closeRcFile(&myThemeConfigFile);
		}
	}

	return TRUE;
}

void parseConfigRcFile(void)
{
	guint i;
	gchar **buttonlist = NULL;
	gsize buttonnumber;
	Action buttonact;
	gchar *defaction = NULL;
	gchar *shortcut = NULL;

	g_assert(myConfigFile != NULL);
	printDebug("****CONFIG****\n");

	if(g_key_file_has_group(myConfigFile, "general")) {

		getStrValue(&myConfigFile, "general", "version", &myOptions.version);
		if (g_ascii_strcasecmp(myOptions.version, OBS_VERSION) > 0) {
			printMessage(MSG_WARN, "Version in configuration file is newer than %s, using default configuration.\n", OBS_VERSION);
			return;
		}
		/*else if (g_ascii_strcasecmp(myOptions.version, OBS_VERSION) < 0) {
														// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			printMessage(MSG_WARN, "Rewriting version.\n");
			g_key_file_set_string(myConfigFile, "general", "version", "dupa\n");
		}*/

		if (!myCmdOptions.verbose)
			getBoolValue(&myConfigFile, "general", "verbose", &myOptions.verbose);

		if (!myCmdOptions.theme) {
			getStrValue(&myConfigFile, "general", "theme", &myOptions.theme);
			myOptions.trcfile = getThemeRcPath();
		}

		getBoolValue(&myConfigFile, "general", "composite", &myOptions.composite);

		getStrValue(&myConfigFile, "general", "default_action", &defaction);
		if (defaction) {
			myOptions.default_action = getActionId(defaction);
			if (myOptions.default_action == ACTION_NONE)
				printMessage(MSG_WARN, "Action \"%s\" isn't valid action.\n", defaction);
			g_free(defaction);
		}

		buttonlist = g_key_file_get_string_list(myConfigFile, "general", "buttons", &buttonnumber, NULL);
		if (buttonlist) {
			for (i = 0; i < buttonnumber; i++) {
				buttonact = getActionId(*(buttonlist+i));
				printDebug("Buttons: %s (%d)\n", *(buttonlist+i), getActionId(*(buttonlist+i)));
				if (buttonact == ACTION_NONE)
					printMessage(MSG_WARN, "Action \"%s\" isn't valid action.\n", *(buttonlist+i));
				else
					myOptions.buttons[i] = buttonact;
			}
			g_strfreev(buttonlist);
		}
		else
			printMessage(MSG_WARN, "Buttons list not found in config file, setting default\n");
	}
	else
		printMessage(MSG_WARN, "Config file wasn't have [general] group.\n");

	if(g_key_file_has_group(myConfigFile, "actions")) {
		for (i = 0; i <= 8; i++)
			getStrValue(&myConfigFile, "actions", getActionStr(i), &myOptions.commands[i]);
	}
	else
		printMessage(MSG_WARN, "Config file wasn't have [actions] group.\n");

	if(g_key_file_has_group(myConfigFile, "shortcuts")) {
		for (i = 0; i <= 8; i++) {
			getStrValue(&myConfigFile, "shortcuts", getActionStr(i), &shortcut);
			if (shortcut) {
				printDebug("Shotrcuts: %s - %s (%u)\n", getActionStr(i), shortcut, gdk_keyval_from_name(shortcut));
				myOptions.shortcuts[i] = gdk_keyval_from_name(shortcut);
				g_free(shortcut);
				shortcut = NULL;
			}
		}
	}
	else
		printMessage(MSG_WARN, "Config file wasn't have [shortcuts] group.\n");

	if(g_key_file_has_group(myConfigFile, "labels")) {
		for (i = 0; i <= 8; i++) {
			getStrValue(&myConfigFile, "labels", getActionStr(i), &myOptions.labels[i]);
		}
	}
	else
		printMessage(MSG_WARN, "Config file wasn't have [labels] group.\n");

}

void parseThemeRcFile(void)
{
	gchar *winsize = NULL;
	gchar *winpos = NULL;
	gchar *winback = NULL;
	gchar *winbgcolor = NULL;
	gchar *winbgfile = NULL;
	gchar *butrelief = NULL;

	g_assert(myThemeConfigFile != NULL);
	printDebug("****THEME****\n");

	if(g_key_file_has_group(myThemeConfigFile, "general")) {

		getStrValue(&myThemeConfigFile, "general", "name", &myTheme.name);
		getStrValue(&myThemeConfigFile, "general", "author", &myTheme.author);
		getStrValue(&myThemeConfigFile, "general", "version", &myTheme.version);
		getStrValue(&myThemeConfigFile, "general", "version_req", &myTheme.version_req);
		printDebug("Generel: name=%s, author=%s, version=%s, version_req=%s\n", myTheme.name, myTheme.author, myTheme.version, myTheme.version_req);
		if (g_ascii_strcasecmp(myTheme.version_req, OBS_VERSION) > 0) {
			printMessage(MSG_WARN, "Theme configuration file required obshutdown in version %s, setting default.\n", myTheme.version_req);
			return;
		}
	}
	else
		printMessage(MSG_WARN, "Theme config file wasn't have [general] group.\n");

	if(g_key_file_has_group(myThemeConfigFile, "window")) {
		getBoolValue(&myThemeConfigFile, "window", "decoration", &myTheme.win_decor);
		getStrValue(&myThemeConfigFile, "window", "size", &winsize);
		printDebug("Window: decoration=%d\n", myTheme.win_decor);
		if (winsize) {
			if (!g_ascii_strcasecmp(winsize, "fullscreen"))
				myTheme.win_size = T_WIN_SIZE_FULLSCREEN;
			else if (!g_ascii_strcasecmp(winsize, "custom"))
				myTheme.win_size = T_WIN_SIZE_CUSTOM;
			else
				printMessage(MSG_WARN, "Invallid config key value: %s.\n", winsize);
			printDebug("Window: size=%s (%d)\n", winsize, myTheme.win_size);
			g_free(winsize);
		}
		if (myTheme.win_size == T_WIN_SIZE_CUSTOM) {
			getIntValue(&myThemeConfigFile, "window", "size.width", &myTheme.win_size_width);
			getIntValue(&myThemeConfigFile, "window", "size.heigh", &myTheme.win_size_heigh);
			printDebug("Window: width=%d, height=%d\n", myTheme.win_size_width, myTheme.win_size_heigh);
		}
		getStrValue(&myThemeConfigFile, "window", "position", &winpos);
		if (winpos) {
			if (!g_ascii_strcasecmp(winpos, "center"))
				myTheme.win_pos = T_WIN_POS_CENTER;
			else if (!g_ascii_strcasecmp(winpos, "custom"))
				myTheme.win_pos = T_WIN_POS_CUSTOM;
			else
				printMessage(MSG_WARN, "Invallid config key value: %s.\n", winpos);
			printDebug("Window: position=%s (%d)\n", winpos, myTheme.win_pos);
			g_free(winpos);
			if (myTheme.win_pos == T_WIN_POS_CUSTOM) {
				getIntValue(&myThemeConfigFile, "window", "position.x", &myTheme.win_pos_x);
				getIntValue(&myThemeConfigFile, "window", "position.y", &myTheme.win_pos_y);
				printDebug("Window: x=%d, y=%d\n", myTheme.win_pos_x, myTheme.win_pos_y);
			}
		}
		getStrValue(&myThemeConfigFile, "window", "background", &winback);
		getBoolValue(&myThemeConfigFile, "window", "background.transparency", &myTheme.win_bg_trans);
		if (winback) {
			if (!g_ascii_strcasecmp(winback, "color"))
				myTheme.win_bg = T_WIN_BACK_COLOR;
			else if (!g_ascii_strcasecmp(winback, "image"))
				myTheme.win_bg = T_WIN_BACK_IMAGE;
			else if (!g_ascii_strcasecmp(winback, "image_repeat"))
				myTheme.win_bg = T_WIN_BACK_IMEGEREP;
			else
				printMessage(MSG_WARN, "Invallid config key value: %s.\n", winback);
			printDebug("Window: background=%s (%d), transparency=%d\n", winback, myTheme.win_bg, myTheme.win_bg_trans);
			g_free(winback);
			if (myTheme.win_bg == T_WIN_BACK_COLOR) {
				getStrValue(&myThemeConfigFile, "window", "background.color", &winbgcolor);
				if (winbgcolor) {
					if (!gdk_color_parse(winbgcolor, &myTheme.win_bg_color))
						printMessage(MSG_WARN, "Invallid config key value: %s.\n", winback);
					g_free(winbgcolor);
				}
				if (myTheme.win_bg_trans)
					getDoubleValue(&myThemeConfigFile, "window", "background.alpha", &myTheme.win_bg_alpha);
				printDebug("Window: rgb=(%d, %d, %d), alpha=%.2lf\n", myTheme.win_bg_color.red/257, myTheme.win_bg_color.green/257, myTheme.win_bg_color.blue/257, myTheme.win_bg_alpha);
			}
			else {
				getStrValue(&myThemeConfigFile, "window", "background.file", &winbgfile);
				if (winbgfile) {
					printDebug("Window: background.file=%s\n", winbgfile);
					myTheme.win_bg_file = getImagePath(winbgfile);
					printDebug("Window: background.file=%s\n", myTheme.win_bg_file);
					g_free(winbgfile);
				}
				if (!myTheme.win_bg_file)
					myTheme.win_bg = T_WIN_BACK_COLOR;
			}
		}
	}
	else
		printMessage(MSG_WARN, "Theme config file wasn't have [window] group.\n");

	if(g_key_file_has_group(myThemeConfigFile, "buttons")) {
		getStrValue(&myThemeConfigFile, "buttons", "relief", &butrelief);
		if (butrelief) {
			if (!g_ascii_strcasecmp(butrelief, "none"))
				myTheme.bt_relief = T_BT_RELIEF_NONE;
			else if (!g_ascii_strcasecmp(butrelief, "normal"))
				myTheme.bt_relief = T_BT_RELIEF_NORMAL;
			else
				printMessage(MSG_WARN, "Invallid config key value: %s.\n", butrelief);
		}
		getBoolValue(&myThemeConfigFile, "buttons", "images", &myTheme.bt_img);
		if (myTheme.bt_img)
			getStrValue(&myThemeConfigFile, "buttons", "images.directory", &myTheme.bt_img_dir);
		getIntValue(&myThemeConfigFile, "buttons", "spacing", &myTheme.bt_spacing);
		getBoolValue(&myThemeConfigFile, "buttons", "labels", &myTheme.bt_labels);
		if (myTheme.bt_labels)
			getStrValue(&myThemeConfigFile, "buttons", "labels.style", &myTheme.bt_labels_style);
	}
	else
		printMessage(MSG_WARN, "Theme config file wasn't have [buttons] group.\n");
}

void getStrValue(GKeyFile **keyfile, const gchar *group, const gchar *key, gchar **dest)
{
	GError *error = NULL;
	gchar *value = g_key_file_get_string(*keyfile, group, key, &error);
	if(!value && error) {
		if (error->code == G_KEY_FILE_ERROR_INVALID_VALUE)
			printMessage(MSG_WARN, "Invallid config key value: %s\n", error->message);
		return;
	}
	else {
		if (value[0] != '\0') {
			if (*dest) {
				g_free(*dest);
				*dest = NULL;
			}
			(*dest) = g_strdup(value);
		}
		g_free(value);
	}
}

void getBoolValue(GKeyFile **keyfile, const gchar *group, const gchar *key, gboolean *dest)
{
	GError *error = NULL;
	gboolean value = g_key_file_get_boolean(*keyfile, group, key, &error);
	if(!value && error) {
		if (error->code == G_KEY_FILE_ERROR_INVALID_VALUE)
			printMessage(MSG_WARN, "Invallid config key value: %s\n", error->message);
		return;
	}
	*dest = value;
}

void getIntValue(GKeyFile **keyfile, const gchar *group, const gchar *key, gint *dest)
{
	GError *error = NULL;
	gint value = g_key_file_get_integer(*keyfile, group, key, &error);
	if(!value && error) {
		if (error->code == G_KEY_FILE_ERROR_INVALID_VALUE)
			printMessage(MSG_WARN, "Invallid config key value: %s\n", error->message);
		return;
	}
	*dest = value;
}

void getDoubleValue(GKeyFile **keyfile, const gchar *group, const gchar *key, gdouble *dest)
{
	GError *error = NULL;
	gdouble value = g_key_file_get_double(*keyfile, group, key, &error);
	if(!value && error) {
		if (error->code == G_KEY_FILE_ERROR_INVALID_VALUE)
			printMessage(MSG_WARN, "Invallid config key value: %s\n", error->message);
		return;
	}
	*dest = value;
}

gboolean parseCommandline(int *argc, char ***argv)
{
	gboolean ret = TRUE;
	gchar *rc_file = NULL;
	gchar *run_action = NULL;
	gchar *temp = NULL;
	gchar *config_message = g_strdup_printf("Set custom rc file(default: %s/obshutdown/obshutdown.rc)", g_get_user_config_dir());
	GError *error = NULL;
	GOptionContext *context;
	GOptionEntry entries[] = {
		{"verbose", 'v', 0, G_OPTION_ARG_NONE, &myCmdOptions.verbose, "Set verbose mode", NULL},
		{"theme", 't', 0, G_OPTION_ARG_STRING, &myCmdOptions.theme, "Set custom theme", "THEME"},
		{"config", 'c', 0, G_OPTION_ARG_STRING, &rc_file, config_message, "FILE"},
		{"run", 'r', 0, G_OPTION_ARG_STRING, &run_action, "Specifying action to run", "ACTION"},
		{"wait", 'w', 0, G_OPTION_ARG_INT, &myCmdOptions.timeout, "Wait n minutes before executing the action from --run", "n"},
		{NULL}
	};

	context = g_option_context_new("- Openbox shutdown dialog.");
	g_option_context_add_main_entries(context, entries, NULL);
	g_option_context_add_group(context, gtk_get_option_group(TRUE));
	if (!g_option_context_parse(context, argc, argv, &error)) {
		printMessage(MSG_ERR, "Commandline options parsing failed: %s\n", error->message);
		ret = FALSE;
	}

	myOptions.verbose = myCmdOptions.verbose;

	if (run_action) {
		if (ret)
			myCmdOptions.run_action = getActionId(run_action);
			if (myCmdOptions.run_action == ACTION_NONE) {
				ret = FALSE;
				printMessage(MSG_ERR, "Action \"%s\" isn't valid action.\n", run_action);
			}
		g_free(run_action);
	}

	if (myCmdOptions.theme) {
		g_free(myOptions.theme);
		myOptions.theme = g_strdup(myCmdOptions.theme);
		myOptions.trcfile = getThemeRcPath();
	}

	if (rc_file) {
		if (ret) {

			if (!g_path_is_absolute(rc_file)) {
				temp = g_get_current_dir();
				myCmdOptions.rcfile = g_build_filename(temp, rc_file, NULL);
				g_free(temp);
			}
			else
				myCmdOptions.rcfile = g_strdup(rc_file);

			if(!g_file_test(myCmdOptions.rcfile , G_FILE_TEST_EXISTS)) {
				printMessage(MSG_ERR, "Configuration file from commandline not found, using ~/.obshutdown.rc.\n");
			}
			else {
				g_free(myOptions.rcfile);
				myOptions.rcfile = g_strdup(myCmdOptions.rcfile);
			}

		}
		g_free(rc_file);
	}

	g_option_context_free(context);
	g_free(config_message);
	return ret;
}
