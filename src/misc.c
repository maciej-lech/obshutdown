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

	path = g_build_filename(g_get_user_config_dir(), OBS_NAME, "obshutdown.rc", NULL);
	if (g_file_test(path, G_FILE_TEST_EXISTS)) {
		return path;
	}
	else {
		printMessage(MSG_WARN, "Configuration file not found, using default configuration.\n");
		g_free(path);
	}

	return NULL;
}

gchar *getThemeRcPath(void)
{
	const gchar * const *system_data_dirs = g_get_system_data_dirs();
	gchar *path = NULL;
	gchar *filename = g_strdup_printf("%s.rc", myOptions.theme);
	gint i=0;

	path = g_build_filename(g_get_user_data_dir(), OBS_NAME, "themes", filename, NULL);
	if (g_file_test(path, G_FILE_TEST_EXISTS)) {
		return path;
	}
	else {
		printMessage(MSG_WARN, "Theme configuration file not found: \"%s\"\n", path);
		g_free(path);
	}

	for (i = 0; system_data_dirs[i]; i++) {
		path = g_build_filename(system_data_dirs[i], OBS_NAME, "themes", filename, NULL);
		if (g_file_test(path, G_FILE_TEST_EXISTS)) {
			return path;
		}
		else {
			printMessage(MSG_WARN, "Theme configuration file not found: \"%s\"\n", path);
			g_free(path);
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
