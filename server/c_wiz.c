/*
 * static char *rcsid_c_wiz_c =
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

/*
 * End of non-DM commands.  DM-only commands below.
 * (This includes commands directly from socket)
 */

/* Some commands not accepted from socket */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <treasure.h>
#include <skills.h>

/* Enough of the DM functions seem to need this that I broke
 * it out to a seperate function.  name is the person
 * being saught, rq is who is looking for them.  This
 * prints diagnostics messages, and returns the 
 * other player, or NULL otherwise.
 */
static player *get_other_player_from_name(object *op, char *name)
{
    player *pl;

    if (!name) return NULL;

    for(pl=first_player;pl!=NULL;pl=pl->next) 
	if(!strncmp(pl->ob->name, name, MAX_NAME)) 
	    break;

    if(pl==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"No such player.");
	return NULL;
    }

    if (pl->ob == op) {
	new_draw_info(NDI_UNIQUE, 0, op, "You can't do that to yourself.");
	return NULL;
    }
    if(pl->state != ST_PLAYING) {
	new_draw_info(NDI_UNIQUE, 0,op,"That player is in no state for that right now.");
	return NULL;
    }
    return pl;
}


int command_loadtest(object *op, char *params){
	int x,y;
	char buf[1024];
	/*This command will stress server*/
	new_draw_info(NDI_UNIQUE, 0,op, "loadtest will stress server through teleporting");
	new_draw_info(NDI_UNIQUE, 0,op, "at different map places.");
	new_draw_info(NDI_UNIQUE, 0,op, "use at your own risks.");
	new_draw_info(NDI_UNIQUE, 0,op, "Very long loop used so server may have to be reset.");
	new_draw_info(NDI_UNIQUE, 0,op, "type loadtest TRUE to run");
	new_draw_info_format(NDI_UNIQUE, 0, op,"{%s}",params);
	if (!params)
		return 0;
	if (strncmp (params,"TRUE",4))
		return 0;
	new_draw_info_format(NDI_UNIQUE, 0, op,"gogogo",params);
	for (x=0; x<settings.worldmaptilesx; x++)
	for (y=0; y<settings.worldmaptilesy; y++){
		sprintf (buf,"/world/world_%d_%d",x+settings.worldmapstartx,y+settings.worldmapstarty);
		/*new_draw_info_format(NDI_UNIQUE, 0, op,"going to %s",buf);*/
		command_goto (op, buf);
	}
	return 0;
}

int command_hide(object *op, char *params)
{
    if (op->contr->hidden) {
        op->contr->hidden=0;
        new_draw_info(NDI_UNIQUE, 0,op, "You are no longer hidden from other players");
	op->map->players++;
	new_draw_info_format(NDI_UNIQUE | NDI_ALL, 5, NULL,
             "%s has entered the game.",op->name);
    }
    else {
        op->contr->hidden=1;
        new_draw_info(NDI_UNIQUE, 0,op, "Other players will no longer see you.");
	op->map->players--;
	new_draw_info_format(NDI_UNIQUE | NDI_ALL, 5, NULL,
             "%s left the game.",op->name);
    }
    return 1;
}


/* This finds and returns the object which matches the name or
 * object nubmer (specified via num #whatever).
 */

static object *find_object_both(char *params)
{
    if (!params) return NULL;
    if(params[0]=='#') return find_object(atol(params+1));
    else return find_object_name(params);
}

/* Sets the god for some objects.  params should contain two values -
 * first the object to change, followed by the god to change it to.
 */
int command_setgod(object *op, char *params)
{
    object *ob, *god;
    char    *str;

    if (!params || !(str=strchr(params,' '))) {
	new_draw_info(NDI_UNIQUE,0,op,"Usage: setgod object god");
	return 0;
    }
    /* kill the space, and set string to the next param */
    *str++='\0';
    if(!(ob=find_object_both(params))) {
    	new_draw_info_format(NDI_UNIQUE,0,op,"Set whose god - can not find object %s?",params);
    	return 1;
    }
    /* Perhaps this is overly restrictive?  Should we perhaps be able
     * to rebless altars and the like?
     */
    if (ob->type!=PLAYER) {
    	new_draw_info_format(NDI_UNIQUE,0,op,"%s is not a player - can not change its god",ob->name);
    	return 1;
    }
    god = find_god(str);
    if(god==NULL) {
    	new_draw_info_format(NDI_UNIQUE,0,op,"No such god %s.",str);
    	return 1;
    }
    become_follower(ob,god);
    return 1;
}

