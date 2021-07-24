%{
/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "global.h"

#include <ctype.h>

#include "loader.h"
#include "newserver.h"
#include "sproto.h"
#include "stringbuffer.h"

#define YY_DECL int lex_load(int *depth, object **items, int maxdepth, int map_flags, int linemode, int *ismore)

static char *yval(void);

static int lex_error;
static char msgbuf[HUGE_BUF];
size_t msglen = 0, total_msglen = 0;
static char lorebuf[HUGE_BUF];

/** Utility macro to transform a bit into a flag. */
#define SET_OR_CLEAR_FLAG(op, flag, val) \
        { if (val) SET_FLAG(op, flag); else CLEAR_FLAG(op, flag); }

#define SET_RESIST(op, type, val) op->resist[type] = val;


/** Next value as integer. */
#define IVAL atoi(yval())
/** Next value as float. */
#define FVAL atof(yval())
extern int arch_init;
extern int artifact_init;

/**
 * Keep track of whether we're currently inside an object...end block. This
 * might have been better designed as a separate start condition, but was
 * added to give a warning when an object started before the previous ended.
 */
static bool seen_object = false;

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
    for (cp = params; !isspace(*cp); cp++)
        ;

    *cp++ = 0;  /* null it out */

    for (i = 0; i < NUM_BODY_LOCATIONS; i++) {
        if (!strcmp(params, body_locations[i].save_name)) {
            op->body_info[i] = atoi(cp);
            return;     /* Only one line passed in params */
        }
    }
    LOG(llevError, "set_body_info called with bogus params: %s\n", params);
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
    if (artifact_init)
        /* Artifacts are special beasts, let's not check them. */
        return;

    if (!is_type_valid(op->type)) {
        LOG(llevError, "Invalid type %d for %s\n", op->type, op->arch ? op->arch->name : op->name);
        op->type = 0;
    }

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
        if (op->name_pl) {
            free_string(op->name_pl);
            op->name_pl = NULL;
        }
    }
    if (!op->name_pl && op->name)
        op->name_pl = add_string(op->name);

    /* objects now have a materialname.  try to patch it in */
    if (!(IS_WEAPON(op) && op->level > 0)) {
        set_materialname(op);
    }
    /* only do these when program is first run - a bit
     * excessive to do this at every run - most of this is
     * really just to catch any errors - program will still run, but
     * not in the ideal fashion.
     */
    if ((op->type == WEAPON || op->type == BOW) && arch_init) {
        if (!op->skill) {
            LOG(llevError, "Weapon %s lacks a skill.\n", op->name);
        } else if ((!strcmp(op->skill, "one handed weapons") && op->body_info[1] != -1) ||
            (!strcmp(op->skill, "two handed weapons") && op->body_info[1] != -2)) {
            LOG(llevError, "weapon %s arm usage does not match skill: %d, %s\n", op->name, op->body_info[1], op->skill);
        }
    }

    /* spellbooks & runes use slaying.  But not to arch name, but to spell name */
    if ((op->type == SPELLBOOK || op->type == RUNE) && op->slaying && !op->inv && !arch_init) {
        object *tmp;

        tmp = create_archetype_by_object_name(op->slaying);
        object_insert_in_ob(tmp, op);
        op->randomitems = NULL; /* So another spell isn't created for this object */
        /* without this, value is all screwed up */
        op->value = op->arch->clone.value*op->inv->value;
    }

    if (QUERY_FLAG(op, FLAG_MONSTER)) {
        if (op->stats.hp > op->stats.maxhp)
            LOG(llevDebug, "Monster %s has hp set higher than maxhp (%d>%d)\n", op->name, op->stats.hp, op->stats.maxhp);
        }
    if ((QUERY_FLAG(op, FLAG_GENERATOR) && QUERY_FLAG(op, FLAG_CONTENT_ON_GEN))
    || op->type == CREATOR
    || op->type == CONVERTER) {
        /* Object will duplicate its content as part of the
         * generation process. To do this, we must flag inventory
         * so it remains unevaluated concerning the randomitems and
         * the living (a demonlord shouldn't cast from inside generator!)
         */
        object_set_flag_inv(op, FLAG_IS_A_TEMPLATE);
    }

    /* if the item is identified, ensure we got the correct face */
    if (QUERY_FLAG(op, FLAG_IDENTIFIED))
        object_give_identified_properties(op);

    /* Here we'll handle custom monsters. In order to handle them correctly, especially in the fix_object
     * method, we'll create a new temporary archetype containing defined values.
     * Of course this doesn't apply when loading archetypes or artifacts.
     */
    if (arch_init == 0
    && artifact_init == 0
    && QUERY_FLAG(op, FLAG_MONSTER)
    && op->arch
    && !object_can_merge(op, &op->arch->clone)) {
        archetype *temp = get_archetype_struct();
        temp->reference_count++;
        temp->name = add_string(op->arch->name);
        temp->tail_x = op->arch->tail_x;
        temp->tail_y = op->arch->tail_y;
        object_copy(op, &temp->clone);
        temp->clone.inv = NULL;
        temp->clone.env = NULL;
        temp->clone.x = 0;
        temp->clone.y = 0;
        temp->clone.map = NULL;
        if (FABS(temp->clone.speed) > MIN_ACTIVE_SPEED) {
            /* Clone has a speed, so need to clear that because it isn't on a map.
             * But we need to keep the value, because otherwise the customized object
             * will have no speed (fix_player() will use the 0 value).  So set it
             * to zero, call object_update_speed() to remove it from active list, then
             * set its speed back to the original.
             */
            temp->clone.speed = 0;
            object_update_speed(&temp->clone);
            temp->clone.speed = op->speed;
        }

        temp->more = op->arch->more;
        op->arch = temp;
        /* LOG(llevDebug, "created temporary archetype for %s at %d,%d\n", op->name, op->x, op->y); */
    }
}

