/*
 * static char *rcsid_input_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copryight (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to master@rahul.net
*/

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <version.h>
#include <spells.h>

static partylist * firstparty=NULL; /* Keeps track of first party in list */
static partylist * lastparty=NULL; /*Keeps track of last party in list */

partylist * form_party(object *op, char *params, partylist * firstparty, partylist * lastparty) {

    partylist * newparty;
    int nextpartyid;

    if(firstparty == NULL) {
        nextpartyid = 1;
    } else {
        nextpartyid = lastparty->partyid;
        nextpartyid++;
    }

    newparty = (partylist *)malloc(sizeof(partylist));
    newparty->partyid = nextpartyid;
    newparty->partyname = strdup_local(params);
    newparty->total_exp=0;
    newparty->kills=0;

    newparty->passwd[0] = '\0';
    newparty->next = NULL;
    newparty->partyleader = strdup_local(op->name);
    if(firstparty != NULL)
        lastparty->next = newparty;
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"You have formed party: %s",newparty->partyname);
    return newparty;
}

char * find_party(int partynumber, partylist * party) {

    while(party != NULL) {
        if(party->partyid == partynumber)
            return party->partyname;
        else
            party = party->next;
    }
    return NULL;
}

partylist *find_party_struct(int partynumber)
{
  partylist *party;

  party=firstparty;
  while(party!=NULL)
    {
      if(party->partyid==partynumber) return party;
      else
	party=party->next;
    }
  return NULL;
}

