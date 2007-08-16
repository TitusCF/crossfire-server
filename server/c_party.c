/*
 * static char *rcsid_c_party_c =
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

/**
 * @file
 * Party-related functions and variables.
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>

static partylist * firstparty=NULL; /**< Keeps track of first party in list */
static partylist * lastparty=NULL;  /**< Keeps track of last party in list */

/**
 * Simple wrapper to get ::firstparty.
 * @return
 * ::firstparty.
 */
partylist* get_firstparty(void)
{
	return firstparty;
}

void remove_party(partylist *target_party);

/**
 * Forms the party struct for a party called 'params'. it is the responsibility
 * of the caller to ensure that the name is unique.
 * New item is placed on the party list.
 * @param op
 * party creator.
 * @param params
 * party name.
 * @return
 * new party.
 */
partylist *form_party(object *op, const char *params) {

    partylist * newparty;

    newparty = (partylist *)malloc(sizeof(partylist));
    newparty->partyname = strdup_local(params);
    newparty->total_exp=0;
    newparty->kills=0;
    newparty->passwd[0] = '\0';
    newparty->next = NULL;
    newparty->partyleader = strdup_local(op->name);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "You have formed party: %s",
			 "You have formed party: %s",
			 newparty->partyname);
    op->contr->party=newparty;

    if (lastparty) {
        lastparty->next=newparty;
        lastparty = lastparty->next;
    }
    else {
        firstparty=newparty;
        lastparty=firstparty;
    }

    return newparty;
}

/**
 * Remove and free party.
 *
 * @param target_party
 * party to remove.
 * @todo clean/simplify the mess.
 */
void remove_party(partylist *target_party) {
    partylist *tmpparty;
    partylist *previousparty;
    partylist *nextparty;
    player *pl;

    if (firstparty==NULL) {
	LOG(llevError, "remove_party(): I was asked to remove party %s, but no parties are defined\n",
	    target_party->partyname);
	return;
    }
    for (pl=first_player;pl!=NULL;pl=pl->next)
	if (pl->party==target_party) pl->party=NULL;

    /* special case-ism for parties at the beginning and end of the list */
    if (target_party==firstparty) {
        if (lastparty == target_party)
            lastparty = NULL;
	firstparty=firstparty->next;
	if (target_party->partyleader) free(target_party->partyleader);
	if (target_party->partyname) free(target_party->partyname);
	free(target_party);
	return;
    }
    else if (target_party == lastparty) {
	for (tmpparty=firstparty;tmpparty->next!=NULL;tmpparty=tmpparty->next) {
	    if (tmpparty->next==target_party) {
		lastparty=tmpparty;
		if (target_party->partyleader) free(target_party->partyleader);
		if (target_party->partyname) free(target_party->partyname);
		free(target_party);
		lastparty->next=NULL;
		return;
	    }
	}
    }
    for (tmpparty=firstparty;tmpparty->next!=NULL;tmpparty=tmpparty->next)
	if (tmpparty->next == target_party) {
	    previousparty=tmpparty;
	    nextparty=tmpparty->next->next;
	    /* this should be safe, because we already dealt with the lastparty case */

	    previousparty->next=nextparty;
	    if (target_party->partyleader) free(target_party->partyleader);
	    if (target_party->partyname) free(target_party->partyname);
	    free(target_party);
	    return;
	}
}

/**
 * Remove unused parties (no players), this could be made to scale a lot better.
 */
void obsolete_parties(void) {
    int player_count;
    player *pl;
    partylist *party;
	partylist* next = NULL;

    if (!firstparty) return; /* we can't obsolete parties if there aren't any */
    for (party=firstparty; party!=NULL; party=next) {
        next = party->next;
        player_count=0;
        for (pl=first_player;pl!=NULL;pl=pl->next)
            if (pl->party==party) player_count++;
        if (player_count == 0)
            remove_party(party);
    }
}

#ifdef PARTY_KILL_LOG
/**
 * Logs a kill for a party.
 *
 * @param party
 * party to log for.
 * @param killer
 * name of the killer.
 * @param dead
 * victim's name.
 * @param exp
 * how much experience was gained.
 * @todo use const char*.
 */
void add_kill_to_party(partylist *party, char *killer, char *dead, long exp)
{
  int i,pos;

  if(party==NULL) return;
  if(party->kills>=PARTY_KILL_LOG)
    {
      pos=PARTY_KILL_LOG-1;
      for(i=0;i<PARTY_KILL_LOG-1;i++)
	memcpy(&(party->party_kills[i]),&(party->party_kills[i+1]),
	       sizeof(party->party_kills[0]));
    }
  else
    pos=party->kills;
  party->kills++;
  party->total_exp+=exp;
  party->party_kills[pos].exp=exp;
  strncpy(party->party_kills[pos].killer,killer,MAX_NAME);
  strncpy(party->party_kills[pos].dead,dead,MAX_NAME);
  party->party_kills[pos].killer[MAX_NAME]=0;
  party->party_kills[pos].dead[MAX_NAME]=0;
}
#endif