/**
 * This extracts the key/value from the yytext field -
 * calls object_set_value() to actually set the value.
 * Function basically has to find spaces, strip out extra,
 * etc.  strchr doesn't work as good because could also
 * be tabs.
 *
 * @param op
 * object in which to insert the value.
 */
static void add_key_value(object *op) {
    char *key = NULL;
    char *value = NULL;
    char *cp;
    char *end;

    /* First, skip over leading whitespace. */
    for (cp = yytext; isspace(*cp); cp++) {
        ;
    }

    key = cp;

    /* Now look for the end of the key/field name. */
    for (; !isspace(*cp); cp++) {
        if (*cp == '\0') {
            /* Oops, ran out of string! Set the key with an empty value. */
            object_set_value(op, key, NULL, TRUE);
            return;
        }
    }

    if (*cp == '\0') {
        object_set_value(op, key, NULL, TRUE);
        return;
    }

    /* Chop off the key, and start at the next character. */
    *cp = '\0';
    cp++;
    if (*cp == '\0') {
        /* Was followed by one space? */
        object_set_value(op, key, NULL, TRUE);
        return;
    }

    /* Now looking for the value. Skip over whitespace. */
    for (; isspace(*cp); cp++) {
        if (*cp == '\0') {
            /* Guess not. */
            object_set_value(op, key, NULL, TRUE);
            return;
        }
    }

    value = cp;

    /* Got last character before null and strip
     * off tailing whitespace
     */
    for (end = value+(strlen(cp)-1); isspace(*end); end--) {
        if (end == value) {
            /* *blink blink* Still no value? */
            object_set_value(op, key, NULL, TRUE);
            return;
        }
        *end = '\0';
    }
    object_set_value(op, key, value, TRUE);
}

/**
 * Handles the different move types an object can have.
 *
 * @param mt
 * field for which we are computing the value, will be set to the result of parsing.
 * @param params
 * string containing the definition of the move type to parse.
 * @param prefix
 * string to be displayed in case the move type is invalid, must not be NULL.
 */
static void set_move(MoveType *mt, char *params, const char *prefix) {
    char *str;
    int i, negate;

    if (isdigit(*params)) {
        *mt = atoi(params);
    } else {
        *mt = 0;
        for (str = strtok(params, " "); str; str = strtok(NULL, " ")) {
            negate = 0;
            if (!strcasecmp(str, "all"))
                *mt |= MOVE_ALL;
            else {
                if (*str == '-') {
                    negate = 1;
                    str++;
                }
                for (i = 0; move_name[i] != NULL; i++)  {
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
                    if (!strcasecmp(str, "flying")) {
                        if (negate) {
                            *mt &= ~MOVE_FLYING;
                        } else {
                            *mt |= MOVE_FLYING;
                        }
                    } else {
                        LOG(llevDebug, "common/loader.l: set_move(%s) - unknown move string '%s'\n", prefix, str);
                    }
                }
            } /* Else not all move types */
        } /* for strtok */
    } /* Else not a numeric value */
}

%}

S       [ \t]+.+
WS      [ \t]*
A       .+

%x MESSAGE
%x LORE
%x SCRIPT

/* Don't have to link with -lfl with this */
%option noyywrap

/* need yy_push_state, yy_pop_state */
%option stack

/* those functions are unused, not sure of compatibility with Flex versions. */
%option nounput
%option noyy_top_state
%option noinput

%%

%{
/* Declare some local variables */
    object *op = items[*depth];
    lex_error = 0;

%}

^msg{WS}$           {   BEGIN(MESSAGE); msgbuf[0] = '\0'; msglen = 0; total_msglen = 0;}
<MESSAGE>^endmsg{WS}$ { BEGIN(INITIAL);
                        object_set_msg(op, msgbuf);
                        /* Just print a warning so we can be reasonably safe
                         * about not overflowing the buffer.
                         */
                        if (total_msglen > (HUGE_BUF / 2))
                            LOG(llevDebug,
                                "warning: msg in '%s' on %s is very long (%lu "
                                "characters)\n",
                                op->name,
                                op->map != NULL ? op->map->path : "no map",
                                (unsigned long)total_msglen);
                    }
<MESSAGE>.*         {   safe_strcat(msgbuf, yytext, &msglen, HUGE_BUF);
                        safe_strcat(msgbuf, "\n", &msglen, HUGE_BUF);
                        total_msglen += strlen(yytext)+1;
                    }

