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
// Removes all references to conf.c  (For now)

#include "tn5250-private.h"


/*** Tn5250Config ***/

Tn5250Config *tn5250_config_new ()
{
    Tn5250Config *This = tn5250_new (Tn5250Config, 1);
    if (This == NULL)
        return NULL;

    This->ref = 1;
    This->vars = NULL;
    return This;
}

Tn5250Config *tn5250_config_ref (Tn5250Config *This)
{
    // unsafe anyway...
    This->ref++;
    return This;
}

// auto-delete...
void tn5250_config_unref (Tn5250Config *This)
{
    if (-- This->ref == 0)
    {
        free (This);
    }
}

int tn5250_config_load (Tn5250Config *, const char *)
{
    return 0;
}

int tn5250_config_load_default (Tn5250Config *)
{
    return 0;
}

int tn5250_config_parse_argv (Tn5250Config *, int , char **)
{
    return 0;
}

const char *tn5250_config_get (Tn5250Config *, const char * name)
{
    // absolutely needed...
    if ( !strcmp(name, "env.TERM") )
        return "IBM-3179-2";
    else if ( !strcmp(name, "map") )
        return "37";
    else
        return NULL;
}

int tn5250_config_get_bool (Tn5250Config *, const char *)
{
    return 0;
}

int tn5250_config_get_int (Tn5250Config *, const char *)
{
    return 0;
}

extern "C" void tn5250_config_set(Tn5250Config *This,
                                  const char *name,
                                  const int  type,
                                  const gpointer value)
{
}

extern "C" void tn5250_config_unset (Tn5250Config *, const char *)
{
}

// Copy variables prefixed with `prefix' to variables without `prefix'.
void tn5250_config_promote (Tn5250Config *, const char *)
{
}

