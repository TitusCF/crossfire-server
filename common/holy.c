/*
 * static char *rcsid_holy_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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

/**
 * @file holy.c
 * God-related common functions.
 */

#include <stdlib.h>
#include <global.h>
#include <living.h>
#include <spells.h>

static void add_god_to_list (archetype *god_arch);

/**
 * Initializes a god structure.
 *
 * @note
 * Will never return NULL.
 */
static godlink *init_godslist(void) {
    godlink *gl = (godlink *) malloc(sizeof(godlink));
    if(gl==NULL)
        fatal(OUT_OF_MEMORY);
    gl->name=NULL;        /* how to describe the god to the player */
    gl->arch=NULL;   	/* pointer to the archetype of this god */
    gl->id=0;             /* id of the god */
    gl->next=NULL;        /* next god in this linked list */

    return gl;
}

/**
 * This takes a look at all of the archetypes to find
 * the objects which correspond to the GODS (type GOD)
 */
void init_gods (void) {
    archetype *at=NULL;

    LOG(llevDebug, "Initializing gods...\n");
    for(at=first_archetype;at!=NULL;at=at->next)
        if(at->clone.type==GOD) add_god_to_list(at);

    LOG(llevDebug,"done.\n");
}

/**
 * Adds specified god to linked list, gives it an id.
 *
 * @param god_arch
 * God to add. If NULL, will log an error.
 */
static void add_god_to_list (archetype *god_arch) {
    godlink *god;

    if(!god_arch) {
        LOG(llevError,"ERROR: Tried to add null god to list!\n");
        return;
    }

    god = init_godslist();

    god->arch = god_arch;
    god->name=add_string(god_arch->clone.name);
    if(!first_god)
        god->id = 1;
    else {
        god->id = first_god->id + 1;
        god->next = first_god;
    }
    first_god = god;

#ifdef DEBUG_GODS
    LOG(llevDebug,"Adding god %s (%d) to list\n",god->name,god->id);
#endif
}

/**
 * Returns a random god.
 *
 * @return
 * a random god, or NULL if no god was found.
 */
godlink* get_rand_god ( void ) {
    godlink *god=first_god;
    int i;

    if(god)
        for(i=RANDOM()%(god->id)+1;god;god=god->next)
            if(god->id==i) break;

    if(!god) LOG(llevError,"get_rand_god(): can't find a random god!\n");
    return god;
}

/** Returns a pointer to the object
 * We need to be VERY carefull about using this, as we
 * are returning a pointer to the CLONE object. -b.t.
 *
 * @param godlnk
 * god to get object.
 */
object *pntr_to_god_obj(godlink *godlnk) {
    object *god=NULL;

    if(godlnk && godlnk->arch)
        god=&godlnk->arch->clone;
    return god;
}

/**
 * Frees all god information.
 */
void free_all_god(void) {
    godlink *god, *godnext;

    LOG(llevDebug,"Freeing god information\n");
    for (god=first_god; god; god=godnext) {
        godnext=god->next;
        if (god->name)
            free_string(god->name);
        free(god);
    }
    first_god = NULL;
}

/**
 * Prints all gods to stderr.
 *
 * @todo
 * use LOG instead of fprintf().
 */
