%{
/*
 * static char *rcsid_object_c =
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

/* Eneq(@csd.uu.se): Added weight-modifiers in environment of objects.
   sub/add_weight will transcend the environment updating the carrying
   variable. */


#include <global.h>
#include <loader.h>
#include <newserver.h>
#include <sproto.h>

#define YY_DECL int lex_load(object *op, int map_flags)

static char *yval();

static int lex_error;
static char msgbuf[HUGE_BUF];
static char lorebuf[HUGE_BUF];

#define SET_OR_CLEAR_FLAG(op, flag, val) \
	{ if (val) SET_FLAG(op, flag); else CLEAR_FLAG(op, flag); }

/* SET_RESIST is really only really needed for transition code.  We normally
 * don't care about multiple values overwriting each other, but this is
 * to catch items that have multiple protection/immune/vulnerable.
 * This can be simplified later on to just do the set after all the archs
 * and maps have been updated.
 * We always keep the last value because otherwise the value from the
 * arch may take precedence.
 * Unfortunately, we will report warnings here simply because an object has
 * been modified from the arch.
 */
#if 0	/* #if's don't work in #define macros */
#define SET_RESIST(op, type, val) \
	{if (op->resist[type]!=0) { \
	    LOG(llevInfo, "object %s having multiple resistances set, type=%s, old=%d, new=%d\n", \
		       op->name?op->name:(op->arch?op->arch->name:"unknown"), \
					resist_plus[type], op->resist[type], val); \
	}  op->resist[type] = val;  }
#else
#define SET_RESIST(op, type, val)  op->resist[type] = val;
#endif

#define IVAL	atoi(yval())
#define FVAL	atof(yval())
extern int arch_init;

/* Put this here since it is used below */
static void set_protection(object *op, uint32 mask, uint16 pro_val)
{
    int i;

    if (!mask) return;	/* Unlikely, but might as well check */
    for (i=0; i<NROFATTACKS; i++) {
	if (mask & (1<<i)) SET_RESIST(op, i, pro_val);
    }
}


static void set_body_info(object *op, char *params) {
    int i;
    char *cp;

    /* go to first space character */
    for (cp=params; !isspace(*cp); cp++) ;

    *cp++ = 0;	/* null it out */

    for (i=0; i<NUM_BODY_LOCATIONS; i++) {
	if (!strcmp(params, body_locations[i].save_name)) {
	    op->body_info[i] = atoi(cp);
	    return;	/* Only one line passed in params */
	}
    }
    LOG(llevError,"set_body_info called with bogus params: %s\n", params);
}


/* This function checks the object after it has been loaded (when we
 * get the 'end' in the input stream).  This function can be used to 
 * deal with legacy objects where fields may have changed.  It can also be used
 * to check for objects to make sure there are no common errors.
 */
static void check_loaded_object(object *op) {
    int ip;
    char buf[MAX_BUF], *p;
    materialtype_t *mt;

    /* We do some specialized handling to handle legacy cases of name_pl.
     * If the object doesn't have a name_pl, we just use the object name -
     * this isn't perfect (things won't be properly pluralized), but works to
     * that degree (5 heart is still quite understandable).  But the case we
     * also have to catch is if this object is not using the normal name for
     * the object.  In that case, we also want to use the loaded name.
     * Otherwise, what happens is that the the plural name will lose
     * information (appear as just 'hearts' and not 'goblins heart') 
     */
    if (op->arch && op->name != op->arch->clone.name && op->name_pl == op->arch->clone.name_pl) {
	if (op->name_pl) free_string(op->name_pl);
	    op->name_pl = NULL;
    }
    if (!op->name_pl) op->name_pl = add_string(op->name);

    /* objects now have a materialname.  try to patch it in */
    if (!(IS_WEAPON(op) && op->level > 0)) {
        if (op->map != NULL)
            set_materialname(op, op->map->difficulty, NULL);
        else
            set_materialname(op, 5, NULL);
    }
    /* back out the materialname from op->name very limited subset of obs */
    if ((IS_WEAPON(op) || IS_ARMOR(op)) && op->name && op->materialname) {
        mt = name_to_material(op->materialname);
        if (mt && strstr(op->name, mt->description) &&
	    strstr(op->name, mt->description) == op->name) {
	    sprintf(buf, "%s", op->name);
	    p = buf + strlen(mt->description) + 1;
	    free_string(op->name);
	    op->name = add_string(p);
	    sprintf(buf, "%s", op->name_pl);
	    p = buf + strlen(mt->description) + 1;
	    free_string(op->name_pl);
	    op->name_pl = add_string(p);
	}
    }


    /* We changed last_heal to gen_sp_armour, which is what it
     * really does for many objects.  Need to catch any in maps
     * that may have an old value.
     */
    if ((op->type == WEAPON) ||
        (op->type == ARMOUR)   || (op->type == HELMET) ||
        (op->type == SHIELD)   || (op->type == RING) ||
        (op->type == BOOTS)    || (op->type == GLOVES) ||
        (op->type == AMULET )  || (op->type == GIRDLE) ||
        (op->type == BRACERS ) || (op->type == CLOAK)) {
	if (op->last_heal) {
	    LOG(llevDebug,"Object %s still has last_heal set, not gen_sp_armour\n",
		op->name?op->name:"NULL");
	    op->gen_sp_armour = op->last_heal;
	    op->last_heal = 0;
	}
	if (editor) ip =0;
	else ip = calc_item_power(op, 0);
	/* Legacy objects from before item power was in the game */
	if (!op->item_power && ip) {
	    if (ip > 3) {
		LOG(llevDebug,"Object %s had no item power, using %d\n", 
		    op->name?op->name:"NULL", ip);
	    }
	    op->item_power = ip;
	}
	/* Check for possibly bogus values.  Has to meet both these criteria -
	 * something that has item_power 1 is probably just fine if our calculated
	 * value is 1 or 2 - these values are small enough that hard to be precise.
	 * similarly, it item_power is 0, the first check will always pass,
	 * but not the second one.
	 */
	if (ip > 2 *op->item_power && ip > (op->item_power + 3)) {
	    LOG(llevDebug,"Object %s seems to have too low item power? %d > %d\n",
		op->name?op->name:"NULL", ip, op->item_power);
	}

    }
    if (QUERY_FLAG(op, FLAG_MONSTER)) {
	if (op->stats.hp > op->stats.maxhp)
	    LOG(llevDebug,"Monster %s has hp set higher than maxhp (%d>%d)\n",
		op->name,
		op->stats.hp, op->stats.maxhp);
    }
    if ((QUERY_FLAG(op,FLAG_GENERATOR)) && QUERY_FLAG(op,FLAG_CONTENT_ON_GEN)){
        /* Generator will duplicate it's content as part of the
         * generation process. To do this, we must flag inventory
         * so it remains unevaluated concerning the randomitems and
         * the living (a demonlord shouldn't cast from inside generator!)
         */
        flag_inv(op,FLAG_IS_A_TEMPLATE);
    }

}


%}



S	[ \t]+.+
WS	[ \t]*
A	.+

%x MESSAGE
%x LORE
%x SCRIPT

/* Don't have to link with -lfl with this */
%option noyywrap

/* need yy_push_state, yy_pop_state */
%option stack

%%

%{
/* Declare some local variables */
    int ismore=0;

    lex_error=0;

%}

^msg{WS}$	    {	BEGIN( MESSAGE ); msgbuf[0]='\0'; }
<MESSAGE>^endmsg{WS}$ {	BEGIN( INITIAL );
			op->msg=add_string(msgbuf);
			/* Just print a warning so we can be reasonably safe
			 * about not overflowing the buffer.
			 */
			if (strlen(op->msg) > (HUGE_BUF/2))
			    LOG(llevDebug, "\n\tWarning message length > %d (max allowed=%d): %d\n>%.80s<\n",
				HUGE_BUF/2, HUGE_BUF, strlen(op->msg),op->msg);
		    }
