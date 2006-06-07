/*
 * static char *rcsid_check_object_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/* 
 * This is the toolkit for common only operations
 * All methods should start with cctk (common check toolkit)
 */

#include <global.h>
#include <stdlib.h>
#include <check.h>

#define STATUS_LOGDIR    0x0001
#define STATUS_DATADIR   0x0002

#define STATUS_GLOBALS   0x0100
#define STATUS_HASHTABLE 0x0200
#define STATUS_OBJECTS   0x0400
#define STATUS_VARS      0x0800
#define STATUS_BLOCK     0x1000
#define STATUS_BMAP      0x2000
#define STATUS_ANIM      0x4000
#define STATUS_ARCH      0x8000
#define SET_TKFLAG(__flag) (status_flag|=__flag)
#define CHECK_TKFLAG(__flag) (status_flag & __flag)
#define CCTK_ASSERT(__flag) { \
        if (!CHECK_TKFLAG(__flag)) \
            fail("Improper initialisation, flag 0x%H",__flag);\
        }
static int status_flag=0;

/**
 * set the logdir to use
 */
void cctk_setlog(char* logfile){
    settings.logfilename=logfile;
    SET_TKFLAG(STATUS_LOGDIR);
}

void cctk_setdatadir(char* datadir){
    settings.datadir=datadir;
    SET_TKFLAG(STATUS_DATADIR);
}
/**
 * Loads up to archetype initialisation using standard crossfire files in source
 * tree. This function requires that cctk_setlog and cctk_setdatadir have both
 * been run already.
 */
void cctk_init_std_archetypes(){
    CCTK_ASSERT(STATUS_LOGDIR|STATUS_DATADIR);
    settings.archetypes="archetypes";
    settings.treasures="treasures.bld";
    init_globals();
    init_hash_table();
    init_objects();
    init_vars();
    init_block();
    read_bmap_names ();
    read_smooth();
    init_anim();
    init_archetypes();
    SET_TKFLAG(STATUS_GLOBALS|STATUS_HASHTABLE|STATUS_OBJECTS|STATUS_VARS|STATUS_BLOCK|STATUS_BMAP|STATUS_ANIM|STATUS_ARCH);
}

/**
 * Initialize a simple object. Make it appear as an actual ingame object
 * by setting appropriate flags (so it is part of game, not just a storage)
 * Requires arch and object initialized in status flag
 * @param archname the archetype name to use (NULL= default one)
 */
object* cctk_create_game_object(char* archname){
  archetype *arch;
  object *obj;
  CCTK_ASSERT(STATUS_OBJECTS|STATUS_ARCH);
  if (archname==NULL)
    archname="empty_archetype";
  arch = find_archetype(archname);
  if (arch==NULL) return NULL;
  obj = arch_to_object(arch);
  if (obj==NULL) return NULL;
  CLEAR_FLAG(obj,FLAG_FREED);
  return obj;
}
/**
 * Set all strings in given object to given parameter string, used for
 * checking cleaning of objects, mainly
 * @param op The object to initialize
 * @param string The string to set in all object's char* fields
 */
void cctk_set_object_strings(object* op, char* string){
  op->name=add_string(string);
  op->name_pl=add_string(string);
  op->title=add_string(string);
  op->race=add_string(string);
  op->slaying=add_string(string);
  op->skill=add_string(string);
  op->msg=add_string(string);
  op->lore=add_string(string);
  op->materialname=add_string(string);
}