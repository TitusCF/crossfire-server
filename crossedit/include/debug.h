/*
 * debug.h - Brute Force debugging utilities
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

#ifndef _debug_h
#define _debug_h

#include <stdio.h>

#ifdef DEBUG
#undef DEBUG
#endif

#if defined(DEBUG) || defined(DebugBruteForce)
#define debug(fmt) \
	{(void)fprintf(stdout,fmt);(void)fflush(stdout);}

#define debug0(fmt) \
	{(void)fprintf(stdout,fmt);(void)fflush(stdout);}

#define debug1(fmt,p1)\
	{(void)fprintf(stdout,fmt,p1);(void)fflush(stdout);}

#define debug2(fmt,p1,p2)\
	{(void)fprintf(stdout,fmt,p1,p2);(void)fflush(stdout);}

#define debug3(fmt,p1,p2,p3)\
	{(void)fprintf(stdout,fmt,p1,p2,p3);(void)fflush(stdout);}

#define debug4(fmt,p1,p2,p3,p4)\
	{(void)fprintf(stdout,fmt,p1,p2,p3,p4);(void)fflush(stdout);}

#define debug5(fmt,p1,p2,p3,p4,p5)\
	{(void)fprintf(stdout,fmt,p1,p2,p3,p4,p5);(void)fflush(stdout);}

#if defined(CrossFire) || defined(CrossEdit)
#endif /* CrossFire || CrossEdit */

#define aborting() \
        {(void)fprintf(stdout,"Aborting ...\n");fflush(stdout);(void)abort();}

#else

#define debug(fmt)
#define debug0(fmt)
#define debug1(fmt,p1)
#define debug2(fmt,p1,p2)
#define debug3(fmt,p1,p2,p3)
#define debug4(fmt,p1,p2,p3,p4)
#define debug5(fmt,p1,p2,p3,p4,p5)
#define aborting()

#endif /* DEBUG */

#endif /* _debug_h */