<MESSAGE>.*	    {strcat(msgbuf, yytext); strcat(msgbuf,"\n"); }

^lore{WS}$	    {	BEGIN( LORE ); lorebuf[0]='\0'; }
<LORE>^endlore{WS}$ {	BEGIN( INITIAL );
			op->lore=add_string(lorebuf);
			/* Just print a warning so we can be reasonably safe
			 * about not overflowing the buffer.
			 */
			if (strlen(op->lore) > (HUGE_BUF/2))
			    LOG(llevDebug, "\n\tWarning lore length > %d (max allowed=%d): %d\n>%.80s<\n",
				HUGE_BUF/2, HUGE_BUF, strlen(op->lore),op->lore);
		    }
<LORE>.*	    {strcat(lorebuf, yytext); strcat(lorebuf,"\n"); }

^object{S}	    {	char *yv=yval();

			if (*yv=='\0') {
			    LOG(llevError,"Object lacks name.\n");
			    return LL_IGNORED;
			}
		        if (!arch_init) {
			    LOG(llevError,"Got object info when not in arch_init (%s)?\n", yv);
			} else {
			    if (op->arch!=NULL) op->arch->name=add_string(yv);
			    op->name = add_string(yv);
			}
		    }

^name{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Name without val\n");
			else FREE_AND_COPY(op->name, yv);
		    }
^name_pl{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Name without val\n");
			else FREE_AND_COPY(op->name_pl, yv);
		    }
^race{S}	    FREE_AND_COPY(op->race,yval());
^slaying{S}	    FREE_AND_COPY(op->slaying, yval());
^inventory.*$	    LOG(llevError,"Got depreciated Inventory command?\n");


^arch{S}         {	/* If op->arch has been set, then this new object
			 * must be part of the inventory.  So process
			 * appropriately.
			 */
			if (op->arch) {
			    object *tmp;

			    tmp=get_object();
			    tmp->arch = find_archetype(yval());
			    if (tmp->arch!=NULL)
				copy_object(&tmp->arch->clone,tmp);
			    strcpy(msgbuf, "");
			    strcpy(lorebuf, "");
			    lex_load(tmp, map_flags);
			    insert_ob_in_ob(tmp,op);
			}
			/* This is the actual archetype definition then */
			else {
			    op->arch=find_archetype(yval());
			    if (op->arch!=NULL) copy_object(&op->arch->clone,op);
			}
		    }

^other_arch{S}        op->other_arch=find_archetype(yval());
^animation{S}	    {
			if (strcmp (yval(), "NONE") == 0) {
			    op->animation_id = 0;
			    CLEAR_FLAG (op, FLAG_ANIMATE);
			} else {
			    op->animation_id = find_animation (yval());
			    SET_FLAG (op, FLAG_ANIMATE);
			}
		    }

^more{WS}$	    { /* We need to record that this is a multipart object,
		       * so the calling function can glue things back together
		       */
			ismore=1;
		    }

^end{WS}$	    {	check_loaded_object(op);
			if (ismore) return LL_MORE; 
			else return LL_NORMAL;
		    }
^last_heal{S}	    op->last_heal = IVAL;
^last_sp{S}	    op->last_sp = IVAL;
^last_grace{S}	    op->last_grace = IVAL;
^last_eat{S}	    op->last_eat = IVAL;
^speed{S}	    {	op->speed = FVAL;
			if (!(map_flags & MAP_STYLE)) {
			    if (op->speed<0) op->speed_left = op->speed_left-RANDOM()%100/100.0;
			    update_ob_speed(op);
			}
		    }
^speed_left{S}    op->speed_left = FVAL;
^slow_move{S}	    {	SET_SLOW_PENALTY(op,FVAL);
			SET_FLAG(op, FLAG_SLOW_MOVE);
		    }
^title{S}	    {	char *y=yval();
			if (*y=='\0') LOG(llevError,"Title without value.\n");
			else FREE_AND_COPY(op->title, y);
		    }

^face{S}	op->face = &new_faces[FindFace(yval(), 0)];
^str{S}		op->stats.Str = IVAL;
^dex{S}		op->stats.Dex = IVAL;
^con{S}		op->stats.Con = IVAL;
^wis{S}		op->stats.Wis = IVAL;
^cha{S}		op->stats.Cha = IVAL;
^int{S}		op->stats.Int = IVAL;
^pow{S}		op->stats.Pow = IVAL;
^hp{S}		op->stats.hp = IVAL;
^maxhp{S}	op->stats.maxhp = IVAL;
^sp{S}		op->stats.sp = IVAL;
^maxsp{S}	op->stats.maxsp = IVAL;
^grace{S}	op->stats.grace = IVAL;
^maxgrace{S}	op->stats.maxgrace = IVAL;
^exp{S}		op->stats.exp = atoll(yval());
^food{S}	op->stats.food = IVAL;
^dam{S}		op->stats.dam = IVAL;
^wc{S}		op->stats.wc = IVAL;
^ac{S}		op->stats.ac = IVAL;
^x{S}		{op->x = IVAL; op->ox= op->x; }
^y{S}		{op->y = IVAL; op->oy= op->y; }
^nrof{S}	op->nrof= atol(yval());
^level{S}	op->level = IVAL;
^direction{S}	op->direction = IVAL;
^type{S}	op->type = IVAL;
^subtype{S}	op->subtype = IVAL;
^material{S}	op->material = IVAL;
^materialname{S}    {	char *yv=yval();
			if (*yv=='\0')
			    LOG(llevError,"Materialname without val\n");
			else
			    FREE_AND_COPY(op->materialname, yv);
		    }

^value{S}	op->value = IVAL;
^weight{S}	op->weight = atol(yval());
^carrying{S}	op->carrying = atol(yval());
^attacktype{S}  op->attacktype = IVAL;
^path_attuned{S}  op->path_attuned = IVAL;
^path_repelled{S} op->path_repelled = IVAL;
^path_denied{S}   op->path_denied = IVAL;
^invisible{S}	    op->invisible = IVAL;
^magic{S}	    op->magic = IVAL;
^state{S}	    op->state = IVAL;
^alive{S}	    SET_OR_CLEAR_FLAG(op, FLAG_ALIVE, IVAL);
^applied{S}	    SET_OR_CLEAR_FLAG(op, FLAG_APPLIED, IVAL);
^unpaid{S}	    SET_OR_CLEAR_FLAG(op, FLAG_UNPAID, IVAL);
^need_an{S}	    { /* not used - just ignore */ }
^need_ie{S}	    { /* not used - jsut ignore */ }
^no_pick{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_PICK, IVAL);
^no_pass{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_PASS, IVAL);
^is_animated{S}	    SET_OR_CLEAR_FLAG(op, FLAG_ANIMATE, IVAL);
^walk_on{S}	    SET_OR_CLEAR_FLAG(op, FLAG_WALK_ON, IVAL);
^walk_off{S}	    SET_OR_CLEAR_FLAG(op, FLAG_WALK_OFF, IVAL);
^fly_on{S}	    SET_OR_CLEAR_FLAG(op, FLAG_FLY_ON, IVAL);
^fly_off{S}	    SET_OR_CLEAR_FLAG(op, FLAG_FLY_OFF, IVAL);
^flying{S}	    SET_OR_CLEAR_FLAG(op, FLAG_FLYING, IVAL);
^monster{S}	    SET_OR_CLEAR_FLAG(op, FLAG_MONSTER, IVAL);
^neutral{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NEUTRAL, IVAL);
^no_attack{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_ATTACK, IVAL);
^no_damage{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_DAMAGE, IVAL);
^friendly{S}	    {	if (IVAL) {
			    SET_FLAG(op, FLAG_FRIENDLY);
			    if (op->type != PLAYER) {
				LOG(llevDebug," Adding friendly object %s.\n",op->name);
				add_friendly_object(op);
			    }
			}
			else CLEAR_FLAG(op, FLAG_FRIENDLY);
		    }