^lore{WS}$          {   BEGIN(LORE); lorebuf[0] = '\0'; }
<LORE>^endlore{WS}$ {   BEGIN(INITIAL);
                        op->lore = add_string(lorebuf);
                        /* Just print a warning so we can be reasonably safe
                         * about not overflowing the buffer.
                         */
                        if (strlen(op->lore) > (HUGE_BUF/2))
                            LOG(llevDebug, "\n\tWarning lore length > %d (max allowed=%d): %lu\n>%.80s<\n",
                                HUGE_BUF/2, HUGE_BUF, (unsigned long)strlen(op->lore), op->lore);
                    }
<LORE>.*            { strcat(lorebuf, yytext); strcat(lorebuf, "\n"); }

^object{S}          {
                        char *yv = yval();

                        if (*yv == '\0') {
                            LOG(llevError, "Object lacks name.\n");
                            return LL_IGNORED;
                        }
                        if (seen_object) {
                            LOG(llevError, "An object started before the 'end' of object %s.\n", (op->arch && op->arch->name) ? op->arch->name : "(no name)");
                        }
                        if (op->arch != NULL)
                            op->arch->name = add_string(yv);
                        op->name = add_string(yv);
                        seen_object = true;
                    }

^name{S}            {
                        char *yv = yval();

                        if (*yv == '\0')
                            LOG(llevError, "Name without val\n");
                        else
                            FREE_AND_COPY(op->name, yv);
                    }
^name_pl{S}         {
                        char *yv = yval();

                        if (*yv == '\0')
                            LOG(llevError, "Name without val\n");
                        else
                            FREE_AND_COPY(op->name_pl, yv);
                    }
^anim_suffix{S}     {
                        char *yv = yval();

                        if (*yv == '\0')
                            LOG(llevError, "Anim suffix without val\n");
                        else
                            FREE_AND_COPY(op->anim_suffix, yv);
                    }
^skill{S}           FREE_AND_COPY(op->skill, yval());
^custom_name{S}     {
                        char *yv = yval();

                        if (*yv == '\0')
                            LOG(llevError, "Custom name without val\n");
                        else
                            FREE_AND_COPY(op->custom_name, yv);
                    }
^race{S}            FREE_AND_COPY(op->race, yval());
^slaying{S}         FREE_AND_COPY(op->slaying, yval());
^arch{S}            {
                        if (op->arch) {
                          /*
                           Here an object must be inserted into another object,
                           during archetypes, artifact or map loading.

                           The logic is as follow:
                               - in linemode, we are loading artifacts, so just
                                 increase depth, next calls will work on object
                                 in inventory, and insertion in previous object done
                                 in the "end" part
                               - when not in linemode, two cases:
                                - arch_init is not set, then we are loading a map
                                - arch_init is set, we are loading archetypes,
                                  discard the object, it'll be handled in a second pass
                            */
                            object *tmp;
                            char *yv = yval();

                            tmp = object_new();
                            tmp->arch = find_archetype(yv);
                            object_copy(&tmp->arch->clone, tmp);
                            strcpy(msgbuf, "");
                            strcpy(lorebuf, "");
                            msglen = 0;
                            total_msglen = 0;
                            (*depth)++;
                            items[*depth] = tmp;
                            if (linemode == 0) {
                                lex_load(depth, items, maxdepth, map_flags, linemode, ismore);
                                (*depth)--;
                                object_insert_in_ob(tmp, op);
                            }
                        } else {
                            /* Here we are loading an object in a map, on a spot, not
                             * in another object's inventory */
                            char *yv = yval();

                            op->arch = find_archetype(yv);
                            object_copy(&op->arch->clone, op);
                        }
                    }
^artifact{S}        {
                        const char *yv = yval();
                        const artifact *art = find_artifact(op, yv);
                        if (art == NULL) {
                            LOG(llevError, "can't find artifact %s [%d]\n for loading", yv, op->type);
                        } else {
                            give_artifact_abilities(op, art->item);
                        }
                    }

^other_arch{S}      op->other_arch = find_archetype(yval());
^animation{S}       {
                        if (strcmp (yval(), "NONE") == 0) {
                            op->animation = NULL;
                            CLEAR_FLAG (op, FLAG_ANIMATE);
                        } else {
                            op->animation = find_animation (yval());
                            SET_FLAG (op, FLAG_ANIMATE);
                        }
                    }

^more{WS}$          { /* We need to record that this is a multipart object,
                       * so the calling function can glue things back together
                       */
                        if ((*depth) > 0) {
                            LOG(llevError, "Got 'more' in depth %d (object %s)\n", *depth, items[*depth]->name);
                        }
                        (*ismore) = 1;
                    }

^end{WS}$           {
                        seen_object = false;
                        check_loaded_object(op);
                        if (linemode) {
                            /* Linemode is only set for artifact loading. */
                            if ((*depth) > 0) {
                                /* Object we finished is in another object's inventory */
                                object_insert_in_ob(items[*depth], items[(*depth)-1]);
                            }
                            return LL_NORMAL;
                        }
                        if (*ismore)
                            return LL_MORE;
                        else
                            return LL_NORMAL;
                    }
^last_heal{S}       op->last_heal = IVAL;
^last_sp{S}         op->last_sp = IVAL;
^last_grace{S}      op->last_grace = IVAL;
^last_eat{S}        op->last_eat = IVAL;
^speed{S}           {
                        op->speed = FVAL;
                        if (!(map_flags&MAP_STYLE)) {
                            if (op->speed < 0) {
                                op->speed_left = op->speed_left-RANDOM()%100/100.0;
                                op->speed = -op->speed; // Make this always positive
                            }
                            object_update_speed(op);
                        }
                    }
