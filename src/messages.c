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
#include <glib/gprintf.h>

#include "messages.h"
#include "varibles.h"


void printMessage(MSGTYPE type, const char *format, ...)
{
	GtkWidget *dialog = NULL;
	gchar *text = NULL;
	va_list ap;

	g_assert(type >= 0 && type <= 3);
	va_start(ap, format);

	text = g_strdup_vprintf(format, ap);

	switch (type) {
		case MSG_ERR:
			g_fprintf(stderr, "ERROR: ");
			g_fprintf(stderr, text);

			/*dialog = gtk_message_dialog_new(GTK_WINDOW(winWidget), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, text);*/
			dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, text);
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
		break;
		case MSG_WARN:
			g_printf("WARNING: ");
			g_printf(text);
		break;
		case MSG_VERB:
			if (myOptions.verbose) {
				g_printf(text);
			}
		break;
		case MSG_INFO:
			g_printf(text);
		break;
	}

	g_free(text);
	va_end(ap);
}

#ifdef DEBUG
void printDebug(const char *format, ...)
{
	va_list ap;
	g_fprintf(stderr, "DEBUG: ");
	va_start(ap, format);
	g_vfprintf(stderr, format, ap);
	va_end(ap);
}
#endif /* DEBUG */