/*Add player's IP to ban_file and kick them off the server
* I know most people have dynamic IPs but this is more of a short term
* solution if they have to get a new IP to play maybe they'll calm down.
* This uses the banish_file in the local directory *not* the ban_file
* The action is logged with a ! for easy searching. -tm
*/
int command_banish (object *op, char *params)
  {
	  player *pl;
	  FILE *banishfile;
	  char  buf[MAX_BUF];
	  
	if (!params) {
         new_draw_info(NDI_UNIQUE, 0,op,"Usage: banish <player>.");
         return 1;
    }
	
	pl = get_other_player_from_name(op, params);
	if (!pl) return 1;
	
	sprintf (buf, "%s/%s", settings.localdir, BANISHFILE);
	
    if ((banishfile = fopen(buf, "a")) == NULL) {
    	LOG (llevDebug, "Could not find file banish_file.\n");
		new_draw_info(NDI_UNIQUE,0,op,"Could not find banish_file.");
    	return(0);
  		}
		
  	fprintf(banishfile,"*@%s\n",pl->socket.host);
	LOG (llevDebug, "! %s banned %s from IP: %s.\n", op->name, pl->ob->name, pl->socket.host);
  	new_draw_info_format(NDI_UNIQUE | NDI_RED, 0,op,"You banish %s", pl->ob->name);
	new_draw_info_format(NDI_UNIQUE | NDI_ALL | NDI_RED, 5, op,
			     "%s banishes %s from the land!", op->name, pl->ob->name);
	command_kick(op, pl->ob->name);
    fclose(banishfile);
	return 1;
  }
  
int command_kick (object *op, char *params)
{
    struct pl *pl;

    for(pl=first_player;pl!=NULL;pl=pl->next) 
	if((params==NULL || !strcmp(pl->ob->name,params)) && pl->ob!=op) {
	    object *op;
	    int removed=0;

	    op=pl->ob;
	    if (!QUERY_FLAG(op, FLAG_REMOVED)) {
		remove_ob(op);
		removed=1;
	    }
	    op->direction=0;
	    new_draw_info_format(NDI_UNIQUE | NDI_ALL | NDI_RED, 5, op,
			     "%s is kicked out of the game.",op->name);
	    strcpy(op->contr->killer,"left");
	    check_score(op); /* Always check score */
	    if (!removed) {
		(void)save_player(op,0);
		op->map->players--;
	    }
#if MAP_MAXTIMEOUT
	    op->map->timeout = MAP_TIMEOUT(op->map);
#endif
	    pl->socket.status = Ns_Dead;
	}
    return 1;
}

int command_save_overlay(object *op, char *params)
{
    if (!op)
	return(0);

    if (op!=NULL && !QUERY_FLAG(op, FLAG_WIZ)) {
	new_draw_info(NDI_UNIQUE, 0, op,
	    "Sorry, you can't force an overlay save.");
	return(1);
    }
    new_save_map(op->map, 2);
    new_save_map(op->map, 0);
    new_draw_info(NDI_UNIQUE, 0, op, "Current map has been saved as an"
	" overlay.");

    ready_map_name(op->map->path, 0);

    return(1);
} 
/*a simple toggle for the no_shout field.

*/
int command_toggle_shout(object *op, char *params)
{
	player *pl;
	
	if (!params) {
         new_draw_info(NDI_UNIQUE, 0,op,"Usage: toggle_shout <player>.");
         return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl) return 1;
	
	if (pl->ob->contr->no_shout == 0){
		pl->ob->contr->no_shout = 1;
    new_draw_info(NDI_UNIQUE | NDI_RED, 0, pl->ob,
			 "You have been muzzled by the DM!");
    new_draw_info_format(NDI_UNIQUE , 0, op,
			 "You toggle shout for %s.", pl->ob->name);
		return 1;
	}else{
		pl->ob->contr->no_shout = 0;
    new_draw_info(NDI_UNIQUE | NDI_ORANGE, 0, pl->ob,
			 "You are allowed to shout again.");
    new_draw_info_format(NDI_UNIQUE , 0, op,
			 "You toggle shout for %s.", pl->ob->name);
		return 1;
	}
	
}

int command_shutdown(object *op, char *params)
{

    if(op!=NULL && !QUERY_FLAG(op,FLAG_WIZ)) 
    {
	new_draw_info(NDI_UNIQUE,0,op,"Sorry, you can't shutdown the server.");
	return 1;
    }
    /* We need to give op - command_kick expects it.  however, this means
     * the op won't get kicked off, so we do it ourselves
     */
    command_kick(op,NULL);
    check_score(op); /* Always check score */
    (void)save_player(op,0);
    play_again(op);
    cleanup();
    /* not reached */
    return 1;
}

int command_goto (object *op, char *params)
{
    char *name;
    object *dummy;

    if (!op)
	return 0;

    if(params == NULL) {
        new_draw_info(NDI_UNIQUE, 0,op,"Go to what level?");
        return 1;
      }
    name = params;
    dummy=get_object();
    dummy->map = op->map;
    EXIT_PATH(dummy) = add_string (name);
    dummy->name = add_string(name);

    enter_exit(op,dummy);
    free_object(dummy);
    if(op->contr->loading == NULL) {
	new_draw_info_format(NDI_UNIQUE, 0, op,
	    "Difficulty: %d.",op->map->difficulty);
    }
    return 1;
}

/* is this function called from somewhere ? -Tero */
int command_generate (object *op, char *params)
{
      object *tmp;
      int nr = 1, i, retry;

  if (!op)
    return 0;

  if (params != NULL)
    sscanf(params, "%d", &nr);
      for(i = 0; i < nr; i++) {
        retry = 50;
        while((tmp=generate_treasure(0,op->map->difficulty))==NULL && --retry);
        if (tmp != NULL) {
          tmp = insert_ob_in_ob(tmp, op);
	  if (op->type == PLAYER)
	      esrv_send_item (op, tmp);
        }
      }
      return 1;
 }

