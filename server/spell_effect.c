/*
 * static char *rcsid_spell_effect_c =
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

#include <global.h>
#include <object.h>
#include <living.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <sounds.h>

/* cast_magic_storm: This is really used mostly for spell
 * fumbles at the like.  tmp is the object to propogate.
 * op is what is casting this.
 */
void cast_magic_storm(object *op, object *tmp, int lvl)
{
    if (!tmp) return;	/* error */
    tmp->level=op->level;
    tmp->x=op->x;
    tmp->y=op->y; 
    tmp->range+=lvl/5;  /* increase the area of destruction */
    tmp->duration+=lvl/5;
    tmp->stats.dam=lvl; /* nasty recoils! */
    tmp->stats.maxhp=tmp->count; /* tract single parent */ 
    insert_ob_in_map(tmp,op->map,op,0);

}


int recharge(object *op, object *caster, object *spell_ob) {
    object *wand, *tmp;
    int ncharges;

    wand = find_marked_object(op);
    if(wand == NULL) {
	new_draw_info(NDI_UNIQUE, 0, op, "You need to mark the wand you want to recharge.");
	return 0;
    }
    if(!(random_roll(0, 3, op, PREFER_HIGH))) {
	new_draw_info_format(NDI_UNIQUE, 0, op,
			     "The %s vibrates violently, then explodes!",query_name(wand));
	play_sound_map(op->map, op->x, op->y, SOUND_OB_EXPLODE);
	esrv_del_item(op->contr, wand->count);
	remove_ob(wand);
	free_object(wand);
	tmp = get_archetype("fireball");
	tmp->stats.dam = (spell_ob->stats.dam + SP_level_dam_adjust(caster, spell_ob)) / 10;
	if (!tmp->stats.dam) tmp->stats.dam = 1;
	tmp->stats.hp = tmp->stats.dam / 2;
	if (tmp->stats.hp < 2) tmp->stats.hp = 2;
	tmp->x = op->x;
	tmp->y = op->y;
	insert_ob_in_map(tmp, op->map, NULL, 0);
	return 1;
    }

    ncharges = (spell_ob->stats.dam + SP_level_dam_adjust(caster, spell_ob));
    if (wand->inv && wand->inv->level)
	ncharges /= wand->inv->level;
    else {
	new_draw_info_format(NDI_UNIQUE, 0, op, "Your %s is broken.",
			     query_name(wand));
	return 0;
    }
    if (!ncharges) ncharges = 1;

    wand->stats.food += ncharges;
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"The %s glows with power.",query_name(wand));
    if(wand->arch && QUERY_FLAG(&wand->arch->clone, FLAG_ANIMATE)) {
	SET_FLAG(wand, FLAG_ANIMATE);
	wand->speed = wand->arch->clone.speed;
	update_ob_speed(wand);
    }
    return 1;
}

/******************************************************************************
 * Start of polymorph related functions.
 *
 * Changed around for 0.94.3 - it will now look through and use all the
 * possible choices for objects/monsters (before it was teh first 80 -
 * arbitrary hardcoded limit in this file.)  Doing this will be a bit
 * slower however - while before, it traversed the archetypes once and
 * stored them into an array, it will now potentially traverse it
 * an average of 1.5 times.  This is probably more costly on the polymorph
 * item function, since it is possible a couple lookups might be needed before
 * an item of proper value is generated.
 */

/* polymorph_living - takes a living object (op) and turns it into 
 * another monster of some sort.  Currently, we only deal with single
 * space monsters.
 */

void polymorph_living(object *op) {
    archetype *at;
    int nr = 0, x = op->x, y = op->y, numat=0, choice,friendly;
    mapstruct *map = op->map;
    object *tmp, *next, *owner;

    if(op->head != NULL || op->more != NULL)
	return;

    /* High level creatures are immune, as are creatures immune to magic.  Otherwise,
     * give the creature a saving throw.
     */
    if (op->level>=20 || did_make_save(op, op->level, op->resist[ATNR_MAGIC]/10) ||
	(op->resist[ATNR_MAGIC]==100))
	return;

    /* First, count up the number of legal matches */
    for(at = first_archetype ; at != NULL; at = at->next)
	if(QUERY_FLAG((&at->clone),FLAG_MONSTER) == QUERY_FLAG(op, FLAG_MONSTER) &&
	   at->more == NULL && EDITABLE((&at->clone)))
    {
	numat++;
    }
    if (!numat) return;	/* no valid matches? if so, return */

    /* Next make a choice, and loop through until we get to it */
    choice = rndm(0, numat-1);
    for(at = first_archetype ; at != NULL; at = at->next)
	if(QUERY_FLAG((&at->clone),FLAG_MONSTER) == QUERY_FLAG(op, FLAG_MONSTER) &&
	   at->more == NULL && EDITABLE((&at->clone)))
    {
	if (!choice) break;
	else choice--;
    }

    /* Look through the monster.  Unapply anything they have applied, 
     * and remove any spells.  Note that if this is extended
     * to players, that would need to get fixed somehow.
     */
    for(tmp = op->inv; tmp != NULL; tmp = next) {
	next = tmp->below;
	if(QUERY_FLAG(tmp, FLAG_APPLIED))
	    manual_apply(op,tmp,0);
	if(tmp->type == SPELL) {
	    remove_ob(tmp);
	    free_object(tmp);
	}
    }

    /* Remove the object, preserve some values for the new object */
    remove_ob(op);
    owner = get_owner(op);
    friendly = QUERY_FLAG(op, FLAG_FRIENDLY);
    if (friendly)
	remove_friendly_object(op);

    copy_object(&(at->clone),op);
    if (owner != NULL)
	set_owner(op,owner);
    if (friendly) {
	SET_FLAG(op, FLAG_FRIENDLY);
	op->move_type = PETMOVE;
	add_friendly_object(op);
    }

    /* Put the new creature on the map */
    op->x = x; op->y = y;
    if ((op = insert_ob_in_map (op, map, owner,0)) == NULL)
        return;

    if (HAS_RANDOM_ITEMS(op))
        /* No GT_APPLY here because we'll do it manually. */
	    create_treasure(op->randomitems,op,GT_INVISIBLE,map->difficulty,0);

    /* Apply any objects.  This limits it to the first 20 items, which
     * I guess is reasonable.
     */
    for(tmp = op->inv, nr = 0; tmp != NULL && ++nr < 20; tmp = next) {
	next = tmp->below;
	(void) monster_check_apply(op,tmp);
    }
}


/* polymorph_melt Destroys item from polymorph failure 
 * who is the caster of the polymorph, op is the
 * object destroyed.  We should probably do something
 * more clever ala nethack - create an iron golem or
 * something.
 */
void polymorph_melt(object *who, object *op)
{
    /* Not unique */
    new_draw_info_format(NDI_GREY, 0, who,
	"%s%s glows red, melts and evaporates!",
            op->nrof?"":"The ",query_name(op));
    play_sound_map(op->map, op->x, op->y, SOUND_OB_EVAPORATE);
    remove_ob(op);
    free_object(op);
    return;
}

/* polymorph_item - changes an item to another item of similar type.
 * who is the caster of spell, op is the object to be changed.
 */
void polymorph_item(object *who, object *op) {
    archetype *at;
    int max_value, difficulty, tries=0,choice, charges=op->stats.food,numat=0;
    object *new_ob;

    /* We try and limit the maximum value of the changd object. */
    max_value = op->value * 2;
    if(max_value > 20000)
	max_value = 20000 + (max_value - 20000) / 3;

    /* Look through and try to find matching items.  Can't turn into something
     * invisible.  Also, if the value is too high now, it would almost
     * certainly be too high below.
     */
    for(at = first_archetype ; at != NULL; at = at->next) {
	if(at->clone.type == op->type && !at->clone.invisible && 
	   at->clone.value > 0 && at->clone.value < max_value &&
	   !QUERY_FLAG(&at->clone, FLAG_NO_DROP) &&
	   !QUERY_FLAG(&at->clone, FLAG_STARTEQUIP))
	  numat++;
    }

    if(!numat)
	return;

    difficulty = op->magic * 5;
    if (difficulty<0) difficulty=0;
    new_ob = get_object();
    do {
	choice = rndm(0, numat-1);
	for(at = first_archetype ; at != NULL; at = at->next) {
	    if(at->clone.type == op->type && !at->clone.invisible && 
	       at->clone.value > 0 && at->clone.value < max_value && 
	       !QUERY_FLAG(&at->clone, FLAG_NO_DROP) &&
	       !QUERY_FLAG(&at->clone, FLAG_STARTEQUIP)) {
	      if (!choice) break;
	      else choice--;
	    }
	}
	copy_object(&(at->clone),new_ob);
	fix_generated_item(new_ob,op,difficulty,FABS(op->magic),GT_ENVIRONMENT);
	++tries;
    } while (new_ob->value > max_value && tries<10);
    if (new_ob->invisible) {
	LOG(llevError,"polymorph_item: fix_generated_object made %s invisible?!\n", new_ob->name);
	free_object(new_ob);
    }

    /* Unable to generate an acceptable item?  Melt it */
    if (tries==10) {
	polymorph_melt(who, op);
	free_object(new_ob);
	return;
    }

    if(op->nrof && new_ob->nrof) {
	new_ob->nrof = op->nrof;
	/* decrease the number of items */
	if (new_ob->nrof>2) new_ob->nrof -= rndm(0, op->nrof/2-1);
    }

    /* We don't want rings to keep sustenance/hungry status. There are propably
     *  other cases too that should be checked. 
     */
    if(charges && op->type != RING && op->type != FOOD)
	op->stats.food = charges;

    new_ob->x = op->x;
    new_ob->y = op->y;
    remove_ob(op);
    free_object(op);
    /* 
     * Don't want objects merged or re-arranged, as it then messes up the
     * order
     */
    insert_ob_in_map(new_ob,who->map,new_ob,INS_NO_MERGE | INS_NO_WALK_ON);
}

/* polymorh - caster who has hit object op. */
void polymorph(object *op, object *who) {

    int tmp;

    /* Can't polymorph players right now */
    /* polymorphing generators opens up all sorts of abuses */
    if(op->type == PLAYER || QUERY_FLAG(op, FLAG_GENERATOR))
	return;

    if(QUERY_FLAG(op, FLAG_MONSTER)) {
	polymorph_living(op);
	return;
    }
    /* If it is a living object of some other type, don't handle
     * it now.
     */
    if(QUERY_FLAG(op, FLAG_ALIVE))
	return;

    /* Don't want to morph flying arrows, etc... */
    if(FABS(op->speed) > 0.001 && !QUERY_FLAG(op, FLAG_ANIMATE))
	return;

    /* Do some sanity checking here.  type=0 is unknown, objects
     * without archetypes are not good.  As are a few other
     * cases.
     */
    if(op->type == 0 || op->arch == NULL || 
       QUERY_FLAG(op,FLAG_NO_PICK) 
       || QUERY_FLAG(op, FLAG_NO_PASS) || op->type == TREASURE)
	return;

    tmp = rndm(0, 7);
    if (tmp) polymorph_item(who, op);
    else polymorph_melt(who, op);
}


/* cast_polymorph -
 * object *op is the player/monster
 * caster is object that cast it.
 * spell_ob is the actually spell object.
 * dir is the direction.
 * Returns 0 on illegal cast, otherwise 1.
 */

