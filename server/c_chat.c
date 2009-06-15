/*
 * static char *rcsid_c_chat_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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
 * All player communication commands, except the 'gsay' one.
 */

#include <global.h>
#include <loader.h>
#include <sproto.h>

/**
 * 'say' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_say(object *op, char *params) {
    if (!params)
        return 0;
    monster_communicate(op, params);

    return 0;
}

/**
 * 'me' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_me(object *op, char *params) {
    char buf[MAX_BUF];

    if (!params)
        return 0;
    snprintf(buf, sizeof(buf), "%s %s", op->name, params);
    ext_info_map(NDI_UNIQUE|NDI_BLUE, op->map, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_ME,
                 buf, NULL);
    return 0;
}

/**
 * 'cointoss' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_cointoss(object *op, char *params) {
    char buf[MAX_BUF];
    const char *result;

    result = rndm(1, 2) == 1 ? "Heads" : "Tails";

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
        "You flip a coin.... %s!",
        "You flip a coin.... %s!",
        result);

    snprintf(buf, sizeof(buf), "%s flips a coin.... %s!", op->name, result);
    ext_info_map_except(NDI_WHITE, op->map, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
        buf, NULL);

    return 0;
}

/** Results for the "orcknucle" game. */
static const char *const orcknuckle[7] = {
    "none",
    "beholder",
    "ghost",
    "knight",
    "princess",
    "dragon",
    "orc"
};

#define DICE    4 /**< How many dice to roll for orcknuckle. */

/**
 * Plays the "orcknucke" game.
 *
 * If there is an "dice" archetype in server arches, this command will
 * require the player to have at least 4 dice to play. There is a 5%
 * chance to lose one dice at each play. Dice can be made through alchemy
 * (finding the recipe is left as an exercice to the player).
 * Note that the check is on the name 'dice', so you can have multiple
 * archetypes for that name, they'll be all taken into account.
 *
 * @param op
 * player who plays.
 * @param params
 * string sent by the player. Ignored.
 * @return
 * always 0.
 */
int command_orcknuckle(object *op, char *params) {
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
    object *dice[DICE];
    int i, j, k, l, dice_count, number_dice;
    const char *name;

    /* We only use dice if the archetype is present ingame. */
    name = find_string("dice");
    if (name) {
        for (dice_count = 0; dice_count < DICE; dice_count++)
            dice[dice_count] = NULL;
        dice_count = 0;
        number_dice = 0;

        FOR_INV_PREPARE(op, ob) {
            if (dice_count >= DICE || number_dice >= DICE)
                break;
            if (ob->name == name) {
                number_dice += ob->nrof;
                dice[dice_count++] = ob;
            }
        } FOR_INV_FINISH();

        if (number_dice < DICE) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
                                 "You need at least %d dice to play orcknuckle!",
                                 "You need at least %d dice to play orcknuckle!", DICE);
            return 0;
        }
    }

    i = rndm(1, 5);
    j = rndm(1, 5);
    k = rndm(1, 5);
    l = rndm(1, 6);

    snprintf(buf2, sizeof(buf2), "%s rolls %s, %s, %s, %s!", op->name, orcknuckle[i], orcknuckle[j], orcknuckle[k], orcknuckle[l]);
    snprintf(buf, sizeof(buf), "You roll %s, %s, %s, %s!", orcknuckle[i], orcknuckle[j], orcknuckle[k], orcknuckle[l]);

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
                  buf, NULL);
    ext_info_map_except(NDI_UNIQUE, op->map, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
                        buf2, NULL);

    if (name) {
        /* Randomly lose dice */
        if (die_roll(1, 100, op, 1) < 5) {
            /* Lose one randomly. */
            object_decrease_nrof_by_one(dice[rndm(1, dice_count)-1]);
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
                          "Oops, you lost a dice!", "Oops, you lost a dice!");
        }
    }

    return 0;
#undef DICE
}

/**
 * Utility function for chat or shout.
 *
 * @param op
 * player.
 * @param params
 * message.
 * @param pri
 * message priority.
 * @param color
 * message color.
 * @param subtype
 * message subtype.
 * @param desc
 * 'chat' or 'shouts', will be appened after the player's name and before a :.
 * @return
 * 1.
 */