^speed_left{S}      op->speed_left = FVAL;

^weapon_speed_left{S} op->weapon_speed_left = FVAL;
^weapon_speed{S}    op->weapon_speed = FVAL;

^slow_move{S}       {
                        op->move_slow |= MOVE_WALK;
                        op->move_slow_penalty = FVAL;
                    }
^title{S}           {
                        char *y = yval();

                        if (*y == '\0')
                            LOG(llevError, "Title without value.\n");
                        else
                            FREE_AND_COPY(op->title, y);
                    }

^face{S}            {
                        const char *name = yval();
                        op->face = find_face(yval());
                    }
^str{S}             op->stats.Str = IVAL;
^dex{S}             op->stats.Dex = IVAL;
^con{S}             op->stats.Con = IVAL;
^wis{S}             op->stats.Wis = IVAL;
^cha{S}             op->stats.Cha = IVAL;
^int{S}             op->stats.Int = IVAL;
^pow{S}             op->stats.Pow = IVAL;
^hp{S}              op->stats.hp = IVAL;
^maxhp{S}           op->stats.maxhp = IVAL;
^sp{S}              op->stats.sp = IVAL;
^maxsp{S}           op->stats.maxsp = IVAL;
^grace{S}           op->stats.grace = IVAL;
^maxgrace{S}        op->stats.maxgrace = IVAL;
^exp{S}             op->stats.exp = atoll(yval());
^total_exp{S}       op->total_exp = atoll(yval());
^perm_exp{S}        op->total_exp = (atoll(yval()) * 100 / settings.permanent_exp_ratio);
^food{S}            op->stats.food = IVAL;
^dam{S}             op->stats.dam = IVAL;
^wc{S}              op->stats.wc = IVAL;
^ac{S}              op->stats.ac = IVAL;
^x{S}               { op->x = IVAL; op->ox= op->x; }
^y{S}               { op->y = IVAL; op->oy= op->y; }
^nrof{S}            op->nrof= atol(yval());
^level{S}           op->level = IVAL;
^direction{S}       op->direction = IVAL;
^type{S}            op->type = IVAL;
^subtype{S}         op->subtype = IVAL;
^material{S}        op->material = IVAL;
^materialname{S}    {
                        char *yv = yval();

                        if (*yv == '\0')
                            LOG(llevError, "Materialname without val\n");
                        else
                            FREE_AND_COPY(op->materialname, yv);
                    }

^value{S}           op->value = IVAL;
^weight{S}          {
                        op->weight = atol(yval());
                        if (op->weight == 0) {
                            SET_FLAG(op, FLAG_NO_PICK);
                        }
                    }
^carrying{S}        op->carrying = atol(yval());
^attacktype{S}      op->attacktype = IVAL;
^path_attuned{S}    op->path_attuned = IVAL;
^path_repelled{S}   op->path_repelled = IVAL;
^path_denied{S}     op->path_denied = IVAL;
^invisible{S}       op->invisible = IVAL;
^magic{S}           op->magic = IVAL;
^state{S}           op->state = IVAL;
^alive{S}           SET_OR_CLEAR_FLAG(op, FLAG_ALIVE, IVAL);
^applied{S}         SET_OR_CLEAR_FLAG(op, FLAG_APPLIED, IVAL);
^unpaid{S}          SET_OR_CLEAR_FLAG(op, FLAG_UNPAID, IVAL);
^is_animated{S}     SET_OR_CLEAR_FLAG(op, FLAG_ANIMATE, IVAL);
^no_pick{S}         SET_OR_CLEAR_FLAG(op, FLAG_NO_PICK, IVAL);
^client_anim_sync{S} SET_OR_CLEAR_FLAG(op, FLAG_CLIENT_ANIM_SYNC, IVAL);
^client_anim_random{S} SET_OR_CLEAR_FLAG(op, FLAG_CLIENT_ANIM_RANDOM, IVAL);

%{ /* These are the new values */
%}
^move_block{S}      set_move(&op->move_block, yval(), "block");
^move_allow{S}      set_move(&op->move_allow, yval(), "allow");
^move_type{S}       set_move(&op->move_type, yval(), "type");
^move_on{S}         set_move(&op->move_on, yval(), "on");
^move_off{S}        set_move(&op->move_off, yval(), "off");
^move_slow{S}       set_move(&op->move_slow, yval(), "slow");
^move_slow_penalty{S} op->move_slow_penalty = FVAL;


^monster{S}         SET_OR_CLEAR_FLAG(op, FLAG_MONSTER, IVAL);
^neutral{S}         SET_OR_CLEAR_FLAG(op, FLAG_NEUTRAL, IVAL);
^no_attack{S}       SET_OR_CLEAR_FLAG(op, FLAG_NO_ATTACK, IVAL);
^no_damage{S}       SET_OR_CLEAR_FLAG(op, FLAG_NO_DAMAGE, IVAL);
^friendly{S}        {
                        if (IVAL) {
                            SET_FLAG(op, FLAG_FRIENDLY);
                            if (op->type != PLAYER && !arch_init) {
                                LOG(llevDebug, " Adding friendly object %s.\n", op->name);
                                add_friendly_object(op);
                            }
                        }
                        else
                            CLEAR_FLAG(op, FLAG_FRIENDLY);
                    }