int cast_polymorph(object *op, object *caster, object *spell_ob, int dir) {
    object *tmp, *next;
    int range, mflags, maxrange;
    mapstruct *m;

    if(dir == 0)
	return 0;

    maxrange = spell_ob->range + SP_level_range_adjust(caster, spell_ob);
    for(range = 1;range < maxrange; range++) {
	sint16 x=op->x+freearr_x[dir]*range,y=op->y+freearr_y[dir]*range;
	object *image;

	m = op->map;
	mflags = get_map_flags(m, &m, x, y, &x, &y);

	if (mflags & (P_WALL | P_NO_MAGIC))
	    break;

	/* Get the top most object */
	for(tmp = get_map_ob(m,x,y); tmp != NULL && tmp->above != NULL;
	    tmp = tmp->above);

	/* Now start polymorphing the objects, top down */
	while (tmp!=NULL) {
	    /* Once we find the floor, no need to go further */
	    if (QUERY_FLAG(tmp, FLAG_IS_FLOOR)) break;
	    next = tmp->below;
	    polymorph(tmp, op);
	    tmp = next;
	}
	image = arch_to_object(spell_ob->other_arch);
	image->x = x;
	image->y = y;
	image->stats.food = 5;
	image->speed_left = 0.1;
	insert_ob_in_map(image,m,op,0);
    }
    return 1;
}



/* Create a missile (nonmagic - magic +4). Will either create bolts or arrows
 * based on whether a crossbow or bow is equiped. If neither, it defaults to
 * arrows.
 * Sets the plus based on the casters level. It is also settable with the
 * invoke command. If the caster attempts to create missiles with too
 * great a plus, the default is used.
 * The # of arrows created also goes up with level, so if a 30th level mage
 * wants LOTS of arrows, and doesn't care what the plus is he could
 * create nonnmagic arrows, or even -1, etc...
 */

int cast_create_missile(object *op, object *caster,object *spell, int dir, char *stringarg)
{
    int missile_plus=0;
    char *missile_name;
    object *tmp, *missile;
    tag_t tag;

    missile_name = "arrow";

    for (tmp=op->inv; tmp != NULL; tmp=tmp->below)
	if (tmp->type == BOW && QUERY_FLAG(tmp, FLAG_APPLIED)) {
	    missile_name=tmp->race;
	}

    missile_plus = spell->stats.dam  + SP_level_dam_adjust(caster, spell);

    if (stringarg) {
	if (atoi(stringarg) < missile_plus)
	    missile_plus = atoi(stringarg);
    }
    if (missile_plus > 4)
	missile_plus = 4;
    else if (missile_plus < -4)
	missile_plus = -4;

    if (find_archetype(missile_name)==NULL) {
	LOG(llevDebug, "Cast create_missile: could not find archtype %s\n", 
	    missile_name);
	return 0;
    }
    missile = get_archetype(missile_name);
    missile->nrof = spell->duration + SP_level_duration_adjust(caster, spell);
    missile->nrof -= 3 * missile_plus;
    if (missile->nrof < 1)
	missile->nrof=1;

    missile->magic = missile_plus;
    /* Can't get any money for these objects */
    missile->value=0;

    SET_FLAG(missile, FLAG_IDENTIFIED);
    tag = missile->count;

    if ( ! cast_create_obj (op, caster, missile, dir) && op->type == PLAYER
      && ! was_destroyed (missile, tag)) 
    {
	pick_up(op, missile);
    }
    return 1;
}


/*  allows the choice of what sort of food object to make.
 *  If stringarg is NULL, it will create food dependent on level  --PeterM*/
int cast_create_food(object *op,object *caster, object *spell_ob, int dir, char *stringarg)
{
    int food_value;
    archetype *at=NULL;
    object *new_op;

    food_value=spell_ob->stats.food + 
              + 50 * SP_level_duration_adjust(caster,spell_ob);

    if(stringarg) {
	at=find_archetype(stringarg);
	if (at==NULL || ((at->clone.type != FOOD && at->clone.type != DRINK)
	    || (at->clone.stats.food > food_value)))
		stringarg = NULL;
    }
  
    if(!stringarg) {
	archetype *at_tmp;

	/* We try to find the archetype with the maximum food value.
	 * This removes the dependancy of hard coded food values in this
	 * function, and addition of new food types is automatically added.
	 * We don't use flesh types because the weight values of those need
	 * to be altered from the donor.
	 */

	/* We assume the food items don't have multiple parts */
	for (at_tmp=first_archetype; at_tmp!=NULL; at_tmp=at_tmp->next) {
	    if (at_tmp->clone.type==FOOD || at_tmp->clone.type==DRINK) {
		/* Basically, if the food value is something that is creatable
		 * under the limits of the spell and it is higher than
		 * the item we have now, take it instead.
		 */
		if (at_tmp->clone.stats.food<=food_value && 
		    (!at || at_tmp->clone.stats.food>at->clone.stats.food))
			at=at_tmp;
	    }
	}
    }
    /* Pretty unlikely (there are some very low food items), but you never
     * know
     */
    if (!at) {
	new_draw_info(NDI_UNIQUE, 0, op, "You don't have enough experience to create any food.");
	return 0;
    }

    food_value/=at->clone.stats.food;
    new_op = get_object();
    copy_object(&at->clone, new_op);
    new_op->nrof = food_value;

    new_op->value = 0;
    if (new_op->nrof<1) new_op->nrof = 1;

    cast_create_obj(op, caster,new_op, dir);
    return 1;
}
  
int probe(object *op, object *caster, object *spell_ob, int dir) {
    int r, mflags, maxrange;
    object *tmp;
    mapstruct *m;


    if(!dir) {
	examine_monster(op,op);
	return 1;
    }
    maxrange = spell_ob->range + SP_level_range_adjust(caster, spell_ob);
    for(r=1;r < maxrange; r++) {
	sint16 x=op->x+r*freearr_x[dir],y=op->y+r*freearr_y[dir];

	m = op->map;
	mflags = get_map_flags(m, &m, x, y, &x, &y);

	if (mflags & P_OUT_OF_MAP) break;

	if (mflags & P_NO_MAGIC) {
	    new_draw_info(NDI_UNIQUE, 0,op,"Something blocks your magic.");
	    return 0;
	}
	if (mflags & P_IS_ALIVE) {
	    for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=tmp->above)
		if(QUERY_FLAG(tmp, FLAG_ALIVE)&&(tmp->type==PLAYER||QUERY_FLAG(tmp, FLAG_MONSTER))) {
		    new_draw_info(NDI_UNIQUE, 0,op,"You detect something.");
		    if(tmp->head!=NULL)
			tmp=tmp->head;
		    examine_monster(op,tmp);
		    return 1;
		}
	}
    }
    new_draw_info(NDI_UNIQUE, 0,op,"You detect nothing.");
    return 1;
}


/* This checks to see if 'pl' is invisible to 'mon'.
 * does race check, undead check, etc
 * Returns TRUE if mon can't see pl, false
 * otherwise.  This doesn't check range, walls, etc.  It
 * only checks the racial adjustments, and in fact that
 * pl is invisible.
 */
int makes_invisible_to(object *pl, object *mon)
{

    if (!pl->invisible) return 0;
    if (pl->type == PLAYER ) {
	/* If race isn't set, then invisible unless it is undead */
	if (!pl->contr->invis_race) {
	    if (QUERY_FLAG(mon, FLAG_UNDEAD)) return 0;
	    return 1;
	}
	/* invis_race is set if we get here */
	if (!strcmp(pl->contr->invis_race, "undead") && is_true_undead(mon))
	    return 1;
	/* No race, can't be invisible to it */
	if (!mon->race) return 0;
	if (strstr(mon->race, pl->contr->invis_race)) return 1;
	/* Nothing matched above, return 0 */
	return 0;
    } else {
	/* monsters are invisible to everything */
	return 1;
    }
}

/* Makes the player or character invisible.
 * Note the spells to 'stack', but perhaps in odd ways.
 * the duration for all is cumulative.
 * In terms of invis undead/normal invis, it is the last one cast that
 * will determine if you are invisible to undead or normal monsters.
 * For improved invis, if you cast it with a one of the others, you
 * lose the improved part of it, and the above statement about undead/
 * normal applies.
 */
int cast_invisible(object *op, object *caster, object *spell_ob) {
    object *tmp;

    if(op->invisible>1000) {
	new_draw_info(NDI_UNIQUE, 0,op,"You can not extend the duration of your invisibility any further");
	return 0;
    }

    /* Remove the switch with 90% duplicate code - just handle the differences with
     * and if statement or two.
     */
    op->invisible += spell_ob->duration + SP_level_duration_adjust(caster, spell_ob);
    /* max duration */
    if(op->invisible>1000) op->invisible = 1000;

    if (op->type == PLAYER) {
	if (op->contr->invis_race) FREE_AND_CLEAR_STR(op->contr->invis_race);
	if (spell_ob->race)
	    op->contr->invis_race = add_refcount(spell_ob->race);
	if (QUERY_FLAG(spell_ob, FLAG_MAKE_INVIS))
	    op->contr->tmp_invis=0;
	else
	    op->contr->tmp_invis=1;

	op->contr->hidden = 0;
    }
    if (makes_invisible_to(op, op)) 
	new_draw_info(NDI_UNIQUE, 0,op,"You can't see your hands!");
    else 
	new_draw_info(NDI_UNIQUE, 0,op,"You feel more transparent!");

    update_object(op,UP_OBJ_FACE);

    /* Only search the active objects - only these should actually do
     * harm to the player.
     */
    for (tmp = active_objects; tmp != NULL; tmp = tmp->active_next)
	if (tmp->enemy == op)
	    tmp->enemy = NULL;
    return 1;
}

/* earth to dust spell.  Basically destroys earthwalsl in the area.
 */
int cast_earth_to_dust(object *op,object *caster, object *spell_ob) {
    object *tmp, *next;
    int range,i,j, mflags;
    sint16 sx, sy;
    mapstruct *m;

    if(op->type!=PLAYER)
	return 0;

    range=spell_ob->range + SP_level_range_adjust(caster, spell_ob);

    for(i= -range;i<range;i++)
	for(j= -range;j<range;j++) {
	    sx = op->x + i;
	    sy = op->y + j;
	    m = op->map;
	    mflags = get_map_flags(m, &m, sx, sy, &sx, &sy);

	    if (mflags & P_OUT_OF_MAP) continue;
	    /* If the space doesn't block, no wall here to remove */
	    if (mflags & P_BLOCKED) {
		for(tmp=get_map_ob(m, sx, sy);tmp!=NULL;tmp=next) {
		    next=tmp->above;
		    if(tmp&&QUERY_FLAG(tmp, FLAG_TEAR_DOWN))
			hit_player(tmp,9998,op,AT_PHYSICAL);
		}
	    }
	}
    return 1;
}


void execute_word_of_recall(object *op) {
    object *wor=op;
    while(op!=NULL && op->type!=PLAYER)
	op=op->env;

    if(op!=NULL) {
	if (get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL) & P_NO_CLERIC)
	    new_draw_info(NDI_UNIQUE, 0,op,"You feel something fizzle inside you.");
	else
	    enter_exit(op,wor);
    }
    remove_ob(wor);
    free_object(wor);
}

/* Word of recall causes the player to return 'home'.
 * we put a force into the player object, so that there is a 
 * time delay effect.
 */
int cast_word_of_recall(object *op, object *caster, object *spell_ob) {
    object *dummy;
    int time;

    if(op->type!=PLAYER)
	return 0;

    dummy=get_archetype("force");
    if(dummy == NULL){
	new_draw_info(NDI_UNIQUE, 0,op,"Oops, program error!");
	LOG(llevError,"cast_word_of_recall: get_archetype(force) failed!\n");
	return 0;
    }
    time = spell_ob->duration - SP_level_duration_adjust(caster, spell_ob);
    if (time <1 ) time=1;

    /* value of speed really doesn't make much difference, as long as it is
     * positive.  Lower value may be useful so that the problem doesn't
     * do anything really odd if it say a -1000 or something.
     */
    dummy->speed = 0.002;
    update_ob_speed(dummy);
    dummy->speed_left = -dummy->speed * time;
    dummy->type=SPELL_EFFECT;
    dummy->subtype = SP_WORD_OF_RECALL;

    /* If we could take advantage of enter_player_savebed() here, it would be
     * nice, but until the map load fails, we can't.
     */
    EXIT_PATH(dummy) = add_string(op->contr->savebed_map);
    EXIT_X(dummy) = op->contr->bed_x;
    EXIT_Y(dummy) = op->contr->bed_y;
  
    (void) insert_ob_in_ob(dummy,op);
    new_draw_info(NDI_UNIQUE, 0,op,"You feel a force starting to build up inside you.");
    return 1;
}

