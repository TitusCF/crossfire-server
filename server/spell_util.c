/*
 * static char *rcsid_spell_util_c =
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


#include <global.h>
#include <spells.h>
#include <object.h>
#include <errno.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <sounds.h>

extern char *spell_mapping[];

/* This returns a random spell from 'ob'.  If skill is set, then
 * the spell must be of this skill, it can be NULL in which case all
 * matching spells are used.
 */
object *find_random_spell_in_ob(object *ob, const char *skill)
{
    int k=0,s;
    object *tmp;

    for (tmp=ob->inv; tmp; tmp=tmp->below)
	if (tmp->type == SPELL && (!skill || tmp->skill == skill)) k++;

    /* No spells, no need to progess further */
    if (!k) return NULL;

    s = RANDOM() % k;

    for (tmp=ob->inv; tmp; tmp=tmp->below)
	if (tmp->type == SPELL && (!skill || tmp->skill == skill)) {
	    if (!s) return tmp;
	    else s--;
	}
    /* Should never get here, but just in case */
    return NULL;
}

/* Relatively simple function that gets used a lot.
 * Basically, it sets up the skill pointer for the spell being
 * cast.  If op is really casting the spell, then the skill
 * is whatever skill the spell requires.
 * if instead caster (rod, horn, wand, etc) is casting the skill,
 * then they get exp for the skill that you need to use for
 * that object (use magic device).
 */
void set_spell_skill(object *op, object *caster, object *spob, object *dest)
{
    if (dest->skill) FREE_AND_CLEAR_STR(dest->skill);
    if (caster == op && spob->skill)
	dest->skill = add_refcount(spob->skill);
    else if (caster->skill)
	dest->skill = add_refcount(caster->skill);
}

/* init_spells: This should really be called check_spells, as that
 * is what it does.  It goes through the spells looking for any
 * obvious errors.  This was most useful in debugging when re-doing
 * all the spells to catch simple errors.  To use it all the time
 * will result in it spitting out messages that aren't really errors.
 */
void init_spells(void) {
#ifdef SPELL_DEBUG
    static int init_spells_done = 0;
    int i;
    archetype *at;

    if (init_spells_done)
	return;
    LOG(llevDebug, "Checking spells...\n");

    for (at=first_archetype; at; at=at->next) {
	if (at->clone.type == SPELL) {
	    if (at->clone.skill) {
		for (i=1; i<NUM_SKILLS; i++)
		    if (!strcmp(skill_names[i], at->clone.skill)) break;
		if (i==NUM_SKILLS) {
		    LOG(llevError,"Spell %s has improper associated skill %s\n", at->name, at->clone.skill);
		}
	    }
	    /* other_arch is already checked for in the loader */
	}
    }

    i=0;
    while (spell_mapping[i]) {
	if (!find_archetype(spell_mapping[i])) {
	    LOG(llevError,"Unable to find spell mapping %s (%i)\n", spell_mapping[i], i);
	}
	i++;
    }
    LOG(llevDebug, "Checking spells completed.\n");
#endif
}

/* Dumps all the spells - now also dumps skill associated with the spell.
 * not sure what this would be used for, as the data seems pretty
 * minimal, but easy enough to keep around.
 */
void dump_spells(void)
{
    archetype *at;

    for (at=first_archetype; at; at=at->next) {
	if (at->clone.type == SPELL) {
	    fprintf(stderr, "%s:%s:%s:%s:%d\n", at->clone.name?at->clone.name:"null",
		    at->name, at->clone.other_arch?at->clone.other_arch->name:"null",
		    at->clone.skill?at->clone.skill:"null", at->clone.level);
	}
    }
}

/* pretty basic function - basically just takes
 * an object, sets the x,y, and calls insert_ob_in_map
 */

void spell_effect (object *spob, int x, int y, mapstruct *map,
	object *originator)
{

    if (spob->other_arch !=  NULL) {
	object *effect = arch_to_object(spob->other_arch);

	effect->x = x;
	effect->y = y;

	insert_ob_in_map(effect, map, originator,0);
    }
}

/*
 * This function takes a caster and spell and presents the
 * effective level the caster needs to be to cast the spell.
 * basically, it just adjusts the spell->level with attuned/repelled
 * spellpaths.  Was called path_level_mod
 *
 * caster is person casting hte spell.
 * spell is the spell object.
 * Returns modified level.
 */
int min_casting_level(object *caster, object *spell)
{
    int new_level;

    if (caster->path_denied & spell->path_attuned) {
	/* This case is not a bug, just the fact that this function is
	 * usually called BEFORE checking for path_deny. -AV
	 */
#if 0
	LOG (llevError, "BUG: path_level_mod (arch %s, name %s): casting denied "
	     "spell\n", caster->arch->name, caster->name);
#endif
	return 1;
    }
    new_level = spell->level
             + ((caster->path_repelled & spell->path_attuned) ? +2 : 0)
             + ((caster->path_attuned & spell->path_attuned) ? -2 : 0);
    return (new_level < 1) ? 1 : new_level;
}


/* This function returns the effective level the spell
 * is being cast at.
 * Note that I changed the repelled/attuned bonus to 2 from 5.
 * This is because the new code compares casting_level against
 * min_caster_level, so the difference is effectively 4
 */

int caster_level(object *caster, object *spell)
{
    int level= caster->level;

    /* If this is a player, try to find the matching skill */
    if (caster->type == PLAYER && spell->skill) {
	int i;

	for (i=0; i < NUM_SKILLS; i++)
	    if (caster->contr->last_skill_ob[i] &&
		caster->contr->last_skill_ob[i]->skill == spell->skill) {
		level = caster->contr->last_skill_ob[i]->level;
		break;
	    }
    }
    /* Got valid caster level.  Now adjust for attunement */
    level += ((caster->path_repelled & spell->path_attuned) ? -2 : 0)
             + ((caster->path_attuned & spell->path_attuned) ? 2 : 0);

    /* Always make this at least 1.  If this is zero, we get divide by zero
     * errors in various places.
     */
    if (level < 1) level =1;
    return level;
}

/* The following function scales the spellpoint cost of
 * a spell by it's increased effectiveness.  Some of the
 * lower level spells become incredibly vicious at high
 * levels.  Very cheap mass destruction.  This function is
 * intended to keep the sp cost related to the effectiveness.
 * op is the player/monster
 * caster is what is casting the spell, can be op.
 * spell is the spell object.
 * Note that it is now possible for a spell to cost both grace and
 * mana.  In that case, we return which ever value is higher.
 */