^generator{S}       SET_OR_CLEAR_FLAG(op, FLAG_GENERATOR, IVAL);
^use_content_on_gen{S} SET_OR_CLEAR_FLAG (op, FLAG_CONTENT_ON_GEN, IVAL);
^is_thrown{S}       SET_OR_CLEAR_FLAG(op, FLAG_IS_THROWN, IVAL);
^auto_apply{S}      SET_OR_CLEAR_FLAG(op, FLAG_AUTO_APPLY, IVAL);
^treasure{S}        SET_OR_CLEAR_FLAG(op, FLAG_TREASURE, IVAL);
^see_invisible{S}   SET_OR_CLEAR_FLAG(op, FLAG_SEE_INVISIBLE, IVAL);
^can_roll{S}        SET_OR_CLEAR_FLAG(op, FLAG_CAN_ROLL, IVAL);
^overlay_floor{S}   SET_OR_CLEAR_FLAG(op, FLAG_OVERLAY_FLOOR, IVAL);
^is_turnable{S}     SET_OR_CLEAR_FLAG(op, FLAG_IS_TURNABLE, IVAL);
^is_used_up{S}      SET_OR_CLEAR_FLAG(op, FLAG_IS_USED_UP, IVAL);
^identified{S}      {
                        if (IVAL) {
                            SET_FLAG(op, FLAG_IDENTIFIED);
                            CLEAR_FLAG(op, FLAG_KNOWN_MAGICAL);
                        }
                        else
                            CLEAR_FLAG(op, FLAG_IDENTIFIED);
                    }
^reflecting{S}      SET_OR_CLEAR_FLAG(op, FLAG_REFLECTING, IVAL);
^changing{S}        SET_OR_CLEAR_FLAG(op, FLAG_CHANGING, IVAL);
^splitting{S}       SET_OR_CLEAR_FLAG(op, FLAG_SPLITTING, IVAL);
^hitback{S}         SET_OR_CLEAR_FLAG(op, FLAG_HITBACK, IVAL);
^startequip{S}      SET_OR_CLEAR_FLAG(op, FLAG_STARTEQUIP, IVAL);
^blocksview{S}      SET_OR_CLEAR_FLAG(op, FLAG_BLOCKSVIEW, IVAL);
^editor_folder{S}   { /* This field is used by Gridarta, so it's not an error. */ }
^undead{S}          SET_OR_CLEAR_FLAG(op, FLAG_UNDEAD, IVAL);
^scared{S}          SET_OR_CLEAR_FLAG(op, FLAG_SCARED, IVAL);
^unaggressive{S}    SET_OR_CLEAR_FLAG(op, FLAG_UNAGGRESSIVE, IVAL);
^reflect_missile{S} SET_OR_CLEAR_FLAG(op, FLAG_REFL_MISSILE, IVAL);
^reflect_spell{S}   SET_OR_CLEAR_FLAG(op, FLAG_REFL_SPELL, IVAL);
^no_magic{S}        SET_OR_CLEAR_FLAG(op, FLAG_NO_MAGIC, IVAL);
^wiz{S}             {
                        if (IVAL) {
                            SET_FLAG(op, FLAG_WIZ);
                            SET_FLAG(op, FLAG_WAS_WIZ);
                            SET_FLAG(op, FLAG_WIZPASS);
                            SET_FLAG(op, FLAG_WIZCAST);
                        } else {
                            CLEAR_FLAG(op, FLAG_WIZ);
                            CLEAR_FLAG(op, FLAG_WIZPASS);
                            CLEAR_FLAG(op, FLAG_WIZCAST);
                        }
                    }