/* cast_wonder
 * wonder is really just a spell that will likely cast another
 * spell.
 */
int cast_wonder(object *op, object *caster, int dir, object *spell_ob) {
    object *newspell;

    if(!rndm(0, 3))
	return cast_cone(op,caster,dir, spell_ob);

    if (spell_ob->randomitems) {
	newspell = generate_treasure(spell_ob->randomitems, caster->level);
	if (!newspell) {
	    LOG(llevError,"cast_wonder: Unable to get a spell!\n");
	    return 0;
	}
	if (newspell->type != SPELL) {
	    LOG(llevError,"cast_wonder: spell returned is not a spell (%d, %s)!\n",
		newspell->type, newspell->name);
	    return 0;
	}
	/* Prevent inifinit recursion */
	if (newspell->subtype == SP_WONDER) {
	    LOG(llevError,"cast_wonder: spell returned is another wonder spell!");
	    return 0;
	}
	return cast_spell(op,caster,dir,newspell, NULL);
    }
    return 1;
}


int perceive_self(object *op) {
    char *cp=describe_item(op, op), buf[MAX_BUF];
    archetype *at=find_archetype("depletion");
    object *tmp;
    int i;

    tmp=find_god(determine_god(op));
    if (tmp) 
	new_draw_info_format(NDI_UNIQUE, 0, op, "You worship %s", tmp->name);
    else
	new_draw_info(NDI_UNIQUE, 0,op,"You worship no god");

    tmp=present_arch_in_ob(at,op);

    if(*cp=='\0' && tmp==NULL)
	new_draw_info(NDI_UNIQUE, 0,op,"You feel very mundane");
    else {
	new_draw_info(NDI_UNIQUE, 0,op,"You have:");
	new_draw_info(NDI_UNIQUE, 0,op,cp);
	if (tmp!=NULL) {
	    for (i=0; i<7; i++) {
		if (get_attr_value(&tmp->stats, i)<0) {
		    new_draw_info_format(NDI_UNIQUE, 0,op, 
			 "Your %s is depleted by %d", statname[i],
			    -(get_attr_value(&tmp->stats,i)));
		}
	    }
	}
    }

    if (is_dragon_pl(op)) {
       /* now grab the 'dragon_ability'-force from the player's inventory */
	for (tmp = op->inv; tmp != NULL; tmp = tmp->below) {
	    if (tmp->type == FORCE && !strcmp(tmp->arch->name, "dragon_ability_force")) {
		if(tmp->stats.exp == 0) {
		    sprintf(buf, "Your metabolism isn't focused on anything.");
		} else {
		    sprintf(buf, "Your metabolism is focused on %s.", change_resist_msg[tmp->stats.exp]);
		}
		new_draw_info(NDI_UNIQUE, 0,op, buf);
		break;
	    }
	}
    }
    return 1;
}

/* int cast_create_town_portal (object *op, object *caster, int dir)
 *
 * This function cast the spell of town portal for op
 *
 * The spell operates in two passes. During the first one a place
 * is marked as a destination for the portal. During the second one,
 * 2 portals are created, one in the position the player cast it and
 * one in the destination place. The portal are synchronized and 2 forces
 * are inserted in the player to destruct the portal next time player
 * creates a new portal pair.
 * This spell has a side effect that it allows people to meet each other
 * in a permanent, private,  appartements by making a town portal from it
 * to the town or another public place. So, check if the map is unique and if
 * so return an error
 *
 * Code by Tchize (david.delbecq@usa.net)
 */
int cast_create_town_portal (object *op, object *caster, object *spell, int dir)
{
    object *dummy, *force, *old_force, *tmp;
    archetype *perm_portal;
    char portal_name [1024], portal_message [1024];
    sint16 exitx, exity;
    mapstruct *exitmap;
    int op_level;


    /* Check to see if the map the player is currently on is a per player unique
     * map.  This can be determined in that per player unique maps have the
     * full pathname listed.
     */
    if (!strncmp(op->map->path, settings.localdir, strlen(settings.localdir))) {
	new_draw_info(NDI_UNIQUE | NDI_NAVY, 0,op,"You can't cast that here.\n");
	return 0;
    }

    /* The first thing to do is to check if we have a marked destination
     * dummy is used to make a check inventory for the force
     */
    dummy=arch_to_object(spell->other_arch);
    if(dummy == NULL){
	new_draw_info(NDI_UNIQUE, 0,op,"Oops, program error!");
	LOG(llevError,"get_object failed (force in cast_create_town_portal for %s!\n",op->name);
	return 0;
    }
    force=check_inv_recursive (op,dummy);

    if (force==NULL) {
	/* Here we know there is no destination marked up.
	 * We have 2 things to do:
	 * 1. Mark the destination in the player inventory.
	 * 2. Let the player know it worked.
	 */
	free_string (dummy->name);
	dummy->name = add_string (op->map->path);
	EXIT_X(dummy)= op->x;
	EXIT_Y(dummy)= op->y;
	insert_ob_in_ob (dummy,op);
	new_draw_info(NDI_UNIQUE | NDI_NAVY, 0,op,"You fix this place in your mind.\nYou feel you are able to come here from anywhere.");
	return 1;
    }
    free_object (dummy);

    /* Here we know where the town portal should go to
     * We should kill any existing portal associated with the player.
     * Than we should create the 2 portals.
     * For each of them, we need:
     *    - To create the portal with the name of the player+destination map
     *    - set the owner of the town portal
     *    - To mark the position of the portal in the player's inventory
     *      for easier destruction. 
     *
     *  The mark works has follow:
     *   slaying: Existing town portal
     *   hp, sp : x & y of the associated portal
     *   name   : name of the portal
     *   race   : map the portal is in
     */

    /* First step: killing existing town portals */
    dummy=get_archetype(spell->race);
    if(dummy == NULL){
	new_draw_info(NDI_UNIQUE, 0,op,"Oops, program error!");
	LOG(llevError,"get_object failed (force) in cast_create_town_portal for %s!\n",op->name);
	return 0;
    }
    perm_portal = find_archetype (spell->slaying);

    /* To kill a town portal, we go trough the player's inventory,
     * for each marked portal in player's inventory,
     *   -We try load the associated map (if impossible, consider the portal destructed)
     *   -We find any portal in the specified location.
     *      If it has the good name, we destruct it.
     *   -We destruct the force indicating that portal.
     */
    while ( (old_force=check_inv_recursive (op,dummy))) {
	exitx=EXIT_X(old_force);
	exity=EXIT_Y(old_force);
	LOG (llevDebug,"Trying to kill a portal in %s (%d,%d)\n",old_force->race,exitx,exity);

	if (!strncmp(old_force->race, settings.localdir, strlen(settings.localdir))) 
	    exitmap = ready_map_name(old_force->race, MAP_PLAYER_UNIQUE);
	else exitmap = ready_map_name(old_force->race, 0);

	if (exitmap) {
	    tmp=present_arch (perm_portal,exitmap,exitx,exity);
	    while (tmp) {
		if (tmp->name == old_force->name) {
		    remove_ob (tmp);
		    free_object (tmp);
		    break;
		} else {
		    tmp = tmp->above;
		}
	    }
	}
	remove_ob (old_force);
	free_object (old_force);
	LOG (llevDebug,"\n",old_force->name);
    }
    free_object (dummy);

    /* Creating the portals.
     * The very first thing to do is to ensure
     * access to the destination map.
     * If we can't, don't fizzle. Simply warn player.
     * This ensure player pays his mana for the spell
     * because HE is responsible of forgotting.
     * 'force' is the destination of the town portal, which we got
     * from the players inventory above.
     */

    /* Ensure exit map is loaded*/
    if (!strncmp(force->name, settings.localdir, strlen(settings.localdir)))
	exitmap = ready_map_name(force->name, MAP_PLAYER_UNIQUE);
    else
	exitmap = ready_map_name(force->name, 0);

    /* If we were unable to load (ex. random map deleted), warn player*/
    if (exitmap==NULL) {
	new_draw_info(NDI_UNIQUE | NDI_NAVY, 0,op,"Something strange happens.\nYou can't remember where to go!?");
	remove_ob(force);
	free_object(force);
	return 1;
    }

    op_level = caster_level(caster, spell);
    if (op_level<15)
         snprintf (portal_message,1024,"\nAir moves around you and\na huge smell of ammoniac\nrounds you as you pass\nthrough %s's portal\nPouah!\n",op->name);
    else if (op_level<30)
         snprintf (portal_message,1024,"\n%s's portal smells ozone.\nYou do a lot of movements and finally pass\nthrough the small hole in the air\n",op->name);
    else if (op_level<60)
         snprintf (portal_message,1024,"\nA sort of door opens in the air in front of you,\nshowing you the path to somewhere else.\n");
    else snprintf (portal_message,1024,"\nAs you walk on %s's portal, flowers comes\nfrom the ground around you.\nYou feel quiet.\n",op->name);

    /* Create a portal in front of player
     * dummy contain the portal and
     * force contain the track to kill it later
     */

    snprintf (portal_name,1024,"%s's portal to %s",op->name,force->name);
    dummy=get_archetype(spell->slaying); /*The portal*/
    if(dummy == NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Oops, program error!");
	LOG(llevError,"get_object failed (perm_magic_portal) in cast_create_town_portal for %s!\n",op->name);
	return 0;
    }
    EXIT_PATH(dummy) = add_string (force->name);
    EXIT_X(dummy)=EXIT_X(force);
    EXIT_Y(dummy)=EXIT_Y(force);
    FREE_AND_COPY(dummy->name, portal_name);
    FREE_AND_COPY(dummy->name_pl, portal_name);
    dummy->msg=add_string (portal_message);
    dummy->race=add_string (op->name);  /*Save the owner of the portal*/
    cast_create_obj (op, caster, dummy, 0);

    /* Now we need to to create a town portal marker inside the player
     * object, so on future castings, we can know that he has an active
     * town portal.
     */
    tmp=get_archetype(spell->race);
    if(tmp == NULL){
	new_draw_info(NDI_UNIQUE, 0,op,"Oops, program error!");
	LOG(llevError,"get_object failed (force) in cast_create_town_portal for %s!\n",op->name);
	return 0;
    }
    tmp->race=add_string (op->map->path);
    FREE_AND_COPY(tmp->name, portal_name);
    EXIT_X(tmp)=dummy->x;
    EXIT_Y(tmp)=dummy->y;
    insert_ob_in_ob (tmp,op);

    /* Create a portal in the destination map
     * dummy contain the portal and
     * force the track to kill it later
     * the 'force' variable still contains the 'reminder' of
     * where this portal goes to.
     */
    snprintf (portal_name,1024,"%s's portal to %s",op->name,op->map->path);
    dummy=get_archetype (spell->slaying); /*The portal*/
    if(dummy == NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Oops, program error!");
	LOG(llevError,"get_object failed (perm_magic_portal) in cast_create_town_portal for %s!\n",op->name);
	return 0;
    }
    EXIT_PATH(dummy) = add_string (op->map->path);
    EXIT_X(dummy)=op->x;
    EXIT_Y(dummy)=op->y;
    FREE_AND_COPY(dummy->name, portal_name);
    FREE_AND_COPY(dummy->name_pl, portal_name);
    dummy->msg=add_string (portal_message);
    dummy->x=EXIT_X(force);
    dummy->y=EXIT_Y(force);
    dummy->race=add_string (op->name);  /*Save the owner of the portal*/
    insert_ob_in_map(dummy,exitmap,op,0);

    /* Now we create another town portal marker that
     * points back to the one we just made
     */
    tmp=get_archetype(spell->race);
    if(tmp == NULL){
	new_draw_info(NDI_UNIQUE, 0,op,"Oops, program error!");
	LOG(llevError,"get_object failed (force) in cast_create_town_portal for %s!\n",op->name);
	return 0;
    }
    tmp->race=add_string(force->name);
    FREE_AND_COPY(tmp->name, portal_name);
    EXIT_X(tmp)=dummy->x;
    EXIT_Y(tmp)=dummy->y;
    insert_ob_in_ob (tmp,op);

    /* Describe the player what happened
     */
    new_draw_info(NDI_UNIQUE | NDI_NAVY, 0,op,"You see air moving and showing you the way home.");
    remove_ob(force); /* Delete the force inside the player*/
    free_object(force);
    return 1;
}