sint16 SP_level_spellpoint_cost(object *caster, object *spell, int flags)
{
    int sp, grace, level = caster_level(caster, spell);

    if (settings.spellpoint_level_depend == TRUE) {
	if (spell->stats.sp && spell->stats.maxsp) {
	    sp= (int) (spell->stats.sp *
	     (1.0 + MAX(0,
		 (float)(level-spell->level)/ (float)spell->stats.maxsp)));
	}
	else sp = spell->stats.sp;

	sp *= PATH_SP_MULT(caster,spell);
	if (!sp && spell->stats.sp) sp=1;

	if (spell->stats.grace && spell->stats.maxgrace) {
	    grace= (int) (spell->stats.grace *
	     (1.0 + MAX(0,
		 (float)(level-spell->level)/ (float)spell->stats.maxgrace)));
	}
	else grace = spell->stats.grace;

	grace *= PATH_SP_MULT(caster,spell);
	if (spell->stats.grace && !grace) grace=1;
    } else {
	sp = spell->stats.sp * PATH_SP_MULT(caster,spell);
	if (spell->stats.sp && !sp) sp=1;
	grace = spell->stats.grace * PATH_SP_MULT(caster,spell);
	if (spell->stats.grace && !grace) grace=1;
    }
    if (flags == SPELL_HIGHEST)
	return MAX(sp, grace);
    else if (flags == SPELL_GRACE)
	return grace;
    else if (flags == SPELL_MANA)
	return sp;
    else {
	LOG(llevError, "SP_level_spellpoint_cost: Unknown flags passed: %d\n", flags);
	return 0;
    }
}


/* SP_level_dam_adjust: Returns adjusted damage based on the caster.
 * spob is the spell we are adjusting.
 */
int SP_level_dam_adjust(object *caster, object *spob)
{
    int level = caster_level (caster, spob);
    int adj = level - min_casting_level(caster, spob);

    if(adj < 0) adj=0;
    if (spob->dam_modifier)
	adj/=spob->dam_modifier;
    else adj=0;
    return adj;
}

/* Adjust the strength of the spell based on level.
 * This is basically the same as SP_level_dam_adjust above,
 * but instead looks at the level_modifier value.
 */
int SP_level_duration_adjust(object *caster, object *spob)
{
    int level = caster_level (caster, spob);
    int adj = level - min_casting_level(caster, spob);

    if(adj < 0) adj=0;
    if(spob->duration_modifier)
	adj/=spob->duration_modifier;
    else adj=0;

    return adj;
}

/* Adjust the strength of the spell based on level.
 * This is basically the same as SP_level_dam_adjust above,
 * but instead looks at the level_modifier value.
 */
int SP_level_range_adjust(object *caster, object *spob)
{
    int level = caster_level (caster, spob);
    int adj = level - min_casting_level(caster, spob);

    if(adj < 0) adj=0;
    if(spob->range_modifier)
	adj/=spob->range_modifier;
    else adj=0;

    return adj;
}

/* Checks to see if player knows the spell.  If the name is the same
 * as an existing spell, we presume they know it.
 * returns 1 if they know the spell, 0 if they don't.
 */
object *check_spell_known (object *op, const char *name)
{
    object *spop;

    for (spop=op->inv; spop; spop=spop->below)
	if (spop->type == SPELL && !strcmp(spop->name, name)) return spop;

    return NULL;
}


/**
 * Look at object 'op' and see if they know the spell
 * spname. This is pretty close to check_spell_known
 * above, but it uses a looser matching mechanism.
 *
 * @param op
 * object we're searching the inventory.
 * @param spname
 * partial spell name.
 * @returns
 * matching spell object, or NULL. If we match multiple spells but don't get an exact match, we also return NULL.
 */

object *lookup_spell_by_name(object *op,const char *spname) {
    object *spob1=NULL, *spob;

    if(spname==NULL) return NULL;

    /* Try to find the spell.  We store the results in spob1
     * and spob2 - spob1 is only taking the length of
     * the past spname, spob2 uses the length of the spell name.
     */
    for (spob = op->inv; spob; spob=spob->below) {
        if (spob->type == SPELL) {
            if (!strncmp(spob->name, spname, strlen(spname))) {
                if (strlen(spname) == strlen(spob->name))
                    /* Perfect match, return it. */
                    return spob;
            if (spob1)
                return NULL;
            spob1 = spob;
            }
        }
    }
    return spob1;
}

/* reflwall - decides weither the (spell-)object sp_op will
 * be reflected from the given mapsquare. Returns 1 if true.
 * (Note that for living creatures there is a small chance that
 * reflect_spell fails.)
 * Caller should be sure it passes us valid map coordinates
 * eg, updated for tiled maps.
 */
int reflwall(mapstruct *m,int x,int y, object *sp_op) {
    object *op;

    if(OUT_OF_REAL_MAP(m,x,y)) return 0;
    for(op=get_map_ob(m,x,y);op!=NULL;op=op->above)
	if(QUERY_FLAG(op, FLAG_REFL_SPELL) && (!QUERY_FLAG(op, FLAG_ALIVE) ||
			       sp_op->type==LIGHTNING || (rndm(0, 99)) < 90-(sp_op->level/10)))
	return 1;

    return 0;
}

/* cast_create_object: creates object new_op in direction dir
 * or if that is blocked, beneath the player (op).
 * we pass 'caster', but don't use it for anything.
 * This is really just a simple wrapper function .
 * returns the direction that the object was actually placed
 * in.
 */
int cast_create_obj(object *op,object *caster,object *new_op, int dir)
{
    mapstruct *m;
    sint16  sx, sy;

    if(dir &&
      ((get_map_flags(op->map, &m, op->x+freearr_x[dir],op->y+freearr_y[dir], &sx, &sy) & P_OUT_OF_MAP) ||
      OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(m, sx, sy)))) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_INFO,
			  "Something is in the way. You cast it at your feet.", NULL);
	dir = 0;
    }
    new_op->x=op->x+freearr_x[dir];
    new_op->y=op->y+freearr_y[dir];
    if (dir == 0)
	insert_ob_in_map(new_op,op->map,op,INS_BELOW_ORIGINATOR);
    else
	insert_ob_in_map(new_op,op->map,op,0);
    return dir;
}

/* Returns true if it is ok to put spell *op on the space/may provided.
 * immune_stop is basically the attacktype of the spell (why
 * passed as a different value, not sure of).  If immune_stop
 * has the AT_MAGIC bit set, and there is a counterwall
 * on the space, the object doesn't get placed.  if immune stop
 * does not have AT_MAGIC, then counterwalls do not effect the spell.
 *
 */

int ok_to_put_more(mapstruct *m,sint16 x,sint16 y,object *op,int immune_stop) {
    object *tmp;
    int mflags;
    mapstruct *mp;

    mp = m;
    mflags = get_map_flags(m, &mp, x, y, &x, &y);

    if (mflags & P_OUT_OF_MAP) return 0;

    if (OB_TYPE_MOVE_BLOCK(op, GET_MAP_MOVE_BLOCK(mp, x, y))) return 0;

    for(tmp=get_map_ob(mp,x,y);tmp!=NULL;tmp=tmp->above) {
	/* If there is a counterspell on the space, and this
	 * object is using magic, don't progess.  I believe we could
	 * leave this out and let in progress, and other areas of the code
	 * will then remove it, but that would seem to to use more
	 * resources, and may not work as well if a player is standing
	 * on top of a counterwall spell (may hit the player before being
	 * removed.)  On the other hand, it may be more dramatic for the
	 * spell to actually hit the counterwall and be sucked up.
	 */
	if ((tmp->attacktype & AT_COUNTERSPELL) &&
	    (tmp->type != PLAYER) && !QUERY_FLAG(tmp,FLAG_MONSTER) &&
	    (tmp->type != WEAPON) && (tmp->type != BOW) &&
	    (tmp->type != ARROW) && (tmp->type != GOLEM) &&
	    (immune_stop & AT_MAGIC)) return 0;

	/* This is to prevent 'out of control' spells.  Basically, this
	 * limits one spell effect per space per spell.  This is definately
	 * needed for performance reasons, and just for playability I believe.
	 * there are no such things as multispaced spells right now, so
	 * we don't need to worry about the head.
	 */
	if ((tmp->stats.maxhp == op->stats.maxhp) && (tmp->type == op->type) &&
	    (tmp->subtype == op->subtype))
	    return 0;

	/* Perhaps we should also put checks in for no magic and unholy
	 * ground to prevent it from moving along?
	 */
    }
    /* If it passes the above tests, it must be OK */
    return 1;
}