#ifdef PARTY_KILL_LOG
void add_kill_to_party(int numb,char *killer,char *dead,long exp)
{
  partylist *party;
  int i,pos;

  party=find_party_struct(numb);
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
  partylist * tmppartylist;

  tmppartylist = firstparty;
  while(tmppartylist != NULL) {
    if(op->contr->party_number_to_join == tmppartylist->partyid) {
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
    op->contr->party_number = op->contr->party_number_to_join;
    op->contr->party_number_to_join = (-1);
    new_draw_info_format(NDI_UNIQUE, 0,op,
	"You have joined party: %s\n",find_party(op->contr->party_number
	,firstparty));
    op->contr->state = ST_PLAYING;
    return;
  }
  else {
    new_draw_info(NDI_UNIQUE, 0,op,"You entered the wrong password");
    op->contr->party_number_to_join = (-1);
    op->contr->state = ST_PLAYING;
    return;
  }
}

void send_party_message(object *op,char *msg)
{
  player *pl;
  int no=op->contr->party_number;
  for(pl=first_player;pl!=NULL;pl=pl->next)
    if(pl->ob->contr->party_number==no && pl->ob!=op)
	new_draw_info(NDI_UNIQUE, NDI_WHITE, pl->ob, msg);
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
  partylist * tmpparty; /* For iterating over linked list */
  char * currentparty; /* For iterating over linked list */

  if(params == NULL) {
        if(op->contr->party_number<=0) {
          new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
          new_draw_info(NDI_UNIQUE, 0,op,"For help try: party help");
        }
        else {
          currentparty = find_party(op->contr->party_number,firstparty);
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
    new_draw_info(NDI_UNIQUE, 0,op,"To change a passwd for a party type: party passwd <password>"\
);
    new_draw_info(NDI_UNIQUE, 0,op,"There is an 8 character max");
    new_draw_info(NDI_UNIQUE, 0,op,"To talk to party members type: party say <msg>");
    new_draw_info(NDI_UNIQUE, 0,op,"To see who is in your party: party who");
    new_draw_info(NDI_UNIQUE, 0,op,"To see what you've killed, type: party kills");
    return 1;
  }
#ifdef PARTY_KILL_LOG
  if(!strncmp(params, "kills",5))
    {
      int i,max;
      char chr;
      char buffer[80];
      float exp;

      if(op->contr->party_number<=0)
	{
	  new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
	  return 1;
	}
      tmpparty = find_party_struct(op->contr->party_number);
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
          if(op->contr->party_number<=0)
            {
              new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
              return 1;
            }
      params += 4;
      sprintf(buf, "%s says: %s", op->name, params);
          send_party_message(op,buf);
          new_draw_info(NDI_UNIQUE, 0,op,"Ok.");
          return 1;
        }

  if(strncmp(params, "form ",5) == 0) {

    params += 5;

    if(firstparty == NULL) {
      firstparty = form_party(op, params, firstparty, lastparty);
      lastparty = firstparty;
      return 0;
    }
    else
      tmpparty = firstparty->next;

    if(tmpparty == NULL) {
      if(strcmp(firstparty->partyname, params) != 0) {
        lastparty = form_party(op, params, firstparty, lastparty);
        return 0;
      }
      else {
	new_draw_info_format(NDI_UNIQUE, 0,op,
		"The party %s already exists, pick another name",params);
        return 1;
      }
    }
    tmpparty=firstparty;
    while(tmpparty != NULL) {
      if(strcmp(tmpparty->partyname,params) == 0) {
        new_draw_info_format(NDI_UNIQUE, 0, op,
		"The party %s already exists, pick a new name",params);
        return 1;
      }
      tmpparty = tmpparty->next;
    }

    lastparty = form_party(op, params, firstparty, lastparty);
    return 0;
  } /* form */

  if(strcmp(params, "leave")==0) {
    if(op->contr->party_number<=0)
      {
        new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
        return 1;
      }
    currentparty = find_party(op->contr->party_number,firstparty);
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"You leave party %s.",currentparty);
    sprintf(buf,"%s leaves party %s.",op->name,currentparty);
    send_party_message(op,buf);
    op->contr->party_number=-1;
    return 1;
  }
  if(strcmp(params, "who")==0) {
    player *pl;
    int no=op->contr->party_number;
    if(no<=0) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of any party.");
      return 1;
    }
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"Members of party: %s.",find_party(no,firstparty));
    for(pl=first_player;pl!=NULL;pl=pl->next)
      if(pl->ob->contr->party_number==no) {
#ifdef SET_TITLE
        if(pl->ob->contr->own_title[0]!='\0')
          sprintf(buf,"%3d %s the %s",
                  pl->ob->level,pl->ob->name,pl->ob->contr->own_title);
        else
#endif /* SET_TITLE */
          sprintf(buf,"%3d %s the %s",
                  pl->ob->level,pl->ob->name,pl->ob->contr->title);
        new_draw_info(NDI_UNIQUE, 0,op,buf);
      }
    return 1;
  } /* leave */

  if(strncmp(params, "passwd ", 7) == 0) {
    partylist *tmplist;

    params += 7;

    if(op->contr->party_number <= 0) {
      new_draw_info(NDI_UNIQUE, 0,op,"You are not a member of a party");
      return 1;
    }

    tmplist = firstparty;
    while(tmplist != NULL) {
      if(tmplist->partyid == op->contr->party_number) {
        strncpy(tmplist->passwd,params,8);
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"The password for party %s is %s"
                ,tmplist->partyname,tmplist->passwd);
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
        if(op->contr->party_number == firstparty->partyid) {
	  new_draw_info_format(NDI_UNIQUE, 0, op,
		"You are already in party: %s"
                  ,firstparty->partyname);
          return 1;
        }
        /* found party player wants to join */
        if(firstparty->passwd[0] == '\0') {
          op->contr->party_number = firstparty->partyid;
	  new_draw_info_format(NDI_UNIQUE, 0, op,
		"You have joined party: %s",firstparty->partyname);
          return 0;
        }
        else {
          get_party_password(op,firstparty->partyid);
          return 0;
        }
      }
    }

    tmpparty = firstparty;
    while(tmpparty != NULL) {
      if(strcmp(tmpparty->partyname,params) == 0) {
        if(op->contr->party_number == tmpparty->partyid) {
	  new_draw_info_format(NDI_UNIQUE, 0, op,
		"You are already a member of party: %s"
                  ,tmpparty->partyname);
          return 1;
        }
        else {
          if(tmpparty->passwd[0] == '\0') {
	    new_draw_info_format(NDI_UNIQUE, 0, op,
		"You have joined party: %s",tmpparty->partyname);
            op->contr->party_number = tmpparty->partyid;
            return 0;
          }
          else {
            get_party_password(op, tmpparty->partyid);
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
  new_draw_info(NDI_UNIQUE, 0,op,"If the party has a passwd, it will you prompt you for it.");
  new_draw_info(NDI_UNIQUE, 0,op,"For a list of current parties type: party list");
  new_draw_info(NDI_UNIQUE, 0,op,"To leave a party type: party leave");
  new_draw_info(NDI_UNIQUE, 0,op,"To change a passwd for a party type: party passwd <password>");
  new_draw_info(NDI_UNIQUE, 0,op,"There is an 8 character max");
  new_draw_info(NDI_UNIQUE, 0,op,"To talk to party members type: party say <msg>");
  new_draw_info(NDI_UNIQUE, 0,op,"To see who is in your party: party who");
  return 1;
}


