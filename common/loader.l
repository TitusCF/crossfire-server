%{
/*
 * static char *rcsid_object_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2000 Mark Wedel
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

    The author can be reached via e-mail to mwedel@scruz.net
*/

/* Eneq(@csd.uu.se): Added weight-modifiers in environment of objects.
   sub/add_weight will transcend the environment updating the carrying
   variable. */


#include <global.h>
#include <loader.h>
#include <newserver.h>
#include <sproto.h>

/* All the variable names.  Each entry should correspond to the
 * V_ vlaue in loader.h (that is, the 20'th value in this array should
 * correspond to the 20'th V_(value) in loader.h)  The V_??? get
 * used as indexes into this array.
 */

/* If you change this list (add/delete/rename), also update the list
 * in crossedit/Attr.c, around line 360.  That list is the variables
 * that can be set in crossedit for various objects.
 * Realistically, these are not needed anymore - they were perhaps useful
 * at one point when both the loader and saver might have used them.
 * The only thing they are used for at all right now is some of the
 * get_ob_diff code uses these and indexes with the V_.. fields.
 * Most of the more recent changes just put the names write in the
 * get_ob_diff, which then also means you don't really need to worry
 * about the V_ flags.
 */
static char *variable_const[NR_OF_VARIABLES] = {
  "Object","name","race","slaying","msg","endmsg",
  "Inventory","arch","other_arch","More",
  "animation","end","last_heal","last_sp","last_grace","last_eat",
  "speed","speed_left","slow_move",
  "face","Str","Dex","Con","Wis","Cha","Int","Pow","hp","maxhp","sp","maxsp",
  "grace","maxgrace",
  "exp","food","dam","wc","ac","x","y","nrof","level","direction",
  "type","material","value", "weight","carrying",
  "immune","protected","attacktype","vulnerable",
  "path_attuned","path_repelled","path_denied",
  "invisible","magic","state","alive","applied","unpaid","need_an","need_ie",
  "no_pick","no_pass","walk_on","walk_off","fly_on","fly_off","is_animated",
  "flying","monster","neutral", "no_attack", "no_damage",
  "friendly","generator","is_thrown","auto_apply","treasure",
  "apply_once","see_invisible","can_roll","is_turnable",
  "is_used_up","identified","reflecting","changing","splitting","hitback",
  "startequip","blocksview","editable","undead","scared","unaggressive",
  "reflect_missile","reflect_spell","no_magic",
  "wiz","was_wiz","no_fix_player","tear_down", "luck",
  "run_away","pass_thru",
  "can_pass_thru","pick_up","anim_speed","container","no_drop",
  "no_pretext","will_apply","random_movement", "can_apply",
  "can_cast_spell","can_use_scroll","can_use_wand","can_use_bow",
  "can_use_armour","can_use_weapon","can_use_ring","has_ready_wand",
  "has_ready_bow","xrays","is_floor","lifesave","no_strength",
  "sleep","stand_still","random_move","only_attack","armour",
  "attack_movement","move_state","confused","stealth","connected",
  "cursed","damned","see_anywhere","known_magical","known_cursed",
  "can_use_skill","been_applied","title","has_ready_rod","can_use_rod",
  "has_ready_horn","can_use_horn","expmul",
  "unique","make_invisible","inv_locked",
  "is_wooded","is_hilly","has_ready_skill","has_ready_weapon",
  "no_skill_ident","glow_radius","is_blind","can_see_in_dark",
  "is_cauldron","randomitems","is_dust", "no_steal", "one_hit","berserk",
  "weapontype","sub_type", "sub_type2","casting_speed",
  "elevation",
/* GROS - And those are for the new plugin system */
  "event_apply",    "event_apply_plugin",   "event_apply_options",
  "event_attack",   "event_attack_plugin",  "event_attack_options",
  "event_born",     "event_born_plugin",    "event_born_options",
  "event_crash",    "event_crash_plugin",   "event_crash_options",
  "event_death",    "event_death_plugin",   "event_death_options",
  "event_drop",     "event_drop_plugin",    "event_drop_options",
  "event_login",    "event_login_plugin",   "event_login_options",
  "event_logout",   "event_logout_plugin",  "event_logout_options",
  "event_pickup",   "event_pickup_plugin",  "event_pickup_options",
  "event_remove",   "event_remove_plugin",  "event_remove_options",
  "event_say",      "event_say_plugin",     "event_say_options",
  "event_shout",    "event_shout_plugin",   "event_shout_options",
  "event_tell",     "event_tell_plugin",    "event_tell_options",
  "event_time",     "event_time_plugin",    "event_time_options",
  "event_throw",    "event_throw_plugin",   "event_throw_options",
  "event_trigger",  "event_trigger_plugin", "event_trigger_options",
  "event_close", "event_close_plugin", "event_close_options",
  "event_timer",    "event_timer_plugin",   "event_timer_options",
#ifdef NPC_PROG
  "npc_status","npc_program",
#endif
};


#define YY_DECL int lex_load(object *op, int map_flags)

static char *yval();

static int lex_error;
static char msgbuf[HUGE_BUF];

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

%}



