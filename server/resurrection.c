/*
 * static char *rcsid_resurrection_c =
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
 * @file
 * Resurrection / raise dead related code.
 * This is used on servers with permanent death on.
 * @todo document permanent death and death :)
 */

/*  the contents of this file were create solely by peterm@soda.berkeley.edu
    all of the above disclaimers apply.  */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <errno.h>

static int resurrection_fails(int levelcaster,int leveldead);


/**
 * Resurrect a player. This may change the player's race, or reduce experience.
 *
 * @param op
 * who is resurrecting.
 * @param playername
 * the name of the player to resurrect.
 * @param spell
 * spell that was used to resurrect.
 * @retval 0
 * resurrection failed.
 * @retval 1
 * playername is living again.
 */
static int resurrect_player(object *op,char *playername,object *spell)
{
    FILE *deadplayer,*liveplayer;

    char oldname[MAX_BUF];
    char newname[MAX_BUF];
    char path[MAX_BUF];
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
    const char *race=NULL;
    sint64 exp;
    int Con;


    /* reincarnation, which changes the race */
    if (spell->race) {
	treasurelist *tl = find_treasurelist(spell->race);
	treasure *t;
	int value;
	if (!tl) {
	    LOG(llevError,"resurrect_player: race set to %s, but no treasurelist of that name!\n", spell->race);
	    return 0;
	}
	value = RANDOM() % tl->total_chance;
	for (t=tl->items; t; t=t->next) {
	    value -= t->chance;
	    if (value<0) break;
	}
	if (!t) {
	    LOG(llevError,"resurrect_player: got null treasure from treasurelist %s!\n", spell->race);
	    return 0;
	}
	race = t->item->name;
    }

    /*  set up our paths/strings...  */
    sprintf(path,"%s/%s/%s/%s",settings.localdir,settings.playerdir,playername,
	  playername);

    strcpy(newname,path);
    strcat(newname,".pl");

    strcpy(oldname,newname);
    strcat(oldname,".dead");

    if(! (deadplayer=fopen(oldname,"r"))) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		     "The soul of %s cannot be reached.",
		     "The soul of %s cannot be reached.",
		     playername);
	return 0;
    }

    if(!access(newname,0)) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			     "The soul of %s has already been reborn!",
			     "The soul of %s has already been reborn!",
			     playername);
	fclose(deadplayer);
	return 0;
    }

    if(! (liveplayer=fopen(newname,"w"))) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		     "The soul of %s cannot be re-embodied at the moment.",
		     "The soul of %s cannot be re-embodied at the moment.",
		     playername);
	LOG(llevError,"Cannot write player in resurrect_player!\n");
	fclose(deadplayer);
	return 0;
    }

    while (!feof(deadplayer)) {
	fgets(buf,255,deadplayer);
	sscanf(buf,"%s",buf2);
	if( ! (strcmp(buf2,"exp"))) {
	    sscanf(buf,"%s %" FMT64, buf2, &exp);
	    if (spell->stats.exp) {
		exp-=exp/spell->stats.exp;
		sprintf(buf,"exp %" FMT64 "\n",exp);
	    }
	}
	if(! (strcmp(buf2,"Con"))) {
	    sscanf(buf,"%s %d",buf2,&Con);
	    Con -= spell->stats.Con;
	    if (Con < 1) Con = 1;
	    sprintf(buf,"Con %d\n",Con);
	}
	if(race && !strcmp(buf2,"race")) {
	    sprintf(buf,"race %s\n",race);
	}
	fputs(buf,liveplayer);
    }
    fclose(liveplayer);
    fclose(deadplayer);
    unlink(oldname);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_SUCCESS,
			 "%s lives again!",
			 "%s lives again!",
			 playername);

    return 1;
}


