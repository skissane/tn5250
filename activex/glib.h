/* 
 * Copyright (C) 2003 Marc-Antoine Ruel at Cardinal Health
 * 
 * This file is part of TN5250
 *
 * TN5250 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 * 
 * TN5250 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 *
 */
// MAR: I know it's an really *UGLY* hack but for now I can't compile with glib.


// Glue
#pragma once

// gmem.h
#define g_free free
#define g_malloc malloc
#define g_realloc realloc
typedef void * gpointer;

// gtypes.h
typedef struct _GSList		GSList;

// GList.h
struct _GSList
{
  gpointer data;
  GSList *next;
};
#define  g_slist_next(slist)	((slist) ? (((GSList *)(slist))->next) : NULL)


// Uses this hook to add some of my glue.

// Glue
struct CTerm5250;
#define _Tn5250TerminalPrivate CTerm5250
#define _TN5250_TERMINAL_PRIVATE_DEFINED

