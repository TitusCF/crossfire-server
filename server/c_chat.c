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

#include <global.h>
#include <loader.h>
#include <sproto.h>

int command_say (object *op, char *params)
{
    char buf[MAX_BUF];

    if (!params) return 0;
    snprintf(buf, MAX_BUF-1, "%s says: %s",op->name, params);
    ext_info_map(NDI_WHITE,op->map, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_SAY,
		 buf, NULL);
    communicate(op, params);
  
    return 0;
}


int command_me (object *op, char *params)
{
    char buf[MAX_BUF];

    if (!params) return 0;
    snprintf(buf, MAX_BUF-1, "%s %s",op->name, params);
    ext_info_map(NDI_UNIQUE|NDI_BLUE,op->map, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_ME,
		 buf, NULL);

    return 0;
}


int command_cointoss(object *op, char *params)
{
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
    int i;

    i = rndm(1, 2);
    if (i == 1) {
	snprintf(buf, MAX_BUF-1, "%s flips a coin.... Heads!", op->name);
	snprintf(buf2, MAX_BUF-1, "You flip a coin.... Heads!");
    } else {
	snprintf(buf, MAX_BUF-1, "%s flips a coin.... Tails!", op->name);
	snprintf(buf2, MAX_BUF-1, "You flip a coin.... Tails!");
    }
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
		  buf2, NULL);
    ext_info_map_except(NDI_WHITE, op->map, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
			buf, NULL);
    return 0;
}

static const char* const orcknuckle[7] = {"none", "beholder", "ghost", "knight",
		       "princess", "dragon", "orc"};

int command_orcknuckle(object *op, char *params)
{
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
    int i, j, k, l;

    i = rndm(1, 5);
    j = rndm(1, 5);
    k = rndm(1, 5);
    l = rndm(1, 6);

    snprintf(buf2, MAX_BUF-1, "%s rolls %s, %s, %s, %s!", op->name,
	orcknuckle[i], orcknuckle[j], orcknuckle[k], orcknuckle[l]);
    snprintf(buf, MAX_BUF-1, "You roll %s, %s, %s, %s!",
	orcknuckle[i], orcknuckle[j], orcknuckle[k], orcknuckle[l]);

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
		  buf, NULL);
    ext_info_map_except(NDI_UNIQUE, op->map, op, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_RANDOM,
			buf2, NULL);
    return 0;
}

static int command_tell_all(object *op, char *params, int pri, int color, const char *desc)
{
    if (op->contr->no_shout == 1){
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You are no longer allowed to shout or chat.", NULL);
	return 1;
    } else {
	if (params == NULL) {
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  "Shout/Chat what?", NULL);
	    return 1;
	}
	draw_ext_info_format(NDI_UNIQUE | NDI_ALL | color, pri, NULL, 
		     MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
		     "%s %s: %s", 
		     "%s %s: %s", 
		     op->name, desc, params);
 
        /* Lauwenmark : Here we handle the SHOUT global event */
        execute_global_event(EVENT_SHOUT,op,params,pri);
	return 1;
    }
}

int command_shout (object *op, char *params)
{
    return command_tell_all(op, params, 1, NDI_RED, "shouts");
}

int command_chat (object *op, char *params)
{
    return command_tell_all(op, params, 9, NDI_BLUE, "chats");
}



int command_tell (object *op, char *params)
{
    char buf[MAX_BUF],*name = NULL ,*msg = NULL;
    player *pl;

    if ( params != NULL){
        name = params;
        msg = strchr(name, ' ');
        if(msg){
	     *(msg++)=0;
	     if(*msg == 0)
		msg = NULL;
        }
    }

    if( name == NULL ){
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Tell whom what?", NULL);
	return 1;
    } else if ( msg == NULL){
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "Tell %s what?", 
			     "Tell %s what?", 
			     name);
	return 1;
    }

    snprintf(buf,MAX_BUF-1, "%s tells you: %s",op->name, msg);

    pl = find_player_partial_name( name );

    if ( pl )
        {
	    draw_ext_info(NDI_UNIQUE | NDI_ORANGE, 0, pl->ob, 
			  MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
			  buf, NULL);
	    draw_ext_info_format(NDI_UNIQUE | NDI_ORANGE, 0, op,
			 MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
			 "You tell %s: %s", 
			 pl->ob->name, msg);

	    /* Update last_tell value [mids 01/14/2002] */
	    strcpy(pl->last_tell, op->name);
	    return 1;
        }

    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		  "No such player or ambiguous name.", NULL);
    return 1;
}

