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

#ifndef _VARIBLES_H_
#define _VARIBLES_H_

#include <gtk/gtk.h>
#include "types.h"


#define OBS_NAME "obshutdown"
#define OBS_TITLE "Openbox Shutdown Dialog"
#define OBS_VERSION "0.1-rc1" /* !!!!!!!!!!!!!!!!!!!!!! */

GtkWidget *winWidget;
GtkWidget *hboxButtonWidget;

/* Options from rc file */
Options myOptions;
/* Commandline options */
CommandlineOptions myCmdOptions;
/* Varibles from themes rc */
Theme myTheme;

/* Lock file (/tmp/obshutdown.lock) */
GFile *myLockFile;

/* Configuration files */
GKeyFile *myConfigFile;
GKeyFile *myThemeConfigFile;

/* Other */
gboolean isFullscreen;
gboolean exitCode;

#endif