S	[ \t]+.+
WS	[ \t]*

%x MESSAGE

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

^end{WS}$	    {	/* We do some specialized handling to handle legacy cases of
			 * name_pl.  If the object doesn't have a name_pl, we just
			 * use the object name - this isn't perfect (things won't be
			 * properly pluralized), but works to that degree (5 heart is still
			 * quite understandable).  But the case we also have to catch is
			 * if this object is not using the normal name for the object.  In that
			 * case, we also want to use the loaded name.  Otherwise, what happens
			 * is that the the plural name will lose information (appear as
			 * just 'hearts' and not 'goblins heart')
			 */
			if (op->arch && op->name != op->arch->clone.name && op->name_pl == op->arch->clone.name_pl) {
			    if (op->name_pl) free_string(op->name_pl);
			    op->name_pl = NULL;
			}
			if (!op->name_pl) op->name_pl = add_string(op->name);

			if (ismore) return LL_MORE; 
			else return LL_NORMAL;
		    }
^last_heal{S}	    op->last_heal = IVAL;
^last_sp{S}	    op->last_sp = IVAL;
^last_grace{S}    op->last_grace = IVAL;
^last_eat{S}	    op->last_eat = IVAL;
^speed{S}	    {	op->speed = FVAL;
			if (op->speed<0) op->speed_left = op->speed_left-RANDOM()%100/100.0;
			if (!(map_flags & MAP_STYLE)) update_ob_speed(op);
			if ((map_flags & MAP_STYLE) && (FABS(op->speed)<=MIN_ACTIVE_SPEED)) update_ob_speed(op);
		    }
^speed_left{S}    op->speed_left = FVAL;
^slow_move{S}	    {	SET_SLOW_PENALTY(op,FVAL);
			SET_FLAG(op, FLAG_SLOW_MOVE);
		    }
^face{S}	    op->face = &new_faces[FindFace(yval(), 0)];
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
^exp{S}		op->stats.exp = atol(yval());
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
^material{S}	op->material = IVAL;
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
^walk_on{S}	    SET_OR_CLEAR_FLAG(op, FLAG_WALK_ON, IVAL);
^walk_off{S}	    SET_OR_CLEAR_FLAG(op, FLAG_WALK_OFF, IVAL);
^fly_on{S}	    SET_OR_CLEAR_FLAG(op, FLAG_FLY_ON, IVAL);
^fly_off{S}	    SET_OR_CLEAR_FLAG(op, FLAG_FLY_OFF, IVAL);
^is_animated{S}	    SET_OR_CLEAR_FLAG(op, FLAG_ANIMATE, IVAL);
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
^is_thrown{S}	    SET_OR_CLEAR_FLAG(op, FLAG_IS_THROWN, IVAL);
^auto_apply{S}	    SET_OR_CLEAR_FLAG(op, FLAG_AUTO_APPLY, IVAL);
^treasure{S}	    SET_OR_CLEAR_FLAG(op, FLAG_TREASURE, IVAL);
^apply_once{S}	    SET_OR_CLEAR_FLAG(op, FLAG_APPLY_ONCE, IVAL);
^see_invisible{S}   SET_OR_CLEAR_FLAG(op, FLAG_SEE_INVISIBLE, IVAL);
^can_roll{S} 	    SET_OR_CLEAR_FLAG(op, FLAG_CAN_ROLL, IVAL);
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
^anim_speed{S}	    op->anim_speed = IVAL;
^container{S}	    op->weight_limit = IVAL;
^no_drop{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_DROP, IVAL);
^no_pretext{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_PRETEXT, IVAL);
^will_apply{S}	    op->will_apply = IVAL;
^random_movement{S}	    SET_OR_CLEAR_FLAG(op, FLAG_RANDOM_MOVE, IVAL);
^can_apply{S}		    op->can_apply = IVAL;
^can_cast_spell{S}	    SET_OR_CLEAR_FLAG(op, FLAG_CAST_SPELL, IVAL);
^can_use_scroll{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_SCROLL, IVAL);
^can_use_wand{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_WAND, IVAL);
^can_use_bow{S}		    SET_OR_CLEAR_FLAG(op, FLAG_USE_BOW, IVAL);
^can_use_armour{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_ARMOUR, IVAL);
^can_use_weapon{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_WEAPON, IVAL);
^can_use_ring{S}	    SET_OR_CLEAR_FLAG(op, FLAG_USE_RING, IVAL);
^has_ready_wand{S}	    SET_OR_CLEAR_FLAG(op, FLAG_READY_WAND, IVAL);
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
^title{S}	    {	char *y=yval();
			if (*y=='\0') LOG(llevError,"Title without value.\n");
			else FREE_AND_COPY(op->title, y);
		    }