^generator{S}	    SET_OR_CLEAR_FLAG(op, FLAG_GENERATOR, IVAL);
^use_content_on_gen{S} SET_OR_CLEAR_FLAG (op,FLAG_CONTENT_ON_GEN, IVAL);
^is_thrown{S}	    SET_OR_CLEAR_FLAG(op, FLAG_IS_THROWN, IVAL);
^auto_apply{S}	    SET_OR_CLEAR_FLAG(op, FLAG_AUTO_APPLY, IVAL);
^treasure{S}	    SET_OR_CLEAR_FLAG(op, FLAG_TREASURE, IVAL);
^see_invisible{S}   SET_OR_CLEAR_FLAG(op, FLAG_SEE_INVISIBLE, IVAL);
^can_roll{S} 	    SET_OR_CLEAR_FLAG(op, FLAG_CAN_ROLL, IVAL);
^overlay_floor{S}   SET_OR_CLEAR_FLAG(op, FLAG_OVERLAY_FLOOR, IVAL);
^is_turnable{S}	    SET_OR_CLEAR_FLAG(op, FLAG_IS_TURNABLE, IVAL);
^is_used_up{S}	    SET_OR_CLEAR_FLAG(op, FLAG_IS_USED_UP, IVAL);
^identified{S}	    {	if (IVAL) {
			    SET_FLAG(op, FLAG_IDENTIFIED);
			    CLEAR_FLAG(op, FLAG_KNOWN_MAGICAL);
			}
			else CLEAR_FLAG(op, FLAG_IDENTIFIED);
		    }
^reflecting{S}	    SET_OR_CLEAR_FLAG(op, FLAG_REFLECTING, IVAL);
^changing{S} 	    SET_OR_CLEAR_FLAG(op, FLAG_CHANGING, IVAL);
^splitting{S}	    SET_OR_CLEAR_FLAG(op, FLAG_SPLITTING, IVAL);
^hitback{S}  	    SET_OR_CLEAR_FLAG(op, FLAG_HITBACK, IVAL);
^startequip{S}	    SET_OR_CLEAR_FLAG(op, FLAG_STARTEQUIP, IVAL);
^blocksview{S}	    SET_OR_CLEAR_FLAG(op, FLAG_BLOCKSVIEW, IVAL);
^editable{S}	    op->arch->editable = IVAL;
^editor_folder{S}   { }
^undead{S}  	    SET_OR_CLEAR_FLAG(op, FLAG_UNDEAD, IVAL);
^scared{S}  	    SET_OR_CLEAR_FLAG(op, FLAG_SCARED, IVAL);
^unaggressive{S}    SET_OR_CLEAR_FLAG(op, FLAG_UNAGGRESSIVE, IVAL);
^reflect_missile{S} SET_OR_CLEAR_FLAG(op, FLAG_REFL_MISSILE, IVAL);
^reflect_spell{S}   SET_OR_CLEAR_FLAG(op, FLAG_REFL_SPELL, IVAL);
^no_magic{S} 	    SET_OR_CLEAR_FLAG(op, FLAG_NO_MAGIC, IVAL);
^wiz{S}  	    {	if (IVAL) {
			    SET_FLAG(op, FLAG_WIZ);
			    SET_FLAG(op, FLAG_WAS_WIZ);
			    SET_FLAG(op, FLAG_WIZPASS);
			}
			else {
			    CLEAR_FLAG(op, FLAG_WIZ);
			    CLEAR_FLAG(op, FLAG_WIZPASS);
			}
		    }
^was_wiz{S}  	    SET_OR_CLEAR_FLAG(op, FLAG_WAS_WIZ, IVAL);
^no_fix_player{S}   SET_OR_CLEAR_FLAG(op, FLAG_NO_FIX_PLAYER, IVAL);
^tear_down{S}	    SET_OR_CLEAR_FLAG(op, FLAG_TEAR_DOWN, IVAL);
^luck{S}  	    op->stats.luck = IVAL;
^run_away{S}	    op->run_away = IVAL;
^pass_thru{S}	    SET_OR_CLEAR_FLAG(op, FLAG_PASS_THRU, IVAL);
^can_pass_thru{S}   SET_OR_CLEAR_FLAG(op, FLAG_CAN_PASS_THRU, IVAL);
^pick_up{S}	    op->pick_up = IVAL;
^item_power{S}	    op->item_power = IVAL;
^gen_sp_armour{S}   op->gen_sp_armour = IVAL;
^anim_speed{S}	    op->anim_speed = IVAL;
^container{S}	    op->weight_limit = IVAL;
^no_drop{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_DROP, IVAL);
^will_apply{S}	    op->will_apply = IVAL;
^random_movement{S}	    SET_OR_CLEAR_FLAG(op, FLAG_RANDOM_MOVE, IVAL);
^can_apply{S}		    { }
^can_use_shield{S}  SET_OR_CLEAR_FLAG(op, FLAG_USE_SHIELD, IVAL);
^can_cast_spell{S}	    SET_OR_CLEAR_FLAG(op, FLAG_CAST_SPELL, IVAL);
^can_use_scroll{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_SCROLL, IVAL);
^can_use_range{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_RANGE, IVAL);
^can_use_bow{S}		    SET_OR_CLEAR_FLAG(op, FLAG_USE_BOW, IVAL);
^can_use_armour{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_ARMOUR, IVAL);
^can_use_weapon{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_WEAPON, IVAL);
^can_use_ring{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_RING, IVAL);
^has_ready_bow{S}   SET_OR_CLEAR_FLAG(op, FLAG_READY_BOW, IVAL);
^xrays{S}	    SET_OR_CLEAR_FLAG(op, FLAG_XRAYS, IVAL);
^is_floor{S}	    SET_OR_CLEAR_FLAG(op, FLAG_IS_FLOOR, IVAL);
^lifesave{S}	    SET_OR_CLEAR_FLAG(op, FLAG_LIFESAVE, IVAL);
^no_strength{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_STRENGTH, IVAL);
^sleep{S}	    {
			SET_OR_CLEAR_FLAG(op, FLAG_SLEEP, IVAL);
			/*(LOG(llevDebug," Warning: Object %s has sleep set in arch.\n",op->name);*/
		    }
^stand_still{S}	    SET_OR_CLEAR_FLAG(op, FLAG_STAND_STILL, IVAL);
^random_move{S}	    SET_OR_CLEAR_FLAG(op, FLAG_RANDOM_MOVE, IVAL);
^only_attack{S}	    SET_OR_CLEAR_FLAG(op, FLAG_ONLY_ATTACK, IVAL);

^activate_on_push{S}	SET_OR_CLEAR_FLAG(op, FLAG_ACTIVATE_ON_PUSH, IVAL);
^activate_on_release{S}	SET_OR_CLEAR_FLAG(op, FLAG_ACTIVATE_ON_RELEASE, IVAL);

    /* armour is loaded for compatiblity reasons */
^armour{S}		SET_RESIST(op, ATNR_PHYSICAL, IVAL);
    /* Start of various attacktypes */