/* fire_arch_from_position: fires an archetype.
 * op: person firing the object.
 * caster: object casting the spell.
 * x, y: where to fire the spell (note, it then uses op->map for the map
 *   for these coordinates, which is probably a really bad idea.
 * dir: direction to fire in.
 * spell: spell that is being fired.  It uses other_arch for the archetype
 * to fire.
 * returns 0 on failure, 1 on success.
 */

int fire_arch_from_position (object *op, object *caster, sint16 x, sint16 y,
	int dir, object *spell)
{
    object *tmp;
    int mflags;
    mapstruct *m;

    if(spell->other_arch==NULL)
	return 0;

    m = op->map;
    mflags = get_map_flags(m, &m, x, y, &x, &y);
    if (mflags & P_OUT_OF_MAP) {
	return 0;
    }

    tmp=arch_to_object(spell->other_arch);

    if(tmp==NULL)
	return 0;

    if (OB_TYPE_MOVE_BLOCK(tmp, GET_MAP_MOVE_BLOCK(m, x, y))) {
        if ( caster->type == PLAYER )
            /* If caster is not player, it's for instance a swarm, so don't say there's an issue. */
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
			  "You can't cast the spell on top of a wall!", NULL);
	free_object(tmp);
	return 0;
    }



    tmp->stats.dam=spell->stats.dam+SP_level_dam_adjust(caster,spell);
    tmp->duration=spell->duration+SP_level_duration_adjust(caster,spell);
    /* code in time.c uses food for some things, duration for others */
    tmp->stats.food = tmp->duration;
    tmp->range=spell->range+SP_level_range_adjust(caster,spell);
    tmp->attacktype = spell->attacktype;
    tmp->x=x;
    tmp->y=y;
    tmp->direction=dir;
    if (get_owner (op) != NULL)
	copy_owner (tmp, op);
    else
	set_owner (tmp, op);
    tmp->level = caster_level (caster, spell);
    set_spell_skill(op, caster, spell, tmp);

    /* needed for AT_HOLYWORD,AT_GODPOWER stuff */
    if(tmp->attacktype&AT_HOLYWORD || tmp->attacktype&AT_GODPOWER) {
	if(!tailor_god_spell(tmp,op)) return 0;
    }
    if(QUERY_FLAG(tmp, FLAG_IS_TURNABLE))
	SET_ANIMATION(tmp, dir);

    if ((tmp = insert_ob_in_map (tmp, m, op,0)) == NULL)
	return 1;

    move_spell_effect(tmp);

    return 1;
}



/*****************************************************************************
 *
 * Code related to rods - perhaps better located in another file?
 *
 ****************************************************************************/

void regenerate_rod(object *rod) {
    if (rod->stats.hp < rod->stats.maxhp) {
	rod->stats.hp+= 1 + rod->stats.maxhp/10;

	if (rod->stats.hp > rod->stats.maxhp)
	    rod->stats.hp = rod->stats.maxhp;
    }
}


void drain_rod_charge(object *rod) {
    rod->stats.hp -= SP_level_spellpoint_cost(rod, rod->inv, SPELL_HIGHEST);
}




/* this function is commonly used to find a friendly target for
 * spells such as heal or protection or armour
 * op is what is looking for the target (which can be a player),
 * dir is the direction we are looking in.  Return object found, or
 * NULL if no good object.
 */

object *find_target_for_friendly_spell(object *op,int dir) {
    object *tmp;
    mapstruct *m;
    sint16 x, y;
    int mflags;

    /* I don't really get this block - if op isn't a player or rune,
     * we then make the owner of this object the target.
     * The owner could very well be no where near op.
     */
    if(op->type!=PLAYER && op->type!=RUNE)
    {
        tmp=get_owner(op);
        /* If the owner does not exist, or is not a monster, than apply the spell
         * to the caster.
         */
        if(!tmp || !QUERY_FLAG(tmp,FLAG_MONSTER)) tmp=op;
    }
    else
    {
        m = op->map;
        x =  op->x+freearr_x[dir];
        y =  op->y+freearr_y[dir];

        mflags = get_map_flags(m, &m, x, y, &x, &y);

        if (mflags & P_OUT_OF_MAP)
            tmp=NULL;
        else
        {
            for(tmp=get_map_ob(m, x, y); tmp!=NULL; tmp=tmp->above)
            {
                if(tmp->type==PLAYER)
                    break;
            }
        }
    }
    /* didn't find a player there, look in current square for a player */
    if(tmp==NULL)
        for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above)
        {
            if(tmp->type==PLAYER)
                break;
            /* Don't forget to browse inside transports ! - gros 2006/07/25 */
            if(tmp->type==TRANSPORT)
            {
                object* inv;
                for (inv=tmp->inv; inv; inv=inv->below)
                {
                    if ((inv->type == PLAYER)&&(op == inv))
                        return inv;
                }
            }
        }
    return tmp;
}



/* raytrace:
 * spell_find_dir(map, x, y, exclude) will search first the center square
 * then some close squares in the given map at the given coordinates for
 * live objects.
 * It will not consider the object given as exclude (= caster) among possible
 * live objects. If the caster is a player, the spell will go after
 * monsters/generators only. If not, the spell will hunt players only.
 * Exception is player on a battleground, who will be targeted unless excluded.
 * It returns the direction toward the first/closest live object if it finds
 * any, otherwise -1.
 * note that exclude can be NULL, in which case all bets are off.
 */

int spell_find_dir(mapstruct *m, int x, int y, object *exclude) {
    int i,max=SIZEOFFREE;
    sint16 nx,ny;
    int owner_type=0, mflags;
    object *tmp;
    mapstruct *mp;

    if (exclude && exclude->head)
	exclude = exclude->head;
    if (exclude && exclude->type)
	owner_type = exclude->type;

    for(i=rndm(1, 8);i<max;i++) {
	nx = x + freearr_x[i];
	ny = y + freearr_y[i];
	mp = m;
	mflags = get_map_flags(m, &mp, nx, ny, &nx, &ny);
	if (mflags & (P_OUT_OF_MAP | P_BLOCKSVIEW)) continue;

	tmp=get_map_ob(mp,nx,ny);

	while(tmp!=NULL && (
             ((owner_type==PLAYER && !QUERY_FLAG(tmp,FLAG_MONSTER) && !QUERY_FLAG(tmp,FLAG_GENERATOR) && !(tmp->type == PLAYER && op_on_battleground(tmp, NULL, NULL)))
              || (owner_type!=PLAYER && tmp->type!=PLAYER))
             || (tmp == exclude || (tmp->head && tmp->head == exclude))))
	    tmp=tmp->above;

	if(tmp!=NULL && can_see_monsterP(m,x,y,i))
	    return freedir[i];
    }
    return -1;  /* flag for "keep going the way you were" */
}



