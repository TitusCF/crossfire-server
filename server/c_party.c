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

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>

static partylist * firstparty=NULL; /* Keeps track of first party in list */
static partylist * lastparty=NULL; /*Keeps track of last party in list */

void remove_party(partylist *target_party);

/* Forms the party struct for a party called 'params'. it is the responsibility
 * of the caller to ensure that the name is unique, and that it is placed in the 
 * main party list correctly */
static partylist *form_party(object *op, const char *params) {

    partylist * newparty;

    newparty = (partylist *)malloc(sizeof(partylist));
    newparty->partyname = strdup_local(params);
    newparty->total_exp=0;
    newparty->kills=0;
    newparty->passwd[0] = '\0';
    newparty->next = NULL;
    newparty->partyleader = strdup_local(op->name);
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"You have formed party: %s",newparty->partyname);	
    op->contr->party=newparty;
    return newparty;
}

void remove_party(partylist *target_party) {
    partylist *tmpparty;
    partylist *previousparty;
    partylist *nextparty;
    player *pl;
    
    if (firstparty==NULL) {
	LOG(llevError, "remove_party(): I was asked to remove party %s, but no parties are defined",
	    target_party->partyname);
	return;
    }
    for (pl=first_player;pl!=NULL;pl=pl->next)
	if (pl->party==target_party) pl->party=NULL;
    
    /* special case-ism for parties at the beginning and end of the list */
    if (target_party==firstparty) {
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

/* Remove unused parties, this could be made to scale a lot better. */
void obsolete_parties() {
    int player_count;
    player *pl;
    partylist *party;

    if (!firstparty) return; /* we can't obsolete parties if there aren't any */
    for (party=firstparty; party!=NULL; party=party->next) {
	player_count=0; 
	for (pl=first_player;pl!=NULL;pl=pl->next)
	    if (pl->party==party) player_count++;
	if (player_count == 0)
	    remove_party(party);
    }
}

#ifdef PARTY_KILL_LOG
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

int confirm_party_password(object *op) {
    partylist *tmppartylist;
    for(tmppartylist = firstparty; tmppartylist != NULL;tmppartylist = tmppartylist->next) {
	if(!strcmp(op->contr->party_to_join->partyname, tmppartylist->partyname)) {
            if(strcmp(op->contr->write_buf+1,tmppartylist->passwd) == 0)
          	return 0;
	    else
		return 1;
	}
    tmppartylist = tmppartylist->next;
    }
    return 1;
}

void receive_party_password(object *op, char k) {
 
  if(confirm_party_password(op) == 0) {
    partylist* joined_party = op->contr->party_to_join;
    char buf[ MAX_BUF ];
    op->contr->party = op->contr->party_to_join;
    op->contr->party_to_join = NULL;
    new_draw_info_format(NDI_UNIQUE, 0,op,
	"You have joined party: %s\n",joined_party->partyname);
    snprintf( buf, MAX_BUF, "%s joins party %s", op->name, joined_party->partyname );
    send_party_message( op, buf );
    op->contr->state = ST_PLAYING;
    return;
  }
  else {
    new_draw_info(NDI_UNIQUE, 0,op,"You entered the wrong password");
    op->contr->party_to_join = NULL;
    op->contr->state = ST_PLAYING;
    return;
  }
}

void send_party_message(object *op,char *msg)
{
  player *pl;
  for(pl=first_player;pl!=NULL;pl=pl->next)
    if(pl->ob->contr->party==op->contr->party && pl->ob!=op)
	new_draw_info(NDI_WHITE, 0, pl->ob, msg);
}

int command_gsay(object *op, char *params)
{
  char party_params[MAX_BUF];

  if (!params) return 0;
  strcpy(party_params, "say "); 
  strcat(party_params,params);
  command_party(op,party_params);
  return 0;
}


int command_party (object *op, char *params)
{
  char buf[MAX_BUF];
  partylist *tmpparty, *oldparty;  /* For iterating over linked list */
  char * currentparty; 	 /* For iterating over linked list */

  if(params == NULL) {
        if(op->contr->party==NULL) {
          new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
          new_draw_info(NDI_UNIQUE, 0,op,"For help try: party help");
        }
        else {
          currentparty = op->contr->party->partyname;
	  new_draw_info_format(NDI_UNIQUE, 0, op,
		"You are a member of party %s.", currentparty);
        }
        return 1;
      }
  if(strcmp(params, "help")==0) {
    new_draw_info(NDI_UNIQUE, 0,op,"To form a party type: party form <partyname>");
    new_draw_info(NDI_UNIQUE, 0,op,"To join a party type: party join <partyname>");
    new_draw_info(NDI_UNIQUE, 0,op,"If the party has a passwd, it will you prompt you for it.");
    new_draw_info(NDI_UNIQUE, 0,op,"For a list of current parties type: party list");
    new_draw_info(NDI_UNIQUE, 0,op,"To leave a party type: party leave");
    new_draw_info(NDI_UNIQUE, 0,op,"To change a passwd for a party type: party passwd <password>");
    new_draw_info(NDI_UNIQUE, 0,op,"There is an 8 character max");
    new_draw_info(NDI_UNIQUE, 0,op,"To talk to party members type: party say <msg>");
    new_draw_info(NDI_UNIQUE, 0,op,"To see who is in your party: party who");
#ifdef PARTY_KILL_LOG
    new_draw_info(NDI_UNIQUE, 0,op,"To see what you've killed, type: party kills");
#endif
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
	  new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
	  return 1;
	}
      tmpparty = op->contr->party;
      if(!tmpparty->kills)
	{
	  new_draw_info(NDI_UNIQUE,0,op,"You haven't killed anything yet.");
	  return 1;
	}
      max=tmpparty->kills-1;
      if(max>PARTY_KILL_LOG-1) max=PARTY_KILL_LOG-1;
      new_draw_info(NDI_UNIQUE,0,op,
		    "Killed          |          Killer|     Exp"); 
      new_draw_info(NDI_UNIQUE,0,op,
		    "----------------+----------------+--------");
      for(i=0;i<=max;i++)
	{
	  exp=tmpparty->party_kills[i].exp;
	  chr=' ';
	  if(exp>1000000) { exp/=1000000; chr='M'; }
	  else
	    if(exp>1000) { exp/=1000; chr='k'; }
	  sprintf(buffer,"%16s|%16s|%6.1f%c",
		  tmpparty->party_kills[i].dead,
		  tmpparty->party_kills[i].killer,exp,chr);
	  new_draw_info(NDI_UNIQUE,0,op,buffer);
	}
      exp=tmpparty->total_exp;
      chr=' ';
      if(exp>1000000) { exp/=1000000; chr='M'; }
      else
	if(exp>1000) { exp/=1000; chr='k'; }
      new_draw_info(NDI_UNIQUE,0,op,
		    "----------------+----------------+--------");
      sprintf(buffer,"Totals: %d kills, %.1f%c exp",tmpparty->kills,
	      exp,chr);
      new_draw_info(NDI_UNIQUE,0,op,buffer);
      return 1;
    }
#endif /* PARTY_KILL_LOG */
  if(strncmp(params, "say ", 4)==0)
    {
         if(op->contr->party==NULL)
            {
              new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
              return 1;
            }
         params += 4;
         currentparty = op->contr->party->partyname;
         snprintf(buf,MAX_BUF-1, "[%s] %s says: %s", currentparty, op->name, params);
         send_party_message(op,buf);
         new_draw_info_format(NDI_WHITE, 0,op,"[%s] You say: %s", currentparty, params);
         return 1;
    }

  if(strncmp(params, "form ",5) == 0) {
    int player_count;
    player *pl;

    params += 5;
    if (op->contr->party) oldparty = op->contr->party;
 
    if (firstparty) {
	for (tmpparty = firstparty; tmpparty != NULL;tmpparty = tmpparty->next) {
	    if (!strcmp(tmpparty->partyname, params)) {
		new_draw_info_format(NDI_UNIQUE, 0,op,
		"The party %s already exists, pick another name",params);
        	return 1;
	    }
	}
    lastparty->next=form_party(op, params);
    lastparty = lastparty->next;
    }
    else {
	firstparty=form_party(op, params);
	lastparty=firstparty;
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
        new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
        return 1;
      }
    currentparty = op->contr->party->partyname;
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"You leave party %s.",currentparty);
    sprintf(buf,"%s leaves party %s.",op->name,currentparty);
    send_party_message(op,buf);
    op->contr->party=NULL;
    return 1;
  }
  if(strcmp(params, "who")==0) {
    player *pl;
    tmpparty = op->contr->party;
    if(op->contr->party==NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
      return 1;
    }
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"Members of party: %s.", op->contr->party->partyname);
    for(pl=first_player;pl!=NULL;pl=pl->next)
      if(pl->ob->contr->party==op->contr->party) {
	  if (settings.set_title == TRUE) {
	      if(pl->ob->contr->own_title[0]!='\0')
		  sprintf(buf,"%3d %s the %s",
		      pl->ob->level,pl->ob->name,pl->ob->contr->own_title);
	      else
		  sprintf(buf,"%3d %s the %s",
		      pl->ob->level,pl->ob->name,pl->ob->contr->title);
	  } else
	      sprintf(buf,"%3d %s the %s",
		  pl->ob->level,pl->ob->name,pl->ob->contr->title);
        new_draw_info(NDI_UNIQUE, 0,op,buf);
      }
    return 1;
  } /* leave */

  if(strncmp(params, "passwd ", 7) == 0) {
    partylist *tmplist;

    params += 7;

    if(op->contr->party == NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of a party");
      return 1;
    }

    if(strlen(params) > 8) {
      new_draw_info(NDI_UNIQUE, 0,op,"The password must not exceed 8 characters");
      return 1;
    }

    tmplist = firstparty;
    while(tmplist != NULL) {
      if(tmplist == op->contr->party) {
        strcpy(tmplist->passwd,params);
	    new_draw_info_format(NDI_UNIQUE, 0, op,
		    "The password for party %s is %s", tmplist->partyname,tmplist->passwd);
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
      new_draw_info(NDI_UNIQUE, 0,op,"There are no parties active right now");
      return 1;
    }

    new_draw_info(NDI_UNIQUE, 0,op,"Party name                       Leader");
    new_draw_info(NDI_UNIQUE, 0,op,"----------                       ------");

    while(tmplist != NULL) {
      new_draw_info_format(NDI_UNIQUE, 0,op,
	"%-32s %s",tmplist->partyname
              ,tmplist->partyleader);
      tmplist = tmplist->next;
    }
    return 0;
  } /* list */

  if(strncmp(params,"join ",5) == 0) {

    params += 5;

    /* Can't join a party cause non exist */
    if(firstparty == NULL) {
      new_draw_info_format(NDI_UNIQUE, 0, op,
	"Party: %s does not exist.  You must form it first",params);
      return 1;
    }

    /* Special case if thier is only one party */
    if(firstparty->next == NULL) {
      if(strcmp(firstparty->partyname,params) != 0) {
	new_draw_info_format(NDI_UNIQUE, 0,op,
	    "Party: %s does not exist. You must form it first",params);
        return 1;
      }
      else {
        if(op->contr->party == firstparty) {
	  new_draw_info_format(NDI_UNIQUE, 0, op,
		"You are already in party: %s"
                  ,firstparty->partyname);
          return 1;
        }
        /* found party player wants to join */
        if(firstparty->passwd[0] == '\0') {
          op->contr->party = firstparty;
	  new_draw_info_format(NDI_UNIQUE, 0, op,
	    	"You have joined party: %s",firstparty->partyname);
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
	  new_draw_info_format(NDI_UNIQUE, 0, op,
		"You are already a member of party: %s"
                  ,tmpparty->partyname);
          return 1;
        }
        else {
          if(tmpparty->passwd[0] == '\0') {
            new_draw_info_format(NDI_UNIQUE, 0, op,
		        "You have joined party: %s",tmpparty->partyname);
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

    new_draw_info_format(NDI_UNIQUE, 0,op,
	"Party %s does not exist.  You must form it first.",params);
    return 1;
  } /* join */

  new_draw_info(NDI_UNIQUE, 0,op,"To form a party type: party form <partyname>");
  new_draw_info(NDI_UNIQUE, 0,op,"To join a party type: party join <partyname>");
  new_draw_info(NDI_UNIQUE, 0,op,"If the party has a passwd, it will you prompt you for it.");
  new_draw_info(NDI_UNIQUE, 0,op,"For a list of current parties type: party list");
  new_draw_info(NDI_UNIQUE, 0,op,"To leave a party type: party leave");
  new_draw_info(NDI_UNIQUE, 0,op,"To change a passwd for a party type: party passwd <password>");
  new_draw_info(NDI_UNIQUE, 0,op,"There is an 8 character max");
  new_draw_info(NDI_UNIQUE, 0,op,"To talk to party members type: party say <msg>");
  new_draw_info(NDI_UNIQUE, 0,op,"To see who is in your party: party who");
#ifdef PARTY_KILL_LOG
  new_draw_info(NDI_UNIQUE, 0,op,"To see what you've killed, type: party kills");
#endif
  return 1;
}
