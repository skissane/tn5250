#ifndef MENU_H
#define MENU_H

/* TN5250 - An implementation of the 5250 telnet protocol.
 * Copyright (C) 2005 James Rich
 * 
 * This file is part of TN5250.
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

#ifdef __cplusplus
extern "C"
{
#endif

#define MENU_TYPE_MENUBAR 0x01	/* Menu bar */
#define MENU_TYPE_SINGLE_SELECT_FIELD 0x11	/* Single choice selection field */
#define MENU_TYPE_MULTIPLE_SELECT_FIELD 0x12	/* Multiple choice selection field */
#define MENU_TYPE_SINGLE_SELECT_LIST 0x21	/* Single choice selection list */
#define MENU_TYPE_MULTIPLE_SELECT_LIST 0x22	/* Multiple choice selection list */
#define MENU_TYPE_SINGLE_SELECT_FIELD_PULL_DOWN 0x31	/* Single choice selection field and a pull-down list */
#define MENU_TYPE_MULTIPLE_SELECT_FIELD_PULL_DOWN 0x32	/* Multiple choice selection field and a pull-down list */
#define MENU_TYPE_PUSH_BUTTONS 0x41	/* Push buttons */
#define MENU_TYPE_PUSH_BUTTONS_PULL_DOWN 0x51	/* Push buttons in a pull-down menu */


  struct _Tn5250Menubar;
  struct _Tn5250DBuffer;

/***** lib5250/Tn5250Menubar
 * NAME
 *    Tn5250Menubar
 * SYNOPSIS
 *    Tn5250Menubar *menubar = tn5250_menubar_new ();
 * DESCRIPTION
 *    The Tn5250Menubar object manages a 5250 menubar on the display.
 * SOURCE
 */
  struct _Tn5250Menubar
  {
    struct _Tn5250Menubar *next;
    struct _Tn5250Menubar *prev;
    unsigned int id;		/* Numeric ID of this menubar */
    unsigned char mdt;
    short use_scrollbar;
    short num_sep_blank;
    short asterisk;
    short inputonly;
    short fieldadvischaradv;
    short nocursormove;
    unsigned char type;
    unsigned int row;		/* Row menubar starts on */
    unsigned int column;	/* Column menubar starts on */
    unsigned int size;		/* max size (in characters) of menubar item */
    unsigned int height;	/* height (in rows) of menubar */
    unsigned int items;		/* number of items on this menubar */
    struct _Tn5250DBuffer *table;
  };

  typedef struct _Tn5250Menubar Tn5250Menubar;

/* Manipulate menubars */
  extern Tn5250Menubar *tn5250_menubar_new ();
  extern Tn5250Menubar *tn5250_menubar_copy (Tn5250Menubar * This);
  extern void tn5250_menubar_destroy (Tn5250Menubar * This);
  extern int tn5250_menubar_start_row (Tn5250Menubar * This);
  extern int tn5250_menubar_start_col (Tn5250Menubar * This);
  extern int tn5250_menubar_size (Tn5250Menubar * This);
  extern int tn5250_menubar_height (Tn5250Menubar * This);
  extern int tn5250_menubar_items (Tn5250Menubar * This);

/* Manipulate menubar lists */
  extern Tn5250Menubar *tn5250_menubar_list_destroy (Tn5250Menubar * list);
  extern Tn5250Menubar *tn5250_menubar_list_add (Tn5250Menubar * list,
						 Tn5250Menubar * node);
  extern Tn5250Menubar *tn5250_menubar_list_remove (Tn5250Menubar * list,
						    Tn5250Menubar * node);
  extern Tn5250Menubar *tn5250_menubar_list_find_by_id (Tn5250Menubar * list,
							int id);
  extern Tn5250Menubar *tn5250_menubar_list_copy (Tn5250Menubar * list);
  extern Tn5250Menubar *tn5250_menubar_hit_test (Tn5250Menubar * list, int x,
						 int y);


  struct _Tn5250Menuitem;
  struct _Tn5250DBuffer;

/***** lib5250/Tn5250Menuitem
 * NAME
 *    Tn5250Menuitem
 * SYNOPSIS
 *    Tn5250Menuitem *menuitem = tn5250_menuitem_new ();
 * DESCRIPTION
 *    The Tn5250Menuitem object manages a 5250 menuitem on the display.
 * SOURCE
 */
  struct _Tn5250Menuitem
  {
    struct _Tn5250Menuitem *next;
    struct _Tn5250Menuitem *prev;
    unsigned int id;		/* Numeric ID of this menuitem */
    unsigned int row;		/* Row menuitem starts on */
    unsigned int column;	/* Column menuitem starts on */
    unsigned int height;	/* height (in characters) of menuitem */
    unsigned int width;		/* width (in characters) of menuitem */
    unsigned int border[4];	/* Characters used to create borders
				 * Uses the same masks as buf5250 */
    struct _Tn5250DBuffer *table;
  };

  typedef struct _Tn5250Menuitem Tn5250Menuitem;

/* Manipulate menuitems */
  extern Tn5250Menuitem *tn5250_menuitem_new ();
  extern Tn5250Menuitem *tn5250_menuitem_copy (Tn5250Menuitem * This);
  extern void tn5250_menuitem_destroy (Tn5250Menuitem * This);
  extern int tn5250_menuitem_start_row (Tn5250Menuitem * This);
  extern int tn5250_menuitem_start_col (Tn5250Menuitem * This);
  extern int tn5250_menuitem_height (Tn5250Menuitem * This);
  extern int tn5250_menuitem_width (Tn5250Menuitem * This);

/* Manipulate menuitem lists */
  extern Tn5250Menuitem *tn5250_menuitem_list_destroy (Tn5250Menuitem * list);
  extern Tn5250Menuitem *tn5250_menuitem_list_add (Tn5250Menuitem * list,
						   Tn5250Menuitem * node);
  extern Tn5250Menuitem *tn5250_menuitem_list_remove (Tn5250Menuitem * list,
						      Tn5250Menuitem * node);
  extern Tn5250Menuitem *tn5250_menuitem_list_find_by_id (Tn5250Menuitem *
							  list, int id);
  extern Tn5250Menuitem *tn5250_menuitem_list_copy (Tn5250Menuitem * list);
  extern Tn5250Menuitem *tn5250_menuitem_hit_test (Tn5250Menuitem * list,
						   int x, int y);

#ifdef __cplusplus
}

#endif
#endif				/* MENU_H */