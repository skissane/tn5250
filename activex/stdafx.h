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
#pragma once


#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define _ATL_ALL_WARNINGS                   // turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_NO_PRAGMA_WARNINGS
#define _ATL_NO_PERF_SUPPORT
#define NOMINMAX

#include "tn5250-config.h"

#ifdef _MSC_VER
    #pragma warning(push,1)
    #pragma warning(disable:4217) // '' : member template functions cannot be used for copy-assignment or copy-construction
    #pragma warning(disable:4505) // '' : unreferenced local function has been removed
    #pragma warning(disable:4548) // expression before comma has no effect; expected expression with side-effect
    #pragma warning(disable:4555) // expression has no effect; expected expression with side-effect
    #pragma warning(disable:4619) // #pragma warning : there is no warning number 'N'
    #pragma warning(disable:4946) // reinterpret_cast used between related classes: '' and ''
#endif


#include <atlbase.h>
#include <atlhost.h>
#include <atlctl.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atlcoll.h>

#include <commdlg.h>
#include <algorithm>
#include <map>
#include <vector>
#include <limits>

#ifdef _MSC_VER
    #pragma warning(pop)
    #ifdef NDEBUG
    #pragma optimize("a", on)
    #endif
#endif

//using namespace ATL;
using ATL::CStringT;
using ATL::CStringA;
using ATL::CStringW;
using ATL::CString;