int command_freeze(object *op, char *params)
{
    int ticks;
    player *pl;

    if (!params) {
         new_draw_info(NDI_UNIQUE, 0,op,"Usage: freeze [ticks] <player>.");
         return 1;
    }

    ticks = atoi(params);
    if (ticks) {
	while ((isdigit(*params) || isspace(*params)) && *params != 0) params++;
	if (*params == 0) {
	    new_draw_info(NDI_UNIQUE, 0,op,"Usage: freeze [ticks] <player>.");
	    return 1;
	}
    } else ticks=100;
    pl = get_other_player_from_name(op, params);
    if (!pl) return 1;
    new_draw_info(NDI_UNIQUE | NDI_RED, 0, pl->ob, "You have been frozen by the DM!");
    new_draw_info_format(NDI_UNIQUE , 0, op,
			 "You freeze %s for %d ticks", pl->ob->name, ticks);
    pl->ob->speed_left = -(pl->ob->speed * ticks);
    return 0;
}


int command_summon (object *op, char *params)
{
    int i;
    object *dummy;
    player *pl;

    if (!op)
	return 0;

    if(params==NULL) {
         new_draw_info(NDI_UNIQUE, 0,op,"Usage: summon <player>.");
         return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl) return 1;

    i=find_free_spot(op->arch,op->map,op->x,op->y,1,8);
    if (i==-1) {
	new_draw_info(NDI_UNIQUE, 0, op, "Can not find a free spot to place summoned player.");
	return 1;
    }
    dummy=get_object();
    EXIT_PATH(dummy)=add_string(op->map->path);
    EXIT_X(dummy)=op->x+freearr_x[i];
    EXIT_Y(dummy)=op->y+freearr_y[i];
    enter_exit(pl->ob,dummy);
    free_object(dummy);
    new_draw_info(NDI_UNIQUE, 0,pl->ob,"You are summoned.");
    new_draw_info(NDI_UNIQUE, 0,op,"OK.");

    return 1;
}

/* Teleport next to target player */
/* mids 01/16/2002 */

int command_teleport (object *op, char *params) {
   int i;
   object *dummy;
   player *pl;

    if (!op)
	return 0;

    if (params==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Usage: teleport <player>.");
	return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl) return 1;

   i = find_free_spot(pl->ob->arch, pl->ob->map, pl->ob->x, pl->ob->y, 1, 8);
   if (i==-1) {
      new_draw_info(NDI_UNIQUE, 0, op, "Can not find a free spot to teleport to.");
      return 1;
   }
   dummy = get_object();
   EXIT_PATH(dummy) = add_string(pl->ob->map->path);
   EXIT_X(dummy) = pl->ob->x + freearr_x[i];
   EXIT_Y(dummy) = pl->ob->y + freearr_y[i];
   enter_exit(op, dummy);
   free_object(dummy);
   new_draw_info(NDI_UNIQUE, 0, pl->ob, "You see a portal open.");
   new_draw_info(NDI_UNIQUE, 0, op, "OK.");
   return 1;
}