/* Reply to last person who told you something [mids 01/14/2002] */
int command_reply (object *op, char *params) {
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

    draw_ext_info_format(NDI_UNIQUE | NDI_ORANGE, 0, pl->ob, 
		 MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
		 "%s tells you: %s", 
		 "%s tells you: %s", 
		 op->name, params);

    draw_ext_info_format(NDI_UNIQUE | NDI_ORANGE, 0, op, 
		 MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_TELL,
		 "You tell to %s: %s", 
		 "You tell to %s: %s", 
		 pl->ob->name, params);
    return 1;
}

/*
 * This function covers basic emotions a player can have.  An emotion can be
 * one of three things currently.  Directed at oneself, directed at someone,
 * or directed at nobody.  The first set is nobody, the second at someone, and
 * the third is directed at oneself.  Every emotion does not have to be
 * filled out in every category.  The default case will take care of the ones
 * that are not.  Helper functions will call basic_emote with the proper
 * arguments, translating them into commands.  Adding a new emotion can be
 * done by editing command.c and command.h.
 * [garbled 09-25-2001]
 */

static int basic_emote(object *op, char *params, int emotion)
{
    char buf[MAX_BUF], buf2[MAX_BUF], buf3[MAX_BUF];
    player *pl;

    if (!params) {
	switch(emotion) {
	case EMOTE_NOD:
	    sprintf(buf, "%s nods solemnly.", op->name);
	    sprintf(buf2, "You nod solemnly.");
	    break;
	case EMOTE_DANCE:
	    sprintf(buf, "%s expresses himself through interpretive dance.",
		    op->name);
	    sprintf(buf2, "You dance with glee.");
	    break;
	case EMOTE_KISS:
	    sprintf(buf, "%s makes a weird facial contortion", op->name);
	    sprintf(buf2, "All the lonely people..");
	    break;
	case EMOTE_BOUNCE:
	    sprintf(buf, "%s bounces around.", op->name);
	    sprintf(buf2, "BOIINNNNNNGG!");
	    break;
	case EMOTE_SMILE:
	    sprintf(buf, "%s smiles happily.", op->name);
	    sprintf(buf2, "You smile happily.");
	    break;
	case EMOTE_CACKLE:
	    sprintf(buf, "%s throws back his head and cackles with insane "
		    "glee!", op->name);
	    sprintf(buf2, "You cackle gleefully.");
	    break;
        case EMOTE_LAUGH:
	    sprintf(buf, "%s falls down laughing.", op->name);
	    sprintf(buf2, "You fall down laughing.");
	    break;
	case EMOTE_GIGGLE:
	    sprintf(buf, "%s giggles.", op->name);
	    sprintf(buf2, "You giggle.");
	    break;
	case EMOTE_SHAKE:
	    sprintf(buf, "%s shakes his head.", op->name);
	    sprintf(buf2, "You shake your head.");
	    break;
	case EMOTE_PUKE:
	    sprintf(buf, "%s pukes.", op->name);
	    sprintf(buf2, "Bleaaaaaghhhhhhh!");
	    break;
	case EMOTE_GROWL:
	    sprintf(buf, "%s growls.", op->name);
	    sprintf(buf2, "Grrrrrrrrr....");
	    break;
	case EMOTE_SCREAM:
	    sprintf(buf, "%s screams at the top of his lungs!", op->name);
	    sprintf(buf2, "ARRRRRRRRRRGH!!!!!");
	    break;
	case EMOTE_SIGH:
	    sprintf(buf, "%s sighs loudly.", op->name);
	    sprintf(buf2, "You sigh.");
	    break;
	case EMOTE_SULK:
	    sprintf(buf, "%s sulks in the corner.", op->name);
	    sprintf(buf2, "You sulk.");
	    break;
	case EMOTE_CRY:
	    sprintf(buf, "%s bursts into tears.", op->name);
	    sprintf(buf2, "Waaaaaaahhh..");
	    break;
	case EMOTE_GRIN:
	    sprintf(buf, "%s grins evilly.", op->name);
	    sprintf(buf2, "You grin evilly.");
	    break;
	case EMOTE_BOW:
	    sprintf(buf, "%s bows deeply.", op->name);
	    sprintf(buf2, "You bow deeply.");
	    break;
	case EMOTE_CLAP:
	    sprintf(buf, "%s gives a round of applause.", op->name);
	    sprintf(buf2, "Clap, clap, clap.");
	    break;
	case EMOTE_BLUSH:
	    sprintf(buf, "%s blushes.", op->name);
	    sprintf(buf2, "Your cheeks are burning.");
	    break;
	case EMOTE_BURP:
	    sprintf(buf, "%s burps loudly.", op->name);
	    sprintf(buf2, "You burp loudly.");
	    break;
	case EMOTE_CHUCKLE:
	    sprintf(buf, "%s chuckles politely.", op->name);
	    sprintf(buf2, "You chuckle politely");
	    break;
	case EMOTE_COUGH:
	    sprintf(buf, "%s coughs loudly.", op->name);
	    sprintf(buf2, "Yuck, try to cover your mouth next time!");
	    break;
	case EMOTE_FLIP:
	    sprintf(buf, "%s flips head over heels.", op->name);
	    sprintf(buf2, "You flip head over heels.");
	    break;
	case EMOTE_FROWN:
	    sprintf(buf, "%s frowns.", op->name);
	    sprintf(buf2, "What's bothering you?");
	    break;
	case EMOTE_GASP:
	    sprintf(buf, "%s gasps in astonishment.", op->name);
	    sprintf(buf2, "You gasp in astonishment.");
	    break;
	case EMOTE_GLARE:
	    sprintf(buf, "%s glares around him.", op->name);
	    sprintf(buf2, "You glare at nothing in particular.");
	    break;
	case EMOTE_GROAN:
	    sprintf(buf, "%s groans loudly.", op->name);
	    sprintf(buf2, "You groan loudly.");
	    break;
	case EMOTE_HICCUP:
	    sprintf(buf, "%s hiccups.", op->name);
	    sprintf(buf2, "*HIC*");
	    break;
	case EMOTE_LICK:
	    sprintf(buf, "%s licks his mouth and smiles.", op->name);
	    sprintf(buf2, "You lick your mouth and smile.");
	    break;
	case EMOTE_POUT:
	    sprintf(buf, "%s pouts.", op->name);
	    sprintf(buf2, "Aww, don't take it so hard.");
	    break;
	case EMOTE_SHIVER:
	    sprintf(buf, "%s shivers uncomfortably.", op->name);
	    sprintf(buf2, "Brrrrrrrrr.");
	    break;
	case EMOTE_SHRUG:
	    sprintf(buf, "%s shrugs helplessly.", op->name);
	    sprintf(buf2, "You shrug.");
	    break;
	case EMOTE_SMIRK:
	    sprintf(buf, "%s smirks.", op->name);
	    sprintf(buf2, "You smirk.");
	    break;
	case EMOTE_SNAP:
	    sprintf(buf, "%s snaps his fingers.", op->name);
	    sprintf(buf2, "PRONTO! You snap your fingers.");
	    break;
	case EMOTE_SNEEZE:
	    sprintf(buf, "%s sneezes.", op->name);
	    sprintf(buf2, "Gesundheit!");
	    break;
	case EMOTE_SNICKER:
	    sprintf(buf, "%s snickers softly.", op->name);
	    sprintf(buf2, "You snicker softly.");
	    break;
	case EMOTE_SNIFF:
	    sprintf(buf, "%s sniffs sadly.", op->name);
	    sprintf(buf2, "You sniff sadly. *SNIFF*");
	    break;
	case EMOTE_SNORE:
	    sprintf(buf, "%s snores loudly.", op->name);
	    sprintf(buf2, "Zzzzzzzzzzzzzzz.");
	    break;
	case EMOTE_SPIT:
	    sprintf(buf, "%s spits over his left shoulder.", op->name);
	    sprintf(buf2, "You spit over your left shoulder.");
	    break;
	case EMOTE_STRUT:
	    sprintf(buf, "%s struts proudly.", op->name);
	    sprintf(buf2, "Strut your stuff.");
	    break;
	case EMOTE_TWIDDLE:
	    sprintf(buf, "%s patiently twiddles his thumbs.", op->name);
	    sprintf(buf2, "You patiently twiddle your thumbs.");
	    break;
	case EMOTE_WAVE:
	    sprintf(buf, "%s waves happily.", op->name);
	    sprintf(buf2, "You wave.");
	    break;
	case EMOTE_WHISTLE:
	    sprintf(buf, "%s whistles appreciatively.", op->name);
	    sprintf(buf2, "You whistle appreciatively.");
	    break;
	case EMOTE_WINK:
	    sprintf(buf, "%s winks suggestively.", op->name);
	    sprintf(buf2, "Have you got something in your eye?");
	    break;
	case EMOTE_YAWN:
	    sprintf(buf, "%s yawns sleepily.", op->name);
	    sprintf(buf2, "You open up your yap and let out a big breeze "
		    "of stale air.");
	    break;
	case EMOTE_CRINGE:
	    sprintf(buf, "%s cringes in terror!", op->name);
	    sprintf(buf2, "You cringe in terror.");
	    break;
	case EMOTE_BLEED:
	    sprintf(buf, "%s is bleeding all over the carpet"
		    " - got a spare tourniquet?", op->name);
	    sprintf(buf2, "You bleed all over your nice new armour.");
	    break;
	case EMOTE_THINK:
	    sprintf(buf, "%s closes his eyes and thinks really hard.",
		    op->name);
	    sprintf(buf2, "Anything in particular that you'd care to think "
		    "about?");
	    break;
	default:
	    sprintf(buf, "%s dances with glee.", op->name);
	    sprintf(buf2, "You are a nut.");
	    break;
	} /*case*/
	ext_info_map_except(NDI_WHITE, op->map, op, 
		    MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
		    buf, NULL);
	draw_ext_info(NDI_UNIQUE|NDI_WHITE, 0, op,
		      MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
		      buf2, NULL);
	return(0);
    } else {
        for(pl=first_player;pl!=NULL;pl=pl->next) {
    	  if(strncasecmp(pl->ob->name, params, MAX_NAME)==0 &&
            pl->ob->map == op->map && pl->ob != op &&
            !(QUERY_FLAG(pl->ob,FLAG_WIZ) && pl->ob->contr->hidden)) {
            /* Hidden dms are not affected by emotions*/
		switch(emotion) {
		case EMOTE_NOD:
		    sprintf(buf, "You nod solemnly to %s.", pl->ob->name);
		    sprintf(buf2, "%s nods solemnly to you.", op->name);
		    sprintf(buf3, "%s nods solemnly to %s.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_DANCE:
		    sprintf(buf, "You grab %s and begin doing the Cha-Cha!",
			    pl->ob->name);
		    sprintf(buf2, "%s grabs you, and begins dancing!",
			    op->name);
		    sprintf(buf3, "Yipe! %s and %s are doing the Macarena!",
			    op->name, pl->ob->name);
		    break;
		case EMOTE_KISS:
		    sprintf(buf, "You kiss %s.", pl->ob->name);
		    sprintf(buf2, "%s kisses you.", op->name);
		    sprintf(buf3, "%s kisses %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_BOUNCE:
		    sprintf(buf, "You bounce around the room with %s.",
			    pl->ob->name);
		    sprintf(buf2, "%s bounces around the room with you.",
			    op->name);
		    sprintf(buf3, "%s bounces around the room with %s.",
			    op->name, pl->ob->name);
		    break;
		case EMOTE_SMILE:
		    sprintf(buf, "You smile at %s.", pl->ob->name);
		    sprintf(buf2, "%s smiles at you.", op->name);
		    sprintf(buf3, "%s beams a smile at %s.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_LAUGH:
		    sprintf(buf, "You take one look at %s and fall down "
			    "laughing.", pl->ob->name);
		    sprintf(buf2, "%s looks at you and falls down on the "
			    "ground laughing.", op->name);
		    sprintf(buf3, "%s looks at %s and falls down on the "
			    "ground laughing.", op->name, pl->ob->name);
		    break;
		case EMOTE_SHAKE:
		    sprintf(buf, "You shake %s's hand.", pl->ob->name);
		    sprintf(buf2, "%s shakes your hand.", op->name);
		    sprintf(buf3, "%s shakes %s's hand.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_PUKE:
		    sprintf(buf, "You puke on %s.", pl->ob->name);
		    sprintf(buf2, "%s pukes on your clothes!", op->name);
		    sprintf(buf3, "%s pukes on %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_HUG:
		    sprintf(buf, "You hug %s.", pl->ob->name);
		    sprintf(buf2, "%s hugs you.", op->name);
		    sprintf(buf3, "%s hugs %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_CRY:
		    sprintf(buf, "You cry on %s's shoulder.", pl->ob->name);
		    sprintf(buf2, "%s cries on your shoulder.", op->name);
		    sprintf(buf3, "%s cries on %s's shoulder.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_POKE:
		    sprintf(buf, "You poke %s in the ribs.", pl->ob->name);
		    sprintf(buf2, "%s pokes you in the ribs.", op->name);
		    sprintf(buf3, "%s pokes %s in the ribs.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_ACCUSE:
		    sprintf(buf, "You look accusingly at %s.", pl->ob->name);
		    sprintf(buf2, "%s looks accusingly at you.", op->name);
		    sprintf(buf3, "%s looks accusingly at %s.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_GRIN:
		    sprintf(buf, "You grin at %s.", pl->ob->name);
		    sprintf(buf2, "%s grins evilly at you.", op->name);
		    sprintf(buf3, "%s grins evilly at %s.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_BOW:
		    sprintf(buf, "You bow before %s.", pl->ob->name);
		    sprintf(buf2, "%s bows before you.", op->name);
		    sprintf(buf3, "%s bows before %s.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_FROWN:
		    sprintf(buf, "You frown darkly at %s.", pl->ob->name);
		    sprintf(buf2, "%s frowns darkly at you.", op->name);
		    sprintf(buf3, "%s frowns darkly at %s.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_GLARE:
		    sprintf(buf, "You glare icily at %s.", pl->ob->name);
		    sprintf(buf2, "%s glares icily at you, you feel cold to"
			    " your bones.", op->name);
		    sprintf(buf3, "%s glares at %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_LICK:
		    sprintf(buf, "You lick %s.", pl->ob->name);
		    sprintf(buf2, "%s licks you.", op->name);
		    sprintf(buf3, "%s licks %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_SHRUG:
		    sprintf(buf, "You shrug at %s.", pl->ob->name);
		    sprintf(buf2, "%s shrugs at you.", op->name);
		    sprintf(buf3, "%s shrugs at %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_SLAP:
		    sprintf(buf, "You slap %s.", pl->ob->name);
		    sprintf(buf2, "You are slapped by %s.", op->name);
		    sprintf(buf3, "%s slaps %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_SNEEZE:
		    sprintf(buf, "You sneeze at %s and a film of snot shoots"
			    " onto him.", pl->ob->name);
		    sprintf(buf2, "%s sneezes on you, you feel the snot cover"
			    " you. EEEEEEW.", op->name);
		    sprintf(buf3, "%s sneezes on %s and a film of snot covers"
			    " him.", op->name, pl->ob->name);
		    break;
		case EMOTE_SNIFF:
		    sprintf(buf, "You sniff %s.", pl->ob->name);
		    sprintf(buf2, "%s sniffs you.", op->name);
		    sprintf(buf3, "%s sniffs %s", op->name, pl->ob->name);
		    break;
		case EMOTE_SPIT:
		    sprintf(buf, "You spit on %s.", pl->ob->name);
		    sprintf(buf2, "%s spits in your face!", op->name);
		    sprintf(buf3, "%s spits in %s's face.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_THANK:
		    sprintf(buf, "You thank %s heartily.", pl->ob->name);
		    sprintf(buf2, "%s thanks you heartily.", op->name);
		    sprintf(buf3, "%s thanks %s heartily.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_WAVE:
		    sprintf(buf, "You wave goodbye to %s.", pl->ob->name);
		    sprintf(buf2, "%s waves goodbye to you. Have a good"
			    " journey.", op->name);
		    sprintf(buf3, "%s waves goodbye to %s.", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_WHISTLE:
		    sprintf(buf, "You whistle at %s.", pl->ob->name);
		    sprintf(buf2, "%s whistles at you.", op->name);
		    sprintf(buf2, "%s whistles at %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_WINK:
		    sprintf(buf, "You wink suggestively at %s.", pl->ob->name);
		    sprintf(buf2, "%s winks suggestively at you.", op->name);
		    sprintf(buf2, "%s winks at %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_BEG:
		    sprintf(buf, "You beg %s for mercy.", pl->ob->name);
		    sprintf(buf2, "%s begs you for mercy! Show no quarter!",
			    op->name);
		    sprintf(buf2, "%s begs %s for mercy!", op->name,
			    pl->ob->name);
		    break;
		case EMOTE_BLEED:
		    sprintf(buf, "You slash your wrist and bleed all over %s",
			    pl->ob->name);
		    sprintf(buf2, "%s slashes his wrist and bleeds all over"
			    " you.", op->name);
		    sprintf(buf2, "%s slashes his wrist and bleeds all "
			    "over %s.", op->name, pl->ob->name);
		    break;
		case EMOTE_CRINGE:
		    sprintf(buf, "You cringe away from %s.", pl->ob->name);
		    sprintf(buf2, "%s cringes away from you.", op->name);
		    sprintf(buf2, "%s cringes away from %s in mortal terror.",
			    op->name, pl->ob->name);
		    break;
		default:
		  sprintf(buf, "You are still nuts.");
		  sprintf(buf2, "You get the distinct feeling that %s is nuts.",
			  op->name);
		  sprintf(buf3, "%s is eyeing %s quizzically.", pl->ob->name,
			  op->name);
		  break;
		} /*case*/
		draw_ext_info(NDI_UNIQUE|NDI_WHITE, 0, op, 
			      MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
			      buf, NULL);
		draw_ext_info(NDI_UNIQUE|NDI_WHITE, 0, pl->ob, 
			      MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
			      buf2, NULL);
		ext_info_map_except2(NDI_WHITE, op->map, op, pl->ob,
			     MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
			     buf3, NULL);
		return(0);
	    }
	    if(strncasecmp(pl->ob->name, params, MAX_NAME)==0 &&
	       pl->ob->map == op->map && pl->ob == op) {
	        switch(emotion) {
		case EMOTE_DANCE:
		    sprintf(buf, "You skip and dance around by yourself.");
		    sprintf(buf2, "%s embraces himself and begins to dance!",
			    op->name);
		    break;
		case EMOTE_LAUGH:
		    sprintf(buf, "Laugh at yourself all you want, the others "
			    "won't understand.");
		    sprintf(buf2, "%s is laughing at something.", op->name);
		    break;
		case EMOTE_SHAKE:
		    sprintf(buf, "You are shaken by yourself.");
		    sprintf(buf2, "%s shakes and quivers like a bowlful of "
			    "jelly.", op->name);
		    break;
		case EMOTE_PUKE:
		    sprintf(buf, "You puke on yourself.");
		    sprintf(buf2, "%s pukes on his clothes.", op->name);
		    break;
		case EMOTE_HUG:
		    sprintf(buf, "You hug yourself.");
		    sprintf(buf2, "%s hugs himself.", op->name);
		    break;
		case EMOTE_CRY:
		    sprintf(buf, "You cry to yourself.");
		    sprintf(buf2, "%s sobs quietly to himself.", op->name);
		    break;
		case EMOTE_POKE:
		    sprintf(buf, "You poke yourself in the ribs, feeling very"
			    " silly.");
		    sprintf(buf2, "%s pokes himself in the ribs, looking very"
			    " sheepish.", op->name);
		    break;
		case EMOTE_ACCUSE:
		    sprintf(buf, "You accuse yourself.");
		    sprintf(buf2, "%s seems to have a bad conscience.",
			    op->name);
		    break;
		case EMOTE_BOW:
		    sprintf(buf, "You kiss your toes.");
		    sprintf(buf2, "%s folds up like a jackknife and kisses his"
			    " own toes.", op->name);
		    break;
		case EMOTE_FROWN:
		    sprintf(buf, "You frown at yourself.");
		    sprintf(buf2, "%s frowns at himself.", op->name);
		    break;
		case EMOTE_GLARE:
		    sprintf(buf, "You glare icily at your feet, they are "
			    "suddenly very cold.");
		    sprintf(buf2, "%s glares at his feet, what is bothering "
			    "him?", op->name);
		    break;
		case EMOTE_LICK:
		    sprintf(buf, "You lick yourself.");
		    sprintf(buf2, "%s licks himself - YUCK.", op->name);
		    break;
		case EMOTE_SLAP:
		    sprintf(buf, "You slap yourself, silly you.");
		    sprintf(buf2, "%s slaps himself, really strange...",
			    op->name);
		    break;
		case EMOTE_SNEEZE:
		    sprintf(buf, "You sneeze on yourself, what a mess!");
		    sprintf(buf2, "%s sneezes, and covers himself in a slimy"
			    " substance.", op->name);
		    break;
		case EMOTE_SNIFF:
		    sprintf(buf, "You sniff yourself.");
		    sprintf(buf2, "%s sniffs himself.", op->name);
		    break;
		case EMOTE_SPIT:
		    sprintf(buf, "You drool all over yourself.");
		    sprintf(buf2, "%s drools all over himself.", op->name);
		    break;
		case EMOTE_THANK:
		    sprintf(buf, "You thank yourself since nobody else "
			    "wants to!");
		    sprintf(buf2, "%s thanks himself since you won't.",
			    op->name);
		    break;
		case EMOTE_WAVE:
		    sprintf(buf, "Are you going on adventures as well??");
		    sprintf(buf2, "%s waves goodbye to himself.", op->name);
		    break;
		case EMOTE_WHISTLE:
		    sprintf(buf, "You whistle while you work.");
		    sprintf(buf2, "%s whistles to himself in boredom.",
			    op->name);
		    break;
		case EMOTE_WINK:
		    sprintf(buf, "You wink at yourself?? What are you up to?");
		    sprintf(buf2, "%s winks at himself - something strange "
			    "is going on...", op->name);
		    break;
		case EMOTE_BLEED:
		    sprintf(buf, "Very impressive! You wipe your blood all "
			    "over yourself.");
		    sprintf(buf2, "%s performs some satanic ritual while "
			    "wiping his blood on himself.", op->name);
		    break;
		default:
		    sprintf(buf, "My god! is that LEGAL?");
		    sprintf(buf2, "You look away from %s.", op->name);
		    break;
		}/*case*/
		draw_ext_info(NDI_UNIQUE|NDI_WHITE, 0, op,
			      MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
			      buf, NULL);
		ext_info_map_except(NDI_WHITE, op->map, op, 
			    MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_EMOTE,
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

int command_nod(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_NOD));
}

int command_dance(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_DANCE));
}

int command_kiss(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_KISS));
}

int command_bounce(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_BOUNCE));
}

int command_smile(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SMILE));
}

int command_cackle(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_CACKLE));
}

int command_laugh(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_LAUGH));
}

int command_giggle(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_GIGGLE));
}

int command_shake(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SHAKE));
}

int command_puke(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_PUKE));
}

int command_growl(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_GROWL));
}

int command_scream(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SCREAM));
}

int command_sigh(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SIGH));
}