/* put_a_monster: puts a monster named monstername near by
 * op.  This creates the treasures for the monsters, and
 * also deals with multipart monsters properly.
 */

static void put_a_monster(object *op,const char *monstername) {
    object *tmp,*head=NULL,*prev=NULL;
    archetype *at;
    int dir;

    /* Handle cases where we are passed a bogus mosntername */

    if((at=find_archetype(monstername))==NULL) return;

    /* find a free square nearby
     * first we check the closest square for free squares
     */

    dir=find_first_free_spot(&at->clone,op->map,op->x,op->y);
    if(dir!=-1) {
	/* This is basically grabbed for generate monster.  Fixed 971225 to
	 * insert multipart monsters properly
	 */
	while (at!=NULL) {
	    tmp=arch_to_object(at);
	    tmp->x=op->x+freearr_x[dir]+at->clone.x;
	    tmp->y=op->y+freearr_y[dir]+at->clone.y;
	    tmp->map = op->map;
	    if (head) {
		tmp->head=head;
		prev->more=tmp;
	    }
	    if (!head) head=tmp;
	    prev=tmp;
	    at=at->more;
	}

	if (head->randomitems)
	    create_treasure(head->randomitems, head, GT_INVISIBLE, op->map->difficulty,0);

	insert_ob_in_map(head,op->map,op,0);

	/* thought it'd be cool to insert a burnout, too.*/
	tmp=create_archetype("burnout");
	tmp->map = op->map;
	tmp->x=op->x+freearr_x[dir];
	tmp->y=op->y+freearr_y[dir];
	insert_ob_in_map(tmp,op->map,op,0);
    }
}

/* peterm:  function which summons hostile monsters and
 * places them in nearby squares.
 * op is the summoner.
 * n is the number of monsters.
 * monstername is the name of the monster.
 * returns the number of monsters, which is basically n.
 * it should really see how many it successfully replaced and
 * return that instead.
 * Note that this is not used by any spells (summon evil monsters
 * use to call this, but best I can tell, that spell/ability was
 * never used.  This is however used by various failures on the
 * players part (alchemy, reincarnation, etc)
 */

int summon_hostile_monsters(object *op,int n,const char *monstername){
    int i;
    for(i=0;i<n;i++)
	put_a_monster(op,monstername);

    return n;
}


/*  Some local definitions for shuffle-attack */
    struct {
	int attacktype;
	int face;
    } ATTACKS[22] = {
	{AT_PHYSICAL,0},
	{AT_PHYSICAL,0},  /*face = explosion*/
	{AT_PHYSICAL,0},
	{AT_MAGIC,1},
	{AT_MAGIC,1},   /* face = last-burnout */
	{AT_MAGIC,1},
	{AT_FIRE,2},
	{AT_FIRE,2},  /* face = fire....  */
	{AT_FIRE,2},
	{AT_ELECTRICITY,3},
	{AT_ELECTRICITY,3},  /* ball_lightning */
	{AT_ELECTRICITY,3},
	{AT_COLD,4},
	{AT_COLD,4},  /* face=icestorm*/
	{AT_COLD,4},
	{AT_CONFUSION,5},
	{AT_POISON,7},
	{AT_POISON,7}, /* face = acid sphere.  generator */
	{AT_POISON,7},  /* poisoncloud face */
	{AT_SLOW,8},
	{AT_PARALYZE,9},
	{AT_FEAR,10}  };



/* shuffle_attack:  peterm
 * This routine shuffles the attack of op to one of the
 * ones in the list.  It does this at random.  It also
 *  chooses a face appropriate to the attack that is
 *  being committed by that square at the moment.
 *  right now it's being used by color spray and create pool of
 *  chaos.
 * This could really be a better implementation - the
 * faces and attacktypes above are hardcoded, which is never
 * good.  The faces refer to faces in the animation sequence.
 * Not sure how to do better - but not having it hardcoded
 * would be nice.
 * I also fixed a bug here in that attacktype was |= -
 * to me, that would be that it would quickly get all
 * attacktypes, which probably wasn't the intent.  MSW 2003-06-03
 */
void shuffle_attack(object *op,int change_face)
{
    int i;
    i=rndm(0, 21);

    op->attacktype=ATTACKS[i].attacktype|AT_MAGIC;

    if(change_face) {
	SET_ANIMATION(op, ATTACKS[i].face);
    }
}


/* prayer_failure: This is called when a player fails
 * at casting a prayer.
 * op is the player.
 * failure is basically how much grace they had.
 * power is how much grace the spell would normally take to cast.
 */

static void prayer_failure(object *op, int failure, int power)
{
    const char *godname;
    object *tmp;

    if(!strcmp((godname=determine_god(op)),"none")) godname="Your spirit";

    if(failure<= -20 && failure > -40) /* wonder */
    {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			     "%s gives a sign to renew your faith.",
			     "%s gives a sign to renew your faith.",
			     godname);
	tmp = create_archetype(SPELL_WONDER);
	cast_cone(op,op,0,tmp);
	free_object(tmp);
    }

    else if (failure <= -40 && failure > -60) /* confusion */
    {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "Your diety touches your mind!", NULL);
	confuse_player(op,op,99);
    }
    else if (failure <= -60 && failure> -150) /* paralysis */
    {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		     "%s requires you to pray NOW. You comply, ignoring all else.",
		     "%s requires you to pray NOW. You comply, ignoring all else.",
		     godname);

	paralyze_player(op,op,99);
    }
    else if (failure <= -150) /* blast the immediate area */
    {
	tmp = create_archetype(GOD_POWER);
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			     "%s smites you!",
			     "%s smites you!",
			     godname);
	cast_magic_storm(op,tmp, power);
    }
}

/*
 * spell_failure()  handles the various effects for differing degrees
 * of failure badness.
 * op is the player that failed.
 * failure is a random value of how badly you failed.
 * power is how many spellpoints you'd normally need for the spell.
 * skill is the skill you'd need to cast the spell.
 */

