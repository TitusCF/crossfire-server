/*
 * Xaw.h - convience header for using Athena Widget Set
 * Copyright (C) 1993 Petri Heinila
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author can be connected by email from hevi@lut.fi
 */

#ifndef _Xaw_h
#define _Xaw_h

#ifdef __cplusplus
#define Object XtObject
#define String XtString
#endif /* __cplusplus */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>

/*** Simple Widgets ***/
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Grip.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Panner.h>
#include <X11/Xaw/Repeater.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/StripChart.h>
#include <X11/Xaw/Toggle.h>

/*** Menus ***/
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>

/*** Text Widgets ***/
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>

/*** Composite and Constraint Widgets ***/
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Porthole.h>
#include <X11/Xaw/Tree.h>
#include <X11/Xaw/Viewport.h>

#ifdef __cplusplus
#undef Object
#undef String
#endif /* __cplusplus */

#endif /* _Xaw_h */