void dump_gods(void) {
    godlink *glist;

    fprintf(stderr,"\n");
    for(glist=first_god;glist;glist=glist->next) {
        object *god=pntr_to_god_obj(glist);
        char tmpbuf[HUGE_BUF];
        int tmpvar,gifts=0;

        fprintf(stderr,"GOD: %s\n",god->name);
        fprintf(stderr," avatar stats:\n");
        fprintf(stderr,"  S:%d C:%d D:%d I:%d W:%d P:%d\n",
        god->stats.Str,god->stats.Con,god->stats.Dex,
        god->stats.Int,god->stats.Wis,god->stats.Pow);
        fprintf(stderr,"  lvl:%d speed:%4.2f\n",
        god->level,god->speed);
        fprintf(stderr,"  wc:%d ac:%d hp:%d dam:%d \n",
        god->stats.wc,god->stats.ac,god->stats.hp,god->stats.dam);
        fprintf(stderr," enemy: %s\n",god->title?god->title:"NONE");
        if(god->other_arch) {
            object *serv = &god->other_arch->clone;
            fprintf(stderr," servant stats: (%s)\n",god->other_arch->name);
            fprintf(stderr,"  S:%d C:%d D:%d I:%d W:%d P:%d\n",
            serv->stats.Str,serv->stats.Con,serv->stats.Dex,
            serv->stats.Int,serv->stats.Wis,serv->stats.Pow);
            fprintf(stderr,"  lvl:%d speed:%4.2f\n",
            serv->level,serv->speed);
            fprintf(stderr,"  wc:%d ac:%d hp:%d dam:%d \n",
            serv->stats.wc,serv->stats.ac,serv->stats.hp,serv->stats.dam);
        } else
            fprintf(stderr," servant: NONE\n");
        fprintf(stderr," aligned_race(s): %s\n",god->race);
        fprintf(stderr," enemy_race(s): %s\n",(god->slaying?god->slaying:"none"));
        describe_resistance(god, 1, tmpbuf, HUGE_BUF);
        fprintf(stderr,"%s", tmpbuf);
        sprintf(tmpbuf," attacktype:");
        if((tmpvar=god->attacktype)) {
            strcat(tmpbuf,"\n  ");
            DESCRIBE_ABILITY(tmpbuf, tmpvar, "Attacks");
        }
        strcat(tmpbuf,"\n aura:");

        strcat(tmpbuf,"\n paths:");
        if((tmpvar=god->path_attuned)) {
            strcat(tmpbuf,"\n  ");
            DESCRIBE_PATH(tmpbuf, tmpvar, "Attuned");
        }
        if((tmpvar=god->path_repelled)) {
            strcat(tmpbuf,"\n  ");
            DESCRIBE_PATH(tmpbuf, tmpvar, "Repelled");
        }
        if((tmpvar=god->path_denied)) {
            strcat(tmpbuf,"\n  ");
            DESCRIBE_PATH(tmpbuf, tmpvar, "Denied");
        }
        fprintf(stderr,"%s\n",tmpbuf);
        fprintf(stderr," Desc: %s",god->msg?god->msg:"---\n");
        fprintf(stderr," Priest gifts/limitations: ");
        if(!QUERY_FLAG(god,FLAG_USE_WEAPON)) {gifts=1; fprintf(stderr,"\n  weapon use is forbidden");}
        if(!QUERY_FLAG(god,FLAG_USE_ARMOUR)) {gifts=1; fprintf(stderr,"\n  no armour may be worn");}
        if(QUERY_FLAG(god,FLAG_UNDEAD)) {gifts=1; fprintf(stderr,"\n  is undead");}
        if(QUERY_FLAG(god,FLAG_SEE_IN_DARK)) {gifts=1; fprintf(stderr,"\n  has infravision ");}
        if(QUERY_FLAG(god,FLAG_XRAYS)) {gifts=1; fprintf(stderr,"\n  has X-ray vision");}
        if(QUERY_FLAG(god,FLAG_REFL_MISSILE)) {gifts=1; fprintf(stderr,"\n  reflect missiles");}
        if(QUERY_FLAG(god,FLAG_REFL_SPELL)) {gifts=1; fprintf(stderr,"\n  reflect spells");}
        if(QUERY_FLAG(god,FLAG_STEALTH)) {gifts=1; fprintf(stderr,"\n  is stealthy");}
        if(QUERY_FLAG(god,FLAG_MAKE_INVIS)) {gifts=1; fprintf(stderr,"\n  is (permanently) invisible");}
        if(QUERY_FLAG(god,FLAG_BLIND)) {gifts=1; fprintf(stderr,"\n  is blind");}
        if(god->last_heal) {gifts=1; fprintf(stderr,"\n  hp regenerate at %d",god->last_heal);}
        if(god->last_sp) {gifts=1; fprintf(stderr,"\n  sp regenerate at %d",god->last_sp);}
        if(god->last_eat) {gifts=1; fprintf(stderr,"\n  digestion is %s (%d)",
            god->last_eat<0?"slowed":"faster",god->last_eat);}
        if(god->last_grace) {gifts=1; fprintf(stderr,"\n  grace regenerates at %d",god->last_grace);}
        if(god->stats.luck) {gifts=1; fprintf(stderr,"\n  luck is %d",god->stats.luck);}
        if(!gifts) fprintf(stderr,"NONE");
        fprintf(stderr,"\n\n");
    }
}