void spell_failure(object *op, int failure,int power, object *skill)
{
    object *tmp;

    if (settings.spell_failure_effects == FALSE)
	return;

    if (failure<=-20 && failure > -40) /* wonder */
    {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "Your spell causes an unexpected effect.", NULL);
	tmp = create_archetype(SPELL_WONDER);
	cast_cone(op,op,0,tmp);
	free_object(tmp);
    }

    else if (failure <= -40 && failure > -60) /* confusion */
    {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "Your magic recoils on you, making you confused!", NULL);
	confuse_player(op,op,99);
    }
    else if (failure <= -60 && failure> -80) /* paralysis */
    {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
		      "Your magic stuns you!", NULL);
	paralyze_player(op,op,99);
    }
    else if (failure <= -80) /* blast the immediate area */
    {
	object *tmp;
	/* Safety check to make sure we don't get any mana storms in scorn */
	if (get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL) & P_NO_MAGIC) {
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			  "The magic warps and you are turned inside out!", NULL);
	    hit_player(op,9998,op,AT_INTERNAL,1);

	} else {
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			  "You lose control of the mana! The uncontrolled magic blasts you!",
			  NULL);
	    tmp=create_archetype(LOOSE_MANA);
	    tmp->level=skill->level;
	    tmp->x=op->x;
	    tmp->y=op->y;

	    /* increase the area of destruction a little for more powerful spells */
	    tmp->range+=isqrt(power);

	    if (power>25) tmp->stats.dam = 25 + isqrt(power);
	    else tmp->stats.dam=power; /* nasty recoils! */

	    tmp->stats.maxhp=tmp->count;
	    insert_ob_in_map(tmp,op->map,NULL,0);
	}
    }
}

static int cast_party_spell(object *op, object *caster,int dir,object *spell_ob, char *stringarg)
    {
    int success;
    player *pl;
    object *spell;

    if ( !spell_ob->other_arch )
        {
        LOG( llevError, "cast_party_spell: empty other arch\n" );
        return 0;
        }
    spell = arch_to_object( spell_ob->other_arch );

    /* Always cast spell on caster */
    success = cast_spell( op, caster, dir, spell, stringarg );

    if ( caster->contr->party == NULL )
        {
        remove_ob( spell );
        return success;
        }
    for( pl=first_player; pl!=NULL; pl=pl->next )
        if( ( pl->ob->contr->party == caster->contr->party ) && ( on_same_map( pl->ob, caster ) ) )
            {
            cast_spell( pl->ob, caster, pl->ob->facing, spell, stringarg );
            }
    remove_ob( spell );
    return success;
    }

/**
 * This transforms one random item of op to a flower.
 *
 * Only some items are considered, mostly magical ones.
 *
 * Transformed item is put in a force in a flower, weights are adjusted.
 *
 * See remove_force() in server/time.c to see object removal.
 *
 * @param op
 * player who is the victim.
 */
static void transmute_item_to_flower(object* op) {
    object* force;
    object* item;
    object* flower;
    object* first = NULL;
    int count = 0;
    char name[HUGE_BUF];

    for (item = op->inv; item; item = item->below) {
        if (!item->invisible && (item->type == POTION || item->type == SCROLL || item->type == WAND || item->type == ROD || item->type == WEAPON)) {
            if (!first)
                first = item;
            count++;
        }
    }

    if (count == 0)
        return;

    count = rndm(0, count-1);
    for (item = first; item && count>0; item = item->below) {
        if (!item->invisible && (item->type == POTION || item->type == SCROLL || item->type == WAND || item->type == ROD || item->type == WEAPON)) {
        count--;
        }
    }

    if (!item)
        return;

    force = create_archetype(FORCE_NAME);
    force->duration = 100 + rndm(0,10)*100;
    force->subtype = FORCE_TRANSFORMED_ITEM;
    force->speed = 1;
    update_ob_speed(force);

    flower = create_archetype("flowers_permanent");

    if (QUERY_FLAG(item, FLAG_APPLIED))
        manual_apply(op, item, AP_NOPRINT | AP_IGNORE_CURSE | AP_UNAPPLY);
    remove_ob(item);
    flower->weight = item->nrof ? item->nrof * item->weight : item->weight;
    item->weight = 0;
    esrv_del_item(op->contr, item->count);
    insert_ob_in_ob(item, force);

    query_short_name(item, name, HUGE_BUF);
    draw_ext_info_format(NDI_UNIQUE, 0,op,
        MSG_TYPE_ITEM, MSG_TYPE_ITEM_CHANGE,
        "Your %s turns to a flower!",
        "Your %s turns to a flower!",
        name);

    insert_ob_in_ob(force, flower);
    flower = insert_ob_in_ob(flower, op);
    esrv_send_item(op, flower);
}

/**
 * Randomly swaps 2 stats of op.
 *
 * Swapping is merely a FORCE inserted into op's inventory.
 *
 * Used for spell casting when confused.
 *
 * @param op
 * player who is the victim.
 */
static void swap_random_stats(object* op) {
    object* force;
    int first, second;

    first = RANDOM()%NUM_STATS;
    second = RANDOM()%(NUM_STATS-1);
    if (second>=first)
        second++;

    draw_ext_info_format(NDI_UNIQUE, 0,op,
        MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_SPELL,
        "You suddenly feel really weird!",
        "You suddenly feel really weird!");

    force = create_archetype(FORCE_NAME);
    force->duration = 100 + rndm(0,10)*100;
    force->speed = 1;
    SET_FLAG(force, FLAG_APPLIED);
    set_attr_value(&force->stats, second, get_attr_value(&op->stats, first) - get_attr_value(&op->stats, second));
    set_attr_value(&force->stats, first, get_attr_value(&op->stats, second) - get_attr_value(&op->stats, first));
    update_ob_speed(force);
    insert_ob_in_ob(force, op);
    change_abil(op,force);
    fix_object(op);
}

/**
 * This does a random effect for op, which tried to cast a spell in a confused state.
 *
 * Note that random spell casting is handled in cast_spell itself.
 *
 * Used for spell casting when confused.
 *
 * @param op
 * player for which to produce a random effect.
 */
static void handle_spell_confusion(object* op) {
    switch (RANDOM()%2) {
        case 0:
            transmute_item_to_flower(op);
            break;
        case 1:
            swap_random_stats(op);
            break;
    }
}

/* This is where the main dispatch when someone casts a spell.
 *
 * op is the creature that is owner of the object that is casting the spell -
 *    eg, the player or monster.
 * caster is the actual object (wand, potion) casting the spell. can be
 *    same as op.
 * dir is the direction to cast in.  Note in some cases, if the spell
 *    is self only, dir really doesn't make a difference.
 * spell_ob is the spell object that is being cast.  From that,
 *    we can determine what to do.
 * stringarg is any options that are being used.  It can be NULL.  Almost
 *    certainly, only players will set it.  It is basically used as optional
 *    parameters to a spell (eg, item to create, information for marking runes,
 *    etc.
 * returns 1 on successful cast, or 0 on error.  These values should really
 * be swapped, so that 0 is successful, and non zero is failure, with a code
 * of what it failed.
 *
 * Note that this function is really a dispatch routine that calls other
 * functions - it just blindly returns what ever value those functions
 * return.  So if your writing a new function that is called from this,
 * it shoudl also return 1 on success, 0 on failure.
 *
 * if it is a player casting the spell (op->type == PLAYER, op == caster),
 * this function will decrease teh mana/grace appropriately.  For other
 * objects, the caller should do what it considers appropriate.
 */