^has_ready_rod{S}	SET_OR_CLEAR_FLAG(op, FLAG_READY_ROD, IVAL);
^can_use_rod{S}		SET_OR_CLEAR_FLAG(op, FLAG_USE_ROD, IVAL);
^has_ready_horn{S}	SET_OR_CLEAR_FLAG(op, FLAG_READY_HORN, IVAL);
^can_use_horn{S}	SET_OR_CLEAR_FLAG(op, FLAG_USE_HORN, IVAL);
^expmul{S}		op->expmul = FVAL;
^unique{S}		SET_OR_CLEAR_FLAG(op, FLAG_UNIQUE, IVAL);
^make_invisible{S}	SET_OR_CLEAR_FLAG(op, FLAG_MAKE_INVIS, IVAL);
^inv_locked{S}		SET_OR_CLEAR_FLAG(op, FLAG_INV_LOCKED, IVAL);
^is_wooded{S}		SET_OR_CLEAR_FLAG(op, FLAG_IS_WOODED, IVAL);
^is_hilly{S}		SET_OR_CLEAR_FLAG(op, FLAG_IS_HILLY, IVAL);
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

^can_knockback{S}	{ /* Some archetypes have these values in them */ }
^can_parry{S}		{ /* Probably the pupland archetypes - I imagined */ }
^can_impale{S}		{ /* That these are for the new combat code */ }
^can_cut{S}		{ /* just ignore for now */ }
^can_dam_armour{S}	{ }
^weapontype{S}		op->weapontype = FVAL;
^casting_speed{S}	op->casting_speed = FVAL;
^elevation{S}		op->elevation = IVAL;
^client_type{S}		op->client_type = IVAL;

^event_apply{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (apply) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_APPLY],(yv));
                                                };
		    }
^event_apply_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (apply) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_APPLY], yv);
                                                };
		    }
^event_apply_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (apply) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_APPLY], yv);
                                                };
		    }
^event_attack{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (attack) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_ATTACK],(yv));
                                                };
		    }
^event_attack_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (attack) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_ATTACK], yv);
                                                };
		    }
^event_attack_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (attack) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_ATTACK], yv);
                                                };
		    }
^event_born{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (born) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_BORN],(yv));
                                                };
		    }
^event_born_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (born) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_BORN], yv);
                                                };
		    }
^event_born_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (born) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_BORN], yv);
                                                };
		    }
^event_crash{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (crash) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_CRASH],(yv));
                                                };
		    }
^event_crash_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (crash) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_CRASH], yv);
                                                };
		    }
^event_crash_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (crash) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_CRASH], yv);
                                                };
		    }
^event_death{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (death) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_DEATH],(yv));
                                                };
		    }
^event_death_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (death) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_DEATH], yv);
                                                };
		    }
^event_death_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (death) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_DEATH], yv);
                                                };
		    }
^event_drop{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (drop) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_DROP],(yv));
                                                };
		    }
^event_drop_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (drop) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_DROP], yv);
                                                };
		    }
^event_drop_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (drop) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_DROP], yv);
                                                };
		    }
^event_login{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (login) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_LOGIN],(yv));
                                                };
		    }
^event_login_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (login) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_LOGIN], yv);
                                                };
		    }
^event_login_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (login) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_LOGIN], yv);
                                                };
		    }
^event_logout{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (logout) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_LOGOUT],(yv));
                                                };
		    }
^event_logout_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (logout) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_LOGOUT], yv);
                                                };
		    }
^event_logout_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (logout) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_LOGOUT], yv);
                                                };
		    }
^event_pickup{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (pickup) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_PICKUP],(yv));
                                                };
		    }
^event_pickup_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (pickup) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_PICKUP], yv);
                                                };
		    }
^event_pickup_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (pickup) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_PICKUP], yv);
                                                };
		    }
^event_remove{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (remove) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_REMOVE],(yv));
                                                };
		    }
^event_remove_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (remove) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_REMOVE], yv);
                                                };
		    }
^event_remove_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (remove) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_REMOVE], yv);
                                                };
		    }
^event_say{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (say) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_SAY],(yv));
                                                };
		    }
^event_say_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (say) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_SAY], yv);
                                                };
		    }
^event_say_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (say) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_SAY], yv);
                                                };
		    }
^event_shout{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (shout) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_SHOUT],(yv));
                                                };
		    }
^event_shout_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (shout) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_SHOUT], yv);
                                                };
		    }
^event_shout_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (shout) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_SHOUT], yv);
                                                };
		    }
^event_tell{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (tell) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_TELL],(yv));
                                                };
		    }
^event_tell_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (tell) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_TELL], yv);
                                                };
		    }
^event_tell_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (tell) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_TELL], yv);
                                                };
		    }
^event_time{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (time) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_TIME],(yv));
                                                };
		    }
^event_time_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (time) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_TIME], yv);
                                                };
		    }
^event_time_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (time) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_TIME], yv);
                                                };
		    }
^event_timer{S}          {	char *yv=yval();
			if (*yv=='\0') LOG(llevError,"Event (timer) without val\n");
			else
			{
				FREE_AND_COPY(op->event_hook[EVENT_TIMER],(yv));
			};
		}
^event_timer_plugin{S}  {	char *yv=yval();
			if (*yv=='\0') LOG(llevError,"Event (timer) without plugin\n");
			else
			{
				FREE_AND_COPY(op->event_plugin[EVENT_TIMER], yv);
			};
		}
