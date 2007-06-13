%{
/*
 * static char *rcsid_object_c =
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

/* Eneq(@csd.uu.se): Added weight-modifiers in environment of objects.
   sub/add_weight will transcend the environment updating the carrying
   variable. */


#include <global.h>
#include <loader.h>
#include <newserver.h>
#include <sproto.h>

#define YY_DECL int lex_load(int* depth, object** items, int maxdepth, int map_flags, int linemode)

static char *yval(void);

static int lex_error;
static char msgbuf[HUGE_BUF];
int	msglen=0, total_msglen=0;
static char lorebuf[HUGE_BUF];

/** Maps the MOVE_* values to names */
static const char *const move_name[] = {"walk", "fly_low", "fly_high", "swim", "boat",
    NULL};

/**
 * This table is only necessary to convert objects that existed before the
 * spell object conversion to the new object.  It was not practical
 * to go through every mapping looking for every potion, rod, wand, etc
 * that had a sp set and update to the new value.  So this maps the
 * old spell numbers to the name of the new archs.
 * If you are adding a new spell, you should not modify this - you
 * new spell won't have been used, and thus won't have any legacy object.
 * NULL entries in this table are valid - to denote objects that should
 * not be updated for whatever reason.
 *
 * @todo
 * is this still required? it's used, but can it happen?
 */
const char *const spell_mapping[] = {
    "spell_magic_bullet",		/* 0 */
    "spell_small_fireball",		/* 1 */
    "spell_medium_fireball",	/* 2 */
    "spell_large_fireball",		/* 3 */
    "spell_burning_hands",		/* 4 */
    "spell_sm_lightning",		/* 5 */
    "spell_large_lightning",	/* 6 */
    "spell_magic_missile",		/* 7 */
    "spell_create_bomb",		/* 8 */
    "spell_summon_golem",		/* 9 */
    "spell_summon_fire_elemental",	/* 10 */
    "spell_summon_earth_elemental",	/* 11 */
    "spell_summon_water_elemental",	/* 12 */
    "spell_summon_air_elemental",	/* 13 */
    "spell_dimension_door",		/* 14 */
    "spell_create_earth_wall",	/* 15 */
    "spell_paralyze",		/* 16 */
    "spell_icestorm",		/* 17 */
    "spell_magic_mapping",		/* 18 */
    "spell_turn_undead",		/* 19 */
    "spell_fear",			/* 20 */
    "spell_poison_cloud",		/* 21 */
    "spell_wonder",			/* 22 */
    "spell_destruction",		/* 23 */
    "spell_perceive_self",		/* 24 */
    "spell_word_of_recall",		/* 25 */
    "spell_invisible",		/* 26 */
    "spell_invisible_to_undead",	/* 27 */
    "spell_probe",			/* 28 */
    "spell_lg_magic_bullet",	/* 29 */
    "spell_improved_invisibility",	/* 30 */
    "spell_holy_word",		/* 31 */
    "spell_minor_healing",		/* 32 */
    "spell_medium_healing",		/* 33 */
    "spell_major_healing",		/* 34 */
    "spell_heal",			/* 35 */
    "spell_create_food",		/* 36 */
    "spell_earth_to_dust",		/* 37 */
    "spell_armour",			/* 38 */
    "spell_strength",		/* 39 */
    "spell_dexterity",		/* 40 */
    "spell_constitution",		/* 41 */
    "spell_charisma",		/* 42 */
    "spell_create_fire_wall",	/* 43 */
    "spell_create_frost_wall",	/* 44 */
    "spell_protection_from_cold",	/* 45 */
    "spell_protection_from_electricity",	/* 46 */
    "spell_protection_from_fire",	/* 47 */
    "spell_protection_from_poison",	/* 48 */
    "spell_protection_from_slow",	/* 49 */
    "spell_protection_from_paralysis",	/* 50 */
    "spell_protection_from_draining",	/* 51 */
    "spell_protection_from_magic",	/* 52 */
    "spell_protection_from_attack",	/* 53 */
    "spell_levitate",		/* 54 */
    "spell_small_speedball",	/* 55 */
    "spell_large_speedball",	/* 56 */
    "spell_hellfire",		/* 57 */
    "spell_dragonbreath",		/* 58 */
    "spell_large_icestorm",		/* 59 */
    "spell_charging",		/* 60 */
    "spell_polymorph",		/* 61 */
    "spell_cancellation",		/* 62 */
    "spell_confusion",		/* 63 */
    "spell_mass_confusion",		/* 64 */
    "spell_summon_pet_monster",	/* 65 */
    "spell_slow",			/* 66 */
    "spell_regenerate_spellpoints",	/* 67 */
    "spell_cure_poison",		/* 68 */
    "spell_protection_from_confusion",	/* 69 */
    "spell_protection_from_cancellation",	/* 70 */
    "spell_protection_from_depletion",	/* 71 */
    "spell_alchemy",		/* 72 */
    "spell_remove_curse",		/* 73 */
    "spell_remove_damnation",	/* 74 */
    "spell_identify",		/* 75*/
    "spell_detect_magic",		/* 76 */
    "spell_detect_monster",		/* 77 */
    "spell_detect_evil",		/* 78 */
    "spell_detect_curse",		/* 79 */
    "spell_heroism",		/* 80 */
    "spell_aggravation",		/* 81 */
    "spell_firebolt",		/* 82 */
    "spell_frostbolt",		/* 83 */
    "spell_shockwave",		/* 84 */
    "spell_color_spray",		/* 85 */
    "spell_haste",			/* 86 */
    "spell_face_of_death",		/* 87 */
    "spell_ball_lightning",		/* 88 */
    "spell_meteor_swarm",		/* 89 */
    "spell_comet",			/* 90 */
    "spell_mystic_fist",		/* 91 */
    "spell_raise_dead",		/* 92 */
    "spell_resurrection",		/* 93 */
    "spell_reincarnation",		/* 94 */
    "spell_immunity_to_cold",	/* 95 */
    "spell_immunity_to_electricity",/* 96 */
    "spell_immunity_to_fire",	/* 97 */
    "spell_immunity_to_poison",	/* 98 */
    "spell_immunity_to_slow",	/* 99 */
    "spell_immunity_to_paralysis",	/* 100 */
    "spell_immunity_to_draining",	/* 101 */
    "spell_immunity_to_magic",	/* 102 */
    "spell_immunity_to_attack",	/* 103 */
    "spell_invulnerability",	/* 104 */
    "spell_defense",		/* 105 */
    "spell_rune_of_fire",		/* 106 */
    "spell_rune_of_frost",		/* 107 */
    "spell_rune_of_shocking",	/* 108 */
    "spell_rune_of_blasting",	/* 109 */
    "spell_rune_of_death",		/* 110 */
    "spell_marking_rune",		/* 111 */
    "spell_build_director",		/* 112 */
    "spell_create_pool_of_chaos",	/* 113 */
    "spell_build_bullet_wall",	/* 114 */
    "spell_build_lightning_wall",	/* 115 */
    "spell_build_fireball_wall",	/* 116 */
    "spell_magic_rune",		/* 117 */
    "spell_rune_of_magic_drain",	/* 118 */
    "spell_antimagic_rune",		/* 119 */
    "spell_rune_of_transference",	/* 120 */
    "spell_transference",		/* 121 */
    "spell_magic_drain",		/* 122 */
    "spell_counterspell",		/* 123 */
    "spell_disarm",			/* 124 */
    "spell_cure_confusion",		/* 125 */
    "spell_restoration",		/* 126 */
    "was summon evil monster",	/* 127 */   /* Not implenented as nothing used it */
    "spell_counterwall",		/* 128 */
    "spell_cause_light_wounds",	/* 129 */
    "spell_cause_medium_wounds",	/* 130 */
    "spell_cause_heavy_wounds",	/* 131 */
    "spell_charm_monsters",		/* 132 */
    "spell_banishment",		/* 133 */
    "spell_create_missile",		/* 134 */
    "spell_show_invisible",		/* 135 */
    "spell_xray",			/* 136 */
    "spell_pacify",			/* 137 */
    "spell_summon_fog",		/* 138 */
    "spell_steambolt",		/* 139 */
    "spell_command_undead",		/* 140 */
    "spell_holy_orb",		/* 141 */
    "spell_summon_avatar",		/* 142 */
    "spell_holy_possession",	/* 143 */
    "spell_bless",			/* 144 */
    "spell_curse",			/* 145 */
    "spell_regeneration",		/* 146 */
    "spell_consecrate",		/* 147 */
    "spell_summon_cult_monsters",	/* 148 */
    "spell_cause_critical_wounds",	/* 149 */
    "spell_holy_wrath",		/* 150 */
    "spell_retributive_strike",	/* 151 */
    "spell_finger_of_death",	/* 152 */
    "spell_insect_plague",		/* 153 */
    "spell_call_holy_servant",	/* 154 */
    "spell_wall_of_thorns",		/* 155 */
    "spell_staff_to_snake",		/* 156 */
    "spell_light",			/* 157 */
    "spell_darkness",		/* 158 */
    "spell_nightfall",		/* 159 */
    "spell_daylight",		/* 160 */
    "spell_sunspear",		/* 161 */
    "spell_faery_fire",		/* 162 */
    "spell_cure_blindness",		/* 163 */
    "spell_dark_vision",		/* 164 */
    "spell_bullet_swarm",		/* 165 */
    "spell_bullet_storm",		/* 166 */
    "spell_cause_many_wounds",	/* 167 */
    "spell_small_snowstorm",	/* 168 */
    "spell_medium_snowstorm",	/* 169 */
    "spell_large_snowstorm",	/* 170 */
    "spell_cure_disease",		/* 171 */
    "spell_cause_red_death",	/* 172 */
    "spell_cause_flu",		/* 173 */
    "spell_cause_black_death",	/* 174 */
    "spell_cause_leprosy",		/* 175 */
    "spell_cause_smallpox",		/* 176 */
    "spell_cause_white_death",	/* 177 */
    "spell_cause_anthrax",		/* 178 */
    "spell_cause_typhoid",		/* 179 */
    "spell_mana_blast",		/* 180 */
    "spell_small_manaball",		/* 181 */
    "spell_medium_manaball",	/* 182 */
    "spell_large_manaball",		/* 183 */
    "spell_manabolt",		/* 184 */
    "spell_dancing_sword",		/* 185 */
    "spell_animate_weapon",		/* 186 */
    "spell_cause_cold",		/* 187 */
    "spell_divine_shock",		/* 188 */
    "spell_windstorm",		/* 189 */
    "spell_sanctuary",		/* 190 */
    "spell_peace",			/* 191 */
    "spell_spiderweb",		/* 192 */
    "spell_conflict",		/* 193 */
    "spell_rage",			/* 194 */
    "spell_forked_lightning",	/* 195 */
    "spell_poison_fog",		/* 196 */
    "spell_flaming_aura",		/* 197 */
    "spell_vitriol",		/* 198 */
    "spell_vitriol_splash",		/* 199 */
    "spell_iron_skin",		/* 200 */
    "spell_wrathful_eye",		/* 201 */
    "spell_town_portal",		/* 202 */
    "spell_missile_swarm",		/* 203 */
    "spell_cause_rabies",		/* 204 */
    "spell_glyph",			/* 205 */
    NULL
};

/** Utility macro to transform a bit into a flag. */
#define SET_OR_CLEAR_FLAG(op, flag, val) \
	{ if (val) SET_FLAG(op, flag); else CLEAR_FLAG(op, flag); }

/**
 * SET_RESIST is really only really needed for transition code.  We normally
 * don't care about multiple values overwriting each other, but this is
 * to catch items that have multiple protection/immune/vulnerable.
 * This can be simplified later on to just do the set after all the archs
 * and maps have been updated.
 * We always keep the last value because otherwise the value from the
 * arch may take precedence.
 * Unfortunately, we will report warnings here simply because an object has
 * been modified from the arch.
 */
#define SET_RESIST(op, type, val)  op->resist[type] = val;


/**
 * Insert discrete damage.
 */
static void set_damage(object* op, int type, int val) {
    if (!op->discrete_damage) {
        op->discrete_damage = calloc(1, sizeof(sint16) * NROFATTACKS);
    }
    op->discrete_damage[type] = val;
}

/** Next value as integer. */
#define IVAL	atoi(yval())
/** Next value as float. */
#define FVAL	atof(yval())
extern int arch_init;
extern int artifact_init;

/**
 * Fills in the body information for one slot.
 *
 * @param op
 * object
 * @param params
 * the body part, which should be in the ::body_locations structure.
 */
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


/**
 * This function checks the object after it has been loaded (when we
 * get the 'end' in the input stream).  This function can be used to
 * deal with legacy objects where fields may have changed.  It can also be used
 * to check for objects to make sure there are no common errors.
 *
 * @param op
 * object to check.
 */
static void check_loaded_object(object *op) {
    int ip;

    if (artifact_init)
        /* Artifacts are special beasts, let's not check them. */
        return;

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
    if (!op->name_pl && op->name)
        op->name_pl = add_string(op->name);

    /* objects now have a materialname.  try to patch it in */
    if (!(IS_WEAPON(op) && op->level > 0)) {
        if (op->map != NULL)
            set_materialname(op, op->map->difficulty, NULL);
        else
            set_materialname(op, 5, NULL);
    }
    /* only do these when program is first run - a bit
     * excessive to do this at every run - most of this is
     * really just to catch any errors - program will still run, but
     * not in the ideal fashion.
     */
    if ((op->type == WEAPON || op->type==BOW) && arch_init) {
        if (!op->skill) {
            LOG(llevError,"Weapon %s lacks a skill.\n", op->name);
        } else if ((!strcmp(op->skill,"one handed weapons") && op->body_info[1] != -1) ||
            (!strcmp(op->skill,"two handed weapons") && op->body_info[1] != -2)) {
            LOG(llevError,"weapon %s arm usage does not match skill: %d, %s\n",
            op->name, op->body_info[1], op->skill);
        }
    }

    /* Old spellcasting object - need to load in the appropiate object
     * This is actually required as of 2007-06-09, since Gridarta doesn't yet support spells in
     * inventory. See
     * http://sourceforge.net/tracker/index.php?func=detail&aid=1555828&group_id=166996&atid=841182
     *
     * This can be removed when Gridarta is updated, and archetypes, maps and artifacts too.
     */
    if ((op->type == ROD || op->type == WAND || op->type == SCROLL || op->type == HORN
        || op->type == FIREWALL ||
        /* POTIONS and ALTARS don't always cast spells, but if they do, update them */
        ((op->type == POTION || op->type == ALTAR) && op->stats.sp)) && !op->inv && !arch_init)  {
        object *tmp;

    /* Fireall is bizarre in that spell type was stored in dam.  Rest are 'normal'
     * in that spell was stored in sp.
     */
        tmp = create_archetype(spell_mapping[op->type == FIREWALL?op->stats.dam:op->stats.sp]);
        insert_ob_in_ob(tmp, op);
        op->randomitems = NULL;	/* So another spell isn't created for this object */
    }

    /* spellbooks & runes use slaying.  But not to arch name, but to spell name */
    if ((op->type == SPELLBOOK || op->type == RUNE) && op->slaying && !op->inv && !arch_init) {
        object *tmp;

        tmp = create_archetype_by_object_name(op->slaying);
        insert_ob_in_ob(tmp, op);
        op->randomitems = NULL;	/* So another spell isn't created for this object */
        /* without this, value is all screwed up */
        op->value = op->arch->clone.value * op->inv->value;
    }

    if (QUERY_FLAG(op, FLAG_MONSTER)) {
        if (op->stats.hp > op->stats.maxhp)
            LOG(llevDebug,"Monster %s has hp set higher than maxhp (%d>%d)\n",
            op->name,
            op->stats.hp, op->stats.maxhp);
        }
    if ((QUERY_FLAG(op,FLAG_GENERATOR) && QUERY_FLAG(op,FLAG_CONTENT_ON_GEN))
        || op->type == CREATOR
        || op->type == CONVERTER) {
        /* Object will duplicate it's content as part of the
         * generation process. To do this, we must flag inventory
         * so it remains unevaluated concerning the randomitems and
         * the living (a demonlord shouldn't cast from inside generator!)
         */
        flag_inv(op,FLAG_IS_A_TEMPLATE);
    }

    /* Here we'll handle custom monsters. In order to handle them correctly, especially in the fix_object
     * method, we'll create a new temporary archetype containing defined values.
     * Of course this doesn't apply when loading archetypes or artifacts.
     */
    if (arch_init == 0 && artifact_init == 0 && QUERY_FLAG(op, FLAG_MONSTER) && op->arch && !can_merge(op, &op->arch->clone)) {
        archetype* temp = get_archetype_struct();
        temp->reference_count++;
        temp->name = add_string(op->arch->name);
        temp->tail_x = op->arch->tail_x;
        temp->tail_y = op->arch->tail_y;
        copy_object(op, &temp->clone);
        temp->clone.inv = NULL;
        temp->clone.env = NULL;
        temp->clone.x = 0;
        temp->clone.y = 0;
        temp->clone.map = NULL;
        if (FABS(temp->clone.speed)>MIN_ACTIVE_SPEED) {
            /* Clone has a speed, so need to clear that because it isn't on a map.
             * But we need to keep the value, because otherwise the customized object
             * will have no speed (fix_player() will use the 0 value).  So set it
             * to zero, call update_ob_speed() to remove it from active list, then
             * set its speed back to the original.
             */
            temp->clone.speed = 0;
            update_ob_speed(&temp->clone);
            temp->clone.speed = op->speed;
        }

        temp->more = op->arch->more;
        op->arch = temp;
        /* LOG(llevDebug, "created temporary archetype for %s at %d,%d\n", op->name, op->x, op->y); */
    }
}

/**
 * This extracts the key/value from the yytext field -
 * calls set_ob_key_value() to actually set the value.
 * Function basically has to find spaces, strip out extra,
 * etc.  strchr doesn't work as good because could also
 * be tabs.
 *
 * @param op
 * object in which to insert the value.
 */
static void add_key_value(object * op) {
    char * key = NULL;
    char * value = NULL;
    char * cp;
    char * end;

    /* First, skip over leading whitespace. */
    for (cp = yytext; isspace(*cp); cp++) { ; }

    key = cp;

    /* Now look for the end of the key/field name. */
    for (; !isspace(*cp); cp++) {
        if (*cp == '\0') {
            /* Oops, ran out of string! Set the key with an empty value. */
            set_ob_key_value(op, key, NULL, TRUE);
            return;
        }
    }

    if (*cp == '\0') {
        set_ob_key_value(op, key, NULL, TRUE);
        return;
    }

    /* Chop off the key, and start at the next character. */
    *cp = '\0';
    cp++;
    if (*cp == '\0') {
        /* Was followed by one space? */
        set_ob_key_value(op, key, NULL, TRUE);
        return;
    }

    /* Now looking for the value. Skip over whitespace. */
    for (; isspace(*cp); cp++) {
        if (*cp == '\0') {
            /* Guess not. */
            set_ob_key_value(op, key, NULL, TRUE);
            return;
        }
    }

    value = cp;

    /* Got last character before null and strip
     * off tailing whitespace
     */
    for (end = value + (strlen(cp)-1); isspace(*end); end--) {
        if (end == value) {
            /* *blink blink* Still no value? */
            set_ob_key_value(op, key, NULL, TRUE);
            return;
        }
        *end='\0';
    }
    set_ob_key_value(op, key, value, TRUE);
}

/**
 * Handles the different move types an object can have.
 *
 * @param mt
 * field for which we are computing the value.
 * @param params
 * string containing the definition of the move type.
 */
static void set_move(MoveType *mt, char *params) {
    char *str;
    int i, negate;

    if (isdigit(*params)) {
        *mt = atoi(params);
    } else {
        *mt=0;
        for (str=strtok(params, " "); str; str=strtok(NULL, " ")) {
            negate=0;
            if (!strcasecmp(str, "all"))
                *mt |= MOVE_ALL;
            else {
                if (*str=='-') {
                    negate = 1;
                    str++;
                }
                for (i=0; move_name[i] != NULL; i++)  {
                    if (!strcasecmp(move_name[i], str)) {
                        if (negate) {
                            *mt &= ~(1<<i);
                        } else {
                            *mt |= (1<<i);
                        }
                        break;
                    }
                }
                if (move_name[i] == NULL) {
                    /* fly is a special case - covers both fly_low and
                    * fly_high - since it doesn't match to a specific
                    * single bit, have to special case it.
                    */
                    if (!strcasecmp(str,"flying")) {
                        if (negate) {
                            *mt &= ~MOVE_FLYING;
                        } else {
                            *mt |= MOVE_FLYING;
                        }
                    } else {
                        LOG(llevDebug, "common/loader.l: set_move - unknown move string '%s'\n", str);
                    }
                }
            } /* Else not all move types */
        } /* for strtok */
    } /* Else not a numeric value */
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

    object* op = items[*depth];
    lex_error=0;

%}

^msg{WS}$	    {	BEGIN( MESSAGE ); msgbuf[0]='\0'; msglen=0; total_msglen=0;}
<MESSAGE>^endmsg{WS}$ {	BEGIN( INITIAL );
			op->msg=add_string(msgbuf);
			/* Just print a warning so we can be reasonably safe
			 * about not overflowing the buffer.
			 */
			if (total_msglen > (HUGE_BUF/2))
			    LOG(llevDebug, "\n\tWarning message length > %d (max allowed=%d): %d\n>%.80s<\n",
				HUGE_BUF/2, HUGE_BUF, total_msglen,op->msg);
		    }
<MESSAGE>.*	    {	safe_strcat(msgbuf, yytext, &msglen, HUGE_BUF);
			safe_strcat(msgbuf, "\n", &msglen, HUGE_BUF);
			total_msglen += strlen(yytext) + 1;
		    }

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
^skill{S}	    FREE_AND_COPY(op->skill,yval());
^custom_name{S}	    {	char *yv=yval();

			if (*yv=='\0') LOG(llevError,"Custom name without val\n");
			else FREE_AND_COPY(op->custom_name, yv);
		    }
^race{S}	    FREE_AND_COPY(op->race,yval());
^slaying{S}	    FREE_AND_COPY(op->slaying, yval());
^arch{S}         {	/* If op->arch has been set, then this new object
			 * must be part of the inventory.  So process
			 * appropriately.
			 */
			if (op->arch) {
                /* The logic is a follow:
                 - in linemode, we are loading artifacts, so just increase depth, next calls with work
                  on object in inventory, and linking done in the "end" part
                 - when not in linemode, we are loading an archetype, or a map
                 - if archetype loading, discard the object, it'll be handled by second_arch_pass()
                 - else put object like usual
                */
			    object *tmp;
			    char *yv=yval();

			    tmp=get_object();
			    tmp->arch = find_archetype(yv);
			    if (tmp->arch!=NULL)
				copy_object(&tmp->arch->clone,tmp);
			    else {
				if (tmp->name) free_string(tmp->name);
				/* record the name of the broken object */
				tmp->name = add_string(yv);
			    }
			    strcpy(msgbuf, "");
			    strcpy(lorebuf, "");
			    msglen=0;
			    total_msglen=0;
                (*depth)++;
                items[*depth] = tmp;
                if (linemode == 0) {
                    lex_load(depth, items, maxdepth, map_flags, linemode);
                    (*depth)--;
                    if (tmp->arch && (arch_init == 0)) {
                        insert_ob_in_ob(tmp,op);
                    }
                    else {
                        if (init_archetypes == 0)
                            LOG(llevDebug,"Discarding object without arch: %s\n", tmp->name?tmp->name:"(null)");
                        free_object(tmp);
                    }
                }
			}
			/* This is the actual archetype definition then */
			else {
			    char *yv=yval();

			    op->arch=find_archetype(yv);
			    if (op->arch!=NULL) copy_object(&op->arch->clone,op);
			    else if (!arch_init) {
				if (op->name) free_string(op->name);
				/* record the name of the broken object */
				op->name = add_string(yv);
			    }
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
            if (linemode) {
                /* Linemode is only set for artifact loading. */
                if ((*depth) > 0) {
                    insert_ob_in_ob(items[*depth], items[(*depth)-1]);
                }
                return LL_NORMAL;
            }
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
^slow_move{S}	    {	op->move_slow |= MOVE_WALK;
			op->move_slow_penalty = FVAL;
		    }
^title{S}	    {	char *y=yval();
			if (*y=='\0') LOG(llevError,"Title without value.\n");
			else FREE_AND_COPY(op->title, y);
		    }

^face{S}	op->face = &new_faces[find_face(yval(), 0)];
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
^perm_exp{S}	op->perm_exp = atoll(yval());
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
^is_animated{S}	    SET_OR_CLEAR_FLAG(op, FLAG_ANIMATE, IVAL);
^no_pick{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_PICK, IVAL);
^client_anim_sync{S}	    SET_OR_CLEAR_FLAG(op, FLAG_CLIENT_ANIM_SYNC, IVAL);
^client_anim_random{S}	    SET_OR_CLEAR_FLAG(op, FLAG_CLIENT_ANIM_RANDOM, IVAL);

%{ /* These are all legacy - any new objects should use the move_ .. values */
%}
^walk_on{S}	    { if (IVAL) op->move_on |= MOVE_WALK; else op->move_on &= ~MOVE_WALK; }
^walk_off{S}	    { if (IVAL) op->move_off |= MOVE_WALK; else op->move_off &= ~MOVE_WALK; }
^fly_on{S}	    { if (IVAL) op->move_on |= MOVE_FLY_LOW; else op->move_on &= ~MOVE_FLY_LOW; }
^fly_off{S}	    { if (IVAL) op->move_off |= MOVE_FLY_LOW; else op->move_off &= ~MOVE_FLY_LOW; }

%{ /* These are the new values */
%}
^move_block{S}	    set_move(&op->move_block, yval());
^move_allow{S}	    set_move(&op->move_allow, yval());
^move_type{S}	    set_move(&op->move_type, yval());
^move_on{S}	    set_move(&op->move_on, yval());
^move_off{S}	    set_move(&op->move_off, yval());
^move_slow{S}	    set_move(&op->move_slow, yval());
^move_slow_penalty{S}	    op->move_slow_penalty = FVAL;


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
^editor_folder{S}   { /* This field is used by Gridarta, so it's not an error. */ }
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
			    SET_FLAG(op, FLAG_WIZCAST);
			}
			else {
			    CLEAR_FLAG(op, FLAG_WIZ);
			    CLEAR_FLAG(op, FLAG_WIZPASS);
			    CLEAR_FLAG(op, FLAG_WIZCAST);
			}
		    }
^was_wiz{S}  	    SET_OR_CLEAR_FLAG(op, FLAG_WAS_WIZ, IVAL);
^no_fix_player{S}   SET_OR_CLEAR_FLAG(op, FLAG_NO_FIX_PLAYER, IVAL);
^is_lightable{S}    SET_OR_CLEAR_FLAG(op, FLAG_IS_LIGHTABLE, IVAL);
^tear_down{S}	    SET_OR_CLEAR_FLAG(op, FLAG_TEAR_DOWN, IVAL);
^luck{S}  	    op->stats.luck = IVAL;
^run_away{S}	    op->run_away = IVAL;
^pick_up{S}	    op->pick_up = IVAL;
^item_power{S}	    op->item_power = IVAL;
^gen_sp_armour{S}   op->gen_sp_armour = IVAL;
^anim_speed{S}	    op->anim_speed = IVAL;
^container{S}	    op->weight_limit = IVAL;
^no_drop{S}	    SET_OR_CLEAR_FLAG(op, FLAG_NO_DROP, IVAL);
^will_apply{S}	    op->will_apply = IVAL;
^random_movement{S}	    SET_OR_CLEAR_FLAG(op, FLAG_RANDOM_MOVE, IVAL);
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
^sleep{S}	    SET_OR_CLEAR_FLAG(op, FLAG_SLEEP, IVAL);
^stand_still{S}	    SET_OR_CLEAR_FLAG(op, FLAG_STAND_STILL, IVAL);
^random_move{S}	    SET_OR_CLEAR_FLAG(op, FLAG_RANDOM_MOVE, IVAL);
^only_attack{S}	    SET_OR_CLEAR_FLAG(op, FLAG_ONLY_ATTACK, IVAL);

^activate_on_push{S}	SET_OR_CLEAR_FLAG(op, FLAG_ACTIVATE_ON_PUSH, IVAL);
^activate_on_release{S}	SET_OR_CLEAR_FLAG(op, FLAG_ACTIVATE_ON_RELEASE, IVAL);

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
^resist_life_stealing{S}	SET_RESIST(op, ATNR_LIFE_STEALING, IVAL);
^resist_disease{S}	SET_RESIST(op, ATNR_DISEASE, IVAL);
^attack_movement{S} op->attack_movement = IVAL;
^move_state{S}	    op->move_status = IVAL;
^confused{S}	    SET_OR_CLEAR_FLAG(op, FLAG_CONFUSED, IVAL);
^stealth{S}	    SET_OR_CLEAR_FLAG(op, FLAG_STEALTH, IVAL);
^connected{S}	    add_button_link(op, op->map, IVAL);
^cursed{S}	    SET_OR_CLEAR_FLAG(op, FLAG_CURSED, IVAL);
^damned{S}	    SET_OR_CLEAR_FLAG(op, FLAG_DAMNED, IVAL);
^blessed{S}         SET_OR_CLEAR_FLAG(op, FLAG_BLESSED, IVAL);
^see_anywhere{S}    SET_OR_CLEAR_FLAG(op, FLAG_SEE_ANYWHERE, IVAL);
^known_magical{S}   SET_OR_CLEAR_FLAG(op, FLAG_KNOWN_MAGICAL, IVAL);
^known_cursed{S}    SET_OR_CLEAR_FLAG(op, FLAG_KNOWN_CURSED, IVAL);
^known_blessed{S}   SET_OR_CLEAR_FLAG(op, FLAG_KNOWN_BLESSED, IVAL);
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
^no_steal{S}		SET_OR_CLEAR_FLAG(op, FLAG_NO_STEAL, IVAL);
^one_hit{S}		SET_OR_CLEAR_FLAG(op, FLAG_ONE_HIT, IVAL);
^berserk{S}		SET_OR_CLEAR_FLAG(op, FLAG_BERSERK, IVAL);

^can_knockback{S}      { /* Some archetypes have these values in them */ }
^can_parry{S}          { /* Probably the pupland archetypes - I imagined */ }
^can_impale{S}         { /* That these are for the new combat code */ }
^can_cut{S}            { /* just ignore for now */ }
^can_dam_armour{S}     { }
^weapontype{S}		op->weapontype = IVAL;
^tooltype{S}            op->tooltype = IVAL;
^casting_time{S}	op->casting_time = FVAL;
^elevation{S}		op->elevation = IVAL;
^smoothlevel{S}		op->smoothlevel = IVAL;
^map_layer{S}		{
			    if (IVAL) op->map_layer = IVAL;
			    else {
				int i;
				char *cp = yval();

				for (i=0; i < MAP_LAYERS; i++) {
				    if (!strcasecmp(cp, map_layer_name[i])) {
					op->map_layer = i;
					break;
				    }
				}
				if (i == MAP_LAYERS) {
				    LOG(llevError,"Invalid map_layer name found: %s\n", cp);
				}
			    }
			}
^client_type{S}		op->client_type = IVAL;
^body_{A}		set_body_info(op, yytext);
^duration{S}		op->duration = IVAL;
^range{S}		op->range = IVAL;
^range_modifier{S}	op->range_modifier = IVAL;
^dam_modifier{S}	op->dam_modifier = IVAL;
^duration_modifier{S}	op->duration_modifier = IVAL;
^is_buildable{S}	SET_OR_CLEAR_FLAG( op, FLAG_IS_BUILDABLE, IVAL );

  /* Discrete damage */
^damage_physical{S}	set_damage(op, ATNR_PHYSICAL, IVAL);
^damage_magic{S}	set_damage(op, ATNR_MAGIC, IVAL);
^damage_fire{S}		set_damage(op, ATNR_FIRE, IVAL);
^damage_electricity{S}	set_damage(op, ATNR_ELECTRICITY, IVAL);
^damage_cold{S}		set_damage(op, ATNR_COLD, IVAL);
^damage_confusion{S}	set_damage(op, ATNR_CONFUSION, IVAL);
^damage_acid{S}		set_damage(op, ATNR_ACID, IVAL);
^damage_drain{S}	set_damage(op, ATNR_DRAIN, IVAL);
^damage_weaponmagic{S}	set_damage(op, ATNR_WEAPONMAGIC, IVAL);
^damage_ghosthit{S}	set_damage(op, ATNR_GHOSTHIT, IVAL);
^damage_poison{S}	set_damage(op, ATNR_POISON, IVAL);
^damage_slow{S}		set_damage(op, ATNR_SLOW, IVAL);
^damage_paralyze{S}	set_damage(op, ATNR_PARALYZE, IVAL);
^damage_turn_undead{S}	set_damage(op, ATNR_TURN_UNDEAD, IVAL);
^damage_fear{S}		set_damage(op, ATNR_FEAR, IVAL);
^damage_cancellation{S}	set_damage(op, ATNR_CANCELLATION, IVAL);
^damage_deplete{S}	set_damage(op, ATNR_DEPLETE, IVAL);
^damage_death{S}	set_damage(op, ATNR_DEATH, IVAL);
^damage_chaos{S}	set_damage(op, ATNR_CHAOS, IVAL);
^damage_counterspell{S}	set_damage(op, ATNR_COUNTERSPELL, IVAL);
^damage_godpower{S}	set_damage(op, ATNR_GODPOWER, IVAL);
^damage_holyword{S}	set_damage(op, ATNR_HOLYWORD, IVAL);
^damage_blind{S}	set_damage(op, ATNR_BLIND, IVAL);
^damage_internal{S}	set_damage(op, ATNR_INTERNAL, IVAL);
^damage_life_stealing{S}	set_damage(op, ATNR_LIFE_STEALING, IVAL);
^damage_disease{S}	set_damage(op, ATNR_DISEASE, IVAL);


<*>(^{WS}$)|\n		{/* ignore empty lines, newlines we don't do above */}
#.*\n			{}

<<EOF>>			{/* If we got an error, return the error.  Otherwise, return that we got EOF */
			    if (lex_error!=0) return lex_error; else return LL_EOF;}
.*			{ add_key_value(op); }
%%

/**
 * Error handler.
 *
 * @param s
 * lex-generated error message.
 */
int yyerror(char *s)
{
    LOG(llevError, "%s: %s\n", s, yytext);
    return -1;
}


/**
 * Returns the next token for lex.
 * Our save file syntax is very simple, so we can use a very simple
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


/**
 * Loads an object from the given file-pointer.
 * Variables will be read and parsed and patched into the object
 * until the string "end" is reached, or the end of the file.
 *
 * @param fp
 * file pointer from which to read.
 * @param op
 * where to read the data.
 * @param bufstate
 * used to determine various file attributes:
 * @li LO_REPATE (0): We are reading from the same buffer as the last call.
 * @li LO_LINEMODE (1): file that is being read from is multi purpose (ie, other functions
 *  will also be reading from this (treasure file, artifacts.)
 * @li LO_NEWFILE (2): This is the first read from a particular file, so the buffers should be reset.
 * @li LO_NOREAD (3): Reset the buffers, but don't read from it. (op can be null)
 * @param map_flags
 * ?
 * @return
 * @li LL_NORMAL if object was read.
 * @li LL_EOF if an error occured.
 * @li LL_MORE if this is a multipart object.
 *
 * @todo
 * document map_flags
 */
int load_object(FILE *fp, object *op, int bufstate, int map_flags) {
    int retval;
    char inbuf[MAX_BUF];
#define MAXDEPTH 10
    object* items[MAXDEPTH];
    int depth = 0;

    items[0] = op,
    strcpy(msgbuf, "");
    msglen=0;
    total_msglen=0;
    strcpy(lorebuf, "");
    if (bufstate==LO_NEWFILE || bufstate==LO_NOREAD) {
    /*	LOG(llevDebug,"Switching lex buffers\n");*/
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE));
        if (bufstate==LO_NOREAD) return LL_NORMAL;
    }
    if (bufstate==LO_LINEMODE) {
        YY_BUFFER_STATE  yybufstate;
        yy_delete_buffer(YY_CURRENT_BUFFER);
        while (fgets(inbuf, MAX_BUF-3, fp)) {
            yybufstate=yy_scan_string(inbuf);
            retval=lex_load(&depth, items, MAXDEPTH, map_flags, 1);
            yy_delete_buffer(yybufstate);
            if (retval==LL_NORMAL) {
                if (depth == 0) return retval;
                else depth--;
            }
        }
        LOG(llevDebug,"Got eof while scanning strings\n");
        return LL_EOF;
    }

    retval=lex_load(&depth, items, MAXDEPTH, map_flags, 0);