^resist_physical{S}	SET_RESIST(op, ATNR_PHYSICAL, IVAL);
^resist_magic{S}	SET_RESIST(op, ATNR_MAGIC, IVAL);
^resist_fire{S}		SET_RESIST(op, ATNR_FIRE, IVAL);
^resist_electricity{S}	SET_RESIST(op, ATNR_ELECTRICITY, IVAL);
^resist_cold{S}		SET_RESIST(op, ATNR_COLD, IVAL);
^resist_confusion{S}	SET_RESIST(op, ATNR_CONFUSION, IVAL);
^resist_acid{S}		SET_RESIST(op, ATNR_ACID, IVAL);
^resist_drain{S}	SET_RESIST(op, ATNR_DRAIN, IVAL);
^resist_weaponmagic{S}	SET_RESIST(op, ATNR_WEAPONMAGIC, IVAL);
^resist_ghosthit{S}	SET_RESIST(op, ATNR_GHOSTHIT, IVAL);
^resist_poison{S}	SET_RESIST(op, ATNR_POISON, IVAL);
^resist_slow{S}		SET_RESIST(op, ATNR_SLOW, IVAL);
^resist_paralyze{S}	SET_RESIST(op, ATNR_PARALYZE, IVAL);
^resist_turn_undead{S}	SET_RESIST(op, ATNR_TURN_UNDEAD, IVAL);
^resist_fear{S}		SET_RESIST(op, ATNR_FEAR, IVAL);
^resist_cancellation{S}	SET_RESIST(op, ATNR_CANCELLATION, IVAL);
^resist_deplete{S}	SET_RESIST(op, ATNR_DEPLETE, IVAL);
^resist_death{S}	SET_RESIST(op, ATNR_DEATH, IVAL);
^resist_chaos{S}	SET_RESIST(op, ATNR_CHAOS, IVAL);
^resist_counterspell{S}	SET_RESIST(op, ATNR_COUNTERSPELL, IVAL);
^resist_godpower{S}	SET_RESIST(op, ATNR_GODPOWER, IVAL);
^resist_holyword{S}	SET_RESIST(op, ATNR_HOLYWORD, IVAL);
^resist_blind{S}	SET_RESIST(op, ATNR_BLIND, IVAL);
^resist_internal{S}	SET_RESIST(op, ATNR_INTERNAL, IVAL);

    /* Old style resistances */
^immune{S}		set_protection(op, IVAL, RESIST_IMMUNE);
^protected{S}		set_protection(op, IVAL, RESIST_PROT);
^vulnerable{S}		set_protection(op, IVAL, RESIST_VULN);

    /* old values - keep them around for now, but they should be removed at some point */
^has_ready_rod{S}	SET_OR_CLEAR_FLAG(op, FLAG_READY_RANGE, IVAL);
^has_ready_horn{S}	SET_OR_CLEAR_FLAG(op, FLAG_READY_RANGE, IVAL);
^has_ready_wand{S}	SET_OR_CLEAR_FLAG(op, FLAG_READY_RANGE, IVAL);
^can_use_wand{S}	SET_OR_CLEAR_FLAG(op, FLAG_USE_RANGE, IVAL);

^attack_movement{S} op->move_type = IVAL;
^move_state{S}	    op->move_status = IVAL;
^confused{S}	    SET_OR_CLEAR_FLAG(op, FLAG_CONFUSED, IVAL);
^stealth{S}	    SET_OR_CLEAR_FLAG(op, FLAG_STEALTH, IVAL);
^connected{S}	    add_button_link(op, op->map, IVAL);
^cursed{S}	    SET_OR_CLEAR_FLAG(op, FLAG_CURSED, IVAL);
^damned{S}	    SET_OR_CLEAR_FLAG(op, FLAG_DAMNED, IVAL);
^see_anywhere{S}    SET_OR_CLEAR_FLAG(op, FLAG_SEE_ANYWHERE, IVAL);
^known_magical{S}   SET_OR_CLEAR_FLAG(op, FLAG_KNOWN_MAGICAL, IVAL);
^known_cursed{S}    SET_OR_CLEAR_FLAG(op, FLAG_KNOWN_CURSED, IVAL);
^can_use_skill{S}   SET_OR_CLEAR_FLAG(op, FLAG_CAN_USE_SKILL, IVAL);
^been_applied{S}    SET_OR_CLEAR_FLAG(op, FLAG_BEEN_APPLIED, IVAL);
^has_ready_scroll{S}	SET_OR_CLEAR_FLAG(op, FLAG_READY_SCROLL, IVAL);
^can_use_rod{S}		SET_OR_CLEAR_FLAG(op, FLAG_USE_ROD, IVAL);
^can_use_horn{S}	SET_OR_CLEAR_FLAG(op, FLAG_USE_HORN, IVAL);
^expmul{S}		op->expmul = FVAL;
^unique{S}		SET_OR_CLEAR_FLAG(op, FLAG_UNIQUE, IVAL);
^make_invisible{S}	SET_OR_CLEAR_FLAG(op, FLAG_MAKE_INVIS, IVAL);
^inv_locked{S}		SET_OR_CLEAR_FLAG(op, FLAG_INV_LOCKED, IVAL);
^is_wooded{S}		SET_OR_CLEAR_FLAG(op, FLAG_IS_WOODED, IVAL);
^is_hilly{S}		SET_OR_CLEAR_FLAG(op, FLAG_IS_HILLY, IVAL);
^is_water{S}		SET_OR_CLEAR_FLAG(op, FLAG_IS_WATER, IVAL);
^has_ready_skill{S}	SET_OR_CLEAR_FLAG(op, FLAG_READY_SKILL, IVAL);
^has_ready_weapon{S}	SET_OR_CLEAR_FLAG(op, FLAG_READY_WEAPON, IVAL);
^no_skill_ident{S}	SET_OR_CLEAR_FLAG(op, FLAG_NO_SKILL_IDENT, IVAL);
^glow_radius{S}		op->glow_radius = IVAL;
^is_blind{S}		SET_OR_CLEAR_FLAG(op, FLAG_BLIND, IVAL);
^can_see_in_dark{S}	SET_OR_CLEAR_FLAG(op, FLAG_SEE_IN_DARK, IVAL);
^is_cauldron{S}		SET_OR_CLEAR_FLAG(op, FLAG_IS_CAULDRON, IVAL);
^randomitems{S}		op->randomitems = find_treasurelist(yval());
^is_dust{S}		SET_OR_CLEAR_FLAG(op, FLAG_DUST, IVAL);
^no_steal{S}		SET_OR_CLEAR_FLAG(op, FLAG_NO_STEAL, IVAL);
^one_hit{S}		SET_OR_CLEAR_FLAG(op, FLAG_ONE_HIT, IVAL);
^berserk{S}		SET_OR_CLEAR_FLAG(op, FLAG_BERSERK, IVAL);

^can_knockback{S}	{ /* Some archetypes have these values in them */ }
^can_parry{S}		{ /* Probably the pupland archetypes - I imagined */ }
^can_impale{S}		{ /* That these are for the new combat code */ }
^can_cut{S}		{ /* just ignore for now */ }
^can_dam_armour{S}	{ }
^weapontype{S}		op->weapontype = IVAL;
^tooltype{S}            op->tooltype = IVAL;
^casting_speed{S}	op->casting_speed = FVAL;
^elevation{S}		op->elevation = IVAL;
^smoothlevel{S}		op->smoothlevel = IVAL;
^client_type{S}		op->client_type = IVAL;
^body_{A}		set_body_info(op, yytext);

^event_apply{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (apply) without val\n");
    else
        insert_event(op,EVENT_APPLY,yv,NULL,NULL);
}

^event_apply_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (apply) without plugin\n");
    else
        insert_event(op,EVENT_APPLY,NULL,yv,NULL);
}

^event_apply_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (apply) without options\n");
    else
        insert_event(op,EVENT_APPLY,NULL,NULL,yv);
}

^event_attack{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (attack) without val\n");
    else
        insert_event(op,EVENT_ATTACK,yv,NULL,NULL);
}

^event_attack_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (attack) without plugin\n");
    else
        insert_event(op,EVENT_ATTACK,NULL,yv,NULL);
}

^event_attack_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (attack) without options\n");
    else
        insert_event(op,EVENT_ATTACK,NULL,NULL,yv);
}
^event_death{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (death) without val\n");
    else
        insert_event(op,EVENT_DEATH,yv,NULL,NULL);
}

^event_death_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (death) without plugin\n");
    else
        insert_event(op,EVENT_DEATH,NULL,yv,NULL);
}

^event_death_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (death) without options\n");
    else
        insert_event(op,EVENT_DEATH,NULL,NULL,yv);
}
^event_drop{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (drop) without val\n");
    else
        insert_event(op,EVENT_DROP,yv,NULL,NULL);
}