int command_sulk(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SULK));
}

int command_hug(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_HUG));
}

int command_cry(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_CRY));
}

int command_poke(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_POKE));
}

int command_accuse(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_ACCUSE));
}

int command_grin(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_GRIN));
}

int command_bow(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_BOW));
}

int command_clap(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_CLAP));
}

int command_blush(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_BLUSH));
}

int command_burp(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_BURP));
}

int command_chuckle(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_CHUCKLE));
}

int command_cough(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_COUGH));
}

int command_flip(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_FLIP));
}

int command_frown(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_FROWN));
}

int command_gasp(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_GASP));
}

int command_glare(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_GLARE));
}

int command_groan(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_GROAN));
}

int command_hiccup(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_HICCUP));
}

int command_lick(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_LICK));
}

int command_pout(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_POUT));
}

int command_shiver(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SHIVER));
}

int command_shrug(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SHRUG));
}

int command_slap(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SLAP));
}

int command_smirk(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SMIRK));
}

int command_snap(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SNAP));
}

int command_sneeze(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SNEEZE));
}

int command_snicker(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SNICKER));
}

int command_sniff(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SNIFF));
}

int command_snore(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SNORE));
}

int command_spit(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_SPIT));
}

int command_strut(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_STRUT));
}

int command_thank(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_THANK));
}

int command_twiddle(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_TWIDDLE));
}

int command_wave(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_WAVE));
}

int command_whistle(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_WHISTLE));
}

int command_wink(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_WINK));
}

int command_yawn(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_YAWN));
}

int command_beg(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_BEG));
}

int command_bleed(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_BLEED));
}

int command_cringe(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_CRINGE));
}

int command_think(object *op, char *params)
{
    return(basic_emote(op, params, EMOTE_THINK));
}