int command_create (object *op, char *params)
{
      object *tmp=NULL;
      int nrof,i, magic, set_magic = 0, set_nrof = 0, gotquote, gotspace;
      char buf[MAX_BUF], *cp, *bp = buf, *bp2, *bp3, *bp4, *obp;
      archetype *at;
      artifact *art=NULL;

    if (!op)
	return 0;

    if (params == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, 
	    "Usage: create [nr] [magic] <archetype> [ of <artifact>]"
	    " [variable_to_patch setting]");
        return 1;
    }
    bp = params;
     
    if(sscanf(bp, "%d ", &nrof)) {
	if ((bp = strchr(params, ' ')) == NULL) {
	    new_draw_info(NDI_UNIQUE, 0, op,
		"Usage: create [nr] [magic] <archetype> [ of <artifact>]"
	    " [variable_to_patch setting]");
	    return 1;
	}
        bp++;
        set_nrof = 1;
        LOG(llevDebug, "%s creates: (%d) %s\n", op->name, nrof, bp);
    }
    if (sscanf(bp, "%d ", &magic)) {
	if ((bp = strchr(bp, ' ')) == NULL) {
	    new_draw_info(NDI_UNIQUE, 0, op,
		"Usage: create [nr] [magic] <archetype> [ of <artifact>]"
	    " [variable_to_patch setting]");
	    return 1;
	}
	bp++;
        set_magic = 1;
        LOG(llevDebug, "%s creates: (%d) (%d) %s\n", op->name, nrof, magic, bp);
    }
    if ((cp = strstr(bp, " of ")) != NULL) {
	*cp = '\0';
        cp += 4;
    }

    for (bp2=bp; *bp2; bp2++)
	if (*bp2 == ' ') {
	    *bp2 = '\0';
	    bp2++;
	    break;
	}

    if((at=find_archetype(bp))==NULL) {
	new_draw_info(NDI_UNIQUE, 0, op, "No such archetype.");
        return 1;
    }

    if (cp) {
	if (find_artifactlist(at->clone.type)==NULL) {
	    new_draw_info_format(NDI_UNIQUE, 0, op,
		"No artifact list for type %d\n", at->clone.type);
	}
	else {
	    art = find_artifactlist(at->clone.type)->items;

	    do {
		if (!strcmp(art->item->name, cp)) break;
		art = art->next;
	    } while (art!=NULL);
            if (!art) {
		new_draw_info_format(NDI_UNIQUE, 0, op,
		    "No such artifact ([%d] of %s)", at->clone.type, cp);
	    }
	}
        LOG(llevDebug, "%s creates: (%d) (%d) (%s) of (%s)\n", op->name,
            set_nrof ? nrof : 0, set_magic ? magic : 0 , bp, cp);
    } /* if cp */

    if(at->clone.nrof) {
	tmp=arch_to_object(at);
	tmp->x=op->x,tmp->y=op->y;
	if (set_nrof)
	    tmp->nrof = nrof;
	tmp->map=op->map;
	if (settings.real_wiz == FALSE)
	    SET_FLAG(tmp, FLAG_WAS_WIZ);
	if (set_magic)
	    set_abs_magic(tmp, magic);
	if (art)
	    give_artifact_abilities(tmp, art->item);
        if (need_identify(tmp)) {
	    SET_FLAG(tmp, FLAG_IDENTIFIED);
	    CLEAR_FLAG(tmp, FLAG_KNOWN_MAGICAL);
	}
	while (*bp2) {
	    /* find the first quote */
	    for (bp3=bp2, gotquote=0, gotspace=0; *bp3 && gotspace < 2; bp3++) {
		if (*bp3 == '"') {
		    *bp3 = ' ';
		    gotquote++;
		    bp3++;
		    for (bp4=bp3; *bp4; bp4++)
			if (*bp4 == '"') {
			    *bp4 = '\0';
			    break;
			}
		    break;
		} else if (*bp3 == ' ')
		     gotspace++;
            }
	    if (!gotquote) { /* then find the second space */
		for (bp3=bp2; *bp3; bp3++) {
		    if (*bp3 == ' ') {
			bp3++;
			for (bp4=bp3; *bp4; bp4++) {
			    if (*bp4 == ' ') {
				*bp4 = '\0';
				break;
			    }
			}
			break;
		    }
		}
	    }
	    /* now bp3 should be the argument, and bp2 the whole command */
	    if(set_variable(tmp, bp2) == -1)   
		new_draw_info_format(NDI_UNIQUE, 0, op,
		    "Unknown variable %s", bp2);
	    else
		new_draw_info_format(NDI_UNIQUE, 0, op,
		    "(%s#%d)->%s=%s", tmp->name, tmp->count, bp2, bp3);
	    if (gotquote)
		bp2=bp4+2;
	    else
		bp2=bp4+1;
	    if (obp == bp2)
		break; /* invalid params */
	    obp=bp2;
	}
        tmp = insert_ob_in_ob(tmp, op);
	esrv_send_item(op, tmp);
        return 1;
    }
    for (i=0 ; i < (set_nrof ? nrof : 1); i++) {
	archetype *atmp;
	object *prev=NULL,*head=NULL;
	for (atmp=at; atmp!=NULL; atmp=atmp->more) {
	    tmp=arch_to_object(atmp);
	    if (settings.real_wiz == FALSE)
		SET_FLAG(tmp, FLAG_WAS_WIZ);
	    if(head==NULL)
		head=tmp;
	    tmp->x=op->x+tmp->arch->clone.x;
	    tmp->y=op->y+tmp->arch->clone.y;
	    tmp->map=op->map;
	    if (set_magic)
		set_abs_magic(tmp, magic);
	    if (art)
		give_artifact_abilities(tmp, art->item);
	    if (need_identify(tmp)) {
		SET_FLAG(tmp, FLAG_IDENTIFIED);
		CLEAR_FLAG(tmp, FLAG_KNOWN_MAGICAL);
	    }
	    while (*bp2) {
		/* find the first quote */
		for (bp3=bp2, gotquote=0, gotspace=0; *bp3 && gotspace < 2;
		     bp3++) {
		    if (*bp3 == '"') {
			*bp3 = ' ';
			gotquote++;
			bp3++;
			for (bp4=bp3; *bp4; bp4++)
			    if (*bp4 == '"') {
				*bp4 = '\0';
				break;
			    }
			break;
		    } else if (*bp3 == ' ')
		        gotspace++;
                }
		if (!gotquote) { /* then find the second space */
		    for (bp3=bp2; *bp3; bp3++) {
			if (*bp3 == ' ') {
			    bp3++;
			    for (bp4=bp3; *bp4; bp4++) {
				if (*bp4 == ' ') {
				    *bp4 = '\0';
				    break;
				}
			    }
			    break;
			}
		    }
	        }
	/* now bp3 should be the argument, and bp2 the whole command */
		if(set_variable(tmp, bp2) == -1)   
		    new_draw_info_format(NDI_UNIQUE, 0, op,
			"Unknown variable %s", bp2);
		else
		    new_draw_info_format(NDI_UNIQUE, 0, op,
			"(%s#%d)->%s=%s", tmp->name, tmp->count, bp2, bp3);
		if (gotquote)
		    bp2=bp4+2;
		else
		    bp2=bp4+1;
		if (obp == bp2)
		    break; /* invalid params */
		obp=bp2;
	    }
	    if(head!=tmp)
		tmp->head=head,prev->more=tmp;
	    prev=tmp;
	}
        if (QUERY_FLAG(head, FLAG_ALIVE))
	    insert_ob_in_map(head, op->map, op, 0);
        else
	    head = insert_ob_in_ob(head, op);
        if (at->clone.randomitems!=NULL)
	    create_treasure(at->clone.randomitems, head, GT_APPLY,
                          op->map->difficulty, 0);
	    esrv_send_item(op, head);
    }
    return 1;
}