^event_drop_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (drop) without plugin\n");
    else
        insert_event(op,EVENT_DROP,NULL,yv,NULL);
}

^event_drop_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (drop) without options\n");
    else
        insert_event(op,EVENT_DROP,NULL,NULL,yv);
}
^event_pickup{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (pickup) without val\n");
    else
        insert_event(op,EVENT_PICKUP,yv,NULL,NULL);
}

^event_pickup_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (pickup) without plugin\n");
    else
        insert_event(op,EVENT_PICKUP,NULL,yv,NULL);
}

^event_pickup_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (pickup) without options\n");
    else
        insert_event(op,EVENT_PICKUP,NULL,NULL,yv);
}
^event_say{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (say) without val\n");
    else
        insert_event(op,EVENT_SAY,yv,NULL,NULL);
}

^event_say_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (say) without plugin\n");
    else
        insert_event(op,EVENT_SAY,NULL,yv,NULL);
}

^event_say_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (say) without options\n");
    else
        insert_event(op,EVENT_SAY,NULL,NULL,yv);
}
^event_stop{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (stop) without val\n");
    else
        insert_event(op,EVENT_STOP,yv,NULL,NULL);
}

^event_stop_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (stop) without plugin\n");
    else
        insert_event(op,EVENT_STOP,NULL,yv,NULL);
}

^event_stop_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (stop) without options\n");
    else
        insert_event(op,EVENT_STOP,NULL,NULL,yv);
}
^event_time{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (time) without val\n");
    else
        insert_event(op,EVENT_TIME,yv,NULL,NULL);
}

^event_time_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (time) without plugin\n");
    else
        insert_event(op,EVENT_TIME,NULL,yv,NULL);
}

^event_time_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (time) without options\n");
    else
        insert_event(op,EVENT_TIME,NULL,NULL,yv);
}
^event_throw{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (throw) without val\n");
    else
        insert_event(op,EVENT_THROW,yv,NULL,NULL);
}

^event_throw_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (throw) without plugin\n");
    else
        insert_event(op,EVENT_THROW,NULL,yv,NULL);
}

^event_throw_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (apply) without options\n");
    else
        insert_event(op,EVENT_THROW,NULL,NULL,yv);
}
^event_trigger{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (trigger) without val\n");
    else
        insert_event(op,EVENT_TRIGGER,yv,NULL,NULL);
}

^event_trigger_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (trigger) without plugin\n");
    else
        insert_event(op,EVENT_TRIGGER,NULL,yv,NULL);
}

^event_trigger_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (trigger) without options\n");
    else
        insert_event(op,EVENT_TRIGGER,NULL,NULL,yv);
}
^event_close{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (close) without val\n");
    else
        insert_event(op,EVENT_CLOSE,yv,NULL,NULL);
}

^event_close_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (close) without plugin\n");
    else
        insert_event(op,EVENT_CLOSE,NULL,yv,NULL);
}

^event_close_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (close) without options\n");
    else
        insert_event(op,EVENT_CLOSE,NULL,NULL,yv);
}
^event_timer{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (timer) without val\n");
    else
        insert_event(op,EVENT_TIMER,yv,NULL,NULL);
}

^event_timer_plugin{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (timer) without plugin\n");
    else
        insert_event(op,EVENT_TIMER,NULL,yv,NULL);
}

^event_timer_options{S}	    {
    char *yv=yval();
    if (*yv=='\0')
        LOG(llevError,"Event (timer) without options\n");
    else
        insert_event(op,EVENT_TIMER,NULL,NULL,yv);
}

^current_weapon_script{S} {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Script (current weapon) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->current_weapon_script, yv);
                                                };
		    }

<*>(^{WS}$)|\n		{/* ignore empty lines, newlines we don't do above */}
#.*\n			{}

<<EOF>>			{/* If we got an error, return the error.  Otherwise, return that we got EOF */
			    if (lex_error!=0) return lex_error; else return LL_EOF;}
.*			{ yyerror( "Unrecognized string"); lex_error= -1; }
%%


int yyerror(char *s)
{
  LOG(llevError, "%s: %s\n", s, yytext);
  return -1;
}


/* Our save file syntax is very simple, so we can use a very simple
 * processing mechanism here instead using something like bison
 * This skips over the space and returns the value, or "" if no value
 * is found.  Modified 4/26/2000 to also strip spaces at end of
 * line
 */
static char *yval()
{
    static char *em="";
    char *cp,*end;

    /* First skip over start of line, like animation or name */
   for (cp=yytext; *cp!=' '; cp++) {
	if (*cp=='\0') {
	    return em;
	}
   }

    /* Skip over whitespace */
    for (; *cp==' '; cp++) {
	if (*cp=='\0') {
	    return em;
	}
    }
    /* Got last character before null and strip
     * off tailing whitespace
     */
    for (end=cp+strlen(cp)-1; *end==' '; end--) {
	if (end==cp) return em;
	*end='\0';
    }
    return cp;
}


/*
 * Loads an object from the given file-pointer.
 * Variables will be read and parsed and patched into the object
 * until the string "end" is reached, or the end of the file.
 *
 * bufstat is used to determine various file attributes:
 *  LO_REPATE (0): We are reading from the same buffer as the last call.
 *  LO_LINEMODE (1): file that is being read from is multi purpose (ie, other functions
 *	will also be reading from this (treasure file, artifacts.)
 *  LO_NEWFILE (2): This is the first read from a particular file, so the buffers should
 *	be reset.
 *  LO_NOREAD (3): Reset the buffers, but don't read from it. (op can be null)
 *
 */

int load_object(FILE *fp, object *op, int bufstate, int map_flags) {
    int retval;
    char inbuf[MAX_BUF];

    strcpy(msgbuf, "");
    strcpy(lorebuf, "");
    if (bufstate==LO_NEWFILE || bufstate==LO_NOREAD) {
/*	LOG(llevDebug,"Switching lex buffers\n");*/
	yy_delete_buffer(YY_CURRENT_BUFFER);
	yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE));
	if (bufstate==LO_NOREAD) return LL_NORMAL;
    }
    if (bufstate==LO_LINEMODE) {
	YY_BUFFER_STATE  yybufstate;
	while (fgets(inbuf, MAX_BUF-3, fp)) {
	    yybufstate=yy_scan_string(inbuf);
	    retval=lex_load(op, map_flags);
	    yy_delete_buffer(yybufstate);
	    if (retval==LL_NORMAL) return retval;
	}
	LOG(llevDebug,"Got eof while scanning strings\n");
	return LL_EOF;
    }

    retval=lex_load(op, map_flags);
    if (op->current_weapon_script != NULL)
    {
        op->current_weapon = find_best_weapon_used_match(op, op->current_weapon_script);
        LOG(llevDebug, "CurrentWeapon Loaded !\n");
    };

/*    LOG(llevDebug," load completed, object=%s\n",op->name);*/
    return retval;
}


/* This takes a buffer, scans it for variables, and sets those variables
 * as appropriate in op.
 *
 * This function appears to be used in only 2 places - in crossedit to
 * override values and in c_wiz to mutate values.
 */
int set_variable(object *op,char *buf) {
    YY_BUFFER_STATE  yybufstate,yycurbuf=YY_CURRENT_BUFFER;
    int retval;

    strcpy(msgbuf, "");
    strcpy(lorebuf, "");
    yy_push_state(INITIAL);
    yybufstate=yy_scan_string(buf);
    retval=lex_load(op,0);
    yy_switch_to_buffer(yycurbuf);
    yy_delete_buffer(yybufstate);
    yy_pop_state();
    return retval;
}

/* Start of C code */

/* This array equates the FLAG_ values with the V_ values.  Use -1 to
 * put gaps in the array that should not be processed.
 * The order matches the order of the define values in 'define.h'.
 */