/*    LOG(llevDebug," load completed, object=%s\n",op->name);*/
    return retval;
}


/**
 * This takes a buffer, scans it for variables, and sets those variables
 * as appropriate in op.
 *
 * This function appears to be used in only 2 places - in crossedit to
 * override values and in c_wiz to mutate values.
 *
 * @param op
 * object to alter.
 * @param buf
 * modifications to apply.
 * @return
 * ?
 * @todo
 * document return value.
 */
int set_variable(object *op,char *buf) {
    YY_BUFFER_STATE  yybufstate,yycurbuf=YY_CURRENT_BUFFER;
    int retval, depth;
    object* items[MAXDEPTH];

    strcpy(msgbuf, "");
    msglen=0;
    total_msglen=0;
    strcpy(lorebuf, "");
    items[0] = op;
    depth = 0;
    yy_push_state(INITIAL);
    yybufstate=yy_scan_string(buf);
    retval=lex_load(&depth, items, MAXDEPTH,0, 0);
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
/**
 * This is a list of pointers that correspond to the FLAG_.. values.
 * This is a simple 1:1 mapping - if FLAG_FRIENDLY is 15, then
 * the 15'th element of this array should match that name.
 * If an entry is NULL, that is a flag not to loaded/saved.
 */
static const char *const flag_names[NUM_FLAGS+1] = {
    "alive", "wiz", NULL, NULL, "was_wiz", "applied", "unpaid",
    "can_use_shield", "no_pick", "client_anim_sync", "client_anim_random", /* 10 */
    "is_animated", NULL /* slow_move */,
    NULL /* flying */, "monster", "friendly", "generator",
    "is_thrown", "auto_apply", "treasure", "player sold",   /* 20 */
    "see_invisible", "can_roll", "overlay_floor",
    "is_turnable", NULL /* walk_off */, NULL /* fly_on */,
    NULL /*fly_off*/, "is_used_up", "identified", "reflecting",	/* 30 */
    "changing", "splitting", "hitback", "startequip",
    "blocksview", "undead", "scared", "unaggressive",
    "reflect_missile", "reflect_spell",                             /* 40 */
    "no_magic", "no_fix_player", "is_lightable", "tear_down",
    "run_away", NULL /*pass_thru */, NULL /*can_pass_thru*/,
    NULL /*"pick_up"*/, "unique", "no_drop",                        /* 50 */
    NULL /* wizcast*/, "can_cast_spell", "can_use_scroll", "can_use_range",
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
    "activate_on_release", "is_water", "use_content_on_gen", NULL, "is_buildable",   /* 110 */
    NULL, "blessed", "known_blessed"
};


/**
 * Initialises the array of variable-names.  Needed before any
 * objects can be loaded.  Called by init_library().
 * @todo
 * remove as it is empty.
 */

void init_vars() {
}

/**
 * Frees all memory allocated by the loader.
 */
void free_loader() {
    yy_delete_buffer(YY_CURRENT_BUFFER);

    /* Comment out this yylex_destroy(), as it results in link errors
     * on linux
     */
/*    yylex_destroy();*/
}

/** For get_ob_diff speed reason*/
typedef struct {
    const char *name;
    int   length;
} genericname;

/**
 * This returns a string of the integer movement type
 *
 * @param mt
 * move to describe.
 * @param buf
 * buffer that will contain the description. Must not be NULL.
 * @param size
 * buffer's length.
 */
static void get_string_move_type(MoveType mt, char* buf, int size)
{
    static char	retbuf[MAX_BUF], retbuf_all[MAX_BUF];
    int i, all_count=0, count;

    strcpy(retbuf,"");
    strcpy(retbuf_all," all");

    /* Quick check, and probably fairly common */
    if (mt == MOVE_ALL) {
        strncpy(buf, "all", size);
        return;
    }
    if (mt == 0) {
        strncpy(buf,"0", size);
        return;
    }

    /* We basically slide the bits down.  Why look at MOVE_ALL?
     * because we may want to return a string like 'all -swim',
     * and if we just looked at mt, we couldn't get that.
     */
    for (i=MOVE_ALL, count=0; i!=0; i >>= 1, count++) {
        if (mt & (1<<count)) {
            strcat(retbuf, " ");
            strcat(retbuf, move_name[count]);
        } else {
            strcat(retbuf_all, " -");
            strcat(retbuf_all, move_name[count]);
            all_count++;
        }
    }
    /* Basically, if there is a single negation, return it, eg
     * 'all -swim'.  But more than that, just return the
     * enumerated values.  It doesn't make sense to return
     * 'all -walk -fly_low' - it is shorter to return 'fly_high swim'
     */
    if (all_count <= 1)
        strncpy(buf, retbuf_all+1, size);
    else
        strncpy(buf, retbuf+1, size);
}


/*static int eol_size=strlen("\n");*/
/** Utility variable, to speed computation. */
static int eol_size=sizeof("\n")-1; /*Substract 1 to suppress terminal 0*/

/** Adds a line to the buffer. */
#define ADD_STRINGLINE_ENTRY(buf__,entryname__,entryvalue__,entrysize__){\
    FAST_STRNCAT(buf__,entryname__,entrysize__);\
    FAST_STRCAT(buf__,entryvalue__);\
    FAST_STRNCAT(buf__,"\n",eol_size);}