/* if(<not socket>) */

/*
 * Now follows dm-commands which are also acceptable from sockets
 */

int command_inventory (object *op, char *params)
{
    object *tmp;
    int i;

  if (!params) {
    inventory(op, NULL);
    return 0;
  }

  if(!sscanf(params, "%d", &i) || (tmp=find_object(i))==NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"Inventory of what object (nr)?");
      return 1;
    }
    inventory(op,tmp);
    return 1;
  }

/* just show player's their skills for now. Dm's can
 * already see skills w/ inventory command - b.t.
 */

int command_skills (object *op, char *params)
{
 	show_skills(op);
 	return 0;
}

int command_dump (object *op, char *params)
{
    int i;
  object *tmp;

  if(params!=NULL && !strcmp(params, "me"))
      tmp=op;
  else if(params==NULL || !sscanf(params, "%d", &i) ||
	  (tmp=find_object(i))==NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"Dump what object (nr)?");
      return 1;
    }
    dump_object(tmp);
    new_draw_info(NDI_UNIQUE, 0, op, errmsg);
    if (QUERY_FLAG(tmp, FLAG_OBJ_ORIGINAL))
	new_draw_info(NDI_UNIQUE, 0, op, "Object is marked original");
    return 1;
  }

/* When DM is possessing a monster, flip aggression on and off, to allow
   better motion */

int command_mon_aggr (object *op, char *params)
{
    if (op->enemy || !QUERY_FLAG(op, FLAG_UNAGGRESSIVE)) {
	op->enemy = NULL;
	SET_FLAG(op, FLAG_UNAGGRESSIVE);
	new_draw_info(NDI_UNIQUE, 0, op, "Agression turned OFF");
    } else {
	CLEAR_FLAG(op, FLAG_FRIENDLY);
	CLEAR_FLAG(op, FLAG_UNAGGRESSIVE);
	new_draw_info(NDI_UNIQUE, 0, op, "Agression turned ON");
    }
    return 1;
}

/* DM can possess a monster.  Basically, this tricks the client into thinking
   a given monster, is actually the player it controls.  This allows a DM
   to inhabit a monster's body, and run around the game with it. */

int command_possess (object *op, char *params)
{
    object *victim, *curinv, *nextinv, *tmp;
    player *pl;
    archetype *at;
    int i;
    char buf[MAX_BUF];

    victim=NULL;
    if (params != NULL) {
	if (sscanf(params, "%d", &i))
	    victim=find_object(i);
	else if (sscanf(params, "%s", buf))
	    victim=find_object_name(buf);
    }
    if (victim==NULL) {
	new_draw_info(NDI_UNIQUE, 0, op, "Patch what object (nr)?");
	return 1;
    }
    if (victim==op) {
	new_draw_info(NDI_UNIQUE, 0, op, "As insane as you are, I cannot "
		      "allow you to posess yourself.");
	return 1;
    }

    /* clear out the old inventory */
    curinv = op->inv;
    while (curinv != NULL) {
	nextinv = curinv->below;
	esrv_del_item(op->contr, curinv->count);
	curinv = nextinv;
    }
    /* make the switch */
    pl = op->contr;
    victim->contr = pl;
    pl->ob = victim;
    victim->type = PLAYER;
    SET_FLAG(victim, FLAG_WIZ);

    /* exp objects */
    at = find_archetype("experience_agility");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("experience_charisma");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("experience_mental");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("experience_physical");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("experience_power");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("experience_wis");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    /* need to add basic skills like melee, bows, set chosen skill */
    at = find_archetype("skill_punching");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("skill_melee_weapon");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("skill_missile_weapon");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("skill_use_magic_item");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("skill_spellcasting");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    at = find_archetype("skill_praying");
    tmp = get_object();
    copy_object(&at->clone, tmp);
    tmp = insert_ob_in_ob(tmp, victim);
    /* send the inventory to the client */
    curinv = victim->inv;
    while (curinv != NULL) {
	nextinv = curinv->below;
	esrv_send_item(victim, curinv);
	curinv = nextinv;
    }
    /* basic patchup */
    for (i=0; i<NUM_BODY_LOCATIONS; i++)
	if (i == 1 || i == 6 || i == 8 || i == 9)
	    victim->body_info[i] = 2;
        else
	    victim->body_info[i] = 1;

    esrv_new_player(pl, 80); /* just pick a wieght, we don't care */
    esrv_send_inventory(victim, victim);
    
    fix_player(victim);

    do_some_living(victim);
    return 1;
}