/* This is a list of pointers that correspond to the FLAG_.. values.
 * This is a simple 1:1 mapping - if FLAG_FRIENDLY is 15, then
 * the 15'th element of this array should match that name.
 * If an entry is NULL, that is a flag not to loaded/saved.
 */
static char *flag_names[NUM_FLAGS+1] = {
"alive", "wiz", NULL, NULL, "was_wiz", "applied", "unpaid",
"can_use_shield", "no_pick", "walk_on", "no_pass",              /* 10 */
"is_animated", NULL, "flying", "monster", "friendly", "generator",
"is_thrown", "auto_apply", "treasure", NULL /*invis_undead*/,   /* 20 */
"see_invisible", "can_roll", "overlay_floor",
"is_turnable", "walk_off", "fly_on",
"fly_off", "is_used_up", "identified", "reflecting",            /* 30 */
"changing", "splitting", "hitback", "startequip",
"blocksview", "undead", "scared", "unaggressive",
"reflect_missile", "reflect_spell",                             /* 40 */
"no_magic", "no_fix_player", NULL, "tear_down", "run_away",
"pass_thru", "can_pass_thru", "pick_up", "unique", "no_drop",   /* 50 */
 NULL, "can_cast_spell", "can_use_scroll", "can_use_range",
"can_use_bow",  "can_use_armour", "can_use_weapon",
"can_use_ring", "has_ready_range", "has_ready_bow",             /* 60 */
"xrays", NULL, "is_floor", "lifesave", "no_strength", "sleep",
"stand_still", "random_move", "only_attack", "confused",        /* 70 */
 "stealth", NULL, NULL, "cursed", "damned",
"see_anywhere", "known_magical", "known_cursed",
"can_use_skill", "been_applied",                                /* 80 */
"has_ready_scroll", "can_use_rod", NULL,
"can_use_horn", "make_invisible",  "inv_locked", "is_wooded",
"is_hilly", "has_ready_skill", "has_ready_weapon",              /* 90 */
"no_skill_ident", "is_blind", "can_see_in_dark", "is_cauldron",
"is_dust", "no_steal", "one_hit", NULL, "berserk", "neutral",	/* 100 */
"no_attack", "no_damage", NULL, NULL, "activate_on_push",
"activate_on_release", "is_water", "use_content_on_gen", NULL   /* 109 */
};


void save_double(char *buf,char *name,double v)
{
  char tbuf[200];

  sprintf(tbuf,"%s %f\n",name,v);
  strcat(buf,tbuf);
}

/*
 * Initialises the array of variable-names.  Needed before any
 * objects can be loaded.  Called by init_library().
 */

void init_vars() {
}

/*For get_ob_diff speed reason*/
typedef struct {
    char* name;
    int   length;
}genericname;
static genericname evtnames[13]=
{
    {"event_none ",11},
    {"event_apply ",12},
    {"event_attack ",13},
    {"event_death ",12},
    {"event_drop ",11},
    {"event_pickup ",13},
    {"event_say ",10},
    {"event_stop ",11},
    {"event_time ",11},
    {"event_throw ",12},
    {"event_trigger ",14},
    {"event_close ",12},
    {"event_timer ",12}
} ;

static genericname plgnames[13]=
{
    {"event_none_plugin ",18},
    {"event_apply_plugin ",19},
    {"event_attack_plugin ",20},
    {"event_death_plugin ",19},
    {"event_drop_plugin ",18},
    {"event_pickup_plugin ",20},
    {"event_say_plugin ",17},
    {"event_stop_plugin ",18},
    {"event_time_plugin ",18},
    {"event_throw_plugin ",19},
    {"event_trigger_plugin ",21},
    {"event_close_plugin ",19},
    {"event_timer_plugin ",19}
};

static genericname plgoptions[13]=
{
    {"event_none_options ",19},
    {"event_apply_options ",20},
    {"event_attack_options ",21},
    {"event_death_options ",20},
    {"event_drop_options ",19},
    {"event_pickup_options ",21},
    {"event_say_options ",18},
    {"event_stop_options ",19},
    {"event_time_options ",19},
    {"event_throw_options ",20},
    {"event_trigger_options ",22},
    {"event_close_options ",20},
    {"event_timer_options ",20}
};

/*
 * Returns a pointer to a static string which contains all variables
 * which are different in the two given objects.  op is the what object
 * the different values will be taken from.  This function is
 * typically used to dump objects (op2=empty object), or to save objects
 * (op2 is the objects original archetype)
 */


/*static int eol_size=strlen("\n");*/
static int eol_size=sizeof("\n")-1; /*Substract 1 to suppress terminal 0*/
#define ADD_STRINGLINE_ENTRY(buf__,entryname__,entryvalue__,entrysize__){\
    FAST_STRNCAT(buf__,entryname__,entrysize__);\
    FAST_STRCAT(buf__,entryvalue__);\
    FAST_STRNCAT(buf__,"\n",eol_size);}
#define FAST_SAVE_LONG(buf__,entryname__,entryvalue__,entrysize__) \
    ADD_STRINGLINE_ENTRY(buf__,entryname__,ltostr10(entryvalue__),entrysize__)
#define FAST_SAVE_DOUBLE(buf__,entryname__,entryvalue__,entrysize__) \
    ADD_STRINGLINE_ENTRY(buf__,entryname__,doubletostr10(entryvalue__),entrysize__)