int cast_spell(object *op, object *caster,int dir,object *spell_ob, char *stringarg) {

    const char *godname;
    int success=0,mflags, cast_level=0, old_shoottype;
    object *skill=NULL;
    int confusion_effect = 0;

    old_shoottype = op->contr ? op->contr->shoottype : 0;

    if (!spell_ob) {
	LOG(llevError,"cast_spell: null spell object passed\n");
	return 0;
    }
    if(!strcmp((godname=determine_god(op)),"none")) godname="A random spirit";

    /* the caller should set caster to op if appropriate */
    if (!caster) {
	LOG(llevError,"cast_spell: null caster object passed\n");
	return 0;
    }

    /* Handle some random effect if confused. */
    if (QUERY_FLAG(op, FLAG_CONFUSED) && caster == op && op->type == PLAYER) {
        if (rndm(0, 5) < 4) {
            spell_ob = find_random_spell_in_ob(op, NULL);
            draw_ext_info_format(NDI_UNIQUE, 0,op,
                MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
                "In your confused state, you're not sure of what you cast!",
                "In your confused state, you're not sure of what you cast!");
        }
        else
            /* We fall through to deplate sp/gr, and do some checks. */
            confusion_effect = 1;
    }

    /* if caster is a spell casting object, this normally shouldn't be
     * an issue, because they don't have any spellpaths set up.
     */
    if ((caster->path_denied & spell_ob->path_attuned) && !QUERY_FLAG(caster,FLAG_WIZ)) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
		      "That spell path is denied to you.", NULL);
	return 0;
    }

    /* if it is a player casting the spell, and they are really casting it
     * (vs it coming from a wand, scroll, or whatever else), do some
     * checks.  We let monsters do special things - eg, they
     * don't need the skill, bypass level checks, etc. The monster function
     * should take care of that.
     * Remove the wiz check here and move it further down - some spells
     * need to have the right skill pointer passed, so we need to
     * at least process that code.
     */
    if (op->type == PLAYER && op == caster) {
	cast_level = caster_level(caster, spell_ob);
	if (spell_ob->skill) {
	    skill = find_skill_by_name(op, spell_ob->skill);
	    if (!skill) {
		draw_ext_info_format(NDI_UNIQUE, 0,op,
				     MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
				     "You need the skill %s to cast %s.",
				     "You need the skill %s to cast %s.",
				     spell_ob->skill, spell_ob->name);
		return 0;
	    }
	    if (min_casting_level(op, spell_ob) > cast_level && !QUERY_FLAG(op, FLAG_WIZ)) {
		draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			      "You lack enough skill to cast that spell.", NULL);
		return 0;
	    }
	}
	/* If the caster is the wiz, they don't ever fail, and don't have
	 * to have sufficient grace/mana.
	 */
	if (!QUERY_FLAG(op, FLAG_WIZ)) {
	    if (SP_level_spellpoint_cost(caster, spell_ob, SPELL_MANA) &&
		SP_level_spellpoint_cost(caster, spell_ob, SPELL_MANA) >  op->stats.sp) {
		draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
			      "You don't have enough mana.", NULL);
		return 0;
	    }
	    if (SP_level_spellpoint_cost(caster,spell_ob, SPELL_GRACE) &&
		SP_level_spellpoint_cost(caster,spell_ob, SPELL_GRACE) >  op->stats.grace) {
		if(random_roll(0, op->stats.Wis-1, op, PREFER_HIGH) + op->stats.grace -
		   10*SP_level_spellpoint_cost(caster,spell_ob, SPELL_GRACE)/op->stats.maxgrace >0) {
			draw_ext_info_format(NDI_UNIQUE, 0,op,
				     MSG_TYPE_SPELL, MSG_TYPE_SPELL_INFO,
				     "%s grants your prayer, though you are unworthy.",
				     "%s grants your prayer, though you are unworthy.",
				     godname);
		}
		else {
		    prayer_failure(op,op->stats.grace,
			   SP_level_spellpoint_cost(caster,spell_ob, SPELL_GRACE) - op->stats.grace);
		    draw_ext_info_format(NDI_UNIQUE, 0,op,
					 MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
					 "%s ignores your prayer.",
					 "%s ignores your prayer.",
					 godname);
		    return 0;
		}
	    }

	    /* player/monster is trying to cast the spell.  might fumble it */
	    if (spell_ob->stats.grace && random_roll(0, 99, op, PREFER_HIGH) <
	      (spell_ob->level/(float)MAX(1,op->level) *cleric_chance[op->stats.Wis])) {
		play_sound_player_only(op->contr, SOUND_FUMBLE_SPELL,0,0);
		draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			      "You fumble the spell.", NULL);
		if (settings.casting_time == TRUE) {
		    op->casting_time = -1;
		}
		op->stats.grace -= random_roll(1, SP_level_spellpoint_cost(caster,spell_ob, SPELL_GRACE), op, PREFER_LOW);
		return 0;
	    } else if (spell_ob->stats.sp) {
		int failure = random_roll(0, 199, op, PREFER_HIGH) -
		    op->contr->encumbrance +op->level - spell_ob->level +35;

		if( failure < 0) {
		    draw_ext_info(NDI_UNIQUE, 0,op,
			  MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			  "You bungle the spell because you have too much heavy equipment in use.",
			  NULL);
		    if (settings.spell_failure_effects == TRUE)
			spell_failure(op,failure,
			  SP_level_spellpoint_cost(caster,spell_ob, SPELL_MANA),
			  skill);
		    op->contr->shoottype = old_shoottype;
		    op->stats.sp -= random_roll(0, SP_level_spellpoint_cost(caster,spell_ob, SPELL_MANA), op, PREFER_LOW);
		    return 0;
		}
	    }
	}
    }

    mflags = get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL);

    /* See if we can cast a spell here.  If the caster and op are
     * not alive, then this would mean that the mapmaker put the
     * objects on the space - presume that they know what they are
     * doing.
     */
    if (spell_ob->type == SPELL && caster->type != POTION && !QUERY_FLAG(op, FLAG_WIZCAST) &&
        (QUERY_FLAG(caster, FLAG_ALIVE) || QUERY_FLAG(op, FLAG_ALIVE)) && !QUERY_FLAG(op,FLAG_MONSTER) &&
	(((mflags & P_NO_MAGIC) && spell_ob->stats.sp) ||
	   ((mflags & P_NO_CLERIC) && spell_ob->stats.grace))) {

	if (op->type!=PLAYER)
	    return 0;

	if ((mflags & P_NO_CLERIC) && spell_ob->stats.grace)
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
				 "This ground is unholy!  %s ignores you.",
				 "This ground is unholy!  %s ignores you.",
				 godname);
	else
	switch(op->contr->shoottype) {
	    case range_magic:
		draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
			      "Something blocks your spellcasting.", NULL);
		break;
	    case range_misc:
		draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
			      "Something blocks the magic of your item.", NULL);
		break;
	    case range_golem:
		draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
			      "Something blocks the magic of your scroll.", NULL);
		break;
	    default:
		break;
	}
	return 0;
    }

    if (caster == op && settings.casting_time == TRUE && spell_ob->type == SPELL) {
	if (op->casting_time==-1) { /* begin the casting */
	    op->casting_time = spell_ob->casting_time*PATH_TIME_MULT(op,spell_ob);
	    op->spell = spell_ob;
	    /* put the stringarg into the object struct so that when the
	     * spell is actually cast, it knows about the stringarg.
	     * necessary for the invoke command spells.
	     */
	    if(stringarg) {
		op->spellarg = strdup_local(stringarg);
	    }
	    else op->spellarg=NULL;
	    return 0;
	}
	else if (op->casting_time != 0) {
	    if (op->type == PLAYER )
		draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_INFO,
			      "You are casting!", NULL);
	    return 0;
	} else {    /* casting_time == 0 */
	    op->casting_time = -1;
	    spell_ob = op->spell;
	    stringarg = op->spellarg;
	}
    } else {
	/* Take into account how long it takes to cast the spell.
	 * if the player is casting it, then we use the time in
	 * the spell object.  If it is a spell object, have it
	 * take two ticks.  Things that cast spells on the players
	 * behalf (eg, altars, and whatever else) shouldn't cost
	 * the player any time.
	 * Ignore casting time for firewalls
	 */
	if (caster == op && caster->type != FIREWALL) {
	    op->speed_left -= spell_ob->casting_time*PATH_TIME_MULT(op,spell_ob) * FABS(op->speed);
	    /* Other portions of the code may also decrement the speed of the player, so
	     * put a lower limit so that the player isn't stuck here too long
	     */
	    if ((spell_ob->casting_time > 0) &&
		op->speed_left < -spell_ob->casting_time*PATH_TIME_MULT(op,spell_ob) * FABS(op->speed))
		op->speed_left = -spell_ob->casting_time*PATH_TIME_MULT(op,spell_ob) * FABS(op->speed);
	} else if (caster->type == WAND || caster->type == HORN ||
		   caster->type == ROD || caster->type == POTION ||
		   caster->type == SCROLL) {
	    op->speed_left -= 2 * FABS(op->speed);
	}
    }

    if (op->type == PLAYER && op == caster) {
	op->stats.grace -= SP_level_spellpoint_cost(caster,spell_ob, SPELL_GRACE);
	op->stats.sp -= SP_level_spellpoint_cost(caster,spell_ob, SPELL_MANA);
    }

    /* We want to try to find the skill to properly credit exp.
     * for spell casting objects, the exp goes to the skill the casting
     * object requires.
     */
    if (op != caster && !skill && caster->skill) {
	skill = find_skill_by_name(op, caster->skill);
	if (!skill) {
        char name[MAX_BUF];
        query_name(caster, name, MAX_BUF);
	    draw_ext_info_format(NDI_UNIQUE, 0,op,
				 MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
				 "You lack the skill %s to use the %s",
				 "You lack the skill %s to use the %s",
				 caster->skill, name);
	    return 0;
	}
	change_skill(op, skill, 0);    /* needed for proper exp credit */
    }

    /* Need to get proper ownership for spells cast via runes - these are not
     * the normal 'rune of fire', but rather the magic runes that let the player
     * put some other spell into the rune (glyph, firetrap, magic rune, etc)
     */
    if (caster->type == RUNE) {
	object *owner = get_owner(caster);

	if (owner) skill = find_skill_by_name(owner, caster->skill);
    }

    if (confusion_effect) {
        /* If we get here, the confusion effect was 'random effect', so do it and bail out. */
        draw_ext_info_format(NDI_UNIQUE, 0,op,
            MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
            "In your confused state, you can't control the magic!",
            "In your confused state, you can't control the magic!");
        handle_spell_confusion(op);
        return 0;
    }

    switch(spell_ob->subtype) {
	/* The order of case statements is same as the order they show up
	 * in in spells.h.
	 */
	case SP_RAISE_DEAD:
	    success = cast_raise_dead_spell(op,caster, spell_ob, dir,stringarg);
	    break;

	case SP_RUNE:
	    success = write_rune(op,caster, spell_ob, dir,stringarg);
	    break;

	case SP_MAKE_MARK:
	    success = write_mark(op, spell_ob, stringarg);
	    break;

	case SP_BOLT:
	    success = fire_bolt(op,caster,dir,spell_ob,skill);
	    break;

	case SP_BULLET:
	    success = fire_bullet(op, caster, dir, spell_ob);
	    break;

	case SP_CONE:
	    success = cast_cone(op, caster, dir, spell_ob);
	    break;

	case SP_BOMB:
	    success = create_bomb(op,caster,dir, spell_ob);
	    break;

	case SP_WONDER:
	    success = cast_wonder(op,caster, dir,spell_ob);
	    break;

	case SP_SMITE:
	    success = cast_smite_spell(op,caster, dir,spell_ob);
	    break;

	case SP_MAGIC_MISSILE:
	    success = fire_arch_from_position(op, caster, op->x + freearr_x[dir],
				    op->y + freearr_y[dir], dir, spell_ob);
	    break;

	case SP_SUMMON_GOLEM:
	    success = summon_golem(op, caster, dir, spell_ob);
	    old_shoottype = range_golem;
	    break;

	case SP_DIMENSION_DOOR:
	    /* dimension door needs the actual caster, because that is what is
	     * moved.
	     */
	    success = dimension_door(op,caster, spell_ob, dir);
	    break;

	case SP_MAGIC_MAPPING:
	    if(op->type==PLAYER) {
		spell_effect(spell_ob, op->x, op->y, op->map, op);
		draw_magic_map(op);
		success=1;
	    }
	    else success=0;
	    break;

	case SP_MAGIC_WALL:
	    success = magic_wall(op,caster,dir,spell_ob);
	    break;

	case SP_DESTRUCTION:
	    success = cast_destruction(op,caster,spell_ob);
	    break;

	case SP_PERCEIVE_SELF:
	    success = perceive_self(op);
	    break;

	case SP_WORD_OF_RECALL:
	    success = cast_word_of_recall(op,caster,spell_ob);
	    break;

	case SP_INVISIBLE:
	    success = cast_invisible(op,caster,spell_ob);
	    break;

	case SP_PROBE:
	    success = probe(op,caster, spell_ob, dir);
	    break;

	case SP_HEALING:
	    success = cast_heal(op,caster, spell_ob, dir);
	    break;

	case SP_CREATE_FOOD:
	    success = cast_create_food(op,caster,spell_ob, dir,stringarg);
	    break;

	case SP_EARTH_TO_DUST:
	    success = cast_earth_to_dust(op,caster,spell_ob);
	    break;

	case SP_CHANGE_ABILITY:
	    success = cast_change_ability(op,caster,spell_ob, dir, 0);
	    break;

	case SP_BLESS:
	    success = cast_bless(op,caster,spell_ob, dir);
	    break;

	case SP_CURSE:
	    success = cast_curse(op,caster,spell_ob, dir);
	    break;

	case SP_SUMMON_MONSTER:
	    success = summon_object(op,caster,spell_ob, dir,stringarg);
	    break;

	case SP_CHARGING:
	    success = recharge(op, caster, spell_ob);
	    break;

	case SP_POLYMORPH:
#ifdef NO_POLYMORPH
	    /* Not great, but at least provide feedback so if players do have
	     * polymorph (ie, find it as a preset item or left over from before
  	     * it was disabled), they get some feedback.
	     */
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SPELL, MSG_TYPE_SPELL_FAILURE,
			  "The spell fizzles", NULL);
	    success = 0;