int command_patch (object *op, char *params)
{
    int i;
    char *arg,*arg2;
    char buf[MAX_BUF];
  object *tmp;

    tmp=NULL;
  if(params != NULL) {
    if(!strncmp(params, "me", 2))
        tmp=op;
    else if(sscanf(params, "%d", &i))
        tmp=find_object(i);
    else if(sscanf(params, "%s", buf))
        tmp=find_object_name(buf);
    }
    if(tmp==NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"Patch what object (nr)?");
      return 1;
    }
  arg=strchr(params, ' ');
    if(arg==NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"Patch what values?");
      return 1;
    }
    if((arg2=strchr(++arg,' ')))
      arg2++;
    if (settings.real_wiz == FALSE)
	SET_FLAG(tmp, FLAG_WAS_WIZ); /* To avoid cheating */
    if(set_variable(tmp,arg) == -1)
      new_draw_info_format(NDI_UNIQUE, 0,op,"Unknown variable %s", arg);
    else {
      new_draw_info_format(NDI_UNIQUE, 0, op,
	"(%s#%d)->%s=%s",tmp->name,tmp->count,arg,arg2);
    }
    return 1;
  }

int command_remove (object *op, char *params)
{
    int i;
    object *tmp;

    if(params==NULL || !sscanf(params, "%d", &i) || (tmp=find_object(i))==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Remove what object (nr)?");
	return 1;
    }
    if (QUERY_FLAG(tmp, FLAG_REMOVED)) {
	new_draw_info_format(NDI_UNIQUE, 0,op,"%s is already removed!",
			     query_name(tmp));
	return 1;
    }
    remove_ob(tmp);
    return 1;
}

int command_free (object *op, char *params)
{
    int i;
  object *tmp;

  if(params==NULL || !sscanf(params, "%d", &i) || (tmp=find_object(i))==NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"Free what object (nr)?");
      return 1;
    }
    free_object(tmp);
    return 1;
  }

/* This adds exp to a player.  We now allow adding to a specific skill.
 */
int command_addexp (object *op, char *params)
{
    char buf[MAX_BUF], skill[MAX_BUF];
    int i, q;
    object *skillob=NULL;
    player *pl;

    if(params==NULL || ((q=sscanf(params, "%s %d", buf, &i))<2)) {
	new_draw_info(NDI_UNIQUE, 0,op,"Usage: addexp [who] [how much].");
	return 1;
    }

    for(pl=first_player;pl!=NULL;pl=pl->next) 
	if(!strncmp(pl->ob->name,buf,MAX_NAME)) 
	    break;

    if(pl==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"No such player.");
	return 1;
    }

    if (q >= 3) {
	skillob = find_skill_by_name(pl->ob, skill);
	if (!skillob) {
	    new_draw_info_format(NDI_UNIQUE, 0,op,"Unable to find skill %s in %s", skill, buf);
	    return 1;
	}
	i = check_exp_adjust(skillob,i);
	skillob->stats.exp += i;
	calc_perm_exp(skillob);
	player_lvl_adj(pl->ob, skillob);
    }


    pl->ob->stats.exp += i;
    calc_perm_exp(pl->ob);
    player_lvl_adj(pl->ob, NULL);

    if (settings.real_wiz == FALSE)
	SET_FLAG(pl->ob, FLAG_WAS_WIZ);
    return 1;
}

int command_speed (object *op, char *params)
{
    int i;
  if(params==NULL || !sscanf(params, "%d", &i)) {
      sprintf(errmsg,"Current speed is %ld",max_time);
      new_draw_info(NDI_UNIQUE, 0,op,errmsg);
      return 1;
    }
    set_max_time(i);
    reset_sleep();
    new_draw_info(NDI_UNIQUE, 0,op,"The speed is changed.");
    return 1;
  }


/**************************************************************************/
/* Mods made by Tyler Van Gorder, May 10-13, 1992.                        */
/* CSUChico : tvangod@cscihp.ecst.csuchico.edu                            */
/**************************************************************************/

