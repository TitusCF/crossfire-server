/*
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

#ifndef _util_h
#define _util_h
#include <stdio.h>

#define die(STR) {fprintf(stderr,"Fatal: %s\n",STR);fflush(stderr);exit(1);}
#define sdie(STR) {perror(STR);fflush(stderr);exit(1);}
#define warn(STR) {fprintf(stderr,"Warning: %s\n",STR);fflush(stderr);}

#define LineSize    80+1
#define BufSize     255+1
#define BigBufSize  65535+1
#define KiloBufSize (1<<10)+1
#define MegaBufSize (1<<20)+1
#define GigaBufSize (1<<30)+1

/* These should now be defined in global.h */

#endif /* _util_h */
