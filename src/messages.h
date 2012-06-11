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

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <glib.h>


//#define DEBUG


typedef enum {
	MSG_ERR  = 0,
	MSG_WARN = 1,
	MSG_VERB = 2,
	MSG_INFO = 3,
} MSGTYPE;

void printMessage(MSGTYPE type, const char *format, ...);

#ifndef DEBUG
#define printDebug
#define G_DISABLE_ASSERT
#else
void printDebug(const char *format, ...);
#endif /* DEBUG */


#endif /* _MESSAGES_H_ */