/**
 * Is the password the player entered to join a party the right one?
 *
 * @param op
 * player. Must have party_to_join correctly set.
 * @retval 0
 * password is correct.
 * @retval 1
 * invalid password or party not found.
 */
int confirm_party_password(object *op) {
    partylist *tmppartylist;
    for(tmppartylist = firstparty; tmppartylist != NULL;tmppartylist = tmppartylist->next) {
	if(!strcmp(op->contr->party_to_join->partyname, tmppartylist->partyname)) {
            if(strcmp(op->contr->write_buf+1,tmppartylist->passwd) == 0)
          	return 0;
	    else
		return 1;
	}
    }
    return 1;
}

/**
 * Player entered a party password.
 *
 * @param op
 * player.
 */
void receive_party_password(object *op) {

  if(confirm_party_password(op) == 0) {
    partylist* joined_party = op->contr->party_to_join;
    char buf[ MAX_BUF ];
    op->contr->party = op->contr->party_to_join;
    op->contr->party_to_join = NULL;
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "You have joined party: %s\n",
			 "You have joined party: %s\n",
			 joined_party->partyname);
    snprintf( buf, MAX_BUF, "%s joins party %s", op->name, joined_party->partyname );
    send_party_message( op, buf );
    op->contr->state = ST_PLAYING;
    return;
  }
  else {
    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		  "You entered the wrong password", NULL);
    op->contr->party_to_join = NULL;
    op->contr->state = ST_PLAYING;
    return;
  }
}

/**
 * Send a message to all party members except the speaker.
 *
 * @param op
 * player talking.
 * @param msg
 * message to send.
 */
void send_party_message(object *op,char *msg)
{
  player *pl;
  for(pl=first_player;pl!=NULL;pl=pl->next)
    if(pl->ob->contr->party==op->contr->party && pl->ob!=op)
	draw_ext_info(NDI_WHITE, 0, pl->ob, MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_PARTY,
		      msg, NULL);
}

/**
 * 'gsay' command, talks to party.
 *
 * @param op
 * player.
 * @param params
 * message.
 * @return
 * 0.
 */
int command_gsay(object *op, char *params)
{
  char party_params[MAX_BUF];

  if (!params) {
      draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR, "Say what?", NULL);
      return 0;
  }
  strcpy(party_params, "say ");
  strcat(party_params,params);
  command_party(op,party_params);
  return 0;
}

/**
 * Give help for party commands.
 *
 * @param op
 * player.
 */
static void party_help(object *op)
{
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_HELP,
		  "To form a party type: party form <partyname>. "
		  "To join a party type: party join <partyname> "
		  "If the party has a passwd, it will you prompt you for it. "
		  "For a list of current parties type: party list. "
		  "To leave a party type: party leave "
		  "To change a passwd for a party type: party passwd <password> "
		  "There is an 8 character maximum password length. "
		  "To talk to party members type: party say <msg> "
		  "To see who is in your party: party who "
#ifdef PARTY_KILL_LOG
		  "To see what you've killed, type: party kills"
#endif
		  , NULL);
}

/**
 * 'party' command, subdivided in different sub commands.
 *
 * @param op
 * player.
 * @param params
 * additional parameters.
 * 1.
 * @todo split in different functions. clean the 'form' mess.
 */
int command_party (object *op, char *params)
{
  char buf[MAX_BUF];
  partylist *tmpparty, *oldparty;  /* For iterating over linked list */
  char * currentparty; 	 /* For iterating over linked list */

  if(params == NULL) {
        if(op->contr->party==NULL) {
          draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			"You are not a member of any party. "
			"For help try: party help", NULL);
        }
        else {
          currentparty = op->contr->party->partyname;
	  draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		       "You are a member of party %s.",
		       "You are a member of party %s.",
		       currentparty);
        }
        return 1;
      }
  if(strcmp(params, "help")==0) {
    party_help(op);
    return 1;
  }
