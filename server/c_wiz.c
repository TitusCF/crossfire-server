/*
 * static char *rcsid_c_wiz_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copryight (C) 2000 Mark Wedel
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

/*
 * End of non-DM commands.  DM-only commands below.
 * (This includes commands directly from socket)
 */

/* Some commands not accepted from socket */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <version.h>
#include <spells.h>
#include <treasure.h>
#include <skills.h>

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
    object *ob;
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
    change_skill(ob,SK_PRAYING);
    if(!ob->chosen_skill||ob->chosen_skill->stats.sp!=SK_PRAYING) {
    	new_draw_info_format(NDI_UNIQUE,0,op,"%s doesn't have praying skill.",ob->name);
    	return 1;
    }
    if(find_god(str)==NULL) {
    	new_draw_info_format(NDI_UNIQUE,0,op,"No such god %s.",str);
    	return 1;
    }
    become_follower(ob,find_god(str));
    return 1;
}

int command_kick (object *op, char *params)
{
  struct pl *pl;

  for(pl=first_player;pl!=NULL;pl=pl->next) 
    if((params==NULL || !strcmp(pl->ob->name,params)) && pl->ob!=op)
      {
	object *op;

	op=pl->ob;
	remove_ob(op);
	op->direction=0;
	op->contr->count_left=0;
	new_draw_info_format(NDI_UNIQUE | NDI_ALL, 5, op,
			     "%s is kicked out of the game.",op->name);
	strcpy(op->contr->killer,"left");
	check_score(op); /* Always check score */
	(void)save_player(op,0);
	play_again(op);
	op->map->players--;
#if MAP_MAXTIMEOUT
	op->map->timeout = MAP_TIMEOUT(op->map);
#endif
      }
  return 1;
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
      for(pl=first_player;pl!=NULL;pl=pl->next) 
    if(!strncmp(pl->ob->name, params, MAX_NAME)) 
          break;
      if(pl==NULL) {
        new_draw_info(NDI_UNIQUE, 0,op,"No such player.");
        return 1;
      }
      if(pl->state != ST_PLAYING) {
        new_draw_info(NDI_UNIQUE, 0,op,"That player can't be summoned right now.");
        return 1;
      }
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

int command_create (object *op, char *params)
{
      object *tmp=NULL;
      int nrof,i, magic, set_magic = 0, set_nrof = 0;
      char buf[MAX_BUF], *cp, *bp = buf;
      archetype *at;
      artifact *art=NULL;

  if (!op)
    return 0;

  if (params == NULL) {
        new_draw_info(NDI_UNIQUE, 0,op, "Usage: create [nr] [magic] <archetype> [ of <artifact>]");
        return 1;
      }
  bp = params;
     
      if(sscanf(bp, "%d ", &nrof)) {
    if ((bp = strchr(params, ' ')) == NULL) {
          new_draw_info(NDI_UNIQUE, 0,op, "Usage: create [nr] [magic] <archetype> [ of <artifact>]");
          return 1;
        }
        bp++;
        set_nrof = 1;
        LOG(llevDebug, "(%d) %s\n", nrof, buf);
      }
      if (sscanf(bp, "%d ", &magic)) {
        if ((bp = strchr(bp, ' ')) == NULL) {
          new_draw_info(NDI_UNIQUE, 0,op, "Usage: create [nr] [magic] <archetype> [ of <artifact>]");
          return 1;
        }
        bp++;
        set_magic = 1;
        LOG(llevDebug, "(%d) (%d) %s\n", nrof, magic, buf);
      }
      if ((cp = strstr(bp, " of ")) != NULL) {
        *cp = '\0';
        cp += 4;
      }

      if((at=find_archetype(bp))==NULL) {
        new_draw_info(NDI_UNIQUE, 0,op,"No such archetype.");
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
        LOG(llevDebug, "(%d) (%d) (%s) of (%s)\n",
            set_nrof ? nrof : 0, set_magic ? magic : 0 , bp, cp);
      }
      if(at->clone.nrof) {
        tmp=arch_to_object(at);
        tmp->x=op->x,tmp->y=op->y;
        if (set_nrof)
          tmp->nrof = nrof;
        tmp->map=op->map;
#ifndef REAL_WIZ
	SET_FLAG(tmp, FLAG_WAS_WIZ);
#endif
        if (set_magic)
          set_abs_magic(tmp, magic);
        if (art)
          give_artifact_abilities(tmp, art->item);
        if (need_identify(tmp)) {
	  SET_FLAG(tmp, FLAG_IDENTIFIED);
	  CLEAR_FLAG(tmp, FLAG_KNOWN_MAGICAL);
	}
        tmp = insert_ob_in_ob(tmp,op);
	esrv_send_item(op, tmp);
        return 1;
      } 
      for (i=0 ; i < (set_nrof ? nrof : 1); i++) {
        archetype *atmp;
        object *prev=NULL,*head=NULL;
        for (atmp=at;atmp!=NULL;atmp=atmp->more) {
          tmp=arch_to_object(atmp);
#ifndef REAL_WIZ
	  SET_FLAG(tmp, FLAG_WAS_WIZ);
#endif
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
          if(head!=tmp)
            tmp->head=head,prev->more=tmp;
          prev=tmp;
        }
        if (QUERY_FLAG(head,FLAG_ALIVE))
          insert_ob_in_map(head,op->map,op,0);
        else
          head = insert_ob_in_ob(head,op);
        if (at->clone.randomitems!=NULL)
          create_treasure(at->clone.randomitems,head,GT_APPLY,
                          op->map->difficulty,0);
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
    new_draw_info(NDI_UNIQUE, 0,op,errmsg);
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
#ifndef REAL_WIZ
    SET_FLAG(tmp, FLAG_WAS_WIZ); /* To avoid cheating */
#endif
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

int command_addexp (object *op, char *params)
{
    char buf[MAX_BUF];
    int i;
    object *exp_ob,*skill;
    player *pl;

  if(params==NULL || sscanf(params, "%s %d", buf, &i)!=2) {
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

/* In new system: for dm adding experience to a player, only can 
 * add exp if we satisfy the following: 
 * 1) there is an associated skill readied by the player 
 * 2) added exp doesnt result in exp_ob->stats.exp>MAX_EXP_IN_OBJ 
 */

    if((skill = pl->ob->chosen_skill) && ((exp_ob = pl->ob->chosen_skill->exp_obj)
       || link_player_skill(pl->ob, skill))) { 
      i = check_dm_add_exp_to_obj(exp_ob,i);
      exp_ob->stats.exp += i;
    } else {
      new_draw_info(NDI_UNIQUE, 0,op,"Can't find needed experience object.");
      new_draw_info(NDI_UNIQUE, 0,op,"Player has no associated skill readied.");
      return 1;
    }
    pl->ob->stats.exp += i;
    add_exp(pl->ob,0);
#ifndef REAL_WIZ
    SET_FLAG(pl->ob, FLAG_WAS_WIZ);
#endif
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
         sprintf(buf,"Wis : %-2d       EXP : %d",
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
#ifndef REAL_WIZ  
	  SET_FLAG(pl->ob, FLAG_WAS_WIZ);
#endif
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
	clean_tmp_map(m);
	if (m->tmpname) free(m->tmpname);
	m->tmpname = NULL;
	/* setting this effectively causes an immediate reload */
	m->reset_time = 1;
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
#ifdef REAL_WIZ
     CLEAR_FLAG(op, FLAG_WAS_WIZ);
#endif
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

  sprintf (buf, "%s/%s", settings.datadir, DMFILE);
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
  if (!op)
    return 0;
      op->invisible+=100;
      update_object(op,UP_OBJ_FACE);
      new_draw_info(NDI_UNIQUE, 0,op,"You turn invisible.");
  return 0;
}


static int command_learn_spell_or_prayer (object *op, char *params,
                                          int special_prayer)
{
    int spell;

    if (op->contr == NULL || params == NULL)
        return 0;

    if ((spell = look_up_spell_name (params)) <= 0) {
        new_draw_info (NDI_UNIQUE, 0, op, "Unknown spell.");
        return 1;
    }

    do_learn_spell (op, spell, special_prayer);
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
    int spell;

    if (op->contr == NULL || params == NULL)
        return 0;

    if ((spell = look_up_spell_name (params)) <= 0) {
        new_draw_info (NDI_UNIQUE, 0, op, "Unknown spell.");
        return 1;
    }

    do_forget_spell (op, spell);
    return 1;
}
/* GROS */
/* Lists all plugins currently loaded with their IDs and full names.         */
int command_listplugins(object *op, char *params)
{
    displayPluginsList(op);
    return 1;
};
/* GROS */
/* Loads the given plugin. The DM specifies the name of the library to load  */
/* (no pathname is needed). Do not ever attempt to load the same plugin more */
/* than once at a time, or bad things could happen.                          */
int command_loadplugin(object *op, char *params)
{
    char buf[MAX_BUF];

    strcpy(buf,DATADIR);
    strcat(buf,"/plugins/");
    strcat(buf,params);
    printf("Requested plugin file is %s\n", buf);
    initOnePlugin(buf);
    return 1;
};
/* GROS */
/* Unloads the given plugin. The DM specified the ID of the library to       */
/* unload. Note that some things may behave strangely if the correct plugins */
/* are not loaded.                                                           */
int command_unloadplugin(object *op, char *params)
{
    removeOnePlugin(params);
    return 1;
};
