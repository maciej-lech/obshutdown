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
#include <signal.h>

#include "actions.h"
#include "types.h"
#include "varibles.h"
#include "misc.h"
#include "parsers.h"
#include "messages.h"

static const char* actstr[] = {"cancel", "logout", "shutdown", "restart", "lock", "switch", "suspend", "hibernate", "custom"};

gboolean runActionCommand(Action action)
{
	g_assert(action != ACTION_NONE);

	if (action != ACTION_CANCEL) {
		if(myOptions.commands[action]) {
			printMessage(MSG_INFO, "Executing action: %s...\n", getActionStr(action));
			if(!runCommand(myOptions.commands[action])) {
				exitCode = 1;
				return FALSE;
			}
		}
		else {
			printMessage(MSG_ERR, "Not found \"%s\" command in config file. Default commands haven't been implemented!\n", getActionStr(action));
			exitCode = 1;
			return FALSE;
		}
	}

	return TRUE;
}

// TODO: Zwracane wartośći przez funkcje runAction*

gboolean runActionCommandTimeout(Action action)
{
	g_timeout_add_seconds(60, timeoutFunc, NULL);
	timeout_loop = g_main_loop_new(NULL, TRUE);
	printMessage(MSG_INFO, "Action \"%s\" is going to run in %d min. (Ctrl+C to break)\n", getActionStr(action), myCmdOptions.timeout);
	signal(SIGINT, sigHandler);
	g_main_loop_run(timeout_loop);
	return !exitCode;
}

gboolean timeoutFunc(gpointer data)
{
	myCmdOptions.timeout--;
	if (!myCmdOptions.timeout) {
		g_main_loop_quit(timeout_loop);
		if (!runActionCommand(myCmdOptions.run_action))
			exitCode = 1;
		return FALSE;
	}
	if ((myCmdOptions.timeout < 10) || (myCmdOptions.timeout % 10 == 0))
		printMessage(MSG_INFO, "Action \"%s\" is going to run in %d min.\n", getActionStr(myCmdOptions.run_action), myCmdOptions.timeout);

	return TRUE;
}

void sigHandler(int signum)
{
	exitCode = 1;
	printMessage(MSG_INFO, "\nAction \"%s\" cancelled.\n", getActionStr(myCmdOptions.run_action));
	g_main_loop_quit(timeout_loop);
}

const gchar *getActionStr(Action action)
{
	g_assert(action != ACTION_NONE);

	if (action >= 0 && action <= 8)
		return actstr[action];
	else
		return NULL;
}

Action getActionId(const gchar *name)
{
	gshort i;
	for (i = 0; i <= 8; i++)
		if (!g_ascii_strcasecmp(name, actstr[i]))
			return (Action) i;

	return ACTION_NONE;
}
