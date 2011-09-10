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

#include "callbacks.h"
#include "types.h"
#include "varibles.h"
#include "misc.h"
#include "actions.h"
#include "messages.h"

void onDestroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

gboolean onKeyPress(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	guint i;

	if (event->key.type == GDK_KEY_PRESS) {
		printDebug("Key pressed: %s (%d)\n", gdk_keyval_name(event->key.keyval), event->key.keyval);
		switch(event->key.keyval) {
			case 65307: /* Esc */
				gtk_main_quit();
			break;
			case 65293: /* Enter */
				onButtonClick(widget, (gpointer) myOptions.default_action);
			break;
			default:
				for (i = 0; i <= 8; i++) {
					if (myOptions.shortcuts[i] == event->key.keyval) {
						onButtonClick(widget, (gpointer) i);
						return FALSE;
					}
				}
				printMessage(MSG_VERB, "Keyboard shortcut %s isn't set.\n", gdk_keyval_name(event->key.keyval));
			break;
		}
	}
	return FALSE;
}

gboolean onWindowStateChange(GtkWidget *widget, GdkEventWindowState *event, gpointer data)
{
	isFullscreen = (event->new_window_state == GDK_WINDOW_STATE_FULLSCREEN) ? TRUE : FALSE;
	return FALSE;
}

void onButtonClick(GtkWidget *widget, gpointer data)
{
	Action action = (Action) data;

	g_assert(action != ACTION_NONE);

	runActionCommand(action);

	gtk_main_quit();
}

void onScreenChanged(GtkWidget *widget, GdkScreen *old_screen, gpointer data)
{
	GdkScreen *screen = NULL;
	GdkColormap *colormap = NULL;
	if (myTheme.win_bg_trans) {
		screen = gtk_widget_get_screen(widget);
		colormap = gdk_screen_get_rgba_colormap(screen);

		if (!colormap) {
			colormap = gdk_screen_get_rgb_colormap(screen);
			printMessage(MSG_WARN, "Your screen doesn't support alpha channels, transparency disabled!\n");
			myTheme.win_bg_trans = FALSE;
		}
		else {
			printMessage(MSG_VERB, "Transparency enabled.\n");
			printMessage(MSG_VERB, "Setting transparency to %.2lf.\n", myTheme.win_bg_alpha);
		}
		gtk_widget_set_colormap(widget, colormap);
	}
	else
		printMessage(MSG_VERB, "Transparency disabled.\n");
}

gboolean onExpose(GtkWidget *widget, GdkEventExpose *event)
{
	cairo_t *cr;
	cairo_surface_t *surface;
	cairo_pattern_t *pattern;
	gint img_w, img_h;
	gint win_w;// = winWidget->allocation.width;
	gint win_h;// = winWidget->allocation.height;
	gdouble r = myTheme.win_bg_color.red / 255;
	gdouble g = myTheme.win_bg_color.green / 255;
	gdouble b = myTheme.win_bg_color.blue / 255;

	gtk_window_get_size(GTK_WINDOW(winWidget), &win_w, &win_h);

	cr = gdk_cairo_create(widget->window);

	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	if (myTheme.win_bg == T_WIN_BACK_COLOR) {
			if (!myTheme.win_bg_trans)
			cairo_set_source_rgb(cr, r, g, b);
		else
			cairo_set_source_rgba(cr, r, g, b, myTheme.win_bg_alpha);
	}

	if (myTheme.win_bg == T_WIN_BACK_IMAGE) {
		surface = cairo_image_surface_create_from_png(myTheme.win_bg_file);
		/*pattern = cairo_pattern_create_for_surface(surface);
		cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source(cr, pattern);*/
		img_w = cairo_image_surface_get_width(surface);
		img_h = cairo_image_surface_get_height(surface);
		cairo_scale(cr, win_w / img_w, win_h / img_h);
		cairo_set_source_surface(cr, surface, 0.0, 0.0);
	}

	if (myTheme.win_bg == T_WIN_BACK_IMEGEREP) {
		surface = cairo_image_surface_create_from_png(myTheme.win_bg_file);
		pattern = cairo_pattern_create_for_surface(surface);
		cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source(cr, pattern);
	}

	cairo_paint(cr);

	/*cairo_fill(cr);
	cairo_stroke(cr);*/
	cairo_destroy(cr);
	return FALSE;
}