^event_timer_options{S} {	char *yv=yval();
			if (*yv=='\0') LOG(llevError,"Event (timer) without options\n");
			else
			{
				FREE_AND_COPY(op->event_options[EVENT_TIMER], yv);
			};
		}

^event_throw{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (throw) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_THROW],(yv));
                                                };
		    }
^event_throw_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (throw) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_THROW], yv);
                                                };
		    }
^event_throw_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (throw) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_THROW], yv);
                                                };
		    }
^event_trigger{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (trigger) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_TRIGGER],(yv));
                                                };
		    }
^event_trigger_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (trigger) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_TRIGGER], yv);
                                                };
		    }
^event_trigger_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (trigger) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_TRIGGER], yv);
                                                };
		    }
^event_close{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (close) without val\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_hook[EVENT_CLOSE],(yv));
                                                };
		    }
^event_close_plugin{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (close) without plugin\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_plugin[EVENT_CLOSE], yv);
                                                };
		    }
^event_close_options{S}   {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Event (close) without options\n");
			else
                                                {
                                                        FREE_AND_COPY(op->event_options[EVENT_CLOSE], yv);
                                                };
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

int flag_links[NUM_FLAGS+1][2] ={
{FLAG_ALIVE, V_ALIVE}, {FLAG_WIZ, V_WIZ},
{-1, -1}, {-1, -1},	/* REMOVED and FREED flags */
{FLAG_WAS_WIZ, V_WAS_WIZ}, {FLAG_APPLIED, V_APPLIED},
{FLAG_UNPAID, V_UNPAID}, {-1, -1}, /* was need_an flag */
{FLAG_NO_PICK, V_NO_PICK}, {FLAG_WALK_ON, V_WALK_ON},
{FLAG_NO_PASS, V_NO_PASS},{FLAG_ANIMATE, V_IS_ANIMATED},
{FLAG_SLOW_MOVE, -1}, {FLAG_FLYING, V_FLYING},
{FLAG_MONSTER, V_MONSTER}, {FLAG_FRIENDLY, V_FRIENDLY},
{FLAG_GENERATOR, V_GENERATOR}, {FLAG_IS_THROWN, V_IS_THROWN},
{FLAG_AUTO_APPLY, V_AUTO_APPLY}, {FLAG_TREASURE, V_TREASURE},
{FLAG_APPLY_ONCE, V_APPLY_ONCE},{FLAG_SEE_INVISIBLE, V_SEE_INVISIBLE},
{FLAG_CAN_ROLL, V_CAN_ROLL}, {-1, -1}, /*paralyze is only a effect flag */
{FLAG_IS_TURNABLE, V_IS_TURNABLE},{FLAG_WALK_OFF, V_WALK_OFF},
{FLAG_FLY_ON, V_FLY_ON},{FLAG_FLY_OFF, V_FLY_OFF}, 
{FLAG_IS_USED_UP, V_IS_USED_UP}, {FLAG_IDENTIFIED, V_IDENTIFIED},
{FLAG_REFLECTING, V_REFLECTING}, {FLAG_CHANGING, V_CHANGING},
{FLAG_SPLITTING, V_SPLITTING}, {FLAG_HITBACK, V_HITBACK},
{FLAG_STARTEQUIP, V_STARTEQUIP}, {FLAG_BLOCKSVIEW, V_BLOCKSVIEW},
{FLAG_UNDEAD, V_UNDEAD}, {FLAG_SCARED, V_SCARED},
{FLAG_UNAGGRESSIVE, V_UNAGGRESSIVE}, {FLAG_REFL_MISSILE, V_REFLECT_MISSILE},
{FLAG_REFL_SPELL, V_REFLECT_SPELL}, {FLAG_NO_MAGIC, V_NO_MAGIC},
{FLAG_NO_FIX_PLAYER, V_NO_FIX_PLAYER}, {-1, -1}, /* need_ie */
{FLAG_TEAR_DOWN, V_TEAR_DOWN}, {FLAG_RUN_AWAY, V_RUN_AWAY},
{FLAG_PASS_THRU, V_PASS_THRU}, {FLAG_CAN_PASS_THRU, V_CAN_PASS_THRU},
{FLAG_PICK_UP, V_PICK_UP}, {FLAG_UNIQUE, V_UNIQUE},
{FLAG_NO_DROP, V_NO_DROP}, {FLAG_NO_PRETEXT, V_NO_PRETEXT},
{FLAG_CAST_SPELL, V_CAN_CAST_SPELL}, {FLAG_USE_SCROLL, V_CAN_USE_SCROLL},
{FLAG_USE_WAND, V_CAN_USE_WAND},{FLAG_USE_BOW, V_CAN_USE_BOW},
{FLAG_USE_ARMOUR, V_CAN_USE_ARMOUR},{FLAG_USE_WEAPON, V_CAN_USE_WEAPON},
{FLAG_USE_RING, V_CAN_USE_RING}, {FLAG_READY_WAND, V_HAS_READY_WAND},
{FLAG_READY_BOW, V_HAS_READY_BOW}, {FLAG_XRAYS, V_XRAYS},
{-1, -1 /*NO_APPLY*/},  {FLAG_IS_FLOOR, V_IS_FLOOR},
{FLAG_LIFESAVE, V_LIFESAVE}, {FLAG_NO_STRENGTH, V_NO_STRENGTH},
{FLAG_SLEEP, V_SLEEP}, {FLAG_STAND_STILL, V_STAND_STILL},
{FLAG_RANDOM_MOVE, V_RANDOM_MOVEMENT}, {FLAG_ONLY_ATTACK, V_ONLY_ATTACK},
{FLAG_CONFUSED, V_CONFUSED}, {FLAG_STEALTH, V_STEALTH},
{-1, -1 /*WIZPASS*/}, {-1, -1 /*IS_LINKED */},
{FLAG_CURSED, V_CURSED}, {FLAG_DAMNED, V_DAMNED},
{FLAG_SEE_ANYWHERE, V_SEE_ANYWHERE}, {FLAG_KNOWN_MAGICAL, V_KNOWN_MAGICAL},
{FLAG_KNOWN_CURSED, V_KNOWN_CURSED}, {FLAG_CAN_USE_SKILL, V_CAN_USE_SKILL},
{FLAG_BEEN_APPLIED, V_BEEN_APPLIED},  {FLAG_READY_ROD, V_HAS_READY_ROD},
{FLAG_USE_ROD, V_CAN_USE_ROD}, {FLAG_READY_HORN, V_HAS_READY_HORN}, 
{FLAG_USE_HORN, V_CAN_USE_HORN},{FLAG_MAKE_INVIS,V_MAKE_INVIS},
{FLAG_INV_LOCKED,V_INV_LOCKED},{FLAG_IS_WOODED,V_IS_WOODED},
{FLAG_IS_HILLY,V_IS_HILLY},{FLAG_READY_SKILL,V_HAS_READY_SKILL},
{FLAG_READY_WEAPON,V_HAS_READY_WEAPON},{FLAG_NO_SKILL_IDENT,V_NO_SKILL_IDENT},
{FLAG_BLIND,V_BLIND},{FLAG_SEE_IN_DARK,V_SEE_IN_DARK},
{FLAG_IS_CAULDRON,V_IS_CAULDRON},{FLAG_DUST,V_DUST},{FLAG_NO_STEAL,V_NO_STEAL},
{FLAG_ONE_HIT, V_ONE_HIT},
{-1, -1 /* Client send debug flag (FLAG_CLIENT_SENT)*/},
{FLAG_BERSERK, V_BERSERK}, 
{FLAG_NEUTRAL, V_NEUTRAL}, {FLAG_NO_ATTACK, V_NO_ATTACK},
{FLAG_NO_DAMAGE, V_NO_DAMAGE},
{-1, -1 /* FLAG_OBJ_ORIGINAL */ },
{-1, -1 /* FLAG_OBJ_SAVE_ON_OVL */ },
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
/*
 * Returns a pointer to a static string which contains all variables
 * which are different in the two given objects.  op is the what object
 * the different values will be taken from.  This function is
 * typically used to dump objects (op2=empty object), or to save objects
 * (op2 is the objects original archetype)
 */

char *get_ob_diff(object *op,object *op2) {/* I plan to optimize this heavily */
  static char buf2[HUGE_BUF];
  static char buf[HUGE_BUF];
  int tmp;
  int i;
  
  buf[0]='\0';
  if(op->name && op->name!=op2->name) {
    sprintf(buf2,"name %s\n",op->name);
    strcat(buf,buf2);
  }
  if(op->name_pl && op->name_pl!=op2->name_pl) {
    sprintf(buf2,"name_pl %s\n",op->name_pl);
    strcat(buf,buf2);
  }
  if(op->title && op->title!=op2->title) {
    sprintf(buf2,"title %s\n", op->title);
    strcat(buf, buf2);
  }
  if(op->race && op->race!=op2->race) {
    sprintf(buf2,"race %s\n",op->race);
    strcat(buf,buf2);
  }
  if(op->slaying && op->slaying!=op2->slaying) {
    sprintf(buf2,"slaying %s\n",op->slaying);
    strcat(buf,buf2);
  }
  if(op->msg && op->msg!=op2->msg) {
    strcat(buf,"msg\n");
    strcat(buf,op->msg);
    strcat(buf,"endmsg\n");
  }
  if(op->other_arch!=op2->other_arch&&op->other_arch!=NULL &&
     op->other_arch->name) {
    sprintf(buf2,"other_arch %s\n",op->other_arch->name);
    strcat(buf,buf2);
  }
  if(op->face!=op2->face) {
      sprintf(buf2,"%s %s\n",variable_const[V_FACE],
		op->face->name);
      strcat(buf,buf2);
  }
  for(i=1;i<=NR_EVENTS;i++)
  {
    if(op->event_hook[i] && op->event_hook[i]!=op2->event_hook[i]) {
        switch(i)
        {
            case EVENT_APPLY:
                sprintf(buf2,"event_apply %s\n",op->event_hook[i]);
                break;
            case EVENT_ATTACK:
                sprintf(buf2,"event_attack %s\n",op->event_hook[i]);
                break;
            case EVENT_BORN:
                sprintf(buf2,"event_born %s\n",op->event_hook[i]);
                break;
            case EVENT_CRASH:
                sprintf(buf2,"event_crash %s\n",op->event_hook[i]);
                break;
            case EVENT_DEATH:
                sprintf(buf2,"event_death %s\n",op->event_hook[i]);
                break;
            case EVENT_DROP:
                sprintf(buf2,"event_drop %s\n",op->event_hook[i]);
                break;
            case EVENT_LOGIN:
                sprintf(buf2,"event_login %s\n",op->event_hook[i]);
                break;
            case EVENT_LOGOUT:
                sprintf(buf2,"event_logout %s\n",op->event_hook[i]);
                break;
            case EVENT_PICKUP:
                sprintf(buf2,"event_pickup %s\n",op->event_hook[i]);
                break;
            case EVENT_REMOVE:
                sprintf(buf2,"event_remove %s\n",op->event_hook[i]);
                break;
            case EVENT_SAY:
                sprintf(buf2,"event_say %s\n",op->event_hook[i]);
                break;
            case EVENT_SHOUT:
                sprintf(buf2,"event_shout %s\n",op->event_hook[i]);
                break;
            case EVENT_TELL:
                sprintf(buf2,"event_tell %s\n",op->event_hook[i]);
                break;
            case EVENT_TIME:
                sprintf(buf2,"event_time %s\n",op->event_hook[i]);
                break;
            case EVENT_THROW:
                sprintf(buf2,"event_throw %s\n",op->event_hook[i]);
                break;
            case EVENT_TRIGGER:
                sprintf(buf2,"event_trigger %s\n",op->event_hook[i]);
                break;
            case EVENT_CLOSE:
                sprintf(buf2,"event_close %s\n",op->event_hook[i]);
                break;
            case EVENT_TIMER:
                sprintf(buf2,"event_timer %s\n",op->event_hook[i]);
                break;
        };
        strcat(buf,buf2);
    }
    if(op->event_plugin[i] && op->event_plugin[i]!=op2->event_plugin[i]) {
        switch(i)
        {
            case EVENT_APPLY:
                sprintf(buf2,"event_apply_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_ATTACK:
                sprintf(buf2,"event_attack_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_BORN:
                sprintf(buf2,"event_born_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_CRASH:
                sprintf(buf2,"event_crash_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_DEATH:
                sprintf(buf2,"event_death_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_DROP:
                sprintf(buf2,"event_drop_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_LOGIN:
                sprintf(buf2,"event_login_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_LOGOUT:
                sprintf(buf2,"event_logout_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_PICKUP:
                sprintf(buf2,"event_pickup_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_REMOVE:
                sprintf(buf2,"event_remove_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_SAY:
                sprintf(buf2,"event_say_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_SHOUT:
                sprintf(buf2,"event_shout_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_TELL:
                sprintf(buf2,"event_tell_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_TIME:
                sprintf(buf2,"event_time_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_THROW:
                sprintf(buf2,"event_throw_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_TRIGGER:
                sprintf(buf2,"event_trigger_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_CLOSE:
                sprintf(buf2,"event_close_plugin %s\n",op->event_plugin[i]);
                break;
            case EVENT_TIMER:
                sprintf(buf2,"event_timer_plugin %s\n",op->event_plugin[i]);
                break;
        };
        strcat(buf,buf2);
    }
    if(op->event_options[i] && op->event_options[i]!=op2->event_options[i]) {
        switch(i)
        {
            case EVENT_APPLY:
                sprintf(buf2,"event_apply_options %s\n",op->event_options[i]);
                break;
            case EVENT_ATTACK:
                sprintf(buf2,"event_attack_options %s\n",op->event_options[i]);
                break;
            case EVENT_BORN:
                sprintf(buf2,"event_born_options %s\n",op->event_options[i]);
                break;
            case EVENT_CRASH:
                sprintf(buf2,"event_crash_options %s\n",op->event_options[i]);
                break;
            case EVENT_DEATH:
                sprintf(buf2,"event_death_options %s\n",op->event_options[i]);
                break;
            case EVENT_DROP:
                sprintf(buf2,"event_drop_options %s\n",op->event_options[i]);
                break;
            case EVENT_LOGIN:
                sprintf(buf2,"event_login_options %s\n",op->event_options[i]);
                break;
            case EVENT_LOGOUT:
                sprintf(buf2,"event_logout_options %s\n",op->event_options[i]);
                break;
            case EVENT_PICKUP:
                sprintf(buf2,"event_pickup_options %s\n",op->event_options[i]);
                break;
            case EVENT_REMOVE:
                sprintf(buf2,"event_remove_options %s\n",op->event_options[i]);
                break;
            case EVENT_SAY:
                sprintf(buf2,"event_say_options %s\n",op->event_options[i]);
                break;
            case EVENT_SHOUT:
                sprintf(buf2,"event_shout_options %s\n",op->event_options[i]);
                break;
            case EVENT_TELL:
                sprintf(buf2,"event_tell_options %s\n",op->event_options[i]);
                break;
            case EVENT_TIME:
                sprintf(buf2,"event_time_options %s\n",op->event_options[i]);
                break;
            case EVENT_THROW:
                sprintf(buf2,"event_throw_options %s\n",op->event_options[i]);
                break;
            case EVENT_TRIGGER:
                sprintf(buf2,"event_trigger_options %s\n",op->event_options[i]);
                break;
            case EVENT_CLOSE:
                sprintf(buf2,"event_close_options %s\n",op->event_options[i]);
                break;
            case EVENT_TIMER:
                sprintf(buf2,"event_timer_options %s\n",op->event_options[i]);
                break;
        };
        strcat(buf,buf2);
    }
  }
  if (op->animation_id != op2->animation_id) {
    if (op->animation_id) {
      sprintf(buf2,"animation %s\n", animations[GET_ANIM_ID(op)].name);
      if ( ! QUERY_FLAG (op, FLAG_ANIMATE)) {
        strcat (buf, buf2);
        sprintf (buf2, "is_animated 0\n");
      }
    } else {
      sprintf (buf2, "animation NONE\n");
    }
    strcat (buf, buf2);
  }
  if(op->stats.Str!=op2->stats.Str)
    save_long(buf,variable_const[V_STR],op->stats.Str);
  if(op->stats.Dex!=op2->stats.Dex)
    save_long(buf,variable_const[V_DEX],op->stats.Dex);
  if(op->stats.Con!=op2->stats.Con)
    save_long(buf,variable_const[V_CON],op->stats.Con);
  if(op->stats.Wis!=op2->stats.Wis)
    save_long(buf,variable_const[V_WIS],op->stats.Wis);
  if(op->stats.Pow!=op2->stats.Pow)
    save_long(buf,variable_const[V_POW],op->stats.Pow);
  if(op->stats.Cha!=op2->stats.Cha)
    save_long(buf,variable_const[V_CHA],op->stats.Cha);
  if(op->stats.Int!=op2->stats.Int)
    save_long(buf,variable_const[V_INT],op->stats.Int);
  if(op->stats.hp!=op2->stats.hp)
    save_long(buf,variable_const[V_HP],op->stats.hp);
  if(op->stats.maxhp!=op2->stats.maxhp)
    save_long(buf,variable_const[V_MAXHP],op->stats.maxhp);
  if(op->stats.sp!=op2->stats.sp)
    save_long(buf,variable_const[V_SP],op->stats.sp);
  if(op->stats.maxsp!=op2->stats.maxsp)
    save_long(buf,variable_const[V_MAXSP],op->stats.maxsp);
  if(op->stats.grace!=op2->stats.grace)
    save_long(buf,variable_const[V_GRACE],op->stats.grace);
  if(op->stats.maxgrace!=op2->stats.maxgrace)
    save_long(buf,variable_const[V_MAXGRACE],op->stats.maxgrace);
  if(op->stats.exp!=op2->stats.exp)
    save_long(buf,variable_const[V_EXP],op->stats.exp);
  if(op->expmul!=op2->expmul) 
    save_double(buf,variable_const[V_EXPMUL],op->expmul);
  if(op->stats.food!=op2->stats.food)
    save_long(buf,variable_const[V_FOOD],op->stats.food);
  if(op->stats.dam!=op2->stats.dam)
    save_long(buf,variable_const[V_DAM],op->stats.dam);
  if(op->stats.luck!=op2->stats.luck)
    save_long(buf,variable_const[V_LUCK],op->stats.luck);
  if(op->stats.wc!=op2->stats.wc)
    save_long(buf,variable_const[V_WC],op->stats.wc);
  if(op->stats.ac!=op2->stats.ac)
    save_long(buf,variable_const[V_AC],op->stats.ac);
  if(op->x!=op2->x)
    save_long(buf,variable_const[V_X],op->x);
  if(op->y!=op2->y)
    save_long(buf,variable_const[V_Y],op->y);
  if(op->speed!=op2->speed) {
    sprintf(buf2,"speed %f\n",op->speed);
    strcat(buf,buf2);
  }
  if(op->speed > 0 && op->speed_left!=op2->speed_left) {
    sprintf(buf2,"speed_left %f\n",op->speed_left);
    strcat(buf,buf2);
  }
  if(op->move_status != op2->move_status)
    save_long(buf,variable_const[V_MOVE_STATUS],op->move_status);
  if(op->move_type != op2->move_type)
    save_long(buf,variable_const[V_ATT_MOVE],op->move_type);
  if(op->nrof!=op2->nrof)
    save_long(buf,variable_const[V_NROF],op->nrof);
  if(op->level!=op2->level)
    save_long(buf,variable_const[V_LEVEL],op->level);
  if(op->direction!=op2->direction)
    save_long(buf,variable_const[V_DIRECTION],op->direction);
  if(op->type!=op2->type)
    save_long(buf,variable_const[V_TYPE],op->type);
  if(op->attacktype!=op2->attacktype)
    save_long(buf,variable_const[V_ATTACKTYPE],op->attacktype);

  for (tmp=0; tmp < NROFATTACKS; tmp++) {
   if (op->resist[tmp] != op2->resist[tmp]) {
	sprintf(buf2,"resist_%s %d\n",resist_save[tmp], op->resist[tmp]);
	strcat(buf,buf2);
    }
  }

  if(op->path_attuned!=op2->path_attuned)
    save_long(buf,variable_const[V_PATH_ATTUNED],op->path_attuned);
  if(op->path_repelled!=op2->path_repelled)
    save_long(buf,variable_const[V_PATH_REPELLED],op->path_repelled);
  if(op->path_denied!=op2->path_denied)
    save_long(buf,variable_const[V_PATH_DENIED],op->path_denied);
  if(op->material!=op2->material)
    save_long(buf,variable_const[V_MATERIAL],op->material);
  if(op->value!=op2->value)
    save_long(buf,variable_const[V_VALUE],op->value);
  if(op->carrying!=op2->carrying)
    save_long(buf,variable_const[V_CARRYING],op->carrying);
  if(op->weight!=op2->weight)
    save_long(buf,variable_const[V_WEIGHT],op->weight);
  if(op->invisible!=op2->invisible)
    save_long(buf,variable_const[V_INVISIBLE],op->invisible);
  if(op->state!=op2->state)
    save_long(buf,variable_const[V_STATE],op->state);
  if(op->magic!=op2->magic)
    save_long(buf,variable_const[V_MAGIC],op->magic);
  if(op->last_heal!=op2->last_heal)
    save_long(buf,variable_const[V_LAST_HEAL],op->last_heal);
  if(op->last_sp!=op2->last_sp)
    save_long(buf,variable_const[V_LAST_SP],op->last_sp);
  if(op->last_grace!=op2->last_grace)
    save_long(buf,variable_const[V_LAST_GRACE],op->last_grace);
  if(op->last_eat!=op2->last_eat)
    save_long(buf,variable_const[V_LAST_EAT],op->last_eat);
  if(QUERY_FLAG(op,FLAG_IS_LINKED) && (tmp = get_button_value(op)))
    save_long(buf,variable_const[V_CONNECTED],tmp);
  if(op->glow_radius!=op2->glow_radius)
    save_long(buf,variable_const[V_GLOW_RADIUS],op->glow_radius);
  if (op->randomitems!=op2->randomitems) {
    sprintf(buf2,"randomitems %s\n",(op->randomitems?op->randomitems->name:"none"));
    strcat(buf,buf2);
  }
#ifdef NPC_PROG
  if(op->npc_status!=op2->npc_status)
    save_long(buf,variable_const[V_NPC_STATUS],op->npc_status);
  if(op->npc_program!=op2->npc_program)
    save_long(buf,variable_const[V_NPC_PROGRAM],op->npc_program);
#endif


/* Eneq(@csd.uu.se): Handles run_away and pick_up */
  if(op->run_away!=op2->run_away)
    save_long(buf,variable_const[V_RUN_AWAY],op->run_away);
  if(op->pick_up!=op2->pick_up)
    save_long(buf,variable_const[V_PICK_UP],op->pick_up);
  if(op->weight_limit!=op2->weight_limit)
    save_long(buf,variable_const[V_CONTAINER],op->weight_limit);


/* Vick (@bern.docs.uu.se) @921107 -> Handle 'will_apply' &
   'random_movement.*/

  if (op->will_apply!=op2->will_apply)
    save_long(buf,variable_const[V_WILL_APPLY],op->will_apply);

  if (op->current_weapon_script!=op2->current_weapon_script){
    sprintf(buf2,"current_weapon_script %s\n",(op->current_weapon_script));
    strcat(buf,buf2);
  };

/* Mol(@meryl.csd.uu.se) 921108  Handle can_apply */
  if (op->can_apply!=op2->can_apply)
    save_long(buf,variable_const[V_CAN_APPLY],op->can_apply);

  if(op->weapontype && op->weapontype!=op2->weapontype) {
    sprintf(buf2,"weapontype %d\n",op->weapontype);
    strcat(buf,buf2);
  }
  if (op->elevation && op->elevation != op2->elevation) {
    sprintf(buf2,"elevation %d\n", op->elevation);
    strcat(buf,buf2);
  }
  if (op->client_type && op->client_type != op2->client_type) {
    sprintf(buf2,"client_type %d\n", op->client_type);
    strcat(buf,buf2);
  }

  for (tmp=0; tmp <= NUM_FLAGS; tmp++) {
    if ((flag_links[tmp][0]!=-1) && (QUERY_FLAG(op, flag_links[tmp][0]) !=
	QUERY_FLAG(op2, flag_links[tmp][0]))) {
	    if (flag_links[tmp][0]==FLAG_SLOW_MOVE) {
		sprintf(buf2,"%s %f\n",variable_const[V_SLOW_MOVE],SLOW_PENALTY(op));
		strcat(buf,buf2);
	    }
	    else
	        BUFADD(QUERY_FLAG(op, flag_links[tmp][0]), flag_links[tmp][1]);
	}
  }
  if(buf[0]=='\0')
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