/* This creates magic walls.  Really, it can create most any object,
 * within some reason.
 */

int magic_wall(object *op,object *caster,int dir,object *spell_ob) {
    object *tmp, *tmp2;  
    int i,posblocked,negblocked, maxrange;
    sint16 x, y;
    mapstruct *m;
    char *name;
    archetype *at;

    if(!dir) {
	dir=op->facing;
	x = op->x;
	y = op->y;
    } else {
	x = op->x+freearr_x[dir];
	y = op->y+freearr_y[dir];
    }
    m = op->map;

    if ((QUERY_FLAG(spell_ob, FLAG_NO_PASS) || x != op->x || y != op->y) &&
	get_map_flags(m, &m, x, y, &x, &y) & (P_OUT_OF_MAP | P_BLOCKED)) {
	new_draw_info(NDI_UNIQUE, 0,op,"Something is in the way.");
	return 0;
    }
    if (spell_ob->other_arch) {
	tmp = arch_to_object(spell_ob->other_arch);
    } else if (spell_ob->race) {
	char buf1[MAX_BUF];

	sprintf(buf1,spell_ob->race,dir);
	at = find_archetype(buf1);
	if (!at) {
	    LOG(llevError, "summon_wall: Unable to find archetype %s\n", buf1);
	    new_draw_info(NDI_UNIQUE, 0,op,"This spell is broken.");
	    return 0;
	}
	tmp = arch_to_object(at);
    } else {
	LOG(llevError,"magic_wall: spell %s lacks other_arch\n",
	    spell_ob->name);
	return 0;
    }

    if (tmp->type == SPELL_EFFECT) {
	tmp->attacktype = spell_ob->attacktype;
	tmp->duration = spell_ob->duration +
	    SP_level_duration_adjust(caster, spell_ob);
	tmp->stats.dam = spell_ob->stats.dam +
	    SP_level_dam_adjust(caster, spell_ob);
	tmp->range = 0;
    } else if (QUERY_FLAG(tmp, FLAG_ALIVE)) {
	tmp->stats.hp = spell_ob->duration +
	    SP_level_duration_adjust(caster, spell_ob);
	tmp->stats.maxhp = tmp->stats.hp;
	set_owner(tmp,op);
	set_spell_skill(op, caster, spell_ob, tmp);
    }
    if (QUERY_FLAG(spell_ob, FLAG_IS_USED_UP) || QUERY_FLAG(tmp, FLAG_IS_USED_UP)) {
	tmp->stats.food = spell_ob->duration +
	    SP_level_duration_adjust(caster, spell_ob);
	SET_FLAG(tmp, FLAG_IS_USED_UP);
    }
    if (QUERY_FLAG(spell_ob, FLAG_TEAR_DOWN)) {
        tmp->stats.hp = spell_ob->stats.dam + SP_level_dam_adjust(caster, spell_ob);
	tmp->stats.maxhp = tmp->stats.hp;
	SET_FLAG(tmp, FLAG_TEAR_DOWN);
	SET_FLAG(tmp, FLAG_ALIVE);
    }

    /* This can't really hurt - if the object doesn't kill anything,
     * these fields just won't be used.
     */
    set_owner(tmp,op);
    set_spell_skill(op, caster, spell_ob, tmp);
    tmp->x = x;
    tmp->y = y;
    tmp->level = caster_level(caster, spell_ob) / 2;

    name = tmp->name;
    if ((tmp = insert_ob_in_map (tmp, m, op,0)) == NULL) {
	new_draw_info_format(NDI_UNIQUE, 0,op,"Something destroys your %s", name);
	return 0;
    }

    /*  This code causes the wall to extend some distance in
     * each direction, or until an obstruction is encountered. 
     * posblocked and negblocked help determine how far the
     * created wall can extend, it won't go extend through
     * blocked spaces.
     */
    maxrange = spell_ob->range + SP_level_range_adjust(caster, spell_ob);
    posblocked=0;
    negblocked=0;

    for(i=1; i<=maxrange; i++) {
	int dir2;
	 
	dir2 = (dir<4)?(dir+2):dir-2;
	 
	x = tmp->x+i*freearr_x[dir2]; 
	y = tmp->y+i*freearr_y[dir2];

	if(!(get_map_flags(m, &m, x, y, &x, &y) & (P_OUT_OF_MAP | P_BLOCKED)) && !posblocked) {
	    tmp2 = get_object();
	    copy_object(tmp,tmp2);
	    tmp2->x = x;
	    tmp2->y = y;
	    insert_ob_in_map(tmp2,m,op,0);
	} else posblocked=1;

	x = tmp->x-i*freearr_x[dir2];
	y = tmp->y-i*freearr_y[dir2];
	m = tmp->map;

	if(!(get_map_flags(m, &m, x, y, &x, &y) & (P_OUT_OF_MAP | P_BLOCKED)) && !negblocked) {
	    tmp2 = get_object();
	    copy_object(tmp,tmp2);
	    tmp2->x = x;
	    tmp2->y = y;
	    insert_ob_in_map(tmp2,m,op,0);
	} else negblocked=1;
    }

    if(QUERY_FLAG(tmp, FLAG_BLOCKSVIEW))
	update_all_los(op->map, op->x, op->y);

    return 1;
}

int dimension_door(object *op,object *caster, object *spob, int dir) {
    int dist, maxdist, mflags;

    if(op->type!=PLAYER)
	return 0;

    if(!dir) {
	new_draw_info(NDI_UNIQUE, 0,op,"In what direction?");
	return 0;
    }

    /* Given the new outdoor maps, can't let players dimension door for
     * ever, so put limits in.
     */
    maxdist = spob->range + 
	SP_level_range_adjust(caster, spob);

    if(op->contr->count) {
	if (op->contr->count > maxdist) {
	    new_draw_info(NDI_UNIQUE, 0, op, "You can't dimension door that far!");
	    return 0;
	}

	for(dist=0;dist<op->contr->count; dist++) {
	    mflags = get_map_flags(op->map, NULL, 
		op->x+freearr_x[dir]*(dist+1), op->y+freearr_y[dir]*(dist+1),
		NULL, NULL);

	    if ((mflags & (P_NO_MAGIC | P_OUT_OF_MAP)) ||
		((mflags & P_NO_PASS) && (mflags & P_BLOCKSVIEW))) break;
	}

	if(dist<op->contr->count) {
	    new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of the spell.\n");
	    op->contr->count=0;
	    return 0;
	}
	op->contr->count=0;

	/* Remove code that puts player on random space on maps.  IMO,
	 * a lot of maps probably have areas the player should not get to,
	 * but may not be marked as NO_MAGIC (as they may be bounded
	 * by such squares).  Also, there are probably treasure rooms and
	 * lots of other maps that protect areas with no magic, but the
	 * areas themselves don't contain no magic spaces.
	 */
	if(get_map_flags(op->map, NULL, 
	    op->x+freearr_x[dir]*dist, op->y+freearr_y[dir]*dist,
	    NULL, NULL) & P_BLOCKED) {
		new_draw_info(NDI_UNIQUE, 0,op,"You cast your spell, but nothing happens.\n");
		return 1; /* Maybe the penalty should be more severe... */
	}
    } else { 
	/* Player didn't specify a distance, so lets see how far
	 * we can move the player.  Don't know why this stopped on
	 * spaces that blocked the players view.
	 */

	for(dist=0; dist < maxdist; dist++) {
	    mflags = get_map_flags(op->map, NULL, 
		op->x+freearr_x[dir] * (dist+1), 
	        op->y+freearr_y[dir] * (dist+1),
	        NULL, NULL);
	    if ((mflags & (P_NO_MAGIC | P_OUT_OF_MAP)) ||
		((mflags & P_NO_PASS) && (mflags & P_BLOCKSVIEW))) {
		break;
	    }
	}

	/* If the destination is blocked, keep backing up until we
	 * find a place for the player.
	 */
	for(;dist>0; dist--)
	    if ((get_map_flags(op->map,NULL, 
	      op->x+freearr_x[dir]*dist,
	      op->y+freearr_y[dir]*dist,
	      NULL, NULL) & (P_BLOCKED | P_OUT_OF_MAP)) ==0) 
		break;

	if(!dist) {
	    new_draw_info(NDI_UNIQUE, 0,op,"Your spell failed!\n");
	    return 0;
	}
    }

    /* Actually move the player now */
    remove_ob(op);
    op->x+=freearr_x[dir]*dist;
    op->y+=freearr_y[dir]*dist;
    if ((op = insert_ob_in_map(op,op->map,op,0)) == NULL)
        return 1;

    if (op->type == PLAYER)
	MapNewmapCmd(op->contr);
    op->speed_left= -FABS(op->speed)*5; /* Freeze them for a short while */
    return 1;
}


/* cast_heal: Heals something.
 * op is the caster.
 * dir is the direction he is casting it in.
 * spell is the spell object.
 */
int cast_heal(object *op,object *caster, object *spell, int dir) {
    object *tmp;
    archetype *at;
    object *poison;
    int heal = 0, success = 0;

    tmp = find_target_for_friendly_spell(op,dir);

    if (tmp==NULL) return 0;

    /* Figure out how many hp this spell might cure.
     * could be zero if this spell heals effects, not damage.
     */
    heal = spell->stats.dam;
    if (spell->stats.hp)
	heal += random_roll(spell->stats.hp, 6, op, PREFER_HIGH) +
	    spell->stats.hp;

    if (heal) {
	if (tmp->stats.hp >= tmp->stats.maxhp) {
	    new_draw_info(NDI_UNIQUE, 0,tmp, "Your are already fully healed.");
	}
	else {
	    /* See how many points we actually heal.  Instead of messages
	     * based on type of spell, we instead do messages based
	     * on amount of damage healed.
	     */
	    if (heal > (tmp->stats.maxhp - tmp->stats.hp))
		heal = tmp->stats.maxhp - tmp->stats.hp;
	    tmp->stats.hp += heal;

	    if (tmp->stats.hp >= tmp->stats.maxhp) {
		new_draw_info(NDI_UNIQUE, 0,tmp, "You feel just fine!");
	    } else if (heal > 50) {
		new_draw_info(NDI_UNIQUE, 0,tmp, "Your wounds close!");
	    } else if (heal > 25) {
		new_draw_info(NDI_UNIQUE, 0,tmp, "Your wounds mostly close.");
	    } else if (heal > 10) {
		new_draw_info(NDI_UNIQUE, 0,tmp, "Your wounds start to fade.");
	    } else {
		new_draw_info(NDI_UNIQUE, 0,tmp, "Your wounds start to close.");
	    }
	    success=1;
	}
    }
    if (spell->attacktype & AT_DISEASE)
	 if (cure_disease (tmp, op))
           success = 1;

    if (spell->attacktype & AT_POISON) {
	at = find_archetype("poisoning");
	poison=present_arch_in_ob(at,tmp);
	if (poison) {
	    success = 1;
	    new_draw_info(NDI_UNIQUE, 0,tmp, "Your body feels cleansed");
	    poison->stats.food = 1;
	}
    }
    if (spell->attacktype & AT_CONFUSION) {
	poison=present_in_ob_by_name(FORCE,"confusion", tmp);
	if (poison) {
	    success = 1;
	    new_draw_info(NDI_UNIQUE, 0,tmp, "Your mind feels clearer");
	    poison->duration = 1;
	}
    }
    if (spell->attacktype & AT_BLIND) {
	at=find_archetype("blindness");
	poison=present_arch_in_ob(at,tmp);
	if (poison) {
	    success = 1;
	    new_draw_info(NDI_UNIQUE, 0,tmp,"Your vision begins to return.");
	    poison->stats.food = 1;
	}
    }
    if (spell->last_sp && tmp->stats.sp < tmp->stats.maxsp) {
	tmp->stats.sp += spell->last_sp;
	if (tmp->stats.sp > tmp->stats.maxsp) tmp->stats.sp = tmp->stats.maxsp;
	success = 1;
	new_draw_info(NDI_UNIQUE, 0,tmp,"Magical energies surge through your body!");
    }
    if (spell->last_grace && tmp->stats.grace < tmp->stats.maxgrace) {
	tmp->stats.grace += spell->last_grace;
	if (tmp->stats.grace > tmp->stats.maxgrace) tmp->stats.grace = tmp->stats.maxgrace;
	success = 1;
	new_draw_info(NDI_UNIQUE, 0,tmp,"You feel redeemed with you god!");
    }
    if (spell->stats.food && tmp->stats.food < 999) {
	tmp->stats.food += spell->stats.food;
	if (tmp->stats.food > 999) tmp->stats.food=999;
	success = 1;
	/* We could do something a bit better like the messages for healing above */
	new_draw_info(NDI_UNIQUE, 0,tmp,"You feel your belly fill with food");
    }
    return success;
}


