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

#ifndef _TYPES_H_
#define _TYPES_H_

#include <gtk/gtk.h>


typedef enum {
	ACTION_NONE	      = -1,
	ACTION_CANCEL     =  0,
	ACTION_LOGOUT     =  1,
	ACTION_SHUTDOWN   =  2,
	ACTION_RESTART    =  3,
	ACTION_LOCK       =  4,
	ACTION_USERSWITCH =  5,
	ACTION_SUSPEND    =  6,
	ACTION_HIBERNATE  =  7,
	ACTION_CUSTOM     =  8
} Action;

enum {
	T_WIN_SIZE_FULLSCREEN = 10,
	T_WIN_SIZE_CUSTOM     = 11,
	T_WIN_POS_CENTER      = 20,
	T_WIN_POS_CUSTOM      = 21,
	T_WIN_BACK_COLOR     = 30,
	T_WIN_BACK_IMAGE      = 31,
	T_WIN_BACK_IMEGEREP   = 32,
	T_BT_RELIEF_NONE      = 110,
	T_BT_RELIEF_NORMAL    = 111,
	T_BT_ALIGN_HORIZONTAL = 120,
	T_BT_ALIGN_VERTICAL   = 121
};

/*typedef enum {
	DM_NONE = 0,
	DM_GDM  = 1,
	DM_KDM  = 2
} DisplayManager;

typedef enum {
    WM_NONE     = 0,
    WM_OPENBOX  = 1,
    WM_METACITY = 2,
    WM_KWINE    = 3,
    WM_XFWM4    = 4
} WindowManager;*/

typedef struct {
	gchar* rcfile;
	gchar* trcfile;
	/* General */
	gchar* version;
	/*gchar* log_file;*/
	gchar* theme;
	gboolean verbose;
	/*gboolean arrow_keys;*/
	gboolean composite;
	Action default_action;
	Action buttons[9];
	gchar* commands[9];
	gchar* labels[9];
	guint shortcuts[9];
} Options;

typedef struct {
	gchar *name;
	gchar *author;
	gchar *version;
	gchar *version_req;

	gboolean win_decor;
	gchar win_size;
	gint win_size_width;
	gint win_size_heigh;
	gchar win_pos;
	gint win_pos_x;
	gint win_pos_y;
	gchar win_bg;
	gchar *win_bg_file;
	gboolean win_bg_trans;
	GdkColor win_bg_color;
	gdouble win_bg_alpha;

	gboolean bt_img;
	gchar *bt_img_dir;
	gboolean bt_labels;
	//gchar bt_labels_type;
	//gchar bt_labels_pos;
	gchar *bt_labels_style;
	gchar bt_relief;
	//gchar bt_align;
	gint bt_spacing;
} Theme;

typedef struct {
	gchar* rcfile;
	gchar* theme;
	gboolean verbose;
	Action run_action;
	gint timeout;
} CommandlineOptions;


#endif