static int command_tell_all(object *op, char *params, int pri, int color, int subtype, const char *desc) {
    if (op->contr->no_shout == 1) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "You are no longer allowed to shout or chat.", NULL);
        return 1;
    }

    if (params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Shout/Chat what?", NULL);
        return 1;
    }

    draw_ext_info_format(NDI_UNIQUE|NDI_ALL|color, pri, NULL, MSG_TYPE_COMMUNICATION, subtype,
                         "%s %s: %s",
                         "%s %s: %s",
                         op->name, desc, params);

    /* Lauwenmark : Here we handle the SHOUT global event */
    execute_global_event(EVENT_SHOUT, op, params, pri);
    return 1;
}

/**
 * 'sbout' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_shout(object *op, char *params) {
    return command_tell_all(op, params, 1, NDI_RED, MSG_TYPE_COMMUNICATION_SHOUT, "shouts");
}

/**
 * 'chat' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_chat(object *op, char *params) {
    return command_tell_all(op, params, 9, NDI_BLUE, MSG_TYPE_COMMUNICATION_CHAT, "chats");
}

/**
 * Actual function sending a private message.
 *
 * @param op
 * player trying to tell something to someone.
 * @param params
 * who to tell, and message
 * @param adjust_listen
 * if non-zero, recipient can't ignore the message through 'listen' levels.
 * @return
 * 1.
 */
static int do_tell(object *op, char *params, int adjust_listen) {
    char buf[MAX_BUF], *name = NULL, *msg = NULL;
    player *pl;
    uint8 original_listen;

    if (params != NULL) {
        name = params;
        msg = strchr(name, ' ');
        if (msg) {
            *(msg++) = 0;
            if (*msg == 0)
                msg = NULL;
        }
    }

    if (name == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Tell whom what?", NULL);
        return 1;
    }

    if (msg == NULL) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "Tell %s what?",
                             "Tell %s what?",
                             name);
        return 1;
    }

    snprintf(buf, sizeof(buf), "%s tells you: %s", op->name, msg);

    pl = find_player_partial_name(name);
    if (pl) {
        if (adjust_listen) {
            original_listen = pl->listening;
            pl->listening = 10;
        }

        execute_global_event(EVENT_TELL, op, msg, pl->ob);

        draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl->ob, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
                      buf, NULL);

        if (adjust_listen)
            pl->listening = original_listen;

        /* Update last_tell value [mids 01/14/2002] */
        snprintf(pl->last_tell, sizeof(pl->last_tell), "%s", op->name);

        /* Hidden DMs get the message, but player should think DM isn't online. */
        if (!pl->hidden || QUERY_FLAG(op, FLAG_WIZ)) {
            draw_ext_info_format(NDI_UNIQUE|NDI_ORANGE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
                                 "You tell %s: %s",
                                 "You tell %s: %s",
                                 pl->ob->name, msg);

            return 1;
        }
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                  "No such player or ambiguous name.", NULL);
    return 1;
}

/**
 * Private communication.
 *
 * @param op
 * player trying to tell something to someone.
 * @param params
 * who to tell, and message.
 * @return
 * 1.
 */
int command_tell(object *op, char *params) {
    return do_tell(op, params, 0);
}

/**
 * Private communication, by a DM (can't be ignored by player).
 *
 * @param op
 * player trying to tell something to someone.
 * @param params
 * who to tell, and message.
 * @return
 * 1.
 */
int command_dmtell(object *op, char *params) {
    return do_tell(op, params, 1);
}

/**
 * Reply to last person who told you something [mids 01/14/2002]
 *
 * Must have been told something by someone first.
 *
 * @param op
 * who is telling.
 * @param params
 * message to say.
 * @return
 * 1.
 */
int command_reply(object *op, char *params) {
    player *pl;

    if (params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Reply what?", NULL);
        return 1;
    }

    if (op->contr->last_tell[0] == '\0') {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "You can't reply to nobody.", NULL);
        return 1;
    }

    /* Find player object of player to reply to and check if player still exists */
    pl = find_player(op->contr->last_tell);
    if (pl == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "You can't reply, this player left.", NULL);
        return 1;
    }

    /* Update last_tell value */
    strcpy(pl->last_tell, op->name);

    draw_ext_info_format(NDI_UNIQUE|NDI_ORANGE, 0, pl->ob, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
                         "%s tells you: %s",
                         "%s tells you: %s",
                         op->name, params);

    if (pl->hidden && !QUERY_FLAG(op, FLAG_WIZ)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "You can't reply, this player left.", NULL);
        return 1;
    }

    draw_ext_info_format(NDI_UNIQUE|NDI_ORANGE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
                         "You tell to %s: %s",
                         "You tell to %s: %s",
                         pl->ob->name, params);
    return 1;
}

