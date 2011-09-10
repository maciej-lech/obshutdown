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

#include <glib.h>

#include "misc.h"
#include "types.h"
#include "varibles.h"
#include "actions.h"
#include "messages.h"

gchar* getPrefixDir(void)
{
	gchar *prefix = NULL;
	gchar *filename = NULL;
	gchar *temp = NULL;

	filename = g_file_read_link("/proc/self/exe", NULL);
	if (filename) {
		temp = g_path_get_dirname(filename);
		prefix = g_path_get_dirname(temp);
		g_free(filename);
		g_free(temp);
	} else
		printMessage(MSG_ERR, "Locating prefix directory failed!\n");

	return prefix;
}

gchar* getHomeDir(void)
{
	gchar *home = NULL;
	const gchar *temp = g_getenv("HOME");
	if (!temp)
		temp = g_get_home_dir();
	if (temp)
		home = g_strdup(temp);
	if (!home)
		printMessage(MSG_ERR, "Locating home directory failed!\n");
	return home;
}

gboolean runCommand(const gchar *cmd)
{
	gboolean ret = TRUE;
	GError *error = NULL;

	if (!g_spawn_command_line_async(cmd, &error)) {
		printMessage(MSG_ERR, "Creating process failed: %s\n", error->message);
		ret = FALSE;
	}

	return ret;
}

gchar *getRcPath(void)
{
	gchar *path = NULL;
	gchar *home = getHomeDir();

	if (home) {
		/*path = g_strdup_printf("%s/.obshutdown.rc", home);*/
		path = g_build_filename(home, ".obshutdown.rc", NULL);
		g_free(home);
		if (g_file_test(path, G_FILE_TEST_EXISTS) != FALSE) {
			return path;
		}
		else {
			printMessage(MSG_WARN, "Configuration file not found, using default configuration.\n");
			g_free(path);
		}
	}
	return NULL;
}

gchar *getThemeRcPath(void)
{
	gchar *path1 = NULL;
	gchar *path2 = NULL;
	gchar *home = NULL;
	gchar *prefix = NULL;

	home = getHomeDir();
	if (home) {
		path1 = g_strdup_printf("%s/.themes/obshutdown/%s.rc", home, myOptions.theme);
		g_free(home);
		if (g_file_test(path1, G_FILE_TEST_EXISTS) != FALSE)
			return path1;
	}

	prefix = getPrefixDir();
	if (prefix) {
		path2 = g_strdup_printf("%s/share/obshutdown/themes/%s.rc", prefix, myOptions.theme);
		/*path = g_build_filename(prefix, "share", "themes", myOptions.theme, "obshutdown", myOptions.theme);*/
		g_free(prefix);
		if (g_file_test(path2, G_FILE_TEST_EXISTS) != FALSE) {
			if (path1)
				g_free(path1);
			return path2;
		}
		else {
			if (path1) {
				printMessage(MSG_WARN, "Theme configuration file not found: \"%s\"\n", path1);
				g_free(path1);
			}
			printMessage(MSG_WARN, "Theme configuration file not found: \"%s\"\n", path2);
			g_free(path2);
		}
	}

	return NULL;
}

gchar *getButtonImagePath(Action action)
{
	gchar *path = NULL;
	gchar *prefix = NULL;
	gchar *filename = g_strdup_printf("%s.png", getActionStr(action));

	if (g_path_is_absolute(myTheme.bt_img_dir))
		path = g_build_filename(myTheme.bt_img_dir, filename, NULL);
	else {
		prefix = g_path_get_dirname(myOptions.trcfile);
		if (prefix) {
			path = g_build_filename(prefix, myTheme.bt_img_dir, filename, NULL);
			g_free(prefix);
		}
	}

	g_free(filename);

	if (g_file_test(path, G_FILE_TEST_EXISTS) != FALSE) {
		return path;
	}
	else {
		printMessage(MSG_WARN, "Image file not found: \"%s\"\n", path);
		g_free(path);
	}

	return NULL;
}

gchar *getImagePath(const gchar *imagefile)
{
	gchar *path = NULL;
	gchar *prefix = NULL;

	if (g_path_is_absolute(imagefile))
		path = g_strdup(imagefile);
	else {
		prefix = g_path_get_dirname(myOptions.trcfile);
		if (prefix) {
			path = g_build_filename(prefix, imagefile, NULL);
			g_free(prefix);
		}
	}

	if (g_file_test(path, G_FILE_TEST_EXISTS) != FALSE) {
		return path;
	}
	else {
		printMessage(MSG_WARN, "Image file not found: \"%s\"\n", path);
		g_free(path);
	}

	return NULL;
}