/* This is used for the spells that gain stats.  There are no spells
 * right now that icnrease wis/int/pow on a temp basis, so no
 * good comments for those.
 */
static char *no_gain_msgs[NUM_STATS] = {
"You grow no stronger.",
"You grow no more agile.",
"You don't feel any healthier.",
"no wis",
"You are no easier to look at.",
"no int",
"no pow"
};

int cast_change_ability(object *op,object *caster,object *spell_ob, int dir) {
    object *tmp, *tmp2=NULL;
    object *force=NULL;
    int i;
  
    /* if dir = 99 op defaults to tmp, eat_special_food() requires this. */
    if(dir!=0) {
	tmp=find_target_for_friendly_spell(op,dir);
    } else {
	tmp = op;
    }

    if(tmp==NULL) return 0;
  
    /* If we've already got a force of this type, don't add a new one. */
    for(tmp2=tmp->inv; tmp2!=NULL; tmp2=tmp2->below) {
	if (tmp2->type==FORCE && tmp2->subtype == FORCE_CHANGE_ABILITY)  {
	    if (tmp2->name == spell_ob->name) {
		force=tmp2;    /* the old effect will be "refreshed" */
		break;
	    }
	    else if (spell_ob->race && spell_ob->race == tmp2->name) {
		new_draw_info_format(NDI_UNIQUE, 0, op,
		    "You can not cast %s while %s is in effect",
		    spell_ob->name, tmp2->name_pl);
		return 0;
	    }
	}
    }
    if(force==NULL) {
	force=get_archetype("force");
	force->subtype = FORCE_CHANGE_ABILITY;
	free_string(force->name);
	if (spell_ob->race) 
	    force->name = add_refcount(spell_ob->race);
	else
	    force->name = add_refcount(spell_ob->name);
	free_string(force->name_pl);
	force->name_pl = add_refcount(spell_ob->name);

    } else {
	int duration;

	duration = spell_ob->duration + SP_level_duration_adjust(caster, spell_ob) * 50;
	if (duration > force->duration) {
	    force->duration = duration;
	    new_draw_info(NDI_UNIQUE, 0, op, "You recast the spell while in effect.");
	} else {
	    new_draw_info(NDI_UNIQUE, 0, op, "Recasting the spell had no effect.");
	}
	return 1;
    }
    force->duration = spell_ob->duration + SP_level_duration_adjust(caster, spell_ob) * 50;
    force->speed = 1.0;
    force->speed_left = -1.0;
    SET_FLAG(force, FLAG_APPLIED);

    /* Now start processing the effects.  First, protections */
    for (i=0; i < NROFATTACKS; i++) {
	if (spell_ob->resist[i]) {
	    force->resist[i] = spell_ob->resist[i] + SP_level_dam_adjust(caster, spell_ob);
	    if (force->resist[i] > 100) force->resist[i] = 100;
	}
    }
    if (spell_ob->stats.hp) 
	force->stats.hp = spell_ob->stats.hp + SP_level_dam_adjust(caster,spell_ob);

    if (tmp->type == PLAYER) {
	/* Stat adjustment spells */
	for (i=0; i < NUM_STATS; i++) {
	    int stat = get_attr_value(&spell_ob->stats, i), k, sm;
	    if (stat) {
		sm=0;
		for (k=0; k<stat; k++)
		    sm += rndm(1, 3);

		if ((get_attr_value(&tmp->stats, i) + sm) > (15 + 5 * stat)) {
		    sm = (15 + 5 * stat) - get_attr_value(&tmp->stats, i);
		    if (sm<0) sm = 0;
		}
		set_attr_value(&force->stats, i, sm);
		if (!sm)
		    new_draw_info(NDI_UNIQUE, 0,op,no_gain_msgs[stat]);
	    }
	}
    }
  
    if (QUERY_FLAG(spell_ob, FLAG_FLYING))
	SET_FLAG(force, FLAG_FLYING);

    if (QUERY_FLAG(spell_ob, FLAG_SEE_IN_DARK))
	SET_FLAG(force, FLAG_SEE_IN_DARK);

    if (QUERY_FLAG(spell_ob, FLAG_XRAYS))
	SET_FLAG(force, FLAG_XRAYS);

    /* Haste/bonus speed */
    if (spell_ob->stats.exp) {
	if (op->speed > 0.5) force->stats.exp = (float) spell_ob->stats.exp / (op->speed + 0.5);
	else
	    force->stats.exp = spell_ob->stats.exp;
    }

    force->stats.wc = spell_ob->stats.wc;
    force->stats.ac = spell_ob->stats.ac;
    force->attacktype = spell_ob->attacktype;

    change_abil(tmp,force);	/* Mostly to display any messages */
    insert_ob_in_ob(force,tmp);
    fix_player(tmp);
    return 1;
}

/* This used to be part of cast_change_ability, but it really didn't make
 * a lot of sense, since most of the values it derives are from the god
 * of the caster.
 */

int cast_bless(object *op,object *caster,object *spell_ob, int dir) {
    int i;
    object *god = find_god(determine_god(op)), *tmp2, *force=NULL, *tmp;

    /* if dir = 99 op defaults to tmp, eat_special_food() requires this. */
    if(dir!=0) {
	tmp=find_target_for_friendly_spell(op,dir);
    } else {
	tmp = op;
    }

    /* If we've already got a force of this type, don't add a new one. */
    for(tmp2=tmp->inv; tmp2!=NULL; tmp2=tmp2->below) {
	if (tmp2->type==FORCE && tmp2->subtype == FORCE_CHANGE_ABILITY)  {
	    if (tmp2->name == spell_ob->name) {
		force=tmp2;    /* the old effect will be "refreshed" */
		break;
	    }
	    else if (spell_ob->race && spell_ob->race == tmp2->name) {
		new_draw_info_format(NDI_UNIQUE, 0, op,
		    "You can not cast %s while %s is in effect",
		    spell_ob->name, tmp2->name_pl);
		return 0;
	    }
	}
    }
    if(force==NULL) {
	force=get_archetype("force");
	force->subtype = FORCE_CHANGE_ABILITY;
	free_string(force->name);
	if (spell_ob->race) 
	    force->name = add_refcount(spell_ob->race);
	else
	    force->name = add_refcount(spell_ob->name);
	free_string(force->name_pl);
	force->name_pl = add_refcount(spell_ob->name);
    } else {
	int duration;

	duration = spell_ob->duration + SP_level_duration_adjust(caster, spell_ob) * 50;
	if (duration > force->duration) {
	    force->duration = duration;
	    new_draw_info(NDI_UNIQUE, 0, op, "You recast the spell while in effect.");
	} else {
	    new_draw_info(NDI_UNIQUE, 0, op, "Recasting the spell had no effect.");
	}
	return 0;
    }
    force->duration = spell_ob->duration + SP_level_duration_adjust(caster, spell_ob) * 50;
    force->speed = 1.0;
    force->speed_left = -1.0;
    SET_FLAG(force, FLAG_APPLIED);

    if(!god) {
	new_draw_info(NDI_UNIQUE, 0,op,"Your blessing seems empty.");
    } else {
	/* Only give out good benefits, and put a max on it */
	for (i=0; i<NROFATTACKS; i++) {
	    if (god->resist[i]>0) {
		force->resist[i] = MIN(god->resist[i], spell_ob->resist[ATNR_GODPOWER]);
	    }
	}
	force->path_attuned|=god->path_attuned;
	if (spell_ob->attacktype) {
	    force->attacktype|=god->attacktype | AT_PHYSICAL;
	    if(god->slaying) force->slaying = add_string(god->slaying);
	}
	if (tmp != op) {
		new_draw_info_format(NDI_UNIQUE, 0, op, "You bless %s.", tmp->name);
		new_draw_info_format(NDI_UNIQUE, 0, tmp, "%s blessed you.", op->name);
	} else {
	    new_draw_info_format(NDI_UNIQUE, 0,tmp,
		     "You are blessed by %s!",god->name);
	}

    }
    force->stats.wc = spell_ob->stats.wc;
    force->stats.ac = spell_ob->stats.ac;

    change_abil(tmp,force);	/* Mostly to display any messages */
    insert_ob_in_ob(force,tmp);
    fix_player(tmp);
    return 1;
}



/* Alchemy code by Mark Wedel
 *
 * This code adds a new spell, called alchemy.  Alchemy will turn
 * objects to gold nuggets, the value of the gold nuggets being
 * about 90% of that of the item itself.  It uses the value of the
 * object before charisma adjustments, because the nuggets themselves
 * will be will be adjusted by charisma when sold.
 *
 * Large nuggets are worth 25 gp each (base).  You will always get
 * the maximum number of large nuggets you could get.
 * Small nuggets are worth 1 gp each (base).  You will get from 0
 * to the max amount of small nuggets as you could get.
 *
 * For example, if an item is worth 110 gold, you will get
 * 4 large nuggets, and from 0-10 small nuggets.
 *
 * There is also a chance (1:30) that you will get nothing at all
 * for the object.  There is also a maximum weight that will be
 * alchemied.
 */
 
/* I didn't feel like passing these as arguements to the
 * two functions that need them.  Real values are put in them
 * when the spell is cast, and these are freed when the spell
 * is finished.
 */
static object *small, *large;

static void alchemy_object(object *obj, int *small_nuggets,
	 int *large_nuggets, int *weight)
{
    int	value=query_cost(obj, NULL, F_TRUE);

    /* Give third price when we alchemy money (This should hopefully
     * make it so that it isn't worth it to alchemy money, sell
     * the nuggets, alchemy the gold from that, etc.
     * Otherwise, give 9 silver on the gold for other objects,
     * so that it would still be more affordable to haul
     * the stuff back to town.
     */

    if (QUERY_FLAG(obj, FLAG_UNPAID))
	value=0;
    else if (obj->type==MONEY || obj->type==GEM)
	value /=3;
    else
	value *= 0.9;

    if ((obj->value>0) && rndm(0, 29)) {
	static int value_store;
	int count;

	value_store += value;
	count = value_store / large->value;
	*large_nuggets += count;
	value_store -= count * large->value;
	count = value_store / small->value;
	*small_nuggets += count;
	value_store -= count * small->value;
	/* LOG(llevDebug, "alchemize value %d, remainder %d\n", value, value_store); */
    }

    /* Turn 25 small nuggets into 1 large nugget.  If the value
     * of large nuggets is not evenly divisable by the small nugget
     * value, take off an extra small_nugget (Assuming small_nuggets!=0)
     */
    if (*small_nuggets * small->value >= large->value) {
	(*large_nuggets)++;
	*small_nuggets -= large->value / small->value;
	if (*small_nuggets && large->value % small->value)
		(*small_nuggets)--;
    }
    weight += obj->weight;
    remove_ob(obj);
    free_object(obj);
}