int command_stats (object *op, char *params)
{
  char thing[20];
  player *pl;
  char buf[MAX_BUF];

    thing[0] = '\0';
  if(params==NULL || !sscanf(params, "%s", thing) || thing == NULL) {
       new_draw_info(NDI_UNIQUE, 0,op,"Who?");
       return 1;
    }
    for(pl=first_player;pl!=NULL;pl=pl->next) 
       if(!strcmp(pl->ob->name,thing)) {
         sprintf(buf,"Str : %-2d      H.P. : %-4d  MAX : %d",
                 pl->ob->stats.Str,pl->ob->stats.hp,pl->ob->stats.maxhp);
         new_draw_info(NDI_UNIQUE, 0,op,buf);
         sprintf(buf,"Dex : %-2d      S.P. : %-4d  MAX : %d",
                 pl->ob->stats.Dex,pl->ob->stats.sp,pl->ob->stats.maxsp) ;
         new_draw_info(NDI_UNIQUE, 0,op,buf);
         sprintf(buf,"Con : %-2d        AC : %-4d  WC  : %d",
                 pl->ob->stats.Con,pl->ob->stats.ac,pl->ob->stats.wc) ;
         new_draw_info(NDI_UNIQUE, 0,op,buf);
         sprintf(buf,"Wis : %-2d       EXP : %lld",
                 pl->ob->stats.Wis,pl->ob->stats.exp);
         new_draw_info(NDI_UNIQUE, 0,op,buf);
         sprintf(buf,"Cha : %-2d      Food : %d",
                 pl->ob->stats.Cha,pl->ob->stats.food) ;
         new_draw_info(NDI_UNIQUE, 0,op,buf);
         sprintf(buf,"Int : %-2d    Damage : %d",
                 pl->ob->stats.Int,pl->ob->stats.dam) ;
         sprintf(buf,"Pow : %-2d    Grace : %d",
                 pl->ob->stats.Pow,pl->ob->stats.grace) ;
         new_draw_info(NDI_UNIQUE, 0,op,buf);
         break;
       }
    if(pl==NULL)
       new_draw_info(NDI_UNIQUE, 0,op,"No such player.");
    return 1;
  }

int command_abil (object *op, char *params)
{
  char thing[20], thing2[20];
  int iii;
  player *pl;
  char buf[MAX_BUF];

    iii = 0;
    thing[0] = '\0';
    thing2[0] = '\0';
  if(params==NULL || !sscanf(params, "%s %s %d", thing, thing2, &iii) ||
     thing==NULL) {
       new_draw_info(NDI_UNIQUE, 0,op,"Who?");
       return 1;
    }
    if (thing2==NULL){
       new_draw_info(NDI_UNIQUE, 0,op,"You can't change that.");
       return 1;
    }
    if (iii<MIN_STAT||iii>MAX_STAT) {
      new_draw_info(NDI_UNIQUE, 0,op,"Illegal range of stat.\n");
      return 1;
    }
    for(pl=first_player;pl!=NULL;pl=pl->next) 
       if(!strcmp(pl->ob->name,thing)){ 
	  if (settings.real_wiz == FALSE)
	       SET_FLAG(pl->ob, FLAG_WAS_WIZ);
          if(!strcmp("str",thing2))
            pl->ob->stats.Str = iii,pl->orig_stats.Str = iii;
          if(!strcmp("dex",thing2))   
            pl->ob->stats.Dex = iii,pl->orig_stats.Dex = iii;
          if(!strcmp("con",thing2))
            pl->ob->stats.Con = iii,pl->orig_stats.Con = iii;
          if(!strcmp("wis",thing2))
            pl->ob->stats.Wis = iii,pl->orig_stats.Wis = iii;
          if(!strcmp("cha",thing2))
            pl->ob->stats.Cha = iii,pl->orig_stats.Cha = iii;
          if(!strcmp("int",thing2))
            pl->ob->stats.Int = iii,pl->orig_stats.Int = iii;
          if(!strcmp("pow",thing2))
            pl->ob->stats.Pow = iii,pl->orig_stats.Pow = iii;
          sprintf(buf,"%s has been altered.",pl->ob->name);
          new_draw_info(NDI_UNIQUE, 0,op,buf);
          fix_player(pl->ob);
         return 1;
       } 
    new_draw_info(NDI_UNIQUE, 0,op,"No such player.");
    return 1;
  }

int command_reset (object *op, char *params)
{
    mapstruct *m;
    object *dummy = NULL, *tmp = NULL;

    if (params == NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Reset what map [name]?");
	return 1;	
    }
    if (strcmp(params, ".") == 0)
	params = op->map->path;
    m = has_been_loaded(params);
    if (m==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"No such map.");
	return 1;	
    }

    if (m->in_memory != MAP_SWAPPED) {
	if(m->in_memory != MAP_IN_MEMORY) {
	    LOG(llevError,"Tried to swap out map which was not in memory.\n");
	    return 0;
	}
	/* Only attempt to remove the player that is doing the reset, and not other
	 * players or wiz's.
	 */
	if (op->map == m ) {
	    dummy=get_object();
	    dummy->map = NULL;
	    EXIT_X(dummy) = op->x;
	    EXIT_Y(dummy) = op->y;
	    EXIT_PATH(dummy) = add_string(op->map->path);
	    remove_ob(op);
	    op->map = NULL;
	    tmp=op;
	}
	swap_map(m);
    }


    if (m->in_memory == MAP_SWAPPED) {	
	LOG(llevDebug,"Resetting map %s.\n",m->path);

	/* setting this effectively causes an immediate reload */
	m->reset_time = 1;
	flush_old_maps();
	new_draw_info(NDI_UNIQUE, 0,op,"OK.");
	if (tmp) {
	    enter_exit(tmp, dummy);
	    free_object(dummy);
	}
	return 1;
    } else {
	/* Need to re-insert player if swap failed for some reason */
	if (tmp) {
	    insert_ob_in_map(op, m, NULL,0);
	    free_object(dummy);
	}
	new_draw_info(NDI_UNIQUE, 0,op,"Reset failed, couldn't swap map.\n");
	new_draw_info(NDI_UNIQUE, 0,op,"Probably another player is on the map\n");
	return 1;
    }
}