#else
	    success = cast_polymorph(op,caster,spell_ob, dir);
#endif
	    break;

	case SP_ALCHEMY:
	    success = alchemy(op, caster, spell_ob);
	    break;

	case SP_REMOVE_CURSE:
	    success = remove_curse(op, caster, spell_ob);
	    break;

	case SP_IDENTIFY:
	    success = cast_identify(op, caster, spell_ob);
	    break;

	case SP_DETECTION:
	    success = cast_detection(op, caster, spell_ob, skill);
	    break;

	case SP_MOOD_CHANGE:
	    success = mood_change(op, caster, spell_ob);
	    break;

	case SP_MOVING_BALL:
	    if (spell_ob->path_repelled &&
		(spell_ob->path_repelled & caster->path_attuned) != spell_ob->path_repelled) {
		draw_ext_info_format(NDI_UNIQUE, 0, op,
				     MSG_TYPE_SPELL, MSG_TYPE_SPELL_ERROR,
				     "You lack the proper attunement to cast %s",
				     "You lack the proper attunement to cast %s",
				     spell_ob->name);
		success = 0;
	    } else
		success = fire_arch_from_position(op,caster,
			op->x + freearr_x[dir], op->y + freearr_y[dir],
			  dir, spell_ob);
	    break;

	case SP_SWARM:
	    success = fire_swarm(op, caster, spell_ob, dir);
	    break;

	case SP_CHANGE_MANA:
	    success = cast_transfer(op,caster, spell_ob, dir);
	    break;

	case SP_DISPEL_RUNE:
	    /* in rune.c */
	    success = dispel_rune(op,caster, spell_ob, skill, dir);
	    break;

	case SP_CREATE_MISSILE:
	    success = cast_create_missile(op,caster,spell_ob, dir,stringarg);
	    break;

	case SP_CONSECRATE:
	    success = cast_consecrate(op, caster, spell_ob);
	    break;

	case SP_ANIMATE_WEAPON:
	    success = animate_weapon(op, caster, spell_ob, dir);
	    old_shoottype = range_golem;
	    break;

	case SP_LIGHT:
	    success = cast_light(op, caster, spell_ob, dir);
	    break;

	case SP_CHANGE_MAP_LIGHT:
	    success = cast_change_map_lightlevel(op, caster, spell_ob);
	    break;

	case SP_FAERY_FIRE:
	    success = cast_destruction(op,caster,spell_ob);
	    break;

	case SP_CAUSE_DISEASE:
	    success = cast_cause_disease(op, caster, spell_ob, dir);
	    break;

	case SP_AURA:
	    success = create_aura(op, caster, spell_ob);
	    break;

	case SP_TOWN_PORTAL:
	    success= cast_create_town_portal (op,caster,spell_ob, dir);
	    break;

    case SP_PARTY_SPELL:
        success = cast_party_spell( op, caster, dir, spell_ob, stringarg );
        break;

    case SP_ITEM_CURSE_BLESS:
        success = cast_item_curse_or_curse(op, caster, spell_ob);
        break;

	default:
	    LOG(llevError,"cast_spell: Unhandled spell subtype %d\n",
		spell_ob->subtype);


    }

    /* FIXME - we need some better sound suppport */