static void update_map(object *op, mapstruct *m, int small_nuggets, int large_nuggets,
	int x, int y)
{
    object *tmp;
    int flag=0;

    /* Put any nuggets below the player, but we can only pass this
     * flag if we are on the same space as the player 
     */
    if (x == op->x && y == op->y && op->map == m) flag = INS_BELOW_ORIGINATOR;

    if (small_nuggets) {
	tmp = get_object();
	copy_object(small, tmp);
	tmp-> nrof = small_nuggets;
	tmp->x = x;
	tmp->y = y;
	insert_ob_in_map(tmp, m, op, flag);
    }
    if (large_nuggets) {
	tmp = get_object();
	copy_object(large, tmp);
	tmp-> nrof = large_nuggets;
	tmp->x = x;
	tmp->y = y;
	insert_ob_in_map(tmp, m, op, flag);
    }
}

int alchemy(object *op, object *caster, object *spell_ob)
{
    int x,y,weight=0,weight_max,large_nuggets,small_nuggets, mflags;
    sint16 nx, ny;
    object *next,*tmp;
    mapstruct *mp;

    if(op->type!=PLAYER)
	return 0;

    /* Put a maximum weight of items that can be alchemied.  Limits the power
     * some, and also prevents people from alcheming every table/chair/clock
     * in sight
     */
    weight_max = spell_ob->duration + +SP_level_duration_adjust(caster,spell_ob);
    weight_max *= 1000;
    small=get_archetype("smallnugget"),
    large=get_archetype("largenugget");

    for(y= op->y-1;y<=op->y+1;y++) {
	for(x= op->x-1;x<=op->x+1;x++) {
	    nx = x;
	    ny = y;

	    mp = op->map;

	    mflags = get_map_flags(mp, &mp, nx, ny, &nx, &ny);

	    if(mflags & (P_OUT_OF_MAP | P_WALL | P_NO_MAGIC))
		continue;

	    small_nuggets=0;
	    large_nuggets=0;

	    for(tmp=get_map_ob(mp,nx,ny);tmp!=NULL;tmp=next) {
		next=tmp->above;
		if (tmp->weight>0 && !QUERY_FLAG(tmp, FLAG_NO_PICK) &&
		    !QUERY_FLAG(tmp, FLAG_ALIVE) &&
		    !QUERY_FLAG(tmp, FLAG_IS_CAULDRON)) {

		    if (tmp->inv) {
			object *next1, *tmp1;
			for (tmp1 = tmp->inv; tmp1!=NULL; tmp1=next1) {
			    next1 = tmp1->below;
			    if (tmp1->weight>0 && !QUERY_FLAG(tmp1, FLAG_NO_PICK) &&
				!QUERY_FLAG(tmp1, FLAG_ALIVE) &&
				!QUERY_FLAG(tmp1, FLAG_IS_CAULDRON))
				alchemy_object(tmp1, &small_nuggets, &large_nuggets,
					   &weight);
			}
		    }
		    alchemy_object(tmp, &small_nuggets, &large_nuggets, &weight);
	    
		    if (weight>weight_max) {
			update_map(op, mp, small_nuggets, large_nuggets, nx, ny);
			free_object(large);
			free_object(small);
			return 1;
		    }
		} /* is alchemable object */
	    } /* process all objects on this space */

	    /* Insert all the nuggets at one time.  This probably saves time, but
	     * it also prevents us from alcheming nuggets that were just created
	     * with this spell.
	     */
	    update_map(op, mp, small_nuggets, large_nuggets, nx, ny);
	}
    }
    free_object(large);
    free_object(small);
    /* reset this so that if player standing on a big pile of stuff,
     * it is redrawn properly.
     */
    op->contr->socket.look_position = 0;
    return 1;
}


/* This function removes the cursed/damned status on equipped
 * items.
 */
int remove_curse(object *op, object *caster, object *spell) {
    object *tmp;
    int success = 0, was_one = 0;

    for (tmp = op->inv; tmp; tmp = tmp->below)
	if (QUERY_FLAG(tmp, FLAG_APPLIED) &&
	   ((QUERY_FLAG(tmp, FLAG_CURSED) && QUERY_FLAG(spell, FLAG_CURSED)) ||
	    (QUERY_FLAG(tmp, FLAG_DAMNED) && QUERY_FLAG(spell, FLAG_DAMNED)))) {

	    was_one++;
	    if (tmp->level <= caster_level(caster, spell)) {
		success++;
		if (QUERY_FLAG(spell, FLAG_DAMNED))
		    CLEAR_FLAG(tmp, FLAG_DAMNED);

		CLEAR_FLAG(tmp, FLAG_CURSED);
		CLEAR_FLAG(tmp, FLAG_KNOWN_CURSED);
		tmp->value = 0; /* Still can't sell it */
		if (op->type == PLAYER)
		    esrv_send_item(op, tmp);
	    }
	}

    if (op->type==PLAYER) {
	if (success) {
	    new_draw_info(NDI_UNIQUE, 0,op, "You feel like some of your items are looser now.");
	} else {
	    if (was_one)
		new_draw_info(NDI_UNIQUE, 0,op, "You failed to remove the curse.");
	    else 
		new_draw_info(NDI_UNIQUE, 0,op, "You are not using any cursed items.");
	}
    }
    return success;
}

/* Identifies objects in the players inventory/on the ground */

int cast_identify(object *op, object *caster, object *spell) {
    object *tmp;
    int success = 0, num_ident;

    num_ident = spell->stats.dam + SP_level_dam_adjust(caster, spell);

    if (num_ident < 1) num_ident=1;


    for (tmp = op->inv; tmp ; tmp = tmp->below) {
	if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED) && !tmp->invisible &&  need_identify(tmp)) {
	    identify(tmp);
	    if (op->type==PLAYER) {
		new_draw_info_format(NDI_UNIQUE, 0, op,
		     "You have %s.", long_desc(tmp, op));
		if (tmp->msg) {
		    new_draw_info(NDI_UNIQUE, 0,op, "The item has a story:");
		    new_draw_info(NDI_UNIQUE, 0,op, tmp->msg);
		}
	    }
	    num_ident--;
	    success=1;
	    if (!num_ident) break;
	}
    }
    /* If all the power of the spell has been used up, don't go and identify
     * stuff on the floor.  Only identify stuff on the floor if the spell
     * was not fully used.
     */
    if (num_ident) {
	for(tmp = get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above)
	if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED) && !tmp->invisible &&
	    need_identify(tmp)) {

	    identify(tmp);
	    if (op->type==PLAYER) {
		new_draw_info_format(NDI_UNIQUE, 0,op,
		     "On the ground is %s.", long_desc(tmp, op));
		if (tmp->msg) {
		    new_draw_info(NDI_UNIQUE, 0,op, "The item has a story:");
		    new_draw_info(NDI_UNIQUE, 0,op, tmp->msg);
		}
		esrv_send_item(op, tmp);
	    }
	    num_ident--;
	    success=1;
	    if (!num_ident) break;
	}
    }
    if (!success)
	new_draw_info(NDI_UNIQUE, 0,op, "You can't reach anything unidentified.");
    else {
	spell_effect(spell, op->x, op->y, op->map, op);
    }
    return success;
}


int cast_detection(object *op, object *caster, object *spell) {
    object *tmp, *last, *god, *detect;
    int done_one, range, mflags, floor, level;
    sint16 x, y, nx, ny;
    mapstruct	*m;

    /* We precompute some values here so that we don't have to keep
     * doing it over and over again.
     */
    god=find_god(determine_god(op));
    level=caster_level(caster, spell);
    range = spell->range + SP_level_range_adjust(caster, spell);

    for (x = op->x - range; x <= op->x + range; x++)
	for (y = op->y - range; y <= op->y + range; y++) {

	    m = op->map;
	    mflags = get_map_flags(m, &m, x, y, &nx, &ny);
	    if (mflags & P_OUT_OF_MAP) continue;

	    /* For most of the detections, we only detect objects above the
	     * floor.  But this is not true for show invisible.
	     * Basically, we just go and find the top object and work
	     * down - that is easier than working up.
	     */

	    for (last=NULL, tmp=get_map_ob(m, nx, ny); tmp; tmp=tmp->above) last=tmp;
	    /* Shouldn't happen, but if there are no objects on a space, this
	     * would happen.
	     */
	    if (!last) continue;

	    done_one=0;
	    floor=0;
	    detect = NULL;
	    for (tmp=last; tmp; tmp=tmp->below) {

		/* show invisible */
		if (QUERY_FLAG(spell, FLAG_MAKE_INVIS) &&
		    /* Might there be other objects that we can make visibile? */
		    (tmp->invisible && (QUERY_FLAG(tmp, FLAG_MONSTER) || 
		    (tmp->type==PLAYER && !QUERY_FLAG(tmp, FLAG_WIZ)) ||
		     tmp->type==CF_HANDLE || 
		     tmp->type==TRAPDOOR || tmp->type==EXIT || tmp->type==HOLE ||
		     tmp->type==BUTTON || tmp->type==TELEPORTER ||
		     tmp->type==GATE || tmp->type==LOCKED_DOOR ||
		     tmp->type==WEAPON || tmp->type==ALTAR || tmp->type==SIGN ||
		     tmp->type==TRIGGER_PEDESTAL || tmp->type==SPECIAL_KEY ||
		     tmp->type==TREASURE || tmp->type==BOOK ||
		     tmp->type==HOLY_ALTAR))) {
			if(random_roll(0, op->chosen_skill->level-1, op, PREFER_HIGH) > level/4) {
			    tmp->invisible=0;
			    done_one = 1;
			}
		}
		if (QUERY_FLAG(tmp, FLAG_IS_FLOOR)) floor=1;

		/* All detections below this point don't descend beneath the floor,
		 * so just continue on.  We could be clever and look at the type of
		 * detection to completely break out if we don't care about objects beneath
		 * the floor, but once we get to the floor, not likely a very big issue anyways.
		 */
		if (floor) continue;

		/* I had thought about making detect magic and detect curse
		 * show the flash the magic item like it does for detect monster.
		 * however, if the object is within sight, this would then make it
		 * difficult to see what object is magical/cursed, so the
		 * effect wouldn't be as apparant.
		 */

		/* detect magic */
		if (QUERY_FLAG(spell, FLAG_KNOWN_MAGICAL) && 
		    !QUERY_FLAG(tmp,FLAG_KNOWN_MAGICAL) && 
		    !QUERY_FLAG(tmp, FLAG_IDENTIFIED) &&
		    is_magical(tmp)) {
			SET_FLAG(tmp,FLAG_KNOWN_MAGICAL);
			/* make runes more visibile */
			if(tmp->type==RUNE && tmp->attacktype&AT_MAGIC)
			    tmp->stats.Cha/=4;
			done_one = 1;
		}
		/* detect monster */
		if (QUERY_FLAG(spell, FLAG_MONSTER) &&
		    (QUERY_FLAG(tmp, FLAG_MONSTER) || tmp->type==PLAYER)) {
			done_one = 2;
			if (!detect) detect=tmp;
		}
		/* Basically, if race is set in the spell, then the creatures race must
		 * match that.  if the spell race is set to GOD, then the gods opposing
		 * race must match.
		 */
		if (spell->race && QUERY_FLAG(tmp,FLAG_MONSTER) && tmp->race &&
		    ((!strcmp(spell->race, "GOD") && god && god->slaying && strstr(god->slaying,tmp->race)) ||
		     (strstr(spell->race, tmp->race)))) {
			done_one = 2;
			if (!detect) detect=tmp;
		}
		if (QUERY_FLAG(spell, FLAG_KNOWN_CURSED) && !QUERY_FLAG(tmp, FLAG_KNOWN_CURSED) &&
			(QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED))) {
			SET_FLAG(tmp, FLAG_KNOWN_CURSED);
			done_one = 1;
		}
	    } /* for stack of objects on this space */

	    /* Code here puts an effect of the spell on the space, so you can see
	     * where the magic is.
	     */
	    if (done_one) {
		object *detect_ob = arch_to_object(spell->other_arch);
		detect_ob->x = nx;
		detect_ob->y = ny;
		/* if this is set, we want to copy the face */
		if (done_one == 2 && detect) {
		    detect_ob->face = detect->face;
		    detect_ob->animation_id = detect->animation_id;
		    detect_ob->anim_speed = detect->anim_speed;
		    detect_ob->last_anim=0;
		    /* by default, the detect_ob is already animated */
		    if (!QUERY_FLAG(detect, FLAG_ANIMATE)) CLEAR_FLAG(detect_ob, FLAG_ANIMATE);
		}
		insert_ob_in_map(detect_ob, m, op,0);
	    }
	} /* for processing the surrounding spaces */


    /* Now process objects in the players inventory if detect curse or magic */
    if (QUERY_FLAG(spell, FLAG_KNOWN_CURSED) || QUERY_FLAG(spell, FLAG_KNOWN_MAGICAL)) {
	done_one = 0;
	for (tmp = op->inv; tmp; tmp = tmp->below) {
	    if (!QUERY_FLAG(tmp, FLAG_IDENTIFIED)) {
		if (QUERY_FLAG(spell, FLAG_KNOWN_MAGICAL) && 
		    is_magical(tmp) && !QUERY_FLAG(tmp,FLAG_KNOWN_MAGICAL)) {
			SET_FLAG(tmp,FLAG_KNOWN_MAGICAL);
			if (op->type==PLAYER)
			    esrv_send_item (op, tmp);
		}
		if (QUERY_FLAG(spell, FLAG_KNOWN_CURSED) && !QUERY_FLAG(tmp, FLAG_KNOWN_CURSED) &&
		    (QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED))) {
			SET_FLAG(tmp, FLAG_KNOWN_CURSED);
			if (op->type==PLAYER)
			    esrv_send_item (op, tmp);
		}
	    } /* if item is not identified */
	} /* for the players inventory */
    } /* if detect magic/curse and object is a player */
    return 1;
}