#ifdef PARTY_KILL_LOG
  if(!strncmp(params, "kills",5))
    {
      int i,max;
      char chr;
      char buffer[80];
      float exp;

      if(op->contr->party==NULL)
	{
	  draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			"You are not a member of any party.", NULL);
	  return 1;
	}
      tmpparty = op->contr->party;
      if(!tmpparty->kills)
	{
	  draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			"You haven't killed anything yet.", NULL);
	  return 1;
	}
      max=tmpparty->kills-1;
      if(max>PARTY_KILL_LOG-1) max=PARTY_KILL_LOG-1;
      draw_ext_info(NDI_UNIQUE,0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
	    "[fixed]Killed          |          Killer|     Exp\n----------------+----------------+--------"
	    "Killed          |          Killer|     Exp\n----------------+----------------+--------",
		    NULL);


      for(i=0;i<=max;i++)
	{
	  exp=tmpparty->party_kills[i].exp;
	  chr=' ';
	  if(exp>1000000) { exp/=1000000; chr='M'; }
	  else
	    if(exp>1000) { exp/=1000; chr='k'; }

	  draw_ext_info_format(NDI_UNIQUE,0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		       "[fixed]%16s|%16s|%6.1f%c",
		       "%16s|%16s|%6.1f%c",
		  tmpparty->party_kills[i].dead,
		  tmpparty->party_kills[i].killer,exp,chr);

	}
      exp=tmpparty->total_exp;
      chr=' ';
      if(exp>1000000) { exp/=1000000; chr='M'; }
      else
	if(exp>1000) { exp/=1000; chr='k'; }

      draw_ext_info(NDI_UNIQUE,0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "[fixed]----------------+----------------+--------",
		    "----------------+----------------+--------");
      draw_ext_info_format(NDI_UNIQUE,0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			   "Totals: %d kills, %.1f%c exp",tmpparty->kills,
			   "Totals: %d kills, %.1f%c exp",tmpparty->kills,
			   exp,chr);
      return 1;
    }