char *get_ob_diff(object *op,object *op2) {/* I plan to optimize this heavily */

/* Too late, have done it already :P
 *                            Tchize
 */

/*
 * A bit of explantion on strings handling in this function:
 * buf is never null terminated until the end. So
 * works only with buf2 and fastbuf. What concern fastbuf,
 * use ONLY through the FAST macros. Classical C string handling
 * functions will fail since it is a pointer to the end of the
 * string.
 * NEVER touch buf between PREPARE_FASTCAT(buf) and
 * FINISH_FASTCAT(buf)
 */
  static char buf2[HUGE_BUF];
  static char buf[HUGE_BUF];
  char* fastbuf;
  int tmp;
  int i;
#if 0
/*Memory polluting code. Should help detect problems, very slow*/
  for (i=0;i<HUGE_BUF;i++){
    buf[i]='a'+(unsigned short)(i%25);
  }
#endif
  event *etmp;
  event *etmp2;

  buf[0]='\0';
  fastbuf=PREPARE_FASTCAT(buf);
  if(op->name && op->name!=op2->name) {
    ADD_STRINGLINE_ENTRY(fastbuf,"name ",op->name,5);
  }
  if(op->name_pl && op->name_pl!=op2->name_pl) {
    ADD_STRINGLINE_ENTRY(fastbuf,"name_pl ",op->name_pl,8);
  }
  if(op->title && op->title!=op2->title) {
    ADD_STRINGLINE_ENTRY(fastbuf,"title ",op->title,6);
  }
  if(op->race && op->race!=op2->race) {
    ADD_STRINGLINE_ENTRY(fastbuf,"race ",op->race,5);
  }
  if(op->slaying && op->slaying!=op2->slaying) {
    ADD_STRINGLINE_ENTRY(fastbuf,"slaying ",op->slaying,8);
  }
  if(op->msg && op->msg!=op2->msg) {
    FAST_STRNCAT(fastbuf,"msg\n",3+eol_size);
    FAST_STRCAT(fastbuf,op->msg);
    FAST_STRNCAT(fastbuf,"endmsg\n",6+eol_size);
  }
  if(op->lore && op->lore!=op2->lore) {
    FAST_STRNCAT(fastbuf,"lore\n",4+eol_size);
    FAST_STRCAT(fastbuf,op->lore);
    FAST_STRNCAT(fastbuf,"endlore\n",7+eol_size);

  }
  if(op->other_arch!=op2->other_arch&&op->other_arch!=NULL &&
     op->other_arch->name) {
    ADD_STRINGLINE_ENTRY(fastbuf,"other_arch ",op->other_arch->name,11);
  }
  if(op->face!=op2->face) {
    ADD_STRINGLINE_ENTRY(fastbuf,"face ",op->face->name,5);
  }

  for(etmp=op->events;etmp!=NULL;etmp=etmp->next)
  {
      /* First we find the event for the reference object */
      etmp2=find_event(op2,etmp->type);
      if ((etmp->hook != NULL) && ((etmp2 == NULL) || (etmp2->hook == NULL) || (strcmp(etmp2->hook,etmp->hook))))
      /* Either there's no matching event in the reference object,
       * or the hook is different */
      {
          ADD_STRINGLINE_ENTRY(fastbuf,evtnames[etmp->type].name,etmp->hook,evtnames[etmp->type].length);
      }
      if ((etmp->plugin != NULL) && ((etmp2 == NULL) || (etmp2->plugin == NULL) || (strcmp(etmp2->plugin,etmp->plugin))))
      {

          ADD_STRINGLINE_ENTRY(fastbuf,plgnames[etmp->type].name,etmp->plugin,plgnames[etmp->type].length);
      }
      if ((etmp->options != NULL) && ((etmp2 == NULL) || (etmp2->options == NULL) || (strcmp(etmp2->options,etmp->options))))
      {
          ADD_STRINGLINE_ENTRY(fastbuf,plgoptions[etmp->type].name,etmp->hook,plgoptions[etmp->type].length);
      }

  }

  if (op->animation_id != op2->animation_id) {
    if (op->animation_id) {
      ADD_STRINGLINE_ENTRY(fastbuf,"animation ",animations[GET_ANIM_ID(op)].name,10);
      if ( ! QUERY_FLAG (op, FLAG_ANIMATE)) {
        FAST_STRNCAT(fastbuf,"is_animated 0\n",13+eol_size);
      }
    } else {
      FAST_STRNCAT(fastbuf,"animation NONE\n",14+eol_size);
    }
  }
  if(op->stats.Str!=op2->stats.Str)
    FAST_SAVE_LONG(fastbuf,"Str ",op->stats.Str,4);
  if(op->stats.Dex!=op2->stats.Dex)
    FAST_SAVE_LONG(fastbuf,"Dex ",op->stats.Dex,4);
  if(op->stats.Con!=op2->stats.Con)
    FAST_SAVE_LONG(fastbuf,"Con ",op->stats.Con,4);
  if(op->stats.Wis!=op2->stats.Wis)
    FAST_SAVE_LONG(fastbuf,"Wis ",op->stats.Wis,4);
  if(op->stats.Pow!=op2->stats.Pow)
    FAST_SAVE_LONG(fastbuf,"Pow ",op->stats.Pow,4);
  if(op->stats.Cha!=op2->stats.Cha)
    FAST_SAVE_LONG(fastbuf,"Cha ",op->stats.Cha,4);
  if(op->stats.Int!=op2->stats.Int)
    FAST_SAVE_LONG(fastbuf,"Int ",op->stats.Int,4);
  if(op->stats.hp!=op2->stats.hp)
    FAST_SAVE_LONG(fastbuf,"hp ",op->stats.hp,3);
  if(op->stats.maxhp!=op2->stats.maxhp)
    FAST_SAVE_LONG(fastbuf,"maxhp ",op->stats.maxhp,6);
  if(op->stats.sp!=op2->stats.sp)
    FAST_SAVE_LONG(fastbuf,"sp ",op->stats.sp,3);
  if(op->stats.maxsp!=op2->stats.maxsp)
    FAST_SAVE_LONG(fastbuf,"maxsp ",op->stats.maxsp,6);
  if(op->stats.grace!=op2->stats.grace)
    FAST_SAVE_LONG(fastbuf,"grace ",op->stats.grace,6);
  if(op->stats.maxgrace!=op2->stats.maxgrace)
    FAST_SAVE_LONG(fastbuf,"maxgrace ",op->stats.maxgrace,9);
  if(op->stats.exp!=op2->stats.exp)
    FAST_SAVE_LONG(fastbuf,"exp ",op->stats.exp,4);
  if(op->expmul!=op2->expmul)
    FAST_SAVE_DOUBLE(fastbuf,"expmul ",op->expmul,7);
  if(op->stats.food!=op2->stats.food)
    FAST_SAVE_LONG(fastbuf,"food ",op->stats.food,5);
  if(op->stats.dam!=op2->stats.dam)
    FAST_SAVE_LONG(fastbuf,"dam ",op->stats.dam,4);
  if(op->stats.luck!=op2->stats.luck)
    FAST_SAVE_LONG(fastbuf,"luck ",op->stats.luck,5);
  if(op->stats.wc!=op2->stats.wc)
    FAST_SAVE_LONG(fastbuf,"wc ",op->stats.wc,3);
  if(op->stats.ac!=op2->stats.ac)
    FAST_SAVE_LONG(fastbuf,"ac ",op->stats.ac,3);
  if(op->x!=op2->x)
    FAST_SAVE_LONG(fastbuf,"x ",op->x,2);
  if(op->y!=op2->y)
    FAST_SAVE_LONG(fastbuf,"y ",op->y,2);
  if(op->speed!=op2->speed) {
    FAST_SAVE_DOUBLE(fastbuf,"speed ",op->speed,6);
  }
  if(op->speed > 0 && op->speed_left!=op2->speed_left) {
    FAST_SAVE_DOUBLE(fastbuf,"speed_left ",op->speed_left,11);
  }
  if(op->move_status != op2->move_status)
    FAST_SAVE_LONG(fastbuf,"move_state ",op->move_status,11);
  if(op->move_type != op2->move_type)
    FAST_SAVE_LONG(fastbuf,"attack_movement ",op->move_type,16);
  if(op->nrof!=op2->nrof)
    FAST_SAVE_LONG(fastbuf,"nrof ",op->nrof,5);
  if(op->level!=op2->level)
    FAST_SAVE_LONG(fastbuf,"level ",op->level,6);
  if(op->direction!=op2->direction)
    FAST_SAVE_LONG(fastbuf,"direction ",op->direction,10);
  if(op->type!=op2->type)
    FAST_SAVE_LONG(fastbuf,"type ",op->type,5);
  if(op->subtype!=op2->subtype)
    FAST_SAVE_LONG(fastbuf,"subtype ",op->subtype,2);
  if(op->attacktype!=op2->attacktype)
    FAST_SAVE_LONG(fastbuf,"attacktype ",op->attacktype,11);

  for (tmp=0; tmp < NROFATTACKS; tmp++) {
   if (op->resist[tmp] != op2->resist[tmp]) {
	FAST_STRNCAT(fastbuf,"resist_",7);
    FAST_SAVE_LONG(fastbuf,resist_save[tmp],op->resist[tmp],strlen(resist_save[tmp]));
    }
  }

  if(op->path_attuned!=op2->path_attuned)
    FAST_SAVE_LONG(fastbuf,"path_attuned ",op->path_attuned,13);
  if(op->path_repelled!=op2->path_repelled)
    FAST_SAVE_LONG(fastbuf,"path_repelled ",op->path_repelled,14);
  if(op->path_denied!=op2->path_denied)
    FAST_SAVE_LONG(fastbuf,"path_denied ",op->path_denied,12);
  if(op->material!=op2->material)
    FAST_SAVE_LONG(fastbuf,"material ",op->material,9);
  if(op->materialname && op->materialname!=op2->materialname) {
    ADD_STRINGLINE_ENTRY(fastbuf,"materialname ",op->materialname,13);
  }
  if(op->value!=op2->value)
    FAST_SAVE_LONG(fastbuf,"value ",op->value,6);
  if(op->carrying!=op2->carrying)
    FAST_SAVE_LONG(fastbuf,"carrying ",op->carrying,9);
  if(op->weight!=op2->weight)
    FAST_SAVE_LONG(fastbuf,"weight ",op->weight,7);
  if(op->invisible!=op2->invisible)
    FAST_SAVE_LONG(fastbuf,"invisible ",op->invisible,10);
  if(op->state!=op2->state)
    FAST_SAVE_LONG(fastbuf,"state ",op->state,6);
  if(op->magic!=op2->magic)
    FAST_SAVE_LONG(fastbuf,"magic ",op->magic,6);
  if(op->last_heal!=op2->last_heal)
    FAST_SAVE_LONG(fastbuf,"last_heal ",op->last_heal,10);
  if(op->last_sp!=op2->last_sp)
    FAST_SAVE_LONG(fastbuf,"last_sp ",op->last_sp,8);
  if(op->last_grace!=op2->last_grace)
    FAST_SAVE_LONG(fastbuf,"last_grace ",op->last_grace,11);
  if(op->last_eat!=op2->last_eat)
    FAST_SAVE_LONG(fastbuf,"last_eat ",op->last_eat,9);
  if(QUERY_FLAG(op,FLAG_IS_LINKED) && (tmp = get_button_value(op)))
    FAST_SAVE_LONG(fastbuf,"connected ",tmp,10);
  if(op->glow_radius!=op2->glow_radius)
    FAST_SAVE_LONG(fastbuf,"glow_radius ",op->glow_radius,12);
  if (op->randomitems!=op2->randomitems) {
    ADD_STRINGLINE_ENTRY(fastbuf,"randomitems ",(op->randomitems?op->randomitems->name:"none"),12);
  }
#ifdef NPC_PROG
  if(op->npc_status!=op2->npc_status)
    FAST_SAVE_LONG(fastbuf,"npc_status ",op->npc_status,11);
  if(op->npc_program!=op2->npc_program)
    FAST_SAVE_LONG(fastbuf,"npc_program ",op->npc_program,12);
#endif


  if(op->run_away!=op2->run_away)
    FAST_SAVE_LONG(fastbuf,"run_away ",op->run_away,9);
  if(op->pick_up!=op2->pick_up)
    FAST_SAVE_LONG(fastbuf,"pick_up ",op->pick_up,8);
  if(op->weight_limit!=op2->weight_limit)
    FAST_SAVE_LONG(fastbuf,"container ",op->weight_limit,10);
  if (op->will_apply!=op2->will_apply)
    FAST_SAVE_LONG(fastbuf,"will_apply ",op->will_apply,11);
  if(op->smoothlevel!=op2->smoothlevel)
    FAST_SAVE_LONG(fastbuf,"smoothlevel ",op->smoothlevel,12);

  if (op->current_weapon_script!=op2->current_weapon_script){
    ADD_STRINGLINE_ENTRY(fastbuf,"current_weapon_script ",op->current_weapon_script,22);
  };

  if(op->weapontype && op->weapontype!=op2->weapontype) {
    FAST_SAVE_LONG(fastbuf,"weapontype ",op->weapontype,11);
  }
  if(op->tooltype && op->tooltype!=op2->tooltype) {
    FAST_SAVE_LONG(fastbuf,"tooltype ",op->tooltype,9);
  }
  if (op->elevation && op->elevation != op2->elevation) {
    FAST_SAVE_LONG(fastbuf,"elevation ",op->elevation,10);
  }
  if (op->client_type && op->client_type != op2->client_type) {
    FAST_SAVE_LONG(fastbuf,"client_type ",op->client_type,12);
  }

  if (op->item_power != op2->item_power) {
    FAST_SAVE_LONG(fastbuf,"item_power ",op->item_power,11);
  }

  if (op->gen_sp_armour != op2->gen_sp_armour) {
    FAST_SAVE_LONG(fastbuf,"gen_sp_armour ",op->gen_sp_armour,14);
  }

  for (tmp=0; tmp <= NUM_FLAGS; tmp++) {
    if (flag_names[tmp] && (QUERY_FLAG(op, tmp) != QUERY_FLAG(op2, tmp))) {
	ADD_STRINGLINE_ENTRY(fastbuf,flag_names[tmp],QUERY_FLAG(op, tmp)?" 1":" 0",strlen(flag_names[tmp]));
    }
  }
  /* The old code was buggy - it would lose custom SLOW_PENALTY if the archetype
   * had a different slow_penalty (eg, when saving, it would also reset to the
   * archetype value.
   */
  if (QUERY_FLAG(op, FLAG_SLOW_MOVE)) {
    if (!QUERY_FLAG(op2, FLAG_SLOW_MOVE) || SLOW_PENALTY(op) != SLOW_PENALTY(op2)) {
	FAST_SAVE_DOUBLE(fastbuf,"slow_move ",SLOW_PENALTY(op),10);
    }
  }

  /* Save body locations */
  for (i=0; i<NUM_BODY_LOCATIONS; i++) {
    if (op->body_info[i] != op2->body_info[i]) {
	FAST_STRCAT(fastbuf,body_locations[i].save_name);
    FAST_SAVE_LONG(fastbuf," ",op->body_info[i],1);
    }
  }
  FINISH_FASTCAT(fastbuf);
  if(buf[0]=='\0')/*did not cat anything...*/
    return NULL;
  return buf;
}