int command_nowiz (object *op, char *params) /* 'noadm' is alias */
{
     CLEAR_FLAG(op, FLAG_WIZ);
     CLEAR_FLAG(op, FLAG_WIZPASS);
     CLEAR_FLAG(op, FLAG_FLYING);
     if (settings.real_wiz == TRUE)
	 CLEAR_FLAG(op, FLAG_WAS_WIZ);
    op->contr->hidden=0;
     new_draw_info(NDI_UNIQUE | NDI_ALL, 1, NULL,
	"The Dungeon Master is gone..");
     return 1;
  }

/*
 * object *op is trying to become dm.
 * pl_name is name supplied by player.  Restrictive DM will make it harder
 * for socket users to become DM - in that case, it will check for the players
 * character name.
 */
static int checkdm(object *op, char *pl_name, char *pl_passwd, char *pl_host)
{
  FILE  *dmfile;
  char  buf[MAX_BUF];
  char  line_buf[160], name[160], passwd[160], host[160];

#ifdef RESTRICTIVE_DM
  *pl_name=op->name?op->name:"*";
#endif

  sprintf (buf, "%s/%s", settings.confdir, DMFILE);
  if ((dmfile = fopen(buf, "r")) == NULL) {
    LOG (llevDebug, "Could not find DM file.\n");
    return(0);
  }
  while(fgets(line_buf, 160, dmfile) != NULL) {
    if (line_buf[0]=='#') continue;
    if (sscanf(line_buf,"%[^:]:%[^:]:%s\n",name, passwd, host)!=3) {
	LOG(llevError,"Warning - malformed dm file entry: %s", line_buf);
    }
    else if ((!strcmp(name,"*") || (pl_name && !strcmp(pl_name, name)))
      && (!strcmp(passwd,"*") || !strcmp(passwd,pl_passwd)) &&
      (!strcmp(host,"*") || !strcmp(host, pl_host))) {
	fclose(dmfile);
	return (1);
    }
  }
  fclose(dmfile);
  return (0);
}

/* Actual command to perhaps become dm.  Changed aroun a bit in version 0.92.2
 * - allow people on sockets to become dm, and allow better dm file
 */

int command_dm (object *op, char *params)
{
  if (!op->contr) return 0;
  else {
    if (checkdm(op, op->name,
		(params?params:"*"), op->contr->socket.host)) {
      SET_FLAG(op, FLAG_WIZ);
      SET_FLAG(op, FLAG_WAS_WIZ);
      SET_FLAG(op, FLAG_WIZPASS);
      new_draw_info(NDI_UNIQUE, 0,op, "Ok, you are the Dungeon Master!");
      new_draw_info(NDI_UNIQUE | NDI_ALL, 1, NULL,
  	"The Dungeon Master has arrived!");
      SET_FLAG(op, FLAG_FLYING);
      clear_los(op);
      op->contr->write_buf[0] ='\0';
      return 1;
    } else {
      new_draw_info(NDI_UNIQUE, 0,op, "Sorry Pal, I don't think so.");
      op->contr->write_buf[0] ='\0';
      return 1;
    }
  }
}

int command_invisible (object *op, char *params)
{
    if (op) {
	op->invisible+=100;
	update_object(op,UP_OBJ_FACE);
	new_draw_info(NDI_UNIQUE, 0,op,"You turn invisible.");
    }
    return 0;
}


static int command_learn_spell_or_prayer (object *op, char *params,
                                          int special_prayer)
{
    object *tmp;

    if (op->contr == NULL || params == NULL)
        return 0;

    tmp = get_archetype(params);
    if (!tmp) {
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"Could not find a spell by name of %s\n", params);
	return 0;
    }
    do_learn_spell (op, tmp, special_prayer);
    free_object(tmp);
    return 1;
}

int command_learn_spell (object *op, char *params)
{
    return command_learn_spell_or_prayer (op, params, 0);
}

int command_learn_special_prayer (object *op, char *params)
{
    return command_learn_spell_or_prayer (op, params, 1);
}

int command_forget_spell (object *op, char *params)
{
    if (op->contr == NULL || params == NULL)
        return 0;

    do_forget_spell (op, params);
    return 1;
}
/* GROS */
/* Lists all plugins currently loaded with their IDs and full names.         */
int command_listplugins(object *op, char *params)
{
    displayPluginsList(op);
    return 1;
}
/* GROS */
/* Loads the given plugin. The DM specifies the name of the library to load  */
/* (no pathname is needed). Do not ever attempt to load the same plugin more */
/* than once at a time, or bad things could happen.                          */
int command_loadplugin(object *op, char *params)
{
    char buf[MAX_BUF];

    strcpy(buf,LIBDIR);
    strcat(buf,"/plugins/");
    strcat(buf,params);
    printf("Requested plugin file is %s\n", buf);
    initOnePlugin(buf);
    return 1;
}
/* GROS */
/* Unloads the given plugin. The DM specified the ID of the library to       */
/* unload. Note that some things may behave strangely if the correct plugins */
/* are not loaded.                                                           */
int command_unloadplugin(object *op, char *params)
{
    removeOnePlugin(params);
    return 1;
}
