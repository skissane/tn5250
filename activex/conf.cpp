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

#include "stdafx.h"
extern "C" {
#include "tn5250-private.h"
}

/*** Tn5250Config ***/
/* The intent of this file is to use the registry instead of a file.
 * In the case of an ActiveX, it's much better to do this.
 * But as you see it hasn't been implemented...
 * If you want to use the old way, just include tn5250/src/conf.c 
 * instead of this file.
 */


extern "C" Tn5250Config *tn5250_config_new ()
{
    Tn5250Config *This = tn5250_new (Tn5250Config, 1);
    if (This == NULL)
        return NULL;

    This->ref = 1;
    This->vars = NULL;
    return This;
}

extern "C" Tn5250Config *tn5250_config_ref (Tn5250Config *This)
{
    // unsafe anyway...
    This->ref++;
    return This;
}

// auto-delete...
extern "C" void tn5250_config_unref (Tn5250Config *This)
{
    if (-- This->ref == 0)
    {
        free (This);
    }
}

extern "C" int tn5250_config_load (Tn5250Config *, const char *)
{
    return 0;
}

extern "C" int tn5250_config_load_default (Tn5250Config *)
{
    return 0;
}

extern "C" int tn5250_config_parse_argv (Tn5250Config *, int , char **)
{
    return 0;
}

extern "C" const char *tn5250_config_get (Tn5250Config *, const char * name)
{
    // absolutely needed...
    if ( !strcmp(name, "env.TERM") )
        return "IBM-3179-2";
    else if ( !strcmp(name, "map") )
        return "37";
    else if ( !strcmp(name, "enhanced") )
        return "true";
    else
        return NULL;
}

extern "C" int tn5250_config_get_bool (Tn5250Config * This, const char * name)
{
    const char *v = tn5250_config_get (This, name);
    return (v == NULL ? 0 :
    !(!strcmp (v,"off")
        || !strcmp (v, "no")
        || !strcmp (v,"0")
        || !strcmp (v,"false")));
}

extern "C" int tn5250_config_get_int (Tn5250Config * This, const char * name)
{
    const char *v = tn5250_config_get (This, name);

    if(v == NULL) {
        return 0;
    } else {
        return(atoi(v));
    }
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
extern "C" void tn5250_config_promote (Tn5250Config *, const char *)
{
}