^was_wiz{S}         SET_OR_CLEAR_FLAG(op, FLAG_WAS_WIZ, IVAL);
^no_fix_player{S}   SET_OR_CLEAR_FLAG(op, FLAG_NO_FIX_PLAYER, IVAL);
^is_lightable{S}    SET_OR_CLEAR_FLAG(op, FLAG_IS_LIGHTABLE, IVAL);
^tear_down{S}       SET_OR_CLEAR_FLAG(op, FLAG_TEAR_DOWN, IVAL);
^luck{S}            op->stats.luck = IVAL;
^run_away{S}        op->run_away = IVAL;
^pick_up{S}         op->pick_up = IVAL;
^item_power{S}      op->item_power = IVAL;
^gen_sp_armour{S}   op->gen_sp_armour = IVAL;
^anim_speed{S}      op->anim_speed = IVAL;
^container{S}       op->weight_limit = IVAL;
^no_drop{S}         SET_OR_CLEAR_FLAG(op, FLAG_NO_DROP, IVAL);
^will_apply{S}      op->will_apply = IVAL;
^random_movement{S} SET_OR_CLEAR_FLAG(op, FLAG_RANDOM_MOVE, IVAL);
^can_use_shield{S}  SET_OR_CLEAR_FLAG(op, FLAG_USE_SHIELD, IVAL);
^can_cast_spell{S}  SET_OR_CLEAR_FLAG(op, FLAG_CAST_SPELL, IVAL);
^can_use_scroll{S}  SET_OR_CLEAR_FLAG(op, FLAG_USE_SCROLL, IVAL);
^can_use_range{S}   SET_OR_CLEAR_FLAG(op, FLAG_USE_RANGE, IVAL);
^can_use_bow{S}     SET_OR_CLEAR_FLAG(op, FLAG_USE_BOW, IVAL);
^can_use_armour{S}  SET_OR_CLEAR_FLAG(op, FLAG_USE_ARMOUR, IVAL);
^can_use_weapon{S}  SET_OR_CLEAR_FLAG(op, FLAG_USE_WEAPON, IVAL);
^can_use_ring{S}    SET_OR_CLEAR_FLAG(op, FLAG_USE_RING, IVAL);
^has_ready_bow{S}   SET_OR_CLEAR_FLAG(op, FLAG_READY_BOW, IVAL);
^xrays{S}           SET_OR_CLEAR_FLAG(op, FLAG_XRAYS, IVAL);
^is_floor{S}        SET_OR_CLEAR_FLAG(op, FLAG_IS_FLOOR, IVAL);
^lifesave{S}        SET_OR_CLEAR_FLAG(op, FLAG_LIFESAVE, IVAL);
^no_strength{S}     SET_OR_CLEAR_FLAG(op, FLAG_NO_STRENGTH, IVAL);
^sleep{S}           SET_OR_CLEAR_FLAG(op, FLAG_SLEEP, IVAL);
^stand_still{S}     SET_OR_CLEAR_FLAG(op, FLAG_STAND_STILL, IVAL);
^only_attack{S}     SET_OR_CLEAR_FLAG(op, FLAG_ONLY_ATTACK, IVAL);

^activate_on_push{S} SET_OR_CLEAR_FLAG(op, FLAG_ACTIVATE_ON_PUSH, IVAL);
^activate_on_release{S} SET_OR_CLEAR_FLAG(op, FLAG_ACTIVATE_ON_RELEASE, IVAL);

    /* Start of various attacktypes */
^resist_physical{S} SET_RESIST(op, ATNR_PHYSICAL, IVAL);
^resist_magic{S}    SET_RESIST(op, ATNR_MAGIC, IVAL);
^resist_fire{S}     SET_RESIST(op, ATNR_FIRE, IVAL);
^resist_electricity{S} SET_RESIST(op, ATNR_ELECTRICITY, IVAL);
^resist_cold{S}     SET_RESIST(op, ATNR_COLD, IVAL);
^resist_confusion{S} SET_RESIST(op, ATNR_CONFUSION, IVAL);
^resist_acid{S}     SET_RESIST(op, ATNR_ACID, IVAL);
^resist_drain{S}    SET_RESIST(op, ATNR_DRAIN, IVAL);
^resist_weaponmagic{S} SET_RESIST(op, ATNR_WEAPONMAGIC, IVAL);
^resist_ghosthit{S} SET_RESIST(op, ATNR_GHOSTHIT, IVAL);
^resist_poison{S}   SET_RESIST(op, ATNR_POISON, IVAL);
^resist_slow{S}     SET_RESIST(op, ATNR_SLOW, IVAL);
^resist_paralyze{S} SET_RESIST(op, ATNR_PARALYZE, IVAL);
^resist_turn_undead{S} SET_RESIST(op, ATNR_TURN_UNDEAD, IVAL);
^resist_fear{S}     SET_RESIST(op, ATNR_FEAR, IVAL);
^resist_cancellation{S} SET_RESIST(op, ATNR_CANCELLATION, IVAL);
^resist_deplete{S}  SET_RESIST(op, ATNR_DEPLETE, IVAL);
^resist_death{S}    SET_RESIST(op, ATNR_DEATH, IVAL);
^resist_chaos{S}    SET_RESIST(op, ATNR_CHAOS, IVAL);
^resist_counterspell{S} SET_RESIST(op, ATNR_COUNTERSPELL, IVAL);
^resist_godpower{S} SET_RESIST(op, ATNR_GODPOWER, IVAL);
^resist_holyword{S} SET_RESIST(op, ATNR_HOLYWORD, IVAL);
^resist_blind{S}    SET_RESIST(op, ATNR_BLIND, IVAL);
^resist_internal{S} SET_RESIST(op, ATNR_INTERNAL, IVAL);
^resist_life_stealing{S} SET_RESIST(op, ATNR_LIFE_STEALING, IVAL);
^resist_disease{S}  SET_RESIST(op, ATNR_DISEASE, IVAL);
^attack_movement{S} op->attack_movement = IVAL;
^move_state{S}      op->move_status = IVAL;
^confused{S}        SET_OR_CLEAR_FLAG(op, FLAG_CONFUSED, IVAL);
^stealth{S}         SET_OR_CLEAR_FLAG(op, FLAG_STEALTH, IVAL);
^connected{S}       add_button_link(op, op->map, IVAL);
^cursed{S}          SET_OR_CLEAR_FLAG(op, FLAG_CURSED, IVAL);
^damned{S}          SET_OR_CLEAR_FLAG(op, FLAG_DAMNED, IVAL);
^blessed{S}         SET_OR_CLEAR_FLAG(op, FLAG_BLESSED, IVAL);
^see_anywhere{S}    SET_OR_CLEAR_FLAG(op, FLAG_SEE_ANYWHERE, IVAL);
^known_magical{S}   SET_OR_CLEAR_FLAG(op, FLAG_KNOWN_MAGICAL, IVAL);
^known_cursed{S}    SET_OR_CLEAR_FLAG(op, FLAG_KNOWN_CURSED, IVAL);
^known_blessed{S}   SET_OR_CLEAR_FLAG(op, FLAG_KNOWN_BLESSED, IVAL);
^can_use_skill{S}   SET_OR_CLEAR_FLAG(op, FLAG_CAN_USE_SKILL, IVAL);
^been_applied{S}    SET_OR_CLEAR_FLAG(op, FLAG_BEEN_APPLIED, IVAL);
^has_ready_scroll{S} SET_OR_CLEAR_FLAG(op, FLAG_READY_SCROLL, IVAL);
^expmul{S}          op->expmul = FVAL;
^unique{S}          SET_OR_CLEAR_FLAG(op, FLAG_UNIQUE, IVAL);
^make_invisible{S}  SET_OR_CLEAR_FLAG(op, FLAG_MAKE_INVIS, IVAL);
^inv_locked{S}      SET_OR_CLEAR_FLAG(op, FLAG_INV_LOCKED, IVAL);
^is_wooded{S}       SET_OR_CLEAR_FLAG(op, FLAG_IS_WOODED, IVAL);
^is_hilly{S}        SET_OR_CLEAR_FLAG(op, FLAG_IS_HILLY, IVAL);
^is_water{S}        SET_OR_CLEAR_FLAG(op, FLAG_IS_WATER, IVAL);
^has_ready_skill{S} SET_OR_CLEAR_FLAG(op, FLAG_READY_SKILL, IVAL);
^has_ready_weapon{S} SET_OR_CLEAR_FLAG(op, FLAG_READY_WEAPON, IVAL);
^no_skill_ident{S}  SET_OR_CLEAR_FLAG(op, FLAG_NO_SKILL_IDENT, IVAL);
^glow_radius{S}     op->glow_radius = IVAL;
^is_blind{S}        SET_OR_CLEAR_FLAG(op, FLAG_BLIND, IVAL);
^can_see_in_dark{S} SET_OR_CLEAR_FLAG(op, FLAG_SEE_IN_DARK, IVAL);
^is_cauldron{S}     SET_OR_CLEAR_FLAG(op, FLAG_IS_CAULDRON, IVAL);
^randomitems{S}     op->randomitems = find_treasurelist(yval());
^no_steal{S}        SET_OR_CLEAR_FLAG(op, FLAG_NO_STEAL, IVAL);
^one_hit{S}         SET_OR_CLEAR_FLAG(op, FLAG_ONE_HIT, IVAL);
^berserk{S}         SET_OR_CLEAR_FLAG(op, FLAG_BERSERK, IVAL);