#endif /* PARTY_KILL_LOG */
  if(strncmp(params, "say ", 4)==0)
    {
         if(op->contr->party==NULL)
            {
              draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			    "You are not a member of any party.", NULL);
              return 1;
            }
         params += 4;
         currentparty = op->contr->party->partyname;
         snprintf(buf,MAX_BUF-1, "<%s> %s says: %s", currentparty, op->name, params);
         send_party_message(op,buf);
         draw_ext_info_format(NDI_WHITE, 0,op,MSG_TYPE_COMMUNICATION, MSG_TYPE_COMMUNICATION_PARTY,
			      "<%s> You say: %s",
			      "<%s> You say: %s",
			      currentparty, params);
         return 1;
    }

  if(strncmp(params, "form ",5) == 0) {
    int player_count;
    player *pl;

    params += 5;
    if (op->contr->party) oldparty = op->contr->party;
    else oldparty = NULL;

    if (firstparty) {
	for (tmpparty = firstparty; tmpparty != NULL;tmpparty = tmpparty->next) {
	    if (!strcmp(tmpparty->partyname, params)) {
		draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				     "The party %s already exists, pick another name",
				     "The party %s already exists, pick another name",
				     params);
        	return 1;
	    }
	}
        form_party(op, params);
    }
    else {
        form_party(op, params);
    }
    /*
     * The player might have previously been a member of a party, if so, he will be leaving
     * it, so check if there are any other members and if not, delete the party
     */
    player_count=0;
    if (oldparty) {
    	for (pl=first_player;pl->next!=NULL;pl=pl->next) {
	    if (pl->party==oldparty) player_count++;
	}
	if (player_count == 0)
	    remove_party(oldparty);
    }
    return 0;
  } /* form */

  if(strcmp(params, "leave")==0) {
    if(op->contr->party==NULL)
      {
        draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You are not a member of any party.", NULL);
        return 1;
      }
    currentparty = op->contr->party->partyname;
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "You leave party %s.",
			 "You leave party %s.",
			 currentparty);
    sprintf(buf,"%s leaves party %s.",op->name,currentparty);
    send_party_message(op,buf);
    op->contr->party=NULL;
    return 1;
  }
  if(strcmp(params, "who")==0) {
    player *pl;
    tmpparty = op->contr->party;
    if(op->contr->party==NULL) {
      draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "You are not a member of any party.", NULL);
      return 1;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "Members of party: %s.",
			 "Members of party: %s.",
			 op->contr->party->partyname);

    for(pl=first_player;pl!=NULL;pl=pl->next)
	if(pl->ob->contr->party==op->contr->party) {
	    if (settings.set_title == TRUE && pl->ob->contr->own_title[0]!='\0') {
		draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			     "[fixed]%3d %s the %s",
			     "%3d %s the %s",
			     pl->ob->level,pl->ob->name,pl->ob->contr->own_title);
	    } else {
		draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			     "[fixed]%3d %s the %s",
			     "%3d %s the %s",
			     pl->ob->level,pl->ob->name,pl->ob->contr->title);
	    }
	}
	return 1;
  } /* leave */

  if(strncmp(params, "passwd ", 7) == 0) {
    partylist *tmplist;

    params += 7;

    if(op->contr->party == NULL) {
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "You are not a member of a party", NULL);
      return 1;
    }

    if(strlen(params) > 8) {
      draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "The password must not exceed 8 characters", NULL);
      return 1;
    }

    tmplist = firstparty;
    while(tmplist != NULL) {
      if(tmplist == op->contr->party) {
        strcpy(tmplist->passwd,params);
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		    "The password for party %s is %s",
		    "The password for party %s is %s",
		     tmplist->partyname,tmplist->passwd);

        snprintf( buf, MAX_BUF, "Password for party %s is now %s, changed by %s",
            tmplist->partyname, tmplist->passwd, op->name );
        send_party_message(op,buf);
        return 0;
      }
      tmplist = tmplist->next;
    }
    return 0;
  } /* passwd */

  if(strcmp(params, "list") == 0) {
    partylist * tmplist;

    tmplist = firstparty;

    if(firstparty == NULL) {
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "There are no parties active right now", NULL);
      return 1;
    }

    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
	  "[fixed]Party name                       Leader\n----------                       ------",
	  "Party name                       Leader\n----------                       ------");

    while(tmplist != NULL) {
      draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			   "[fixed]%-32s %s",
			   "%-32s %s",
			   tmplist->partyname, tmplist->partyleader);
      tmplist = tmplist->next;
    }
    return 0;
  } /* list */

  if(strncmp(params,"join ",5) == 0) {

    params += 5;

    /* Can't join a party cause non exist */
    if(firstparty == NULL) {
      draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			   "Party: %s does not exist.  You must form it first",
			   "Party: %s does not exist.  You must form it first",
			   params);
      return 1;
    }

    /* Special case if thier is only one party */
    if(firstparty->next == NULL) {
      if(strcmp(firstparty->partyname,params) != 0) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "Party: %s does not exist. You must form it first",
			     "Party: %s does not exist. You must form it first",
			     params);
        return 1;
      }
      else {
        if(op->contr->party == firstparty) {
	  draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
	       "You are already in party: %s",
	       "You are already in party: %s",
	       firstparty->partyname);
          return 1;
        }
        /* found party player wants to join */
        if(firstparty->passwd[0] == '\0') {
          op->contr->party = firstparty;
	  draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			       "You have joined party: %s",
			       "You have joined party: %s",
			       firstparty->partyname);
          snprintf( buf, MAX_BUF, "%s joins party %s", op->name, firstparty->partyname );
          send_party_message( op, buf );
          return 0;
        }
        else {
          get_party_password(op,firstparty);
          return 0;
        }
      }
    }

    tmpparty = firstparty;
    while(tmpparty != NULL) {
      if(strcmp(tmpparty->partyname,params) == 0) {
        if(op->contr->party == tmpparty) {
	  draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			       "You are already a member of party: %s",
			       "You are already a member of party: %s",
			       tmpparty->partyname);
          return 1;
        }
        else {
          if(tmpparty->passwd[0] == '\0') {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		        "You have joined party: %s",
		        "You have joined party: %s",
			 tmpparty->partyname);
            op->contr->party = tmpparty;
            snprintf( buf, MAX_BUF, "%s joins party %s", op->name, tmpparty->partyname );
            send_party_message( op, buf );
            return 0;
          }
          else {
            get_party_password(op, tmpparty);
            return 0;
          }
        }
      }
      else
        tmpparty = tmpparty->next;
    }

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
	 "Party %s does not exist.  You must form it first.",
	 "Party %s does not exist.  You must form it first.",
	 params);
    return 1;
  } /* join */

  party_help(op);
  return 1;
}

/** Valid modes for 'party_rejoin'. @todo document that */
static const char* rejoin_modes[] = {
    "no",
    "if_exists",
    "always",
    NULL
};

/**
 * Handles the 'party_rejoin' command.
 * @param op
 * player.
 * @param params
 * optional parameters.
 * @return
 * 1.
 */
int command_party_rejoin(object *op, char *params) {
    int mode;
    if (!params) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
            "party rejoin: %s", NULL, rejoin_modes[op->contr->rejoin_party]);
        return 1;
    }
    for (mode = 0; rejoin_modes[mode] != NULL; mode++) {
        if (strcmp(rejoin_modes[mode], params) == 0) {
            op->contr->rejoin_party = mode;
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
                "party rejoin is now: %s", NULL, rejoin_modes[op->contr->rejoin_party]);
            return 1;
        }
    }
    if (strlen(params) > 50)
        params[50] = '\0';
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
        "invalid mode: %s", NULL, params);
    return 1;
}
