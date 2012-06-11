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

#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include <glib.h>

void onDestroy(GtkWidget*, gpointer);
gboolean onKeyPress(GtkWidget*, GdkEvent*, gpointer);
gboolean onWindowStateChange(GtkWidget*, GdkEventWindowState*, gpointer);
void onWindowShow(GtkWidget*, gpointer);
void onButtonClick(GtkWidget*, gpointer);
void onScreenChanged(GtkWidget*, GdkScreen*, gpointer);
gboolean onExpose(GtkWidget*, GdkEventExpose*);

#endif