/** Adds a long to the buffer. */
#define FAST_SAVE_LONG(buf__,entryname__,entryvalue__,entrysize__) \
    ADD_STRINGLINE_ENTRY(buf__,entryname__,ltostr10(entryvalue__),entrysize__)
/** Adds a double to the buffer. */
#define FAST_SAVE_DOUBLE(buf__,entryname__,entryvalue__,entrysize__) \
    ADD_STRINGLINE_ENTRY(buf__,entryname__,doubletostr10(entryvalue__),entrysize__)

/**
 * Returns a pointer to a static string which contains all variables
 * which are different in the two given objects.
 *
 * This function is typically used to dump objects (op2=empty object), or to save objects
 *
 * @param op
 * what object the different values will be taken from.
 * @param op2
 * object's original archetype.
 * @param buf
 * buffer that will contain the difference.
 * @param size
 * buf's length.
 *
 * @note
 * by MSW 2003-09-22:  It's a bug that we need to pass entrysize -
 * we should use strlen entryname instead.  Any smart compiler should
 * optimize that out.
 *
 * @todo
 * use string safe functions everywhere
 */
void get_ob_diff(object *op,object *op2, char* buf, int size) {

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
    static int already_run = 0;
    static int flag_lens[NUM_FLAGS+1];
    static char buf2[HUGE_BUF];
    char* fastbuf;
    int tmp;
    int i;

    key_value * my_field;
    key_value * arch_field;

    /*first init this function if first call*/
    if (!already_run){
        for (i=0;i<=NUM_FLAGS;i++) {
            flag_lens[i]=flag_names[i]?strlen(flag_names[i]):0; /* some flags may be null? :/*/
        }
        already_run=1;
    }
    buf[0]='\0';
    fastbuf=PREPARE_FASTCAT(buf);

    /* This saves the key/value lists.  We do it first so that any
    * keys that match field names will be overwritten by the loader.
    */
    for (my_field = op->key_values; my_field != NULL; my_field = my_field->next) {
        /* Find the field in the opposing member. */
        arch_field = get_ob_key_link(op2, my_field->key);

        /* If there's no partnering field, or it's got a different value, save our field. */
        if (arch_field == NULL || my_field->value != arch_field->value) {
            FAST_STRCAT(fastbuf, my_field->key);
            FAST_STRNCAT(fastbuf, " ", 1);
            /* If this is null, then saving it as a space should
            * cause it to be null again.
            */
            if (my_field->value) FAST_STRCAT(fastbuf, my_field->value);
                FAST_STRNCAT(fastbuf, "\n", eol_size);
        }
    }
    /* We don't need to worry about the arch's extra fields - they
     * will get taken care of the copy_object function.
     */

    if(op->name && op->name!=op2->name) {
        ADD_STRINGLINE_ENTRY(fastbuf,"name ",op->name,5);
    }
    if(op->name_pl && op->name_pl!=op2->name_pl) {
        ADD_STRINGLINE_ENTRY(fastbuf,"name_pl ",op->name_pl,8);
    }
    if(op->custom_name && op->custom_name!=op2->custom_name) {
        ADD_STRINGLINE_ENTRY(fastbuf,"custom_name ",op->custom_name,12);
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
    if(op->skill && op->skill!=op2->skill) {
        ADD_STRINGLINE_ENTRY(fastbuf,"skill ",op->skill,6);
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

    if(op->stats.exp!=op2->stats.exp) {
        sprintf(buf2,"%" FMT64 , op->stats.exp);
        ADD_STRINGLINE_ENTRY(fastbuf,"exp ", buf2, 4);
    }

    if(op->perm_exp!=op2->perm_exp) {
        sprintf(buf2,"%" FMT64 , op->perm_exp);
        ADD_STRINGLINE_ENTRY(fastbuf,"perm_exp ", buf2, 9);
    }

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
    if(op->attack_movement != op2->attack_movement)
        FAST_SAVE_LONG(fastbuf,"attack_movement ",op->attack_movement,16);
    if(op->nrof!=op2->nrof)
        FAST_SAVE_LONG(fastbuf,"nrof ",op->nrof,5);
    if(op->level!=op2->level)
        FAST_SAVE_LONG(fastbuf,"level ",op->level,6);
    if(op->direction!=op2->direction)
        FAST_SAVE_LONG(fastbuf,"direction ",op->direction,10);
    if(op->type!=op2->type)
        FAST_SAVE_LONG(fastbuf,"type ",op->type,5);
    if(op->subtype!=op2->subtype)
        FAST_SAVE_LONG(fastbuf,"subtype ",op->subtype,8);
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

    if(op->map_layer!=op2->map_layer)
        ADD_STRINGLINE_ENTRY(fastbuf,"map_layer ",map_layer_name[op->map_layer],10);

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

    if (op->duration != op2->duration)
        FAST_SAVE_LONG(fastbuf,"duration ",op->duration,9);

    if (op->range != op2->range)
        FAST_SAVE_LONG(fastbuf,"range ",op->range,6);

    if (op->range_modifier != op2->range_modifier)
        FAST_SAVE_LONG(fastbuf,"range_modifier ",op->range_modifier,15);

    if (op->duration_modifier != op2->duration_modifier)
        FAST_SAVE_LONG(fastbuf,"duration_modifier ", op->duration_modifier,18);

    if (op->dam_modifier != op2->dam_modifier)
        FAST_SAVE_LONG(fastbuf,"dam_modifier ", op->dam_modifier,13);

    if (op->gen_sp_armour != op2->gen_sp_armour) {
        FAST_SAVE_LONG(fastbuf,"gen_sp_armour ",op->gen_sp_armour,14);
    }

    /* I've kept the old int move type saving code commented out.
     * In an ideal world, we'd know if we want to do a quick
     * save (say to a temp map, where we don't care about strings),
     * or a slower save/dm dump, where printing out strings is handy.
     */
    if (op->move_type != op2->move_type) {
        /*FAST_SAVE_LONG(fastbuf,"move_type ",op->move_type,10)*/
        get_string_move_type(op->move_type, buf2, HUGE_BUF);
        ADD_STRINGLINE_ENTRY(fastbuf,"move_type ",
            buf2,
            10);
    }
    if (op->move_block != op2->move_block) {
        /*FAST_SAVE_LONG(fastbuf,"move_block ",op->move_block,11)*/
        get_string_move_type(op->move_block, buf2, HUGE_BUF);
        ADD_STRINGLINE_ENTRY(fastbuf,"move_block ",
            buf2,
            11);
    }
    if (op->move_allow != op2->move_allow) {
        /*FAST_SAVE_LONG(fastbuf,"move_allow ",op->move_allow,11);*/
        get_string_move_type(op->move_allow, buf2, HUGE_BUF);
        ADD_STRINGLINE_ENTRY(fastbuf,"move_allow ",
            buf2,
            11);
    }
    if (op->move_on != op2->move_on) {
        /*FAST_SAVE_LONG(fastbuf,"move_on ",op->move_on,8);*/
        get_string_move_type(op->move_on, buf2, HUGE_BUF);
        ADD_STRINGLINE_ENTRY(fastbuf,"move_on ",
            buf2,
            8);
    }
    if (op->move_off != op2->move_off) {
        /*FAST_SAVE_LONG(fastbuf,"move_off ",op->move_off,9);*/
        get_string_move_type(op->move_off, buf2, HUGE_BUF);
        ADD_STRINGLINE_ENTRY(fastbuf,"move_off ",
            buf2,
            9);
    }
    if (op->move_slow != op2->move_slow) {
        /*FAST_SAVE_LONG(fastbuf,"move_slow ",op->move_slow,10);*/
        get_string_move_type(op->move_slow, buf2, HUGE_BUF);
        ADD_STRINGLINE_ENTRY(fastbuf,"move_slow ",
            buf2,
            10);
    }

    if (op->move_slow_penalty != op2->move_slow_penalty) {
        FAST_SAVE_LONG(fastbuf,"move_slow_penalty ",op->move_slow_penalty,18);
    }

    if (!COMPARE_FLAGS(op,op2)) {
        for (tmp=0; tmp <= NUM_FLAGS; tmp++) {
            if (flag_names[tmp] && (QUERY_FLAG(op, tmp) != QUERY_FLAG(op2, tmp))) {
                ADD_STRINGLINE_ENTRY(fastbuf,flag_names[tmp],QUERY_FLAG(op, tmp)?" 1":" 0",flag_lens[tmp]);
            }
        }
    }

    /* Save body locations */
    for (i=0; i<NUM_BODY_LOCATIONS; i++) {
        if (op->body_info[i] != op2->body_info[i]) {
            FAST_STRCAT(fastbuf,body_locations[i].save_name);
            FAST_SAVE_LONG(fastbuf," ",op->body_info[i],1);
        }
    }

    /* Save discrete damage if applicable
     * Note that given how the discrete_damage allocation is done, there can't be any case where
     * op->discrete_damage == NULL && op2->discrete_damage != NULL.
     */
    if (op->discrete_damage) {
        for (i=0; i<NROFATTACKS; i++) {
            if (op2->discrete_damage == NULL || op2->discrete_damage[i] != op->discrete_damage[i]) {
                FAST_STRNCAT(fastbuf,"damage_",7);
                FAST_SAVE_LONG(fastbuf,resist_save[i],op->discrete_damage[i],strlen(resist_save[i]));
            }
        }
    }
    FINISH_FASTCAT(fastbuf);
}

/**
 * Dumps all variables in an object to a file.
 *
 * @param fp
 * file to write to.
 * @param op
 * object to save.
 * @param flag
 * Combination of @ref SAVE_FLAG_xxx flags.
 */
void save_object(FILE *fp,object *op, int flag) {
    archetype *at;
    char cp[HUGE_BUF];
    object *tmp,*old;

    /* Even if the object does have an owner, it would seem that we should
     * still save it.
     */
    if(op->owner!=NULL || fp == NULL)
        return;

    /* If it is unpaid and we don't want to save those, just return. */
    if(!(flag & SAVE_FLAG_SAVE_UNPAID) && (QUERY_FLAG(op, FLAG_UNPAID))) {
        return;
    }

    /* If the object has no_save set, just return */
    if(op->no_save)
        return;

    if((at=op->arch)==NULL) at=empty_archetype;
    fprintf(fp,"arch %s\n",at->name);

    if (op->arch->reference_count > 0) {
        /* The object is a custom item/monster, so we handle its save differently.
         * We compare the custom archetype to the "original" one, then only save hp/gr/sp
         * which are the only values we can't recompute later - all others are modified by items in inventory.
         * Note that hp/gr/sp will appear twice in save, but last value will take precedence.
         */
        archetype* original = find_archetype(op->arch->name);
        if (!original) {
            LOG(llevError, "could not find original archetype %s for custom monster!", op->arch->name);
            abort();
        }
        get_ob_diff(&op->arch->clone, &original->clone, cp, HUGE_BUF);
        if(op->stats.hp != op->arch->clone.stats.hp)
            snprintf(cp + strlen(cp), HUGE_BUF - strlen(cp) - 1, "hp %d\n", op->stats.hp);
        if(op->stats.sp != op->arch->clone.stats.sp)
            snprintf(cp + strlen(cp), HUGE_BUF - strlen(cp) - 1, "sp %d\n", op->stats.sp);
        if(op->stats.grace != op->arch->clone.stats.grace)
            snprintf(cp + strlen(cp), HUGE_BUF - strlen(cp) - 1, "grace %d\n", op->stats.grace);
        if(op->x != op->arch->clone.x)
            snprintf(cp + strlen(cp), HUGE_BUF - strlen(cp) - 1, "x %d\n", op->x);
        if(op->y != op->arch->clone.y)
            snprintf(cp + strlen(cp), HUGE_BUF - strlen(cp) - 1, "y %d\n", op->y);
    }
    else
        get_ob_diff(op,&at->clone, cp, HUGE_BUF);

    if (cp!=NULL)
        fputs(cp,fp);	/* We really should do some status checking on this */

    /* Eneq(@csd.uu.se): Added this to allow containers being saved with contents*/

    old=NULL;

    if (flag & SAVE_FLAG_NO_REMOVE)
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

    if (!(flag & SAVE_FLAG_NO_REMOVE)) {
        remove_ob(op);
        free_object (op);
    }

    fprintf(fp,"end\n");
}