/* cast_transfer
 * This spell transfers sp from the player to another person.
 * We let the target go above their normal maximum SP.
 */

int cast_transfer(object *op,object *caster, object *spell, int dir) {
    object *plyr=NULL;
    sint16 x, y;
    mapstruct *m;
    int mflags;

    m = op->map;
    x =  op->x+freearr_x[dir];
    y = op->y+freearr_y[dir];

    mflags = get_map_flags(m, &m, x, y, &x, &y);

    if (!(mflags & P_OUT_OF_MAP) && mflags & P_IS_ALIVE) {
	for(plyr=get_map_ob(m, x, y); plyr!=NULL; plyr=plyr->above)
	    if(QUERY_FLAG(plyr, FLAG_ALIVE))
		break;
    }


    /* If we did not find a player in the specified direction, transfer
     * to anyone on top of us. This is used for the rune of transference mostly.
     */
    if(plyr==NULL)
        for(plyr=get_map_ob(op->map,op->x,op->y); plyr!=NULL; plyr=plyr->above)
	    if(QUERY_FLAG(plyr,FLAG_ALIVE))
		break;

    if (!plyr) {
	new_draw_info(NDI_BLACK, 0, op, "There is no one there.");
	return 0;
    }
    /* give sp */
    if(spell->stats.dam > 0) {
	plyr->stats.sp += spell->stats.dam + SP_level_dam_adjust(caster, spell);

	new_draw_info(NDI_UNIQUE, 0,plyr,"You feel energy course through you.");

	if(plyr->stats.sp>=plyr->stats.maxsp*2) {
	    object *tmp;

            new_draw_info(NDI_UNIQUE, 0,plyr,"Your head explodes!");

	    /* Explodes a fireball centered at player */
	    tmp = get_archetype(EXPLODING_FIREBALL);
	    tmp->dam_modifier=random_roll(1, op->level, op, PREFER_LOW)/5+1;
	    tmp->stats.maxhp=random_roll(1, op->level, op, PREFER_LOW)/10+2;
	    tmp->x = plyr->x;
	    tmp->y = plyr->y;
	    insert_ob_in_map(tmp, plyr->map, NULL, 0);
	    plyr->stats.sp = 2*plyr->stats.maxsp;
        }
        else if(plyr->stats.sp>=plyr->stats.maxsp*1.88)
            new_draw_info(NDI_UNIQUE, NDI_ORANGE,plyr,"You feel like your head is going to explode.");
        else if(plyr->stats.sp>=plyr->stats.maxsp*1.66)
            new_draw_info(NDI_UNIQUE, 0,plyr, "You get a splitting headache!");
        else if(plyr->stats.sp>=plyr->stats.maxsp*1.5) {
            new_draw_info(NDI_UNIQUE, 0,plyr,"Chaos fills your world.");
	    confuse_player(op,op,99);
	}
        else if(plyr->stats.sp>=plyr->stats.maxsp*1.25)
            new_draw_info(NDI_UNIQUE, 0,plyr,"You start hearing voices.");
        return 1;
    }
    /* suck sp away.  Can't suck sp from yourself */
    else if (op != plyr) {
	/* old dragin magic used floats.  easier to just use ints and divide by 100 */

	int rate = -spell->stats.dam + SP_level_dam_adjust(caster, spell), sucked=0;

	if (rate > 95) rate=95;

	sucked = (plyr->stats.sp * rate) / 100;
	plyr->stats.sp -= sucked;
	/* Player doesn't get full credit */
	if (QUERY_FLAG(op, FLAG_ALIVE)) {
	    sucked = (sucked * rate) / 100;
	    op->stats.sp += sucked;
	}
	return 1;
    }
    return 0;
}


/* counterspell:  nullifies spell effects.
 * op is the counterspell object, dir is the direction
 * it was cast in.
 * Basically, if the object has a magic attacktype,
 * this may nullify it.
 */
void counterspell(object *op,int dir)
{
    object *tmp, *head, *next;
    int mflags;
    mapstruct *m;
    sint16  sx,sy;

    sx = op->x + freearr_x[dir];
    sy = op->y + freearr_y[dir];
    m = op->map;
    mflags = get_map_flags(m, &m, sx, sy, &sx, &sy);
    if (mflags & P_OUT_OF_MAP) return;
    
    for(tmp=get_map_ob(m,sx,sy); tmp!=NULL; tmp=next) {
	next = tmp->above;

	/* Need to look at the head object - otherwise, if tmp
	 * points to a monster, we don't have all the necessary
	 * info for it.
	 */
	if (tmp->head) head = tmp->head;
	else head = tmp;

       /* don't attack our own spells */
       if(tmp->owner && tmp->owner == op->owner) continue;

	/* Basically, if the object is magical and not counterspell, 
	 * we will more or less remove the object.  Don't counterspell
	 * monsters either. 
         */

	if (head->attacktype & AT_MAGIC &&
	   !(head->attacktype & AT_COUNTERSPELL) && 
	   !QUERY_FLAG(head,FLAG_MONSTER) && 
	   (op->level > head->level)) {
		remove_ob(head);
		free_object(head);
	} else switch(head->type) {
	    case SPELL_EFFECT: 
		if(op->level > head->level) {
		    remove_ob(head);
		    free_object(head);
		}
		break;

	    /* I really don't get this rune code that much - that
	     * random chance seems really low.
	     */
	    case RUNE:
		if(rndm(0, 149) == 0) {
		    head->stats.hp--;  /* weaken the rune */
		    if(!head->stats.hp) {
			remove_ob(head);
			free_object(head);
		    }
		}
		break;
	}
    }
}



/* cast_consecrate() - a spell to make an altar your god's */
int cast_consecrate(object *op, object *caster, object *spell) {
    char buf[MAX_BUF];

    object *tmp, *god=find_god(determine_god(op));

    if(!god) {
	new_draw_info(NDI_UNIQUE, 0,op,
		      "You can't consecrate anything if you don't worship a god!");
        return 0;
    }
 
    for(tmp=op->below;tmp;tmp=tmp->below) {
	if(QUERY_FLAG(tmp,FLAG_IS_FLOOR)) break;
        if(tmp->type==HOLY_ALTAR) {

	    if(tmp->level > caster_level(caster, spell)) {
		new_draw_info_format(NDI_UNIQUE, 0,op,
		    "You are not poweful enough to reconsecrate the %s", tmp->name);
		return 0;
	    } else {
		/* If we got here, we are consecrating an altar */
		if(tmp->name)	free_string(tmp->name);
		sprintf(buf,"Altar of %s",god->name);
		tmp->name = add_string(buf);
		tmp->level = caster_level(caster, spell);
		tmp->other_arch = god->arch;
		if(op->type==PLAYER) esrv_update_item(UPD_NAME, op, tmp);
		new_draw_info_format(NDI_UNIQUE,0, op,
				     "You consecrated the altar to %s!",god->name);
		return 1;
	    }
	}
    }
    new_draw_info(NDI_UNIQUE, 0,op,"You are not standing over an altar!");
    return 0;
}

/* animate_weapon - 
 * Generalization of staff_to_snake.  Makes a golem out of the caster's weapon.
 * The golem is based on the archetype specified, modified by the caster's level
 * and the attributes of the weapon.  The weapon is inserted in the golem's 
 * inventory so that it falls to the ground when the golem dies.
 * This code was very odd - code early on would only let players use the spell,
 * yet the code wass full of player checks.  I've presumed that the code
 * that only let players use it was correct, and removed all the other
 * player checks. MSW 2003-01-06
 */