/**
 * This handles the raise dead / resurrection spells. So try to revive a player.
 * 
 * @author peterm and mehlhaff@soda.berkeley.edu
 *
 * @param op
 * who is doing the resurrecting.
 * @param caster
 * what is casting the spell (op or a scroll/rod).
 * @param spell
 * spell object.
 * @param dir
 * direction the spell is cast.
 * @param arg
 * name of the player to revive.
 * @retval 0
 * spell had no effect, or player couldn't revive.
 * @retval 1
 * player revived, or some nasty things happened.
 */
int cast_raise_dead_spell(object *op, object *caster, object *spell, int dir, const char *arg)
{
    object *temp, *newob;
    char name_to_resurrect[MAX_BUF];
    int leveldead=25, mflags, clevel;
    sint16 sx, sy;
    mapstruct *m;

    clevel = caster_level(caster, spell);

    if (spell->last_heal) {
	if (!arg) {
	    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
				 "Cast %s on who?",
				 "Cast %s on who?",
				 spell->name);
	    return 0;
	}
	strcpy(name_to_resurrect, arg);
	temp = NULL;
    } else {
	sx = op->x+freearr_x[dir];
	sy = op->y+freearr_y[dir];
	m = op->map;
	mflags = get_map_flags(m, &m, sx, sy, &sx, &sy);
	if (mflags & P_OUT_OF_MAP)
	    temp=NULL;
	else {
	    /*  First we need to find a corpse, if any.  */
	    /* If no object, temp will be set to NULL */
	    for(temp=get_map_ob(m, sx, sy); temp!=NULL; temp=temp->above)
		/* If it is corpse, this must be what we want to raise */
		if(temp->type == CORPSE)
		    break;
	}

	if(temp == NULL) {
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			  "You need a body for this spell.", NULL);
	    return 0;
	}
	strcpy(name_to_resurrect, temp->name );
    }

     /* no matter what, we fry the corpse.  */
    if( temp && temp->map){
	/* replace corpse object with a burning object */
	newob = arch_to_object(find_archetype("burnout"));
	if(newob != NULL){
	    newob->x = temp->x;
	    newob->y = temp->y;
	    insert_ob_in_map( newob, temp->map, op,0);
	}
	leveldead=temp->level;
	remove_ob(temp);
	free_object(temp);
    }

    if(resurrection_fails(clevel,leveldead)) {
	if (spell->randomitems) {
	    treasure *t;

	    for (t=spell->randomitems->items; t; t=t->next) {
		summon_hostile_monsters(op, t->nrof, t->item->name);
	    }

	}
	return 1;

    } else {
	    return resurrect_player(op,name_to_resurrect,spell);
    }

    return 1;
}

/**
 * Will the resurrection succeed?
 *
 * Rules:
 * - equal in level, 50% success.
 * - +5 % for each level below, -5% for each level above.
 * - minimum 20%
 *
 * @param levelcaster
 * level at which the spell is cast.
 * @param leveldead
 * dead player's level.
 * @return
 * 0 if succees, 1 if failure.
 */
static int resurrection_fails(int levelcaster,int leveldead)
{
    int chance=9;
    chance+=levelcaster-leveldead;
    if(chance<4) chance=4;
    if(chance>rndm(0, 19)) return 0;  /* resurrection succeeds */
    return 1;
}

/**
 * Kill a player on a permanent death server with resurrection.
 *
 * @param op
 * player to kill.
 */
void dead_player(object *op)
{
    char filename[MAX_BUF];
    char newname[MAX_BUF];
    char path[MAX_BUF];

    /*  set up our paths/strings...  */
    sprintf(path,"%s/%s/%s/%s",settings.localdir,settings.playerdir,op->name,
	  op->name);

    strcpy(filename,path);
    strcat(filename,".pl");
    strcpy(newname,filename);
    strcat(newname,".dead");

    if(rename(filename,newname) != 0) {
	LOG(llevError, "Cannot rename dead player's file %s into %s: %s\n", filename, newname, strerror_local(errno, path, sizeof(path)));
    }
}