^can_knockback{S}   { /* Some archetypes have these values in them */ }
^can_parry{S}       { /* Probably the pupland archetypes - I imagined */ }
^can_impale{S}      { /* That these are for the new combat code */ }
^can_cut{S}         { /* just ignore for now */ }
^can_dam_armour{S}  { }
^weapontype{S}      op->weapontype = IVAL;
^casting_time{S}    op->casting_time = FVAL;
^smoothlevel{S}     op->smoothlevel = IVAL;
^smoothface{S}      {
                        /** @todo fix evil const cast */
                        char *p = strchr(yval(), ' ');
                        if (!p)
                            continue;

                        *p = '\0';
                        Face *regular = (Face *)find_face(yval());
                        Face *smoothed = (Face *)find_face(p + 1);
                        regular->smoothface = smoothed;
                    }
^map_layer{S}       {
                        if (IVAL)
                            op->map_layer = IVAL;
                        else {
                            int i;
                            char *cp = yval();

                            for (i = 0; i < MAP_LAYERS; i++) {
                                if (!strcasecmp(cp, map_layer_name[i])) {
                                    op->map_layer = i;
                                    break;
                                }
                            }
                            if (i == MAP_LAYERS) {
                                LOG(llevError, "Invalid map_layer name found: %s\n", cp);
                            }
                        }
                    }
^client_type{S}     op->client_type = IVAL;
^body_{A}           set_body_info(op, yytext);
^duration{S}        op->duration = IVAL;
^range{S}           op->range = IVAL;
^range_modifier{S}  op->range_modifier = IVAL;
^dam_modifier{S}    op->dam_modifier = IVAL;
^duration_modifier{S} op->duration_modifier = IVAL;
^is_buildable{S}    SET_OR_CLEAR_FLAG(op, FLAG_IS_BUILDABLE, IVAL);
^sound_chance{S}    op->sound_chance = IVAL;
^paralyzed{S}       SET_OR_CLEAR_FLAG(op, FLAG_PARALYZED, IVAL);

<*>(^{WS}$)|\n          { /* ignore empty lines, newlines we don't do above */ }
#.*\n                   { }

<<EOF>>                 { /* If we got an error, return the error.  Otherwise, return that we got EOF */
                            if (lex_error != 0)
                                return lex_error;
                            else
                                return LL_EOF;
                        }