int animate_weapon(object *op,object *caster,object *spell, int dir) {
    object *weapon, *tmp;
    char buf[MAX_BUF];
    int a, i;
    sint16 x, y;
    mapstruct *m;
    materialtype_t *mt;
 
    if(!spell->other_arch){
	new_draw_info(NDI_UNIQUE, 0,op,"Oops, program error!");
	LOG(llevError,"animate_weapon failed: spell %s missing other_arch!\n", spell->name);
	return 0;
    }
    /* exit if it's not a player using this spell. */
    if(op->type!=PLAYER) return 0;

    /* if player already has a golem, abort */
    if(op->contr->ranges[range_golem]!=NULL && op->contr->golem_count == op->contr->ranges[range_golem]->count) {
	control_golem(op->contr->ranges[range_golem],dir);
	return 0;
    }

    /* if no direction specified, pick one */
    if(!dir) 
	dir=find_free_spot(NULL,op->map,op->x,op->y,1,9);

    m = op->map;
    x = op->x+freearr_x[dir];
    y = op->y+freearr_y[dir];

    /* if there's no place to put the golem, abort */
    if((dir==-1) || (get_map_flags(m, &m, x, y, &x, &y) & (P_BLOCKED | P_OUT_OF_MAP))) {
	new_draw_info(NDI_UNIQUE, 0,op,"There is something in the way.");
	return 0;
    }

    /* Use the weapon marked by the player. */
    weapon = find_marked_object(op);

    if (!weapon) {
	new_draw_info(NDI_BLACK, 0, op, "You must mark a weapon to use with this spell!");
	return 0;
    }
    if (spell->race && strcmp(weapon->arch->name, spell->race)) {
	new_draw_info(NDI_UNIQUE, 0,op,"The spell fails to transform your weapon.");
	return 0;
    }
    if (weapon->type != WEAPON) {
	new_draw_info(NDI_UNIQUE, 0,op,"You need to wield a weapon to animate it.");
	return 0;
    }
    if (QUERY_FLAG(weapon, FLAG_APPLIED)) {
	new_draw_info_format(NDI_BLACK, 0, op, "You need to unequip %s before using it in this spell",
			 query_name(weapon));
	return 0;
    }

    if (weapon->nrof > 1) {
	tmp = get_split_ob(weapon, 1);
	esrv_send_item(op, weapon);
	weapon = tmp;
    }

    /* create the golem object */
    tmp=arch_to_object(spell->other_arch);

    /* if animated by a player, give the player control of the golem */
    CLEAR_FLAG(tmp, FLAG_MONSTER);
    SET_FLAG(tmp, FLAG_FRIENDLY);
    tmp->stats.exp=0;
    add_friendly_object(tmp);
    tmp->type=GOLEM;
    set_owner(tmp,op);
    set_spell_skill(op, caster, spell, tmp);
    op->contr->ranges[range_golem]=tmp;
    op->contr->shoottype=range_golem;
    op->contr->golem_count = tmp->count;

    /* Give the weapon to the golem now.  A bit of a hack to check the
     * removed flag - it should only be set if get_split_object was
     * used above.
     */
    if (!QUERY_FLAG(weapon, FLAG_REMOVED))
	remove_ob (weapon);
    insert_ob_in_ob (weapon, tmp);
    esrv_send_item(op, weapon);
    /* To do everything necessary to let a golem use the weapon is a pain,
     * so instead, just set it as equipped (otherwise, we need to update
     * body_info, skills, etc)
     */		   
    SET_FLAG (tmp, FLAG_USE_WEAPON);
    SET_FLAG(weapon, FLAG_APPLIED);
    fix_player(tmp);

    /* There used to be 'odd' code that basically seemed to take the absolute
     * value of the weapon->magic an use that.  IMO, that doesn't make sense -
     * if you're using a crappy weapon, it shouldn't be as good.
     */

    /* modify weapon's animated wc */
    tmp->stats.wc = tmp->stats.wc - SP_level_range_adjust(caster,spell) 
	- 5 * weapon->stats.Dex - 2 * weapon->stats.Str - weapon->magic;
    if(tmp->stats.wc<-127) tmp->stats.wc = -127;

    /* Modify hit points for weapon */
    tmp->stats.maxhp = tmp->stats.maxhp + spell->duration + 
	SP_level_duration_adjust(caster, spell) + 
	+ 8 * weapon->magic + 12 * weapon->stats.Con;
    if(tmp->stats.maxhp<0) tmp->stats.maxhp=10;
    tmp->stats.hp = tmp->stats.maxhp;

    /* Modify weapon's damage */
    tmp->stats.dam = spell->stats.dam + SP_level_dam_adjust(caster, spell)
	+ weapon->stats.dam
	+ weapon->magic
	+ 5 * weapon->stats.Str;
    if(tmp->stats.dam<0) tmp->stats.dam=127;


    /* attacktype */
    if ( ! tmp->attacktype)
	tmp->attacktype = AT_PHYSICAL;

    mt = NULL;
    if (op->materialname != NULL)
	mt = name_to_material(op->materialname);
    if (mt != NULL) {
	for (i=0; i < NROFATTACKS; i++)
	    tmp->resist[i] = 50 - (mt->save[i] * 5);
	a = mt->save[0];
    } else {
	for (i=0; i < NROFATTACKS; i++)
	    tmp->resist[i] = 5;
	a = 10;
    }
    /* Set weapon's immunity */
    tmp->resist[ATNR_CONFUSION] = 100;
    tmp->resist[ATNR_POISON] = 100;
    tmp->resist[ATNR_SLOW] = 100;
    tmp->resist[ATNR_PARALYZE] = 100;
    tmp->resist[ATNR_TURN_UNDEAD] = 100;
    tmp->resist[ATNR_FEAR] = 100;
    tmp->resist[ATNR_DEPLETE] = 100;
    tmp->resist[ATNR_DEATH] = 100;
    tmp->resist[ATNR_BLIND] = 100;

    /* Improve weapon's armour value according to best save vs. physical of its material */

    if (a > 14) a = 14;
    tmp->resist[ATNR_PHYSICAL] = 100 - (int)((100.0-(float)tmp->resist[ATNR_PHYSICAL])/(30.0-2.0*a));

    /* Determine golem's speed */
    tmp->speed = 0.4 + 0.1 * SP_level_range_adjust(caster,spell);

    if(tmp->speed > 3.33) tmp->speed = 3.33;

    if (!spell->race) {
	sprintf(buf, "animated %s", weapon->name);
	if(tmp->name) free_string(tmp->name);
	tmp->name = add_string(buf);

	tmp->face = weapon->face;
	tmp->animation_id = weapon->animation_id;
	tmp->anim_speed = weapon->anim_speed;
	tmp->last_anim = weapon->last_anim;
	tmp->state = weapon->state;
	if(QUERY_FLAG(weapon, FLAG_ANIMATE)) {
	    SET_FLAG(tmp,FLAG_ANIMATE); 
	} else {
	    CLEAR_FLAG(tmp,FLAG_ANIMATE); 
	}
	update_ob_speed(tmp);
    }

    /*  make experience increase in proportion to the strength of the summoned creature. */
    tmp->stats.exp *= 1 + (MAX(spell->stats.maxgrace, spell->stats.sp) / caster_level(caster, spell));

    tmp->speed_left= -1;
    tmp->x=x;
    tmp->y=y;
    tmp->direction=dir;
    insert_ob_in_map(tmp,m,op,0);
    return 1;
}

/* cast_daylight() - changes the map darkness level *lower* */

/* cast_change_map_lightlevel: Was cast_daylight/nightfall.
 * This changes the light level for the entire map.
 */

int cast_change_map_lightlevel( object *op, object *caster, object *spell ) {
    int success;

    if(!op->map) return 0;   /* shouldnt happen */ 

    success=change_map_light(op->map,spell->stats.dam);
    if(!success) {
	if (spell->stats.dam < 0)
	    new_draw_info(NDI_UNIQUE,0,op,"It can be no brighter here."); 
	else
	    new_draw_info(NDI_UNIQUE,0,op,"It can be no darker here."); 
    }
    return success;
}





/* create an aura spell object and put it in the player's inventory.
 * as usual, op is player, caster is the object casting the spell,
 * spell is the spell object itself.
 */
int create_aura(object *op, object *caster, object *spell)
{
    int refresh=0;
    object *new_aura;

    new_aura = present_arch_in_ob(spell->other_arch, op);
    if (new_aura) refresh=1;
    else new_aura = arch_to_object(spell->other_arch);

    new_aura->duration  = spell->duration + 
                  10* SP_level_duration_adjust(caster,spell);

    new_aura->stats.dam = spell->stats.dam
                  +SP_level_dam_adjust(caster,spell);

    set_owner(new_aura,op);
    set_spell_skill(op, caster, spell, new_aura);
    new_aura->attacktype= spell->attacktype;

    new_aura->level = caster_level(caster, spell);
    if (refresh) 
	new_draw_info(NDI_UNIQUE, 0, op, "You recast the spell while in effect.");
    else
	insert_ob_in_ob(new_aura, op);
    return 1;
}


/* move aura function.  An aura is a part of someone's inventory,
 * which he carries with him, but which acts on the map immediately
 * around him.
 * Aura parameters:
 * duration:  duration counter.   
 * attacktype:  aura's attacktype 
 * other_arch:  archetype to drop where we attack
 */

void move_aura(object *aura) {
    int i, mflags;
    object *env;

    /* auras belong in inventories */
    env = aura->env;

    /* no matter what we've gotta remove the aura...
     * we'll put it back if its time isn't up.  
     */
    remove_ob(aura);

    /* exit if we're out of gas */
    if(aura->duration--< 0) {
	free_object(aura);
	return;
    }

    /* auras only exist in inventories */
    if(env == NULL || env->map==NULL) {
	free_object(aura);
	return;
    }
    aura->x = env->x;
    aura->y = env->y;

    /* we need to jump out of the inventory for a bit
     * in order to hit the map conveniently. 
     */
    insert_ob_in_map(aura,env->map,aura,0);

    for(i=1;i<9;i++) { 
	int nx, ny;
	nx = aura->x + freearr_x[i];
	ny = aura->y + freearr_y[i];
	mflags = get_map_flags(env->map, NULL, nx, ny, NULL, NULL);
	if ((mflags & (P_WALL | P_OUT_OF_MAP)) == 0) {
	    hit_map(aura,i,aura->attacktype);
	    if(aura->other_arch) {
		object *new_ob;

		new_ob = arch_to_object(aura->other_arch);
		new_ob->x = nx;
		new_ob->y = ny;
		insert_ob_in_map(new_ob,env->map,aura,0);
	    }
	}
    }
    /* put the aura back in the player's inventory */
    remove_ob(aura);
    insert_ob_in_ob(aura, env);
}

/* moves the peacemaker spell.
 * op is the piece object.
 */      

void move_peacemaker(object *op) {
    object *tmp;

    for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above) {
	int atk_lev, def_lev;
	object *victim=tmp;

	if (tmp->head) victim=tmp->head;
	if (!QUERY_FLAG(victim,FLAG_MONSTER)) continue;
	if (QUERY_FLAG(victim,FLAG_UNAGGRESSIVE)) continue;
	if (victim->stats.exp == 0) continue;

	def_lev = MAX(1,victim->level);
	atk_lev = MAX(1,op->level);

	if (rndm(0, atk_lev-1) > def_lev) {
	    /* make this sucker peaceful. */

	    change_exp(get_owner(op),victim->stats.exp, op->skill, 0);
	    victim->stats.exp=0;
#if 0
	    /* No idea why these were all set to zero - if something
	     * makes this creature agressive, he should still do damage.
	     */
	    victim->stats.dam = 0;
	    victim->stats.sp = 0;
	    victim->stats.grace = 0;
	    victim->stats.Pow = 0;
#endif
	    victim->move_type = RANDO2;
	    SET_FLAG(victim,FLAG_UNAGGRESSIVE);
	    SET_FLAG(victim,FLAG_RUN_AWAY);
	    SET_FLAG(victim,FLAG_RANDOM_MOVE);
	    CLEAR_FLAG(victim,FLAG_MONSTER);
	    if(victim->name) {
		new_draw_info_format(NDI_UNIQUE,0,op->owner,"%s no longer feels like fighting.",victim->name);
	    }
	}
    }
}   
      

/* This writes a rune that contains the appropriate message.
 * There really isn't any adjustments we make.
 */

int write_mark(object *op, object *spell, char *msg) {
    char rune[HUGE_BUF];
    object *tmp;

    if (!msg || msg[0] == 0) {
	new_draw_info(NDI_UNIQUE, 0, op, "Write what?");
	return 0;
    }

    if (strcasestr_local(msg, "endmsg")) {
	new_draw_info(NDI_UNIQUE, 0, op, "Trying to cheat are we?");
	    LOG(llevInfo,"write_rune: player %s tried to write bogus rune %s\n", op->name, msg);
	    return 0;
    }
    if (!spell->other_arch) return 0;
    tmp = arch_to_object(spell->other_arch);
    strncpy(rune, msg, HUGE_BUF-2);
    rune[HUGE_BUF-2] = 0;
    strcat(rune, "\n");
    tmp->msg = add_string(rune);
    tmp->x = op->x;
    tmp->y = op->y;
    insert_ob_in_map(tmp, op->map, op, INS_BELOW_ORIGINATOR);
    return 1;
}