/*
 * Dumps all variables in an object to a file.
 * If bit 0 of flag is set, unpaid objects will be saved.  As of now,
 * the only place this is not set is when saving the player.
 * If bit 1 of flag is set, don't remove the object after save.  As of now,
 * all of the callers are setting this.
 */

void save_object(FILE *fp,object *op, int flag) {
    archetype *at;
    char *cp;
    object *tmp,*old;

    /* Even if the object does have an owner, it would seem that we should
     * still save it.
     */
    if(op->owner!=NULL || fp == NULL)
	return;

    /* If it is unpaid and we don't want to save those, just return. */
    if(!(flag&1)&&(QUERY_FLAG(op, FLAG_UNPAID))) {
	return;
    }

    if((at=op->arch)==NULL) at=empty_archetype;
    fprintf(fp,"arch %s\n",at->name);

    if((cp=get_ob_diff(op,&at->clone))!=NULL)
	fputs(cp,fp);	/* We really should do some status checking on this */

    /* Eneq(@csd.uu.se): Added this to allow containers being saved with contents*/

    old=NULL;

    if (flag & 2 )
	for(tmp=op->inv;tmp!=NULL;tmp=tmp->below)
	    save_object(fp,tmp,flag);

    /* Slightly different logic because tmp/op will be removed by
     * the save_object we call.  So we just keep looking at op->inv
     * until there is nothing left.  In theory, the variable old
     * should not be needed, as recursive loops shouldn't happen.
     */
    else while ((tmp=op->inv)!=NULL) {
	if(old==tmp) {
	    LOG(llevError," Recursive loop in inventory\n");
	    break;
	}
	save_object(fp,tmp,flag); 
	old=tmp;
    }
   
    if (!(flag&2)) {
	remove_ob(op);
	free_object (op);
    }

    fprintf(fp,"end\n");
}

void insert_event(object* op, int etype, char *ehook, char *eplug, char *eoptions)
{
    event *evt;
    event *tmp;

    evt = find_event(op,etype);
    if (evt == NULL)
    {
        evt = (event *)malloc(sizeof(event));
        evt->next = NULL;
        evt->type = etype;
        evt->hook = NULL;
        evt->plugin = NULL;
        evt->options = NULL;
        if (op->events==NULL)
        {
            op->events=evt;
        }
        else
        {
            for(tmp=op->events;;tmp=tmp->next)
            {
                if (tmp->next == NULL)
                {
                    tmp->next = evt;
                    break;
                }
            }
        }
    }
    if (ehook != NULL)
        FREE_AND_COPY(evt->hook,ehook);
    if (eplug != NULL)
        FREE_AND_COPY(evt->plugin,eplug);
    if (eoptions != NULL)
        FREE_AND_COPY(evt->options,eoptions);
}

event* find_event(object* op, int etype)
{
    event *found;
    for(found=op->events;found!=NULL;found=found->next)
    {
        if (found->type == etype)
            return found;
    }
    return NULL;
}