.*                      { add_key_value(op); }
%%

/**
 * Returns the next token for lex.
 * Our save file syntax is very simple, so we can use a very simple
 * processing mechanism here instead using something like bison
 * This skips over the space and returns the value, or "" if no value
 * is found.  Modified 4/26/2000 to also strip spaces at end of
 * line
 */
static char *yval(void)
{
    static char em[] = "";
    char *cp, *end;

    /* First skip over start of line, like animation or name */
    for (cp = yytext; *cp != ' '; cp++) {
        if (*cp == '\0') {
            return em;
        }
    }

    /* Skip over whitespace */
    for (; *cp == ' '; cp++) {
        if (*cp == '\0') {
            return em;
        }
    }
    /* Got last character before null and strip
     * off tailing whitespace
     */
    for (end = cp+strlen(cp)-1; *end == ' '; end--) {
        if (end == cp)
            return em;
        *end = '\0';
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
 * combination of @ref MAP_xxx flags, though only MAP_STYLE is used.
 * @return
 * @li LL_NORMAL if object was read.
 * @li LL_EOF if an error occured.
 * @li LL_MORE if this is a multipart object.
 */
int load_object(FILE *fp, object *op, int bufstate, int map_flags) {
    int retval;
    char inbuf[VERY_BIG_BUF];
#define MAXDEPTH 10
    object *items[MAXDEPTH];
    int depth = 0, ismore = 0;

    items[0] = op,
    strcpy(msgbuf, "");
    msglen = 0;
    total_msglen = 0;
    strcpy(lorebuf, "");
    if (bufstate == LO_NEWFILE || bufstate == LO_NOREAD) {
    /*  LOG(llevDebug, "Switching lex buffers\n");*/
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE));
        if (bufstate == LO_NOREAD)
            return LL_NORMAL;
    } else if (bufstate == LO_LINEMODE) {
        YY_BUFFER_STATE yybufstate;
        yy_delete_buffer(YY_CURRENT_BUFFER);
        while (fgets(inbuf, VERY_BIG_BUF-3, fp)) {
            yybufstate = yy_scan_string(inbuf);
            retval = lex_load(&depth, items, MAXDEPTH, map_flags, 1, &ismore);
            yy_delete_buffer(yybufstate);
            if (retval == LL_NORMAL) {
                if (depth == 0)
                    return retval;
                else
                    depth--;
            }
        }
        LOG(llevDebug, "Got eof while scanning strings\n");
        return LL_EOF;
    }

    retval = lex_load(&depth, items, MAXDEPTH, map_flags, 0, &ismore);

/*    LOG(llevDebug, " load completed, object=%s\n", op->name);*/
    return retval;
}

/**
 * Load an object from the specified reader, stopping when the object is complete.
 * @param reader where to read from.
 * @param op object to read.
 * @param map_flags one or more @ref MAP_xxx "map loading flags".
 * @return LL_NORMAL or LL_MORE if no error, LL_EOF if end of file, will LOG an error if unexpected.
 * @todo loading is inefficient because of bufferreader_get_line(), check use of yy_scan_buffer().
 */
int load_object_from_reader(BufferReader *reader, object *op, int map_flags) {
    int retval;
    object *items[MAXDEPTH];
    int depth = 0, ismore = 0;
    char buf[HUGE_BUF];

    items[0] = op,
    strcpy(msgbuf, "");
    msglen = 0;
    total_msglen = 0;
    strcpy(lorebuf, "");

    YY_BUFFER_STATE yybufstate;
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_push_state(INITIAL);

    while (bufferreader_get_line(reader, buf, sizeof(buf)) != NULL) {
        yybufstate = yy_scan_string(buf);
        retval = lex_load(&depth, items, MAXDEPTH, map_flags, 1, &ismore);
        yy_delete_buffer(yybufstate);
        if (retval == LL_NORMAL || retval == LL_MORE) {
            if (depth == 0) {
                yy_pop_state();
                return ismore ? LL_MORE : LL_NORMAL;
            } else
                depth--;
        }
    }
    if (depth > 0) {
        LOG(llevError, "Got eof while scanning strings\n");
    }
    yy_pop_state();
    return LL_EOF;
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
int set_variable(object *op, const char *buf) {
    YY_BUFFER_STATE yybufstate, yycurbuf = YY_CURRENT_BUFFER;
    int retval, depth, ismore = 0;
    object *items[MAXDEPTH];

    strcpy(msgbuf, "");
    msglen = 0;
    total_msglen = 0;
    strcpy(lorebuf, "");
    items[0] = op;
    depth = 0;
    yy_push_state(INITIAL);
    yybufstate = yy_scan_string(buf);
    retval = lex_load(&depth, items, MAXDEPTH, 0, 0, &ismore);
    if (yycurbuf) {
        yy_switch_to_buffer(yycurbuf);
    }
    yy_delete_buffer(yybufstate);
    yy_pop_state();
    return retval;
}

/* Start of C code */

/**
 * Frees all memory allocated by the loader.
 */
void free_loader(void) {
    yy_delete_buffer(YY_CURRENT_BUFFER);

    /* Comment out this yylex_destroy(), as it results in link errors
     * on linux
     */
/*    yylex_destroy();*/
}