/*    play_sound_map(op->map, op->x, op->y, SOUND_CAST_SPELL_0 + type);*/
    /* free the spell arg */
    if(settings.casting_time == TRUE && stringarg) {
	free(stringarg);
	stringarg=NULL;
    }
    /* perhaps a bit of a hack, but if using a wand, it has to change the skill
     * to something like use_magic_item, but you really want to be able to fire
     * it again.
     */
    if (op->contr) op->contr->shoottype = old_shoottype;

    return success;
}


/* This is called from time.c/process_object().  That function
 * calls this for any SPELL_EFFECT type objects.  This function
 * then dispatches them to the appropriate specific routines.
 */
void move_spell_effect(object *op) {

    switch (op->subtype) {
	case SP_BOLT:
	    move_bolt(op);
	    break;

	case SP_BULLET:
	    move_bullet(op);
	    break;

	case SP_EXPLOSION:
	    explosion(op);
	    break;

	case SP_CONE:
	    move_cone(op);
	    break;

	case SP_BOMB:
	    animate_bomb(op);
	    break;

	case SP_MAGIC_MISSILE:
	    move_missile(op);
	    break;

	case SP_WORD_OF_RECALL:
	    execute_word_of_recall(op);
	    break;

	case SP_MOVING_BALL:
	    move_ball_spell(op);
	    break;

	case SP_SWARM:
	    move_swarm_spell(op);
	    break;

	case SP_AURA:
	    move_aura(op);
	    break;

    }
}

#if 0
/* check_spell_effect is no longer used - should perhaps be removed.
 * MSW 2006-06-02
 */

/* this checks to see if something special should happen if
 * something runs into the object.
 */
static void check_spell_effect(object *op) {

    switch (op->subtype) {
	case SP_BOLT:
	    move_bolt(op);
	    return;

	case SP_BULLET:
	    check_bullet(op);
	    return;
    }

}
#endif

/**
 * Stores in the spell when to warn player of expiration.
 *
 * @param spell
 * spell we're considering.
 */
void store_spell_expiry(object* spell) {
    /* Keep when to warn the player of expiration */
    char dur[10];
    int i = spell->duration / 5;
    if (!i)
        i = 1;
    snprintf(dur, sizeof(dur), "%d", i);
    set_ob_key_value(spell, "spell_expiry_warn_1", dur, 1);
    i = i / 5;
    if (i > 0) {
        snprintf(dur, sizeof(dur), "%d", i);
        set_ob_key_value(spell, "spell_expiry_warn_2", dur, 1);
    }
}

/**
 * Checks if player should be warned of soon expiring spell.
 *
 * Should be called at each move of the spell. Will use key stored by store_spell_expiry().
 * If the spell effect/force isn't in a player's inventory, won't do anything.
 *
 * @param spell
 * force or spell whose effects will expire.
 */
void check_spell_expiry(object* spell) {
    const char* key;

    if (!spell->env || !spell->env->type == PLAYER)
        return;

    if ((key = get_ob_key_value(spell, "spell_expiry_warn_1")) != NULL) {
        if (spell->duration == atoi(key)) {
            draw_ext_info_format(NDI_UNIQUE | NDI_NAVY, 0, spell->env, MSG_TYPE_SPELL, MSG_TYPE_SPELL_INFO, "The effects of your %s are draining out.", NULL, spell->name);
            return;
        }
    }
    if ((key = get_ob_key_value(spell, "spell_expiry_warn_2")) != NULL) {
        if (spell->duration == atoi(key)) {
            draw_ext_info_format(NDI_UNIQUE | NDI_NAVY, 0, spell->env, MSG_TYPE_SPELL, MSG_TYPE_SPELL_INFO, "The effects of your %s are about to expire.", NULL, spell->name);
            return;
        }
    }
}