/**
 * This function covers basic emotions a player can have.  An emotion can be
 * one of three things currently.  Directed at oneself, directed at someone,
 * or directed at nobody.  The first set is nobody, the second at someone, and
 * the third is directed at oneself.  Every emotion does not have to be
 * filled out in every category.  The default case will take care of the ones
 * that are not.  Helper functions will call basic_emote with the proper
 * arguments, translating them into commands.  Adding a new emotion can be
 * done by editing command.c and command.h.
 * [garbled 09-25-2001]
 *
 * @param op
 * player.
 * @param params
 * message.
 * @param emotion
 * emotion code, one of @ref EMOTE_xxx "EMOTE_xxx".
 * @return
 * 0 for invalid emotion, 1 else.
 * @todo simplify function (indexed array, for instance).
 */
static int basic_emote(object *op, char *params, int emotion) {
    char buf[MAX_BUF], buf2[MAX_BUF], buf3[MAX_BUF];
    player *pl;

    if (!params) {
        switch (emotion) {
        case EMOTE_NOD:
            snprintf(buf, sizeof(buf), "%s nods solemnly.", op->name);
            snprintf(buf2, sizeof(buf2), "You nod solemnly.");
            break;

        case EMOTE_DANCE:
            snprintf(buf, sizeof(buf), "%s expresses himself through interpretive dance.", op->name);
            snprintf(buf2, sizeof(buf2), "You dance with glee.");
            break;

        case EMOTE_KISS:
            snprintf(buf, sizeof(buf), "%s makes a weird facial contortion", op->name);
            snprintf(buf2, sizeof(buf2), "All the lonely people..");
            break;

        case EMOTE_BOUNCE:
            snprintf(buf, sizeof(buf), "%s bounces around.", op->name);
            snprintf(buf2, sizeof(buf2), "BOIINNNNNNGG!");
            break;

        case EMOTE_SMILE:
            snprintf(buf, sizeof(buf), "%s smiles happily.", op->name);
            snprintf(buf2, sizeof(buf2), "You smile happily.");
            break;

        case EMOTE_CACKLE:
            snprintf(buf, sizeof(buf), "%s throws back his head and cackles with insane glee!", op->name);
            snprintf(buf2, sizeof(buf2), "You cackle gleefully.");
            break;

        case EMOTE_LAUGH:
            snprintf(buf, sizeof(buf), "%s falls down laughing.", op->name);
            snprintf(buf2, sizeof(buf2), "You fall down laughing.");
            break;

        case EMOTE_GIGGLE:
            snprintf(buf, sizeof(buf), "%s giggles.", op->name);
            snprintf(buf2, sizeof(buf2), "You giggle.");
            break;

        case EMOTE_SHAKE:
            snprintf(buf, sizeof(buf), "%s shakes his head.", op->name);
            snprintf(buf2, sizeof(buf2), "You shake your head.");
            break;

        case EMOTE_PUKE:
            snprintf(buf, sizeof(buf), "%s pukes.", op->name);
            snprintf(buf2, sizeof(buf2), "Bleaaaaaghhhhhhh!");
            break;

        case EMOTE_GROWL:
            snprintf(buf, sizeof(buf), "%s growls.", op->name);
            snprintf(buf2, sizeof(buf2), "Grrrrrrrrr....");
            break;

        case EMOTE_SCREAM:
            snprintf(buf, sizeof(buf), "%s screams at the top of his lungs!", op->name);
            snprintf(buf2, sizeof(buf2), "ARRRRRRRRRRGH!!!!!");
            break;

        case EMOTE_SIGH:
            snprintf(buf, sizeof(buf), "%s sighs loudly.", op->name);
            snprintf(buf2, sizeof(buf2), "You sigh.");
            break;

        case EMOTE_SULK:
            snprintf(buf, sizeof(buf), "%s sulks in the corner.", op->name);
            snprintf(buf2, sizeof(buf2), "You sulk.");
            break;

        case EMOTE_CRY:
            snprintf(buf, sizeof(buf), "%s bursts into tears.", op->name);
            snprintf(buf2, sizeof(buf2), "Waaaaaaahhh..");
            break;

        case EMOTE_GRIN:
            snprintf(buf, sizeof(buf), "%s grins evilly.", op->name);
            snprintf(buf2, sizeof(buf2), "You grin evilly.");
            break;

        case EMOTE_BOW:
            snprintf(buf, sizeof(buf), "%s bows deeply.", op->name);
            snprintf(buf2, sizeof(buf2), "You bow deeply.");
            break;

        case EMOTE_CLAP:
            snprintf(buf, sizeof(buf), "%s gives a round of applause.", op->name);
            snprintf(buf2, sizeof(buf2), "Clap, clap, clap.");
            break;

        case EMOTE_BLUSH:
            snprintf(buf, sizeof(buf), "%s blushes.", op->name);
            snprintf(buf2, sizeof(buf2), "Your cheeks are burning.");
            break;

        case EMOTE_BURP:
            snprintf(buf, sizeof(buf), "%s burps loudly.", op->name);
            snprintf(buf2, sizeof(buf2), "You burp loudly.");
            break;

        case EMOTE_CHUCKLE:
            snprintf(buf, sizeof(buf), "%s chuckles politely.", op->name);
            snprintf(buf2, sizeof(buf2), "You chuckle politely");
            break;

        case EMOTE_COUGH:
            snprintf(buf, sizeof(buf), "%s coughs loudly.", op->name);
            snprintf(buf2, sizeof(buf2), "Yuck, try to cover your mouth next time!");
            break;

        case EMOTE_FLIP:
            snprintf(buf, sizeof(buf), "%s flips head over heels.", op->name);
            snprintf(buf2, sizeof(buf2), "You flip head over heels.");
            break;

        case EMOTE_FROWN:
            snprintf(buf, sizeof(buf), "%s frowns.", op->name);
            snprintf(buf2, sizeof(buf2), "What's bothering you?");
            break;

        case EMOTE_GASP:
            snprintf(buf, sizeof(buf), "%s gasps in astonishment.", op->name);
            snprintf(buf2, sizeof(buf2), "You gasp in astonishment.");
            break;

        case EMOTE_GLARE:
            snprintf(buf, sizeof(buf), "%s glares around him.", op->name);
            snprintf(buf2, sizeof(buf2), "You glare at nothing in particular.");
            break;

        case EMOTE_GROAN:
            snprintf(buf, sizeof(buf), "%s groans loudly.", op->name);
            snprintf(buf2, sizeof(buf2), "You groan loudly.");
            break;

        case EMOTE_HICCUP:
            snprintf(buf, sizeof(buf), "%s hiccups.", op->name);
            snprintf(buf2, sizeof(buf2), "*HIC*");
            break;

        case EMOTE_LICK:
            snprintf(buf, sizeof(buf), "%s licks his mouth and smiles.", op->name);
            snprintf(buf2, sizeof(buf2), "You lick your mouth and smile.");
            break;

        case EMOTE_POUT:
            snprintf(buf, sizeof(buf), "%s pouts.", op->name);
            snprintf(buf2, sizeof(buf2), "Aww, don't take it so hard.");
            break;

        case EMOTE_SHIVER:
            snprintf(buf, sizeof(buf), "%s shivers uncomfortably.", op->name);
            snprintf(buf2, sizeof(buf2), "Brrrrrrrrr.");
            break;

        case EMOTE_SHRUG:
            snprintf(buf, sizeof(buf), "%s shrugs helplessly.", op->name);
            snprintf(buf2, sizeof(buf2), "You shrug.");
            break;

        case EMOTE_SMIRK:
            snprintf(buf, sizeof(buf), "%s smirks.", op->name);
            snprintf(buf2, sizeof(buf2), "You smirk.");
            break;

        case EMOTE_SNAP:
            snprintf(buf, sizeof(buf), "%s snaps his fingers.", op->name);
            snprintf(buf2, sizeof(buf2), "PRONTO! You snap your fingers.");
            break;

        case EMOTE_SNEEZE:
            snprintf(buf, sizeof(buf), "%s sneezes.", op->name);
            snprintf(buf2, sizeof(buf2), "Gesundheit!");
            break;

        case EMOTE_SNICKER:
            snprintf(buf, sizeof(buf), "%s snickers softly.", op->name);
            snprintf(buf2, sizeof(buf2), "You snicker softly.");
            break;

        case EMOTE_SNIFF:
            snprintf(buf, sizeof(buf), "%s sniffs sadly.", op->name);
            snprintf(buf2, sizeof(buf2), "You sniff sadly. *SNIFF*");
            break;

        case EMOTE_SNORE:
            snprintf(buf, sizeof(buf), "%s snores loudly.", op->name);
            snprintf(buf2, sizeof(buf2), "Zzzzzzzzzzzzzzz.");
            break;

        case EMOTE_SPIT:
            snprintf(buf, sizeof(buf), "%s spits over his left shoulder.", op->name);
            snprintf(buf2, sizeof(buf2), "You spit over your left shoulder.");
            break;

        case EMOTE_STRUT:
            snprintf(buf, sizeof(buf), "%s struts proudly.", op->name);
            snprintf(buf2, sizeof(buf2), "Strut your stuff.");
            break;

        case EMOTE_TWIDDLE:
            snprintf(buf, sizeof(buf), "%s patiently twiddles his thumbs.", op->name);
            snprintf(buf2, sizeof(buf2), "You patiently twiddle your thumbs.");
            break;

        case EMOTE_WAVE:
            snprintf(buf, sizeof(buf), "%s waves happily.", op->name);
            snprintf(buf2, sizeof(buf2), "You wave.");
            break;

        case EMOTE_WHISTLE:
            snprintf(buf, sizeof(buf), "%s whistles appreciatively.", op->name);
            snprintf(buf2, sizeof(buf2), "You whistle appreciatively.");
            break;

        case EMOTE_WINK:
            snprintf(buf, sizeof(buf), "%s winks suggestively.", op->name);
            snprintf(buf2, sizeof(buf2), "Have you got something in your eye?");
            break;

        case EMOTE_YAWN:
            snprintf(buf, sizeof(buf), "%s yawns sleepily.", op->name);
            snprintf(buf2, sizeof(buf2), "You open up your yap and let out a big breeze of stale air.");
            break;

        case EMOTE_CRINGE:
            snprintf(buf, sizeof(buf), "%s cringes in terror!", op->name);
            snprintf(buf2, sizeof(buf2), "You cringe in terror.");
            break;

        case EMOTE_BLEED:
            snprintf(buf, sizeof(buf), "%s is bleeding all over the carpet - got a spare tourniquet?", op->name);
            snprintf(buf2, sizeof(buf2), "You bleed all over your nice new armour.");
            break;

        case EMOTE_THINK:
            snprintf(buf, sizeof(buf), "%s closes his eyes and thinks really hard.", op->name);
            snprintf(buf2, sizeof(buf2), "Anything in particular that you'd care to think about?");
            break;

        default:
            snprintf(buf, sizeof(buf), "%s dances with glee.", op->name);
            snprintf(buf2, sizeof(buf2), "You are a nut.");
            break;
        } /*case*/
        ext_info_map_except(NDI_WHITE, op->map, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
                            buf, NULL);
        draw_ext_info(NDI_UNIQUE|NDI_WHITE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
                      buf2, NULL);
        return(0);
    } else {
        for (pl = first_player; pl != NULL; pl = pl->next) {
            if (strncasecmp(pl->ob->name, params, MAX_NAME) == 0
            && pl->ob->map == op->map
            && pl->ob != op
            && !(QUERY_FLAG(pl->ob, FLAG_WIZ) && pl->ob->contr->hidden)) {
                /* Hidden dms are not affected by emotions*/
                switch (emotion) {
                case EMOTE_NOD:
                    snprintf(buf, sizeof(buf), "You nod solemnly to %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s nods solemnly to you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s nods solemnly to %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_DANCE:
                    snprintf(buf, sizeof(buf), "You grab %s and begin doing the Cha-Cha!", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s grabs you, and begins dancing!", op->name);
                    snprintf(buf3, sizeof(buf3), "Yipe! %s and %s are doing the Macarena!", op->name, pl->ob->name);
                    break;

                case EMOTE_KISS:
                    snprintf(buf, sizeof(buf), "You kiss %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s kisses you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s kisses %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_BOUNCE:
                    snprintf(buf, sizeof(buf), "You bounce around the room with %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s bounces around the room with you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s bounces around the room with %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_SMILE:
                    snprintf(buf, sizeof(buf), "You smile at %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s smiles at you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s beams a smile at %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_LAUGH:
                    snprintf(buf, sizeof(buf), "You take one look at %s and fall down laughing.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s looks at you and falls down on the ground laughing.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s looks at %s and falls down on the ground laughing.", op->name, pl->ob->name);
                    break;

                case EMOTE_SHAKE:
                    snprintf(buf, sizeof(buf), "You shake %s's hand.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s shakes your hand.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s shakes %s's hand.", op->name, pl->ob->name);
                    break;

                case EMOTE_PUKE:
                    snprintf(buf, sizeof(buf), "You puke on %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s pukes on your clothes!", op->name);
                    snprintf(buf3, sizeof(buf3), "%s pukes on %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_HUG:
                    snprintf(buf, sizeof(buf), "You hug %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s hugs you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s hugs %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_CRY:
                    snprintf(buf, sizeof(buf), "You cry on %s's shoulder.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s cries on your shoulder.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s cries on %s's shoulder.", op->name, pl->ob->name);
                    break;

                case EMOTE_POKE:
                    snprintf(buf, sizeof(buf), "You poke %s in the ribs.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s pokes you in the ribs.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s pokes %s in the ribs.", op->name, pl->ob->name);
                    break;

                case EMOTE_ACCUSE:
                    snprintf(buf, sizeof(buf), "You look accusingly at %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s looks accusingly at you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s looks accusingly at %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_GRIN:
                    snprintf(buf, sizeof(buf), "You grin at %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s grins evilly at you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s grins evilly at %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_BOW:
                    snprintf(buf, sizeof(buf), "You bow before %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s bows before you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s bows before %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_FROWN:
                    snprintf(buf, sizeof(buf), "You frown darkly at %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s frowns darkly at you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s frowns darkly at %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_GLARE:
                    snprintf(buf, sizeof(buf), "You glare icily at %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s glares icily at you, you feel cold to your bones.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s glares at %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_LICK:
                    snprintf(buf, sizeof(buf), "You lick %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s licks you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s licks %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_SHRUG:
                    snprintf(buf, sizeof(buf), "You shrug at %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s shrugs at you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s shrugs at %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_SLAP:
                    snprintf(buf, sizeof(buf), "You slap %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "You are slapped by %s.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s slaps %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_SNEEZE:
                    snprintf(buf, sizeof(buf), "You sneeze at %s and a film of snot shoots onto him.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s sneezes on you, you feel the snot cover you. EEEEEEW.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s sneezes on %s and a film of snot covers him.", op->name, pl->ob->name);
                    break;

                case EMOTE_SNIFF:
                    snprintf(buf, sizeof(buf), "You sniff %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s sniffs you.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s sniffs %s", op->name, pl->ob->name);
                    break;

                case EMOTE_SPIT:
                    snprintf(buf, sizeof(buf), "You spit on %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s spits in your face!", op->name);
                    snprintf(buf3, sizeof(buf3), "%s spits in %s's face.", op->name, pl->ob->name);
                    break;

                case EMOTE_THANK:
                    snprintf(buf, sizeof(buf), "You thank %s heartily.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s thanks you heartily.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s thanks %s heartily.", op->name, pl->ob->name);
                    break;

                case EMOTE_WAVE:
                    snprintf(buf, sizeof(buf), "You wave goodbye to %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s waves goodbye to you. Have a good journey.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s waves goodbye to %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_WHISTLE:
                    snprintf(buf, sizeof(buf), "You whistle at %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s whistles at you.", op->name);
                    snprintf(buf2, sizeof(buf2), "%s whistles at %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_WINK:
                    snprintf(buf, sizeof(buf), "You wink suggestively at %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s winks suggestively at you.", op->name);
                    snprintf(buf2, sizeof(buf2), "%s winks at %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_BEG:
                    snprintf(buf, sizeof(buf), "You beg %s for mercy.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s begs you for mercy! Show no quarter!", op->name);
                    snprintf(buf2, sizeof(buf2), "%s begs %s for mercy!", op->name, pl->ob->name);
                    break;

                case EMOTE_BLEED:
                    snprintf(buf, sizeof(buf), "You slash your wrist and bleed all over %s", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s slashes his wrist and bleeds all over you.", op->name);
                    snprintf(buf2, sizeof(buf2), "%s slashes his wrist and bleeds all over %s.", op->name, pl->ob->name);
                    break;

                case EMOTE_CRINGE:
                    snprintf(buf, sizeof(buf), "You cringe away from %s.", pl->ob->name);
                    snprintf(buf2, sizeof(buf2), "%s cringes away from you.", op->name);
                    snprintf(buf2, sizeof(buf2), "%s cringes away from %s in mortal terror.", op->name, pl->ob->name);
                    break;

                default:
                    snprintf(buf, sizeof(buf), "You are still nuts.");
                    snprintf(buf2, sizeof(buf2), "You get the distinct feeling that %s is nuts.", op->name);
                    snprintf(buf3, sizeof(buf3), "%s is eyeing %s quizzically.", pl->ob->name, op->name);
                    break;
                } /*case*/
                draw_ext_info(NDI_UNIQUE|NDI_WHITE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
                              buf, NULL);
                draw_ext_info(NDI_UNIQUE|NDI_WHITE, 0, pl->ob, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
                              buf2, NULL);
                ext_info_map_except2(NDI_WHITE, op->map, op, pl->ob, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
                                     buf3, NULL);
                return(0);
            }
            if (strncasecmp(pl->ob->name, params, MAX_NAME) == 0
            && pl->ob->map == op->map
            && pl->ob == op) {
                switch (emotion) {
                case EMOTE_DANCE:
                    snprintf(buf, sizeof(buf), "You skip and dance around by yourself.");
                    snprintf(buf2, sizeof(buf2), "%s embraces himself and begins to dance!", op->name);
                    break;

                case EMOTE_LAUGH:
                    snprintf(buf, sizeof(buf), "Laugh at yourself all you want, the others won't understand.");
                    snprintf(buf2, sizeof(buf2), "%s is laughing at something.", op->name);
                    break;

                case EMOTE_SHAKE:
                    snprintf(buf, sizeof(buf), "You are shaken by yourself.");
                    snprintf(buf2, sizeof(buf2), "%s shakes and quivers like a bowlful of jelly.", op->name);
                    break;

                case EMOTE_PUKE:
                    snprintf(buf, sizeof(buf), "You puke on yourself.");
                    snprintf(buf2, sizeof(buf2), "%s pukes on his clothes.", op->name);
                    break;

                case EMOTE_HUG:
                    snprintf(buf, sizeof(buf), "You hug yourself.");
                    snprintf(buf2, sizeof(buf2), "%s hugs himself.", op->name);
                    break;

                case EMOTE_CRY:
                    snprintf(buf, sizeof(buf), "You cry to yourself.");
                    snprintf(buf2, sizeof(buf2), "%s sobs quietly to himself.", op->name);
                    break;

                case EMOTE_POKE:
                    snprintf(buf, sizeof(buf), "You poke yourself in the ribs, feeling very silly.");
                    snprintf(buf2, sizeof(buf2), "%s pokes himself in the ribs, looking very sheepish.", op->name);
                    break;

                case EMOTE_ACCUSE:
                    snprintf(buf, sizeof(buf), "You accuse yourself.");
                    snprintf(buf2, sizeof(buf2), "%s seems to have a bad conscience.", op->name);
                    break;

                case EMOTE_BOW:
                    snprintf(buf, sizeof(buf), "You kiss your toes.");
                    snprintf(buf2, sizeof(buf2), "%s folds up like a jackknife and kisses his own toes.", op->name);
                    break;

                case EMOTE_FROWN:
                    snprintf(buf, sizeof(buf), "You frown at yourself.");
                    snprintf(buf2, sizeof(buf2), "%s frowns at himself.", op->name);
                    break;

                case EMOTE_GLARE:
                    snprintf(buf, sizeof(buf), "You glare icily at your feet, they are suddenly very cold.");
                    snprintf(buf2, sizeof(buf2), "%s glares at his feet, what is bothering him?", op->name);
                    break;

                case EMOTE_LICK:
                    snprintf(buf, sizeof(buf), "You lick yourself.");
                    snprintf(buf2, sizeof(buf2), "%s licks himself - YUCK.", op->name);
                    break;

                case EMOTE_SLAP:
                    snprintf(buf, sizeof(buf), "You slap yourself, silly you.");
                    snprintf(buf2, sizeof(buf2), "%s slaps himself, really strange...", op->name);
                    break;

                case EMOTE_SNEEZE:
                    snprintf(buf, sizeof(buf), "You sneeze on yourself, what a mess!");
                    snprintf(buf2, sizeof(buf2), "%s sneezes, and covers himself in a slimy substance.", op->name);
                    break;

                case EMOTE_SNIFF:
                    snprintf(buf, sizeof(buf), "You sniff yourself.");
                    snprintf(buf2, sizeof(buf2), "%s sniffs himself.", op->name);
                    break;

                case EMOTE_SPIT:
                    snprintf(buf, sizeof(buf), "You drool all over yourself.");
                    snprintf(buf2, sizeof(buf2), "%s drools all over himself.", op->name);
                    break;

                case EMOTE_THANK:
                    snprintf(buf, sizeof(buf), "You thank yourself since nobody else wants to!");
                    snprintf(buf2, sizeof(buf2), "%s thanks himself since you won't.", op->name);
                    break;

                case EMOTE_WAVE:
                    snprintf(buf, sizeof(buf), "Are you going on adventures as well??");
                    snprintf(buf2, sizeof(buf2), "%s waves goodbye to himself.", op->name);
                    break;

                case EMOTE_WHISTLE:
                    snprintf(buf, sizeof(buf), "You whistle while you work.");
                    snprintf(buf2, sizeof(buf2), "%s whistles to himself in boredom.", op->name);
                    break;

                case EMOTE_WINK:
                    snprintf(buf, sizeof(buf), "You wink at yourself?? What are you up to?");
                    snprintf(buf2, sizeof(buf2), "%s winks at himself - something strange is going on...", op->name);
                    break;

                case EMOTE_BLEED:
                    snprintf(buf, sizeof(buf), "Very impressive! You wipe your blood all over yourself.");
                    snprintf(buf2, sizeof(buf2), "%s performs some satanic ritual while wiping his blood on himself.", op->name);
                    break;

                default:
                    snprintf(buf, sizeof(buf), "My god! is that LEGAL?");
                    snprintf(buf2, sizeof(buf2), "You look away from %s.", op->name);
                    break;
                }/*case*/
                draw_ext_info(NDI_UNIQUE|NDI_WHITE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
                              buf, NULL);
                ext_info_map_except(NDI_WHITE, op->map, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
                                    buf2, NULL);
                return(0);
            }/*if self*/
        }/*for*/
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "%s is not around.",
                             "%s is not around.",
                             params);
        return(1);
    } /*else*/

    return(0);
}

/*
 * everything from here on out are just wrapper calls to basic_emote
 */

/**
 * 'nod' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_nod(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_NOD));
}

/**
 * 'dance' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_dance(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_DANCE));
}

/**
 * 'kiss' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_kiss(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_KISS));
}

/**
 * 'bounce' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_bounce(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_BOUNCE));
}

/**
 * 'smile' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_smile(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SMILE));
}

/**
 * 'cackle' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_cackle(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_CACKLE));
}

/**
 * 'laugh' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_laugh(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_LAUGH));
}

/**
 * 'giggle' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_giggle(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_GIGGLE));
}

/**
 * 'shake' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_shake(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SHAKE));
}

/**
 * 'puke' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_puke(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_PUKE));
}

/**
 * 'growl' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_growl(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_GROWL));
}

/**
 * 'scream' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_scream(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SCREAM));
}

/**
 * 'sigh' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_sigh(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SIGH));
}

/**
 * 'sulk' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_sulk(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SULK));
}

/**
 * 'hug' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_hug(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_HUG));
}

/**
 * 'cry' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_cry(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_CRY));
}

/**
 * 'poke' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_poke(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_POKE));
}

/**
 * 'accuse' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_accuse(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_ACCUSE));
}

/**
 * 'grin' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_grin(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_GRIN));
}

/**
 * 'bow' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_bow(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_BOW));
}

/**
 * 'clap' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_clap(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_CLAP));
}

/**
 * 'blush' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_blush(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_BLUSH));
}

/**
 * 'burp' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_burp(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_BURP));
}

/**
 * 'chuckle' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_chuckle(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_CHUCKLE));
}

/**
 * 'cough' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_cough(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_COUGH));
}

/**
 * 'flip' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_flip(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_FLIP));
}

/**
 * 'frown' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_frown(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_FROWN));
}

/**
 * 'gasp' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_gasp(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_GASP));
}

/**
 * 'glare' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_glare(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_GLARE));
}

/**
 * 'groan' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_groan(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_GROAN));
}

/**
 * 'hiccup' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_hiccup(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_HICCUP));
}

/**
 * 'lick' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_lick(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_LICK));
}

/**
 * 'pout' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_pout(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_POUT));
}

/**
 * 'shiver' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_shiver(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SHIVER));
}

/**
 * 'shrug' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_shrug(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SHRUG));
}

/**
 * 'slap' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_slap(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SLAP));
}

/**
 * 'smirk' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_smirk(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SMIRK));
}

/**
 * 'snap' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_snap(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SNAP));
}

/**
 * 'sneeze' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_sneeze(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SNEEZE));
}

/**
 * 'snicker' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_snicker(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SNICKER));
}

/**
 * 'sniff' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_sniff(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SNIFF));
}

/**
 * 'snore' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_snore(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SNORE));
}

/**
 * 'spit' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_spit(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_SPIT));
}

/**
 * 'strut' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_strut(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_STRUT));
}

/**
 * 'thank' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_thank(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_THANK));
}

/**
 * 'twiddle' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_twiddle(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_TWIDDLE));
}

/**
 * 'wave' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_wave(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_WAVE));
}

/**
 * 'whistle' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_whistle(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_WHISTLE));
}

/**
 * 'wink' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_wink(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_WINK));
}

/**
 * 'yawn' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_yawn(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_YAWN));
}

/**
 * 'beg' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_beg(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_BEG));
}

/**
 * 'bleed' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_bleed(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_BLEED));
}

/**
 * 'cringe' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_cringe(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_CRINGE));
}

/**
 * 'think' command.
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_think(object *op, char *params) {
    return(basic_emote(op, params, EMOTE_THINK));
}
