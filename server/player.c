/*
 * static char *rcsid_player_c =
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

#include <pwd.h>
#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <sounds.h>
#include <living.h>
#include <object.h>
#include <spells.h>
#include <skills.h>
#include <newclient.h>

void display_motd(object *op) {
#ifdef MOTD
  char buf[MAX_BUF];
  FILE *fp;
  int comp;

  sprintf(buf,"%s/%s",settings.datadir,MOTD);
  if((fp=open_and_uncompress(buf,0,&comp))==NULL) {
    return;
  }
  while(fgets(buf,MAX_BUF,fp)!=NULL) {
    char *cp;
    if(*buf=='#')
      continue;
    cp=strchr(buf,'\n');
    if(cp!=NULL)
      *cp='\0';
    new_draw_info(NDI_UNIQUE, 0,op,buf);
  }
  close_and_delete(fp, comp);
  new_draw_info(NDI_UNIQUE, 0,op," ");
#endif
}

int playername_ok(char *cp) {
  for(;*cp!='\0';cp++)
    if(!((*cp>='a'&&*cp<='z')||(*cp>='A'&&*cp<='Z'))&&*cp!='-'&&*cp!='_')
      return 0;
  return 1;
}

/* Tries to add player on the connection passwd in ns.
 * All we can really get in this is some settings like host and display
 * mode.
 */

int add_player(NewSocket *ns) {
    player *p;
    char *defname = "nobody";
    mapstruct *m=NULL;


    /* Check for banned players and sites.  usename is no longer accurate,
     * (can't get it over sockets), so all we really have to go on is
     * the host.
     */

    if (checkbanned (defname, ns->host)){
	fprintf (logfile, "Banned player tryed to add. [%s@%s]\n", defname, ns->host);
	fflush (logfile);
	return 0;
    }

    init_beforeplay(); /* Make sure everything is ready */

    if(m == NULL || m->in_memory != MAP_IN_MEMORY)
	m = ready_map_name(first_map_path,0);
    if(m == NULL)
	fatal(MAP_ERROR);
    m->timeout = MAP_TIMEOUT(m); /* Just in case we fail to add the player */
    p=get_player_ob();
    memcpy(&p->socket, ns, sizeof(NewSocket));
    p->socket.inbuf.len=0;

    p->weapon_sp=0,p->last_weapon_sp= -1;
    p->has_hit=0;

    p->peaceful=1;			/* default peaceful */
    p->do_los=1;
    p->last_weight= -1;
#ifdef EXPLORE_MODE
    p->explore=0;
#endif  

    free_string(p->ob->name);
    p->ob->name = NULL;
    free_object(p->ob);
    p->ob=get_player(p,m);
    add_friendly_object(p->ob);
#ifdef MOTD
    display_motd(p->ob);
#endif
    p->peaceful=1;
    p->own_title[0]='\0';
    get_name(p->ob);
    return 0;
}

/*
 * get_player_archetype() return next player archetype from archetype
 * list. Not very efficient routine, but used only creating new players.
 * Note: there MUST be at least one player archetype!
 */
archetype *get_player_archetype(archetype* at)
{
    archetype *start = at;
    for (;;) {
	if (at==NULL || at->next==NULL)
	    at=first_archetype;
	else
	    at=at->next;
	if(at->clone.type==PLAYER)
	    return at;
	if (at == start) {
	    LOG (llevError, "No Player achetypes\n");
	    exit (-1);
	}
    }
}


object *get_player(player *p, mapstruct *m) {
    object *op=arch_to_object(get_player_archetype(NULL));
    int i;

    p->loading = NULL;
    op->map=m;
    if(m->in_memory != MAP_IN_MEMORY) {
	p->loading = m;
	p->new_x = 0;
	p->new_y = 0;
	p->removed = 0;
	op->x=0;
	op->y=0;
    } else {
	i=find_free_spot(NULL,m,EXIT_X(m->map_object),EXIT_Y(m->map_object),
			 0,SIZEOFFREE);
	/* If no free spot, just stuff the player wherever it says */
	if (i==-1) i=0;
	op->x=EXIT_X(m->map_object)+freearr_x[i];
	op->y=EXIT_Y(m->map_object)+freearr_y[i];
    }
    p->fire_on=0,p->run_on=0;
    p->count=0;
    p->count_left=0;
    p->prev_cmd=' ';
    p->prev_fire_on=0;
    p->mode=0;
    p->idle=0;
#ifdef AUTOSAVE
    p->last_save_tick = 9999999;
#endif
    *p->maplevel=0;
    
    strcpy(p->savebed_map, first_map_path);  /* Init. respawn position */
    p->bed_x=0, p->bed_y=0;
    
    op->contr=p; /* this aren't yet in archetype */
    op->speed_left=0.5;
    op->speed=1.0;
    op->direction=5;     /* So player faces south */
    op->stats.wc=2;
    op->run_away = 25; /* Then we panick... */

    roll_stats(op);
    p->state=ST_ROLL_STAT;
    clear_los(op);
    p->last_stats.Str=0,  p->last_stats.Dex=0,
    p->last_stats.Int=0,  p->last_stats.Con=0,
    p->last_stats.Wis=0,  p->last_stats.Cha=0;
    p->last_stats.Pow=0,  p->last_stats.Pow=0;
    p->last_stats.hp=0,   p->last_stats.maxhp=0;
    p->last_stats.wc=0,   p->last_stats.ac=0;
    p->last_stats.sp=0,   p->last_stats.maxsp=0;
    p->last_stats.grace=0, p->last_stats.maxgrace=0;
    p->last_stats.exp= -1,p->last_stats.food=0;
    p->digestion=0,p->gen_hp=0,p->gen_sp=0,p->gen_grace=0;
    p->gen_sp_armour=10;
    p->last_spell= -1;
    p->last_value= -1;
    p->last_speed= -1;
    p->shoottype=range_none;
    p->last_shoot= range_bottom;
    p->listening=9;
    p->golem=NULL;
    p->last_used=NULL;
    p->last_used_id=0;
    strncpy(p->title,op->arch->clone.name,MAX_NAME);
    op->race = add_string (op->arch->clone.race);

    (void)memset((void *)op->contr->drawn,'\0',
	       sizeof(Fontindex)*(WINRIGHT-WINLEFT+1)*(WINLOWER-WINUPPER+1));
    for(i=0;i<NROFREALSPELLS;i++)
	p->known_spells[i]= -1;
    p->nrofknownspells=0;
    p->chosen_spell = -1;
    p->ob->chosen_skill = NULL;
#ifdef LINKED_SKILL_LIST
    p->ob->sk_list = NULL;
#endif
    if(QUERY_FLAG(op,FLAG_READY_SKILL))
        CLEAR_FLAG(op,FLAG_READY_SKILL); 
    p->socket.update_look=0;
    return op;
}

object *get_nearest_player(object *mon) {
  object *op = NULL;
  objectlink *ol;
  int lastdist,tmp;

  for(ol=first_friendly_object,lastdist=1000;ol!=NULL;ol=ol->next) {
    /* Remove special check for player from this.  First, it looks to cause
     * some crashes (ol->ob->contr not set properly?), but secondly, a more
     * complicated method of state checking would be needed in any case -
     * as it was, a clever player could type quit, and the function would  
     * skip them over while waiting for confirmation.
     */
    if(!can_detect_enemy(mon,ol->ob)||ol->ob->map!=mon->map)
#if 0
       ||((ol->ob->invisible&&QUERY_FLAG(ol->ob,FLAG_UNDEAD)==QUERY_FLAG(mon,FLAG_UNDEAD))
	  &&!QUERY_FLAG(mon,FLAG_SEE_INVISIBLE))||ol->ob->map!=mon->map)
#endif
      continue;
    tmp=distance(ol->ob,mon);
    if(lastdist>tmp) {
      op=ol->ob;
      lastdist=tmp;
    }
  }
#if 0
  LOG(llevDebug,"get_nearest_player() finds player: %s\n",op?op->name:"(null)");
#endif
  return op;
}

/*
 * Returns the direction to the player, if valid.  Returns 0 otherwise.
 * modified to verify there is a path to the player.  Does this by stepping towards
 * player and if path is blocked then see if blockage is close enough to player that
 * direction to player is changed (ie zig or zag).  Continue zig zag until either
 * reach player or path is blocked.  Thus, will only return true if there is a free
 * path to player.  Though path may not be a straight line.  Note that it will find
 * player hiding along a corridor at right angles to the corridor with the monster.
 */
int path_to_player(object *mon, object *pl,int mindiff) {
  int dir,x,y,diff;
  int oldx,oldy,olddir,newdir;

  x=mon->x, y=mon->y;
  dir=find_dir_2(x-pl->x,y-pl->y);

  /* diff is how many steps from monster to player */
  diff= abs(x-pl->x) > abs(y-pl->y) ? abs(x-pl->x) : abs(y-pl->y);
  if(diff<mindiff) return 0;

  newdir = olddir = dir;
  oldx = x, oldy=y;

  while( diff>1 ) {
    for( ; diff>1; diff--) {
      oldx=x, oldy=y;
      x+=freearr_x[newdir],y+=freearr_y[newdir];
      if(blocked(mon->map,x,y)){
        if (olddir == (newdir=find_dir_2(oldx-pl->x, oldy-pl->y))){
          if ( (newdir%2) == 0)
            return 0;
            /* if heading straight then try diag step towards player.  needed to find
             * player in a L position compared to monster along corridors.  the 2* and
			 * RANDOM are to go around a block and find the player behind. */
          if ((newdir==1) || (newdir==5)) {
            ( (2*x) > (2*(pl->x+RANDOM()%2)-1) ) ? x-- : x++;
          } else if((newdir==3) || (newdir==7)) {
            ( (2*y) > (2*(pl->y+RANDOM()%2)-1) ) ? y-- : y++;
          }
          if(blocked(mon->map,x,y))  
            return 0;        /* path blocked to player */
          diff = abs(x-pl->x) > abs(y-pl->y) ? abs(x-pl->x) : abs(y-pl->y);
        } else {        /*have a new direction to try */
          olddir = newdir, x=oldx, y=oldy;
          diff = abs(oldx-pl->x) > abs(oldy-pl->y) ? abs(oldx-pl->x) : abs(oldy-pl->y);
        }
      }
    }
    /* diff may have headed us all the way down that direction, but not next to the player
     * so head towards player.  If next to player then will exit while loop */
    newdir=find_dir_2(x-pl->x, y-pl->y);
    diff = abs(x-pl->x) > abs(y-pl->y) ? abs(x-pl->x) : abs(y-pl->y);
  }
  return dir;
}

void give_initial_items(object *pl,treasurelist *items) {
    object *op,*next=NULL;
    static uint8 start_spells[] = {0, 1, 4, 5, 7,17,168};
    static uint8 start_prayers[] = {19, 31, 32, 129}; 
    if(pl->randomitems!=NULL)
	create_treasure(items,pl,GT_INVENTORY,1,0);

    for (op=pl->inv; op; op=next) {
	next = op->below;
	if(op->nrof<2 && op->type!=CONTAINER && op->type!=MONEY && 
	   op->type!=BOOK && !QUERY_FLAG(op,FLAG_IS_THROWN))
		SET_FLAG(op,FLAG_STARTEQUIP);

        if(op->type == CONTAINER) op->value = 0; /* so you can't sell it for money*/
        /* Not marked as starting equipment, so set 0 value. */
	if (QUERY_FLAG(op,FLAG_IS_THROWN))
	    op->value=0;
	if(op->type==FORCE) { SET_FLAG(op,FLAG_APPLIED);};
	if(op->type==SPELLBOOK) { /* fix spells for first level spells */
	    if(!strcmp(op->arch->name,"cleric_book")) 
		op->stats.sp=start_prayers[RANDOM()%(sizeof(start_prayers)/sizeof(uint8))];
	    else
		op->stats.sp=start_spells[RANDOM()%sizeof(start_spells)];
	}
	/* Give starting characters identified, uncursed, and undamned
	 * items.  Just don't identify gold or silver, or it won't be
	 * merged properly.
	 */
	if (need_identify(op)) {
	    SET_FLAG(op, FLAG_IDENTIFIED);
	    CLEAR_FLAG(op, FLAG_CURSED);
	    CLEAR_FLAG(op, FLAG_DAMNED);
	}
#ifndef ALLOW_SKILLS	/* no reason to start with these if no skills exist! */ 
	if(op->type==SKILLSCROLL || op->type==SKILL)  {
	    remove_ob(op);
	    free_object(op);
	}
#endif
	if(op->type==ABILITY)  {
	    pl->contr->known_spells[pl->contr->nrofknownspells++]=op->stats.sp;
	    remove_ob(op);
	    free_object(op);
	}
    }
}

void get_name(object *op) {
    op->contr->write_buf[0]='\0';
    op->contr->state=ST_GET_NAME;
    send_query(&op->contr->socket,0,"What is your name?\n:");
}

void get_password(object *op) {
    op->contr->write_buf[0]='\0';
    op->contr->state=ST_GET_PASSWORD;
    send_query(&op->contr->socket,CS_QUERY_HIDEINPUT, "What is your password?\n:");
}

void play_again(object *op)
{
    op->contr->state=ST_PLAY_AGAIN;
    unlock_player(op->name);
    send_query(&op->contr->socket, CS_QUERY_SINGLECHAR, "Do you want to play again (a/q)?");
}


int receive_play_again(object *op, char key)
{
    if(key=='q'||key=='Q') {
	remove_friendly_object(op);
	leave(op->contr);
	return 2;
    }
    else if(key=='a'||key=='A') {
	object *tmp;
	remove_friendly_object(op);
	op->contr->ob=get_player(op->contr,op->map);
	tmp=op->contr->ob;
	add_friendly_object(tmp);
	tmp->contr->password[0]='~';
	if(tmp->name!=NULL)
	    free_string(tmp->name);
	/* Lets put a space in here */
	new_draw_info(NDI_UNIQUE, 0, op, "\n");
	get_name(tmp);
	add_refcount(tmp->name = op->name);
	op->type=DEAD_OBJECT;
	free_object(op);
	op=tmp;
    } else {
	/* user pressed something else so just ask again... */
	play_again(op);
    }
    return 0;
}

void confirm_password(object *op) {

    op->contr->write_buf[0]='\0';
    op->contr->state=ST_CONFIRM_PASSWORD;
    send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, "Please type your password again.\n:");
}

void get_party_password(object *op, int partyid) {
  op->contr->write_buf[0]='\0';
  op->contr->state=ST_GET_PARTY_PASSWORD;
  op->contr->party_number_to_join = partyid;
  send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, "What is the password?\n:");
}


/* This rolls four 1-6 rolls and sums the best 3 of the 4. */
int roll_stat() {
  int a[4],i,j,k;
  for(i=0;i<4;i++)
    a[i]=(int)RANDOM()%6+1;
  for(i=0,j=0,k=7;i<4;i++)
    if(a[i]<k)
      k=a[i],j=i;
  for(i=0,k=0;i<4;i++) {
    if(i!=j)
      k+=a[i];
  }
  return k;
}

void roll_stats(object *op) {
  int sum=0;
  do {
    op->stats.Str=roll_stat();
    op->stats.Dex=roll_stat();
    op->stats.Int=roll_stat();
    op->stats.Con=roll_stat();
    op->stats.Wis=roll_stat();
    op->stats.Pow=roll_stat();
    op->stats.Cha=roll_stat();
    sum=op->stats.Str+op->stats.Dex+op->stats.Int+
	op->stats.Con+op->stats.Wis+op->stats.Pow+
	op->stats.Cha;
  } while(sum<82||sum>116);
#if defined( USE_SWAP_STATS) && defined(SORT_ROLLED_STATS)
	/* Sort the stats so that rerolling is easier... */
	{
	        int             i = 0, j = 0;
	        int             statsort[7];

	        statsort[0] = op->stats.Str;
	        statsort[1] = op->stats.Dex;
	        statsort[2] = op->stats.Int;
	        statsort[3] = op->stats.Con;
	        statsort[4] = op->stats.Wis;
	        statsort[5] = op->stats.Pow;
	        statsort[6] = op->stats.Cha;

	        /* a quick and dirty bubblesort? */
	        do {
	                if (statsort[i] < statsort[i + 1]) {
	                        j = statsort[i];
	                        statsort[i] = statsort[i + 1];
	                        statsort[i + 1] = j;
	                        i = 0;
	              } else {
	                        i++;
	              }
	      } while (i < 6);

	        op->stats.Str = statsort[0];
	        op->stats.Dex = statsort[1];
	        op->stats.Con = statsort[2];
	        op->stats.Int = statsort[3];
	        op->stats.Wis = statsort[4];
	        op->stats.Pow = statsort[5];
	        op->stats.Cha = statsort[6];
      }
#endif /* SWAP_STATS */

  op->contr->orig_stats.Str=op->stats.Str;
  op->contr->orig_stats.Dex=op->stats.Dex;
  op->contr->orig_stats.Int=op->stats.Int;
  op->contr->orig_stats.Con=op->stats.Con;
  op->contr->orig_stats.Wis=op->stats.Wis;
  op->contr->orig_stats.Pow=op->stats.Pow;
  op->contr->orig_stats.Cha=op->stats.Cha;
  op->stats.hp= -10000;
  op->level=0;
  op->stats.exp=0;
  op->stats.sp=0;
  op->stats.grace=0;
  op->stats.ac=0;
  add_exp(op,0);
  op->stats.sp=op->stats.maxsp;
  op->stats.hp=op->stats.maxhp;
#ifndef ALLOW_SKILLS /* start grace at maxgrace if no skills */
  op->stats.grace=op->stats.maxgrace; 
#else
  op->stats.grace=0;
#endif
  op->contr->orig_stats=op->stats;
}

void Roll_Again(object *op)
{
    esrv_new_player(op->contr, 0);
#ifndef USE_SWAP_STATS
    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"Roll again (y/n)? ");
#else
    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"[y] to roll new stats [n] to use stats\n[1-7] [1-7] to swap stats.\nRoll again (y/n/1-7)?  ");
#endif /* USE_SWAP_STATS */
}

void Swap_Stat(object *op,int Swap_Second)
{
#ifdef USE_SWAP_STATS
  signed char tmp;
  char buf[MAX_BUF];

    if ( op->contr->Swap_First == -1 ) {
	new_draw_info(NDI_UNIQUE, 0,op,"How the hell did you get here?!?!!!");
	new_draw_info(NDI_UNIQUE, 0,op,"Error in Swap_Stat code,");
	new_draw_info(NDI_UNIQUE, 0,op,"mail korg@rdt.monash.edu.au");
	return;
    }

    tmp = get_attr_value(&op->contr->orig_stats, op->contr->Swap_First);

    set_attr_value(&op->contr->orig_stats, op->contr->Swap_First,
	get_attr_value(&op->contr->orig_stats, Swap_Second));

    set_attr_value(&op->contr->orig_stats, Swap_Second, tmp);

    sprintf(buf,"%s done\n", short_stat_name[Swap_Second]);
    new_draw_info(NDI_UNIQUE, 0,op, buf);
    op->stats.Str = op->contr->orig_stats.Str;
    op->stats.Dex = op->contr->orig_stats.Dex;
    op->stats.Con = op->contr->orig_stats.Con;
    op->stats.Int = op->contr->orig_stats.Int;
    op->stats.Wis = op->contr->orig_stats.Wis;
    op->stats.Pow = op->contr->orig_stats.Pow;
    op->stats.Cha = op->contr->orig_stats.Cha;
    op->stats.hp= -10000;
    op->level=0;
    op->stats.exp=0;
    op->stats.sp=0;
    op->stats.grace=0;
    op->stats.ac=0;
    add_exp(op,0);
    op->stats.sp=op->stats.maxsp;
#ifndef ALLOW_SKILLS
    op->stats.grace=op->stats.maxgrace; 
#else
    op->stats.grace=0;
#endif
    op->stats.hp=op->stats.maxhp;
    add_exp(op,0);
    op->contr->Swap_First=-1;
#endif /* USE_SWAP_STATS */
}


/* This code has been greatly reduced, because with set_attr_value
 * and get_attr_value, the stats can be accessed just numeric
 * ids.  stat_trans is a table that translate the number entered
 * into the actual stat.  It is needed because the order the stats
 * are displayed in the stat window is not the same as how
 * the number's access that stat.  The table does that translation.
 */
int key_roll_stat(object *op, char key)
{
    int keynum = key -'0';
    char buf[MAX_BUF];
    static sint8 stat_trans[] = {-1, STR, DEX, CON, INT, WIS, POW, CHA};

#ifdef USE_SWAP_STATS
    if (keynum>0 && keynum<=7) {
	if (op->contr->Swap_First==-1) {
	    op->contr->Swap_First=stat_trans[keynum];
	    sprintf(buf,"%s ->", short_stat_name[stat_trans[keynum]]);
	    new_draw_info(NDI_UNIQUE, 0,op,buf);
	}
	else
	    Swap_Stat(op,stat_trans[keynum]);

	send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"");
	return 1;
    }
#endif
    switch (key) {
	case 'n':
        case 'N': {
	    SET_FLAG(op, FLAG_WIZ);
	    if(op->map==NULL) {
		LOG(llevError,"Map == NULL in state 2\n");
		break;
	    }

	    /* So that enter_exit will put us at startx/starty */
	    op->x= -1;

	    enter_exit(op,NULL);

	    SET_ANIMATION(op, 2);     /* So player faces south */
	    /* Enter exit adds a player otherwise */
	    if(op->contr->loading == NULL) {
		insert_ob_in_map(op,op->map,op);
	    }
	    else {
		op->contr->removed = 0; /* Will insert pl. when map is loaded */
	    }
	    add_statbonus(op);
	    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"Now choose a character.\nPress any key to change outlook.\nPress `d' when you're pleased.\n");
	    op->contr->state = ST_CHANGE_CLASS;
	    if (op->msg)
		new_draw_info(NDI_BLUE, 0, op, op->msg);
	    return 0;
	}
     case 'y':
     case 'Y':
	roll_stats(op);
	send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"");
	return 1;

     case 'q':
     case 'Q':
      play_again(op);
      return 1;

     default:
#ifndef USE_SWAP_STATS
	  send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"Yes, No or Quit. Roll again?");
#else
	  send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,"Yes, No, Quit or 1-6.  Roll again?");
#endif /* USE_SWAP_STATS */
	return 0;
    }
    return 0;
}

/* This function takes the key that is passed, and does the
 * appropriate action with it (change class, or other things.
 */

int key_change_class(object *op, char key)
{
      int tmp_loop;

    if(key=='q'||key=='Q') {
      remove_ob(op);
      play_again(op);
      return 0;
    }
    if(key=='d'||key=='D') {
	char buf[MAX_BUF];

	/* this must before then initial items are given */
	esrv_new_player(op->contr, op->weight+op->carrying);
	create_treasure(find_treasurelist("starting_wealth"),op, 0, 0, 0);

	op->contr->state=ST_PLAYING;

	if (op->msg) {
	    free_string(op->msg);
	    op->msg=NULL;
	}

	/* We create this now because some of the unique maps will need it
	 * to save here.
	 */
	sprintf(buf,"%s/%s/%s",settings.localdir,settings.playerdir,op->name);
	make_path_to_file(buf);

#ifdef AUTOSAVE
	op->contr->last_save_tick = pticks;
#endif
	start_info(op);
	CLEAR_FLAG(op, FLAG_WIZ);
#ifdef ALLOW_SKILLS
	(void) init_player_exp(op);
#endif
	give_initial_items(op,op->randomitems);
#ifdef ALLOW_SKILLS
	(void) link_player_skills(op);
#endif
	esrv_send_inventory(op, op);
	return 0;
    }

    /* Following actually changes the class - this is the default command
     * if we don't match with one of the options above.
     */

    tmp_loop = 0;
    while(!tmp_loop) {
      char *name = add_string (op->name);
      int x = op->x, y = op->y;
      remove_statbonus(op);
      remove_ob (op);
      op->arch = get_player_archetype(op->arch);
      copy_object (&op->arch->clone, op);
      op->stats = op->contr->orig_stats;
      free_string (op->name);
      op->name = name;
      op->x = x;
      op->y = y;
      SET_ANIMATION(op, 2);    /* So player faces south */
      insert_ob_in_map (op, op->map, op);
      strncpy(op->contr->title,op->arch->clone.name,MAX_NAME);
      add_statbonus(op);
      tmp_loop=allowed_class(op);
    }
    update_object(op);
    esrv_update_item(UPD_FACE,op,op);
    fix_player(op);
    op->stats.hp=op->stats.maxhp;
    op->stats.sp=op->stats.maxsp;
#ifndef ALLOW_SKILLS
	 op->stats.grace=op->stats.maxgrace; 
#else
	 op->stats.grace=0;
#endif
    op->contr->last_value= -1;
    if (op->msg) 
	new_draw_info(NDI_BLUE, 0, op, op->msg);
    send_query(&op->contr->socket, CS_QUERY_SINGLECHAR,"");
    return 0;
}

int key_confirm_quit(object *op, char key)
{
    char buf[MAX_BUF];

    if(key!='y'&&key!='Y'&&key!='q'&&key!='Q') {
      op->contr->state=ST_PLAYING;
      new_draw_info(NDI_UNIQUE, 0,op,"OK, continuing to play.");
      return 1;
    }
    terminate_all_pets(op);
    remove_ob(op);
    op->direction=0;
    op->contr->count_left=0;
    op->map->players--;
    /* Just in case we fail to add the player */
    op->map->timeout = MAP_TIMEOUT(op->map);
    new_draw_info_format(NDI_UNIQUE | NDI_ALL, 5, NULL,
	"%s quits the game.",op->name);

    strcpy(op->contr->killer,"quit");
    check_score(op);
    op->contr->party_number=(-1);
#ifdef SET_TITLE
    op->contr->own_title[0]='\0';
#endif /* SET_TITLE */
    if(!QUERY_FLAG(op,FLAG_WAS_WIZ)) {
      sprintf(buf,"%s/%s/%s/%s.pl",settings.localdir,settings.playerdir,op->name,op->name);
      if(unlink(buf)== -1 && settings.debug >= llevDebug)
        perror("crossfire (delete character)");
    }
    play_again(op);
    return 1;
}


void flee_player(object *op) {
  int dir,diff;
  if(op->stats.hp < 0) {
    LOG(llevDebug, "Fleeing player is dead.\n");
    CLEAR_FLAG(op, FLAG_SCARED);
    return;
  }
  if(op->enemy==NULL) {
    LOG(llevDebug,"Fleeing player had no enemy.\n");
    CLEAR_FLAG(op, FLAG_SCARED);
    return;
  }
  if(!(RANDOM()%5)&&RANDOM()%20+1>=savethrow[op->level]) {
    op->enemy=NULL;
    CLEAR_FLAG(op, FLAG_SCARED);
    return;
  }
  dir=absdir(4+find_dir_2(op->x-op->enemy->x,op->y-op->enemy->y));
  for(diff=0;diff<3;diff++) {
    int m=1-(RANDOM()&2);
    if(move_ob(op,absdir(dir+diff*m),op)||
       (diff==0&&move_ob(op,absdir(dir-diff*m),op))) {
      draw(op);
      return;
    }
  }
  /* Cornered, get rid of scared */
  CLEAR_FLAG(op, FLAG_SCARED);
  op->enemy=NULL;
}


/* check_pick sees if there is stuff to be picked up/picks up stuff.
 * IT returns 1 if the player should keep on moving, 0 if he should
 * stop.
 */
int check_pick(object *op) {
  object *tmp, *next;
  tag_t next_tag=0, op_tag;
  int stop = 0;

  if (QUERY_FLAG (op, FLAG_FLYING))
    return 1;

  op_tag = op->count;

  next = op->below;
  if (next)
    next_tag = next->count;
  while (next && ! was_destroyed (next, next_tag))
  {
    tmp = next;
    next = tmp->below;
    if (next)
      next_tag = next->count;

    if (was_destroyed (op, op_tag))
        return 0;

    if ( ! can_pick (op, tmp))
      continue;

#ifdef SEARCH_ITEMS
    if (op->contr->search_str[0]!='\0')
    {
      if (item_matched_string (op, tmp, op->contr->search_str))
        pick_up (op, tmp);
      continue;
    }
#endif /* SEARCH_ITEMS */

    switch (op->contr->mode) {
	case 0:	return 1;	/* don't pick up */

	case 1:
		pick_up (op, tmp);
		return 1;

	case 2:
		pick_up (op, tmp);
		return 0;

	case 3: return 0;	/* stop before pickup */

	case 4:
		pick_up (op, tmp);
		break;

	case 5: 
		pick_up (op, tmp);
		stop = 1;
		break;

	case 6:
		if (QUERY_FLAG (tmp, FLAG_KNOWN_MAGICAL) &&
		    ! QUERY_FLAG(tmp, FLAG_KNOWN_CURSED))
		  pick_up(op, tmp);
		break;

	case 7:
		if (tmp->type == MONEY || tmp->type == GEM)
		  pick_up(op, tmp);
		break;

	default:
		/* use value density */
		if ( ! QUERY_FLAG (tmp, FLAG_UNPAID)
		    && (query_cost (tmp, op, F_TRUE) * 100
		        / (tmp->weight * MAX (tmp->nrof, 1)))
                       >= op->contr->mode)
		  pick_up(op,tmp);
    }
  }
  return ! stop;
}

/*
 *  Find an arrow in the inventory and after that
 *  in the right type container (quiver). Pointer to the 
 *  found object is returned.
 */
object *find_arrow(object *op, char *type)
{
  object *tmp = NULL;

  for(op=op->inv; op; op=op->below)
    if(!tmp && op->type==CONTAINER && op->race==type &&
      QUERY_FLAG(op,FLAG_APPLIED))
      tmp = find_arrow (op, type);
    else if (op->type==ARROW && op->race==type)
      return op;
  return tmp;
}

/*
 *  Player fires a bow. This probably should be combined with
 *  monster_use_bow().
 */
static void fire_bow(object *op, int dir)
{
  object *bow, *arrow = NULL, *left;
  tag_t left_tag;
  for(bow=op->inv; bow; bow=bow->below)
    if(bow->type==BOW && QUERY_FLAG(bow, FLAG_APPLIED))
      break;
#ifdef MANY_CORES /* there must be applied bow if shoot_type is range_bow */
  if (!bow) {
    LOG (llevError, "Range: bow without activated bow.\n");
    abort();
  }
#endif
  if( !bow->race ) {
    sprintf (errmsg, "Your %s is broken.", bow->name);
    new_draw_info(NDI_UNIQUE, 0,op, errmsg);
    op->contr->count_left=0;
    return;
  }
  if ((arrow=find_arrow(op, bow->race)) == NULL) {
    sprintf (errmsg, "You have no %s left.", bow->race);
    new_draw_info(NDI_UNIQUE, 0,op,errmsg);
    op->contr->count_left=0;
    return;
  }
  if(wall(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) {
    new_draw_info(NDI_UNIQUE, 0,op,"Something is in the way.");
    op->contr->count_left=0;
    return;
  }
  /* this should not happen, but sometimes does */
  if (arrow->nrof==0) {
	remove_ob(arrow);
	free_object(arrow);
	return;
  }
  left = arrow; /* these are arrows left to the player */
  left_tag = left->count;
  arrow = get_split_ob(arrow, 1);
  set_owner(arrow,op);
  arrow->direction=dir;
  arrow->x = op->x;
  arrow->y = op->y;
  arrow->speed = 1;
  op->speed_left = 0.01 - (float) FABS(op->speed) * 100 / bow->stats.sp;
  fix_player(op);
  update_ob_speed(arrow);
  arrow->speed_left = 0;
  SET_ANIMATION(arrow,dir);
  arrow->stats.sp = arrow->stats.wc; /* save original wc and dam */
  arrow->stats.hp = arrow->stats.dam; 
  arrow->stats.dam += (QUERY_FLAG(bow, FLAG_NO_STRENGTH) ?
		      0 : dam_bonus[op->stats.Str]) +
			bow->stats.dam + bow->magic + arrow->magic;
  arrow->stats.wc = 20 - bow->magic - arrow->magic - SK_level(op) -
    dex_bonus[op->stats.Dex] - thaco_bonus[op->stats.Str] - arrow->stats.wc -
    bow->stats.wc;
  arrow->level = SK_level (op);

  arrow->map = op->map;
  SET_FLAG(arrow, FLAG_FLYING);
  SET_FLAG(arrow, FLAG_FLY_ON);
  SET_FLAG(arrow, FLAG_WALK_ON);
  play_sound_map(op->map, op->x, op->y, SOUND_FIRE_ARROW);
  insert_ob_in_map(arrow,op->map,op);
  move_arrow(arrow);
  if (was_destroyed (left, left_tag))
      esrv_del_item(op->contr, left_tag);
  else
      esrv_send_item(op, left);
}


void fire(object *op,int dir) {
  object *weap=NULL;
  int spellcost=0;

  /* check for loss of invisiblity/hide */
  if (action_makes_visible(op)) make_visible(op);

   /* a check for players, make sure things are groovy. This routine
    * will change the skill of the player as appropriate in order to
    * fire whatever is requested. In the case of spells (range_magic)
    * it handles whether cleric or mage spell is requested to be cast. 
    * -b.t. 
    */ 
#ifdef ALLOW_SKILLS 
  if(op->type==PLAYER) 
	if(!check_skill_to_fire(op)) return;
#endif

  switch(op->contr->shoottype) {
  case range_none:
    return;

  case range_bow:
    fire_bow(op, dir);
    return;

  case range_magic: /* Casting spells */

    op->contr->shoottype= range_magic;

    spellcost=cast_spell(op,op,dir,op->contr->chosen_spell,0,spellNormal,NULL);

    if(spells[op->contr->chosen_spell].cleric)
        op->stats.grace-=spellcost;
    else
	op->stats.sp-=spellcost;

    return;

  case range_wand:
    for(weap=op->inv;weap!=NULL;weap=weap->below)
      if(weap->type==WAND&&QUERY_FLAG(weap, FLAG_APPLIED))
	break;
    if(weap==NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"You have no wand readied.");
      op->contr->count_left=0;
      return;
    }
    if(weap->stats.food<=0) {
      play_sound_player_only(op->contr, SOUND_WAND_POOF,0,0);
      new_draw_info(NDI_UNIQUE, 0,op,"The wand says poof.");
      return;
    }
    if(cast_spell(op,weap,dir,op->contr->chosen_item_spell,0,spellWand,NULL)) {
      SET_FLAG(op, FLAG_BEEN_APPLIED); /* You now know something about it */
      if (!(--weap->stats.food))
      {
	object *tmp;
	if (weap->arch) {
	  CLEAR_FLAG(weap, FLAG_ANIMATE);
	  weap->face = weap->arch->clone.face;
	  weap->speed = 0;
	  update_ob_speed(weap);
	}
	if ((tmp=is_player_inv(weap)))
	    esrv_update_item(UPD_ANIM, tmp, weap);
      }
    }
    return;
  case range_rod:
  case range_horn:
    for(weap=op->inv;weap!=NULL;weap=weap->below)
      if(QUERY_FLAG(weap, FLAG_APPLIED)&&
	 weap->type==(op->contr->shoottype==range_rod?ROD:HORN))
	break;
    if(weap==NULL) {
      char buf[MAX_BUF];
      sprintf(buf, "You have no %s readied.",
	op->contr->shoottype == range_rod ? "rod" : "horn");
      new_draw_info(NDI_UNIQUE, 0,op, buf);
      op->contr->count_left=0;
      return;
    }
    if(weap->stats.hp<spells[weap->stats.sp].sp) {
#if 0
      LOG(llevDebug,"Horn/rod: %d < %d (%d)\n", weap->stats.hp, spells[weap->stats.sp].sp, weap->stats.sp);
#endif
      play_sound_player_only(op->contr, SOUND_WAND_POOF,0,0);
      if (op->contr->shoottype == range_rod)
	new_draw_info(NDI_UNIQUE, 0,op,"The rod whines for a while, but nothing happens.");
      else
	new_draw_info(NDI_UNIQUE, 0,op,
	          "No matter how hard you try you can't get another note out.");
      return;
    }
    if(cast_spell(op,weap,dir,op->contr->chosen_item_spell,0,
       op->contr->shoottype == range_rod ? spellRod : spellHorn,NULL)) {
      SET_FLAG(op, FLAG_BEEN_APPLIED); /* You now know something about it */
      drain_rod_charge(weap);
    }
    return;
  case range_scroll: /* Control summoned monsters from scrolls */
    if(op->contr->golem==NULL) {
      op->contr->shoottype=range_none;
      op->contr->chosen_spell= -1;
    }
    else 
	control_golem(op->contr->golem, dir);
    return;
  case range_skill:
    if(!op->chosen_skill) { 
	if(op->type==PLAYER)
      	    new_draw_info(NDI_UNIQUE, 0,op,"You have no applicable skill to use.");
	return;
    }
    (void) do_skill(op,dir,NULL);
    return;
  default:
    new_draw_info(NDI_UNIQUE, 0,op,"Illegal shoot type.");
    op->contr->count_left=0;
    return;
  }
}



/* find_key
 * We try to find a key for the door as passed.  If we find a key
 * and successfully use it, we return the key, otherwise NULL
 * This function merges both normal and locked door, since the logic
 * for both is the same - just the specific key is different.
 * pl is the player, 
 * inv is the objects inventory to searched 
 * door is the door we are trying to match against.
 * This function can be called recursively to search containers.
 */

object * find_key(object *pl, object *container, object *door)
{
    object *tmp,*key;

    /* Should not happen, but sanity checking is never bad */
    if (container->inv == NULL) return NULL;

    /* First, lets try to find a key in the top level inventory */
    for (tmp=container->inv; tmp!=NULL; tmp=tmp->below) {
	if (door->type==DOOR && tmp->type==KEY) break;
	/* For sanity, we should really check door type, but other stuff 
	 * (like containers) can be locked with special keys
	 */
	if (tmp->slaying && tmp->type==SPECIAL_KEY &&
	    tmp->slaying==door->slaying) break;
    }
    /* No key found - lets search inventories now */
    /* If we find and use a key in an inventory, return at that time.
     * otherwise, if we search all the inventories and still don't find
     * a key, return
     */
    if (!tmp) {
	for (tmp=container->inv; tmp!=NULL; tmp=tmp->below) {
	    /* No reason to search empty containers */
	    if (tmp->type==CONTAINER && tmp->inv) {
		if ((key=find_key(pl, tmp, door))!=NULL) return key;
	    }
	}
	if (!tmp) return NULL;
    }
    /* We get down here if we have found a key.  Now if its in a container,
     * see if we actually want to use it
     */
    if (pl!=container) {
	/* Only let players use keys in containers */
	if (!pl->contr) return NULL;
	/* cases where this fails:
	 * If we only search the player inventory, return now since we
	 * are not in the players inventory.
	 * If the container is not active, return now since only active
	 * containers can be used.
	 * If we only search keyrings and the container does not have
	 * a race/isn't a keyring.
	 * No checking for all containers - to fall through past here,
	 * inv must have been an container and must have been active.
	 *
	 * Change the color so that the message doesn't disappear with
	 * all the others.
	 */
	if (pl->contr->usekeys == key_inventory ||
	    !QUERY_FLAG(container, FLAG_APPLIED) ||
	    (pl->contr->usekeys == keyrings &&
	     (!container->race || strcmp(container->race, "keys")))
	      ) {
	    new_draw_info_format(NDI_UNIQUE|NDI_BROWN, 0, pl, 
		"The %s in your %s vibrates as you approach the door",
		query_name(tmp), query_name(container));
	    return NULL;
	}
    }
    return tmp;
}

/* This function is just part of a breakup from move_player.
 * It should keep the code cleaner.
 * When this is called, the players direction has been updated
 * (taking into accoutn confusion.)  The player is also actually
 * going to try and move (not fire weapons).
 */

void move_player_attack(object *op, int dir)
{
  object *tmp;
  int nx=freearr_x[dir]+op->x,ny=freearr_y[dir]+op->y;


  /* If braced, or can't move to the square, and it is not out of the
   * map, attack it.  Note order of if statement is important - don't
   * want to be calling move_ob if braced, because move_ob will move the
   * player.  This is a pretty nasty hack, because if we could
   * move to some space, it then means that if we are braced, we should
   * do nothing at all.  As it is, if we are braced, we go through
   * quite a bit of processing.  However, it probably is less than what
   * move_ob uses.
   */
  if ((op->contr->braced || !move_ob(op,dir,op)) &&
    !out_of_map(op->map,nx,ny)) {
    

    if ((tmp=get_map_ob(op->map,nx,ny))==NULL) {
/*	LOG(llevError,"player_move_attack: get_map_ob returns NULL, but player can not more there.\n");*/
	return;
    }

    /* Go through all the objects, and stop if we find one of interest. */
    while (tmp->above!=NULL) {
      if ((QUERY_FLAG(tmp,FLAG_ALIVE) || QUERY_FLAG(tmp,FLAG_CAN_ROLL)
	    || tmp->type ==LOCKED_DOOR) && tmp!=op)
	    break;
      tmp=tmp->above;
    }
    
    if (tmp==NULL)	/* This happens anytime the player tries to move */
	return;		/* into a wall */

    if(tmp->head != NULL)
      tmp = tmp->head;

    /* If its a door, try to find a use a key.  If we do destroy the door,
     * might as well return immediately as there is nothing more to do -
     * otherwise, we fall through to the rest of the code.
     */
    if ((tmp->type==DOOR && tmp->stats.hp>=0) || (tmp->type==LOCKED_DOOR)) {
	object *key=find_key(op, op, tmp);

	/* IF we found a key, do some extra work */
	if (key) {
	    play_sound_map(op->map, op->x, op->y, SOUND_OPEN_DOOR);
	    if(action_makes_visible(op)) make_visible(op);
	    if(tmp->inv && tmp->inv->type ==RUNE) spring_trap(tmp->inv,op);
	    if (tmp->type == DOOR) {
		hit_player(tmp,9999,op,AT_PHYSICAL); /* Break through the door */
	    }
	    else if(tmp->type==LOCKED_DOOR) {
		new_draw_info_format(NDI_UNIQUE, NDI_BROWN, op, 
				     "You open the door with the %s", query_short_name(key));
		remove_door2(tmp); /* remove door without violence ;-) */
	    }
	    /* Do this after we print the message */
	    decrease_ob(key); /* Use up one of the keys */
	    return; /* Nothing more to do below */
	} else if (tmp->type==LOCKED_DOOR) {
	    /* Might as well return now - no other way to open this */
	    new_draw_info(NDI_UNIQUE | NDI_NAVY, 0, op, tmp->msg);
	    return;
	}
    }

    /* The following deals with possibly attacking peaceful
     * or frienddly creatures.  Basically, all players are considered
     * unaggressive.  If the moving player has peaceful set, then the
     * object should be pushed instead of attacked.  It is assumed that
     * if you are braced, you will not attack friends accidently,
     * and thus will not push them.
     */

    /* If the creature is a pet, push it even if the player is not
     * peaceful.  Our assumption is the creature is a pet if the
     * player owns it and it is either friendly or unagressive.
     */
    if ((op->type==PLAYER) && get_owner(tmp)==op && 
	(QUERY_FLAG(tmp,FLAG_UNAGGRESSIVE) ||  QUERY_FLAG(tmp, FLAG_FRIENDLY)))
    {
	/* If we're braced, we don't want to switch places with it */
	if (op->contr->braced) return;
	play_sound_map(op->map, op->x, op->y, SOUND_PUSH_PLAYER);
	(void) push_ob(tmp,dir,op);
	if(op->contr->tmp_invis||op->hide) make_visible(op);
	return;
    }
    
    if ((tmp->type==PLAYER || tmp->enemy != op) &&
      (tmp->type==PLAYER || QUERY_FLAG(tmp,FLAG_UNAGGRESSIVE)
	|| QUERY_FLAG(tmp, FLAG_FRIENDLY)) && (op->contr->peaceful
	&& !op_on_battleground(op, NULL, NULL)) && (!op->contr->braced)) {
	  play_sound_map(op->map, op->x, op->y, SOUND_PUSH_PLAYER);
	  (void) push_ob(tmp,dir,op);
          if(op->contr->tmp_invis||op->hide) make_visible(op);
	}

      /* If the object is a boulder or other rollable object, then
       * roll it if not braced.  You can't roll it if you are braced.
       */
      else if(QUERY_FLAG(tmp,FLAG_CAN_ROLL)&&(!op->contr->braced)) {
	  recursive_roll(tmp,dir,op);
          if(action_makes_visible(op)) make_visible(op);

      }

      /* Any generic living creature.  Including things like doors.
       * Way it works is like this:  First, it must have some hit points
       * and be living.  Then, it must be one of the following:
       * 1) Not a player, 2) A player, but of a different party.  Note
       * that party_number -1 is no party, so attacks can still happen.
       */

      else
	if ((tmp->stats.hp>=0) && QUERY_FLAG(tmp, FLAG_ALIVE) &&
	((tmp->type!=PLAYER || op->contr->party_number==-1 ||
	op->contr->party_number!=tmp->contr->party_number))) {

	op->contr->has_hit = 1; /* The last action was to hit, so use weapon_sp */

#ifdef ALLOW_SKILLS
            skill_attack(tmp, op, 0, NULL); 
#else 
            (void) attack_ob(tmp, op);
#endif 
	  /* If attacking another player, that player gets automatic
	   * hitback, and doesn't loose luck either.
	   */
	  if (tmp->type == PLAYER && tmp->stats.hp >= 0 &&
	     !tmp->contr->has_hit)
	  {
	    short luck = tmp->stats.luck;
	    tmp->contr->has_hit = 1;
#ifdef ALLOW_SKILLS
	    skill_attack(op, tmp, 0, NULL); 
#else
	    (void) attack_ob(op, tmp);
#endif
	    tmp->stats.luck = luck;
	  }
	  if(action_makes_visible(op)) make_visible(op);
	}
    }
}

int move_player(object *op,int dir) {
    int face = dir ? (dir - 1) / 2 : -1, pick;

    if(op->map == NULL || op->map->in_memory != MAP_IN_MEMORY)
	return 0;

    /* peterm:  added following line */
    op->facing = dir;
    if(QUERY_FLAG(op,FLAG_CONFUSED) && dir)
	dir = absdir(dir + RANDOM()%3 + RANDOM()%3 - 2);

    if(op->hide) do_hidden_move(op);

    if(op->contr->fire_on) {
	fire(op,dir);
    }
    else move_player_attack(op,dir);

    /* Add special check for newcs players and fire on - this way, the
     * server can handle repeat firing.
     */
    pick = check_pick(op); 
    if (op->contr->fire_on || (op->contr->run_on && pick!=0)) {
	op->direction = dir;
    } else {
	op->direction=0;
    }

    if(face != -1)
	SET_ANIMATION(op,face);
    update_object(op);

    return 0;
}

/* This is similar to handle_player, below, but is only used by the
 * new client/server stuff.
 * This is sort of special, in that the new client/server actually uses
 * the new speed values for commands.
 *
 * Returns true if there are more actions we can do.
 */
int handle_newcs_player(object *op)
{
    if(op->contr->state == ST_PLAYING && op->contr->loading != NULL) {
	if(op->contr->loading->in_memory == MAP_IN_MEMORY) {
	    LOG(llevDebug,"In handle player, entering map\n");
	    enter_map(op);
	}
	else
	    return 0;
    }
    if(op->invisible&&!(QUERY_FLAG(op,FLAG_MAKE_INVIS))) {
	op->invisible--;
	if(!op->invisible) make_visible(op);
    }
    /* call this here - we also will call this in do_ericserver, but
     * the players time has been increased when doericserver has been
     * called, so we recheck it here.
     */
    HandleClient(&op->contr->socket, op->contr);
    if (op->speed_left<0) return 0;

    if(op->direction && (op->contr->run_on || op->contr->fire_on)) {
	/* All move commands take 1 tick, at least for now */
	op->speed_left--;
	/* Instead of all the stuff below, let move_player take care
	 * of it.  Also, some of the skill stuff is only put in
	 * there, as well as the confusion stuff.
	 */
	move_player(op, op->direction);
	if (op->speed_left>0) return 1;
	else return 0;
    }
    return 0;
}

int save_life(object *op) {
  object *tmp;
  char buf[MAX_BUF];
  if(!QUERY_FLAG(op,FLAG_LIFESAVE))
    return 0;
  for(tmp=op->inv;tmp!=NULL;tmp=tmp->below)
    if(QUERY_FLAG(tmp, FLAG_APPLIED)&&QUERY_FLAG(tmp,FLAG_LIFESAVE)) {
      play_sound_map(op->map, op->x, op->y, SOUND_OB_EVAPORATE);
      sprintf(buf,"Your %s vibrates violently, then evaporates.",
	      query_name(tmp));
      new_draw_info(NDI_UNIQUE, 0,op,buf);
      if (op->contr) 
	esrv_del_item(op->contr, tmp->count);
      remove_ob(tmp);
      free_object(tmp);
      CLEAR_FLAG(op, FLAG_LIFESAVE);
      if(op->stats.hp<0)
	op->stats.hp = op->stats.maxhp;
      if(op->stats.food<0)
	op->stats.food = 999;
      return 1;
    }
  LOG(llevError,"Error: LIFESAVE set without applied object.\n");
  CLEAR_FLAG(op, FLAG_LIFESAVE);
  return 0;
}

/* This goes throws the inventory and removes unpaid objects, and puts them
 * back in the map (location and map determined by values of env).  This
 * function will descend into containers.  op is the object to start the search
 * from.
 */
void remove_unpaid_objects(object *op, object *env)
{
    object *next;

    while (op) {
	next=op->below;	/* Make sure we have a good value, in case 
			 * we remove object 'op'
			 */
	if (QUERY_FLAG(op, FLAG_UNPAID)) {
	    remove_ob(op);
	    op->x = env->x;
	    op->y = env->y;
	    insert_ob_in_map(op, env->map, NULL);
	}
	else if (op->inv) remove_unpaid_objects(op->inv, env);
	op=next;
    }
}


void do_some_living(object *op) {
  int last_food=op->stats.food;
  int gen_hp, gen_sp, gen_grace;
  int over_hp, over_sp, over_grace;
  int i;
  int rate_hp = 1200;
  int rate_sp = 2500;
  int rate_grace = 2000;
  const int max_hp = 1;
  const int max_sp = 1;
  const int max_grace = 1;

  if (op->contr->outputs_sync) {
    for (i=0; i<NUM_OUTPUT_BUFS; i++)
      if (op->contr->outputs[i].buf!=NULL &&
	(op->contr->outputs[i].first_update+op->contr->outputs_sync)<pticks)
	    flush_output_element(op, &op->contr->outputs[i]);
  }

  if(op->contr->state==ST_PLAYING) {

    /* these next three if clauses make it possible to SLOW DOWN
       hp/grace/spellpoint regeneration. */
    if(op->contr->gen_hp >= 0 ) 
      gen_hp=(op->contr->gen_hp+1)*op->stats.maxhp;
    else {
      gen_hp = op->stats.maxhp;
      rate_hp -= rate_hp/2 * op->contr->gen_hp;
    }  
    if(op->contr->gen_sp >= 0 )
      gen_sp=(op->contr->gen_sp+1)*op->stats.maxsp;
    else {
      gen_sp = op->stats.maxsp;
      rate_sp -= rate_sp/2 * op->contr->gen_sp;
    }
    if(op->contr->gen_grace >= 0)
      gen_grace=(op->contr->gen_grace+1)*op->stats.maxgrace;
    else {
      gen_grace = op->stats.maxgrace;
      rate_grace -= rate_grace/2 * op->contr->gen_grace;
    }

    /* Regenerate Spell Points */
    if(op->contr->golem==NULL&&--op->last_sp<0) {
      gen_sp = gen_sp * 10 / (op->contr->gen_sp_armour < 10? 10 : op->contr->gen_sp_armour);
      if(op->stats.sp<op->stats.maxsp) {
	op->stats.sp++;
	op->stats.food--;
	if(op->contr->digestion<0)
	  op->stats.food+=op->contr->digestion;
	else if(op->contr->digestion>0&&RANDOM()%(1+op->contr->digestion))
	  op->stats.food=last_food;
      }
      if (max_sp>1) {
	over_sp = (gen_sp+10)/rate_sp;
	if (over_sp > 0) {
	  if(op->stats.sp<op->stats.maxsp) {
	    op->stats.sp += over_sp>max_sp ? max_sp : over_sp;
	    if(RANDOM()%rate_sp > ((gen_sp+10)%rate_sp))
	      op->stats.sp--;
	    if(op->stats.sp>op->stats.maxsp)
	      op->stats.sp=op->stats.maxsp;
	  }
	  op->last_sp=0;
	} else {
	  op->last_sp=rate_sp/(gen_sp<20 ? 30 : gen_sp+10);
	}
      } else {
	op->last_sp=rate_sp/(gen_sp<20 ? 30 : gen_sp+10);
      }
    }

    /* Regenerate Grace */
    /* I altered this a little - maximum grace is ony achieved through prayer -b.t.*/
    if(--op->last_grace<0) {
#ifndef ALLOW_SKILLS /* allow regen 'naturally' to only 1/2 maxgrace w/ skills code */ 
      if(op->stats.grace<op->stats.maxgrace)
#else
	if(op->stats.grace<op->stats.maxgrace/2)
#endif
	  op->stats.grace++; /* no penalty in food for regaining grace */
      if(max_grace>1) {
	over_grace = (gen_grace<20 ? 30 : gen_grace+10)/rate_grace;
	if (over_grace > 0) {
	  op->stats.sp += over_grace 
	    + (RANDOM()%rate_grace > ((gen_grace<20 ? 30 : gen_grace+10)%rate_grace))? -1 : 0;
	  op->last_grace=0;
	} else {
	  op->last_grace=rate_grace/(gen_grace<20 ? 30 : gen_grace+10);
	}
      } else {
	op->last_grace=rate_grace/(gen_grace<20 ? 30 : gen_grace+10);
      }
      /* wearing stuff doesn't detract from grace generation. */
    }

    /* Regenerate Hit Points */
    if(--op->last_heal<0) {
      if(op->stats.hp<op->stats.maxhp) {
	op->stats.hp++;
	op->stats.food--;
	if(op->contr->digestion<0)
	  op->stats.food+=op->contr->digestion;
	else if(op->contr->digestion>0&&RANDOM()%(1+op->contr->digestion))
	  op->stats.food=last_food;
      }
      if(max_hp>1) {
	over_hp = (gen_hp<20 ? 30 : gen_hp+10)/rate_hp;
	if (over_hp > 0) {
	  op->stats.sp += over_hp 
	    + (RANDOM()%rate_hp > ((gen_hp<20 ? 30 : gen_hp+10)%rate_hp))? -1 : 0;
	  op->last_heal=0;
	} else {
	  op->last_heal=rate_hp/(gen_hp<20 ? 30 : gen_hp+10);
	}
      } else {
	op->last_heal=rate_hp/(gen_hp<20 ? 30 : gen_hp+10);
      }
    }

    /* Digestion */
    if(--op->last_eat<0) {
      int bonus=op->contr->digestion>0?op->contr->digestion:0,
	penalty=op->contr->digestion<0?-op->contr->digestion:0;
      if(op->contr->gen_hp > 0)
	op->last_eat=25*(1+bonus)/(op->contr->gen_hp+penalty+1);
      else
	op->last_eat=25*(1+bonus)/(penalty +1);
      op->stats.food--;
    }
  }

  if(op->contr->state==ST_PLAYING&&op->stats.food<0&&op->stats.hp>=0) {
	  object *tmp;
	  for(tmp=op->inv;tmp!=NULL;tmp=tmp->below)
		  if(!QUERY_FLAG(tmp, FLAG_UNPAID)&&
			  (tmp->type==FOOD||tmp->type==DRINK||tmp->type==POISON))
			  {
		new_draw_info(NDI_UNIQUE, 0,op,"You blindly grab for a bite of food.");
	manual_apply(op,tmp,0);
	if(op->stats.food>=0||op->stats.hp<0)
	  break;
      }
  }
  while(op->stats.food<0&&op->stats.hp>0)
    op->stats.food++,op->stats.hp--;

  if (!op->contr->state&&!QUERY_FLAG(op,FLAG_WIZ)&&(op->stats.hp<0||op->stats.food<0)) 
    kill_player(op);
}

/* If the player should die (lack of hp, food, etc), we call this.
 * op is the player in jeopardy.  If the player can not be saved (not
 * permadeath, no lifesave), this will take care of removing the player
 * file.
 */
void kill_player(object *op)
{
    char buf[MAX_BUF];
    int x,y,i;
    mapstruct *map;  /*  this is for resurrection */
    object *tmp;
    int z;
    int num_stats_lose;
    int lost_a_stat;
    int lose_this_stat;
    int this_stat;
    
    if(save_life(op))
	return;
    
    /* If player dies on BATTLEGROUND, no stat/exp loss! For Combat-Arenas
     * in cities ONLY!!! It is very important that this doesn't get abused.
     * Look at op_on_battleground() for more info       --AndreasV
     */
    if (op_on_battleground(op, &x, &y)) {
      new_draw_info(NDI_UNIQUE | NDI_NAVY, 0,op,
		    "You have been defeated in combat!");
      new_draw_info(NDI_UNIQUE | NDI_NAVY, 0,op,
		    "Local medics have saved your life...");
      
      /* restore player */
      cast_heal(op, 0, SP_CURE_POISON);
      cast_heal(op, 0, SP_CURE_CONFUSION);
      cure_disease(op,0);  /* remove any disease */
      op->stats.hp=op->stats.maxhp;
      if (op->stats.food<=0) op->stats.food=999;
      
      /* create a bodypart-trophy to make the winner happy */
      tmp=arch_to_object(find_archetype("finger"));
      if (tmp != NULL) {
	sprintf(buf,"%s's finger",op->name);
	tmp->name = add_string(buf);
	sprintf(buf,"  This finger has been cutt off %s\n"
	            "  the %s, when he was defeated at\n  level %d by %s.\n",
	        op->name, op->contr->title, (int)(op->level),
	        op->contr->killer);
	tmp->msg=add_string(buf);
	tmp->value=0, tmp->material=0, tmp->type=0;
	tmp->x = op->x, tmp->y = op->y;
	insert_ob_in_map(tmp,op->map,op);
      }
      
      /* teleport defeated player to new destination*/
      transfer_ob(op, x, y, 0, NULL);
      op->contr->braced=0;
      return;
    }

    if(op->stats.food<0) {
#ifdef EXPLORE_MODE
	if (op->contr->explore) {
	    new_draw_info(NDI_UNIQUE, 0,op,"You would have starved, but you are");
	    new_draw_info(NDI_UNIQUE, 0,op,"in explore mode, so...");
	    op->stats.food=999;
	    return;
	}
#endif /* EXPLORE_MODE */
	sprintf(buf,"%s starved to death.",op->name);
	strcpy(op->contr->killer,"starvation");
    }
    else {
#ifdef EXPLORE_MODE
	if (op->contr->explore) {
	    new_draw_info(NDI_UNIQUE, 0,op,"You would have died, but you are");
	    new_draw_info(NDI_UNIQUE, 0,op,"in explore mode, so...");
	    op->stats.hp=op->stats.maxhp;
	    return;
	}
#endif /* EXPLORE_MODE */
	sprintf(buf,"%s died.",op->name);
    }
    play_sound_player_only(op->contr, SOUND_PLAYER_DIES,0,0);

    /*  save the map location for corpse, gravestone*/
    x=op->x;y=op->y;map=op->map;


#ifdef NOT_PERMADEATH
    /* NOT_PERMADEATH code.  This basically brings the character back to life
     * if they are dead - it takes some and a random stat.  See the config.h
     * file for a little more in depth detail about this.
     */

    /* Basically two ways to go - remove a stat permanently, or just
     * make it depletion.  This bunch of code deals with that aspect	
     * of death.
     */

    if (settings.balanced_stat_loss) {
        /* If stat loss is permanent, lose one stat only. */
        /* Lower level chars don't lose as many stats because they suffer more
           if they do. */
        /* Higher level characters can afford things such as potions of
           restoration, or better, stat potions. So we slug them that little
           bit harder. */
        /* GD */
        if (settings.stat_loss_on_death)
            num_stats_lose = 1;
        else
            num_stats_lose = 1 + op->level/BALSL_NUMBER_LOSSES_RATIO;
    } else {
        num_stats_lose = 1;
    }
    lost_a_stat = 0;

    for (z=0; z<num_stats_lose; z++) {
        if (settings.stat_loss_on_death) {
	    /* Pick a random stat and take a point off it.  Tell the player
	     * what he lost.
	     */
            i = RANDOM() % 7; 
            change_attr_value(&(op->stats), i,-1);
            check_stat_bounds(&(op->stats));
            change_attr_value(&(op->contr->orig_stats), i,-1);
            check_stat_bounds(&(op->contr->orig_stats));
            new_draw_info(NDI_UNIQUE, 0,op, lose_msg[i]);
            lost_a_stat = 1;
        } else {
            /* deplete a stat */
            archetype *deparch=find_archetype("depletion");
            object *dep;
            
            i = RANDOM() % 7;
            dep = present_arch_in_ob(deparch,op);
            if(!dep) {
	        dep = arch_to_object(deparch);
	        insert_ob_in_ob(dep, op);
            }
            lose_this_stat = 1;
            if (settings.balanced_stat_loss) {
                /* GD */
                /* Get the stat that we're about to deplete. */
                this_stat = get_attr_value(&(dep->stats), i);
                if (this_stat < 0) {
                    int loss_chance = 1 + op->level/BALSL_LOSS_CHANCE_RATIO;
                    int keep_chance = this_stat * this_stat;
                    /* Yes, I am paranoid. Sue me. */
                    if (keep_chance < 1)
                        keep_chance = 1;

                    /* There is a maximum depletion total per level. */
                    if (this_stat < -1 - op->level/BALSL_MAX_LOSS_RATIO) {
                        lose_this_stat = 0;
                    /* Take loss chance vs keep chance to see if we retain the stat. */
                    } else {
                        if ((RANDOM() % (loss_chance + keep_chance)) < keep_chance)
                            lose_this_stat = 0;
                        /* LOG(llevDebug, "Determining stat loss. Stat: %d Keep: %d Lose: %d Result: %s.\n",
                             this_stat, keep_chance, loss_chance,
                             lose_this_stat?"LOSE":"KEEP"); */
                    }
                }
            }
            
            if (lose_this_stat) {
                this_stat = get_attr_value(&(dep->stats), i);
		/* We could try to do something clever like find another
		 * stat to reduce if this fails.  But chances are, if
		 * stats have been depleted to -50, all are pretty low
		 * and should be roughly the same, so it shouldn't make a
		 * difference.
		 */
		if (this_stat>=-50) {
		    change_attr_value(&(dep->stats), i, -1);
		    SET_FLAG(dep, FLAG_APPLIED);
		    new_draw_info(NDI_UNIQUE, 0,op, lose_msg[i]);
		    fix_player(op);
		    lost_a_stat = 1;
		}
            }
        }
    }
    /* If no stat lost, tell the player. */
    if (!lost_a_stat)
    {
        /* determine_god() seems to not work sometimes... why is this?
           Should I be using something else? GD */
        char *god = determine_god(op);
        if (god && (strcmp(god, "none")))
            new_draw_info_format(NDI_UNIQUE, 0, op, "For a brief moment you feel the holy presence of %s protecting you.", god);
        else
            new_draw_info(NDI_UNIQUE, 0, op, "For a brief moment you feel a holy presence protecting you.");
    }

    /* Put a gravestone up where the character 'almost' died.  List the
     * exp loss on the stone.
     */
    tmp=arch_to_object(find_archetype("gravestone"));
    sprintf(buf,"%s's gravestone",op->name);
    tmp->name=add_string(buf);
    sprintf(buf,"RIP\nHere rests the hero %s the %s,\n"
	        "who lost %d experience when killed\n"
	        "by %s.\n",
	        op->name, op->contr->title, (int)(op->stats.exp * 0.20),
	        op->contr->killer);
    tmp->msg = add_string(buf);
    tmp->x=op->x,tmp->y=op->y;
    insert_ob_in_map (tmp, op->map, NULL);

 /**************************************/
 /*                                    */
 /* Subtract the experience points,    */
 /* if we died cause of food, give us  */
 /* food, and reset HP's...            */
 /*                                    */
 /**************************************/

    /* remove any poisoning and confusion the character may be suffering. */
    cast_heal(op, 0, SP_CURE_POISON);
    cast_heal(op, 0, SP_CURE_CONFUSION);
	 cure_disease(op,0);  /* remove any disease */
	
    add_exp(op, (op->stats.exp * -0.20));
    if(op->stats.food < 0) op->stats.food = 500;
    op->stats.hp = op->stats.maxhp;

 /*
  * Check to see if the player is in a shop.  IF so, then check to see if
  * the player has any unpaid items.  If so, remove them and put them back
  * in the map.
  */

    tmp= get_map_ob(op->map, op->x, op->y);
    if (tmp && tmp->type == SHOP_FLOOR) {
	remove_unpaid_objects(op->inv, op);
    }
 

 /****************************************/
 /*                                      */
 /* Move player to his current respawn-  */
 /* position (usually last savebed)      */
 /*                                      */
 /****************************************/

    tmp=get_object();

    EXIT_PATH(tmp) = add_string(op->contr->savebed_map);
    EXIT_X(tmp) = op->contr->bed_x;
    EXIT_Y(tmp) = op->contr->bed_y;
    enter_exit(op,tmp);

/* commenting this out seems to fix core dumps on some systems. */
    free_object(tmp);

 /**************************************/
 /*                                    */
 /* Repaint the characters inv, and    */
 /* stats, and show a nasty message ;) */
 /*                                    */
 /**************************************/

    new_draw_info(NDI_UNIQUE, 0,op,"YOU HAVE DIED.");
    op->contr->braced=0;
    save_player(op,1);
    return;
#endif

/* If NOT_PERMADETH is set, then the rest of this is not reachable.  This
 * should probably be embedded in an else statement.
 */

    op->contr->party_number=(-1);
#ifdef SET_TITLE
    op->contr->own_title[0]='\0';
#endif /* SET_TITLE */
    op->contr->count_left=0;
    new_draw_info(NDI_UNIQUE|NDI_ALL, 0,NULL, buf);
    check_score(op);
    if(op->contr->golem!=NULL) {
      remove_friendly_object(op->contr->golem);
      remove_ob(op->contr->golem);
      free_object(op->contr->golem);
      op->contr->golem=NULL;
    }
    loot_object(op); /* Remove some of the items for good */
    remove_ob(op);
    op->direction=0;
    if(!QUERY_FLAG(op,FLAG_WAS_WIZ)&&op->stats.exp) {
      delete_character(op->name,0);
#ifndef NOT_PERMADEATH
#ifdef RESURRECTION
	/* save playerfile sans equipment when player dies
	** then save it as player.pl.dead so that future resurrection
	** type spells will work on them nicely
	*/
	op->stats.hp = op->stats.maxhp;
	op->stats.food = 999;

	/*  set the location of where the person will reappear when  */
	/* maybe resurrection code should fix map also */
	strcpy(op->contr->maplevel, EMERGENCY_MAPPATH);
	if(op->map!=NULL)
	    op->map = NULL;
	op->x = EMERGENCY_X;
	op->y = EMERGENCY_Y;
	save_player(op,0);
	op->map = map;
	/* please see resurrection.c: peterm */
	dead_player(op);
#endif
#endif
    }
    play_again(op);
#ifdef NOT_PERMADEATH
    tmp=arch_to_object(find_archetype("gravestone"));
    sprintf(buf,"%s's gravestone",op->name);
    if (tmp->name)
        free_string (tmp->name);
    tmp->name=add_string(buf);
    sprintf(buf,"RIP\nHere rests the hero %s the %s,\nwho was killed by %s.\n",
	    op->name, op->contr->title, op->contr->killer);
    if (tmp->msg)
        free_string (tmp->msg);
    tmp->msg = add_string(buf);
    tmp->x=x,tmp->y=y;
    insert_ob_in_map (tmp, map, NULL);
#else
    /*  peterm:  added to create a corpse at deathsite.  */
    tmp=arch_to_object(find_archetype("corpse_pl"));
    sprintf(buf,"%s", op->name);
    if (tmp->name)
	free_string (tmp->name);
    tmp->name=add_string(buf);
    tmp->level=op->level;
    tmp->x=x;tmp->y=y;
    if (tmp->msg)
	free_string(tmp->msg);
    tmp->msg = add_string (gravestone_text(op));
    SET_FLAG (tmp, FLAG_UNIQUE);
    insert_ob_in_map (tmp, map, NULL);
#endif
}


void loot_object(object *op) { /* Grab and destroy some treasure */
  object *tmp,*tmp2,*next;

  if (op->container) { /* close open sack first */
      esrv_apply_container (op, op->container);
  }

  for(tmp=op->inv;tmp!=NULL;tmp=next) {
    next=tmp->below;
    if (tmp->type==EXPERIENCE) continue;
    remove_ob(tmp);
    tmp->x=op->x,tmp->y=op->y;
    if (tmp->type == CONTAINER) { /* empty container to ground */
	loot_object(tmp);
    }
    if(!QUERY_FLAG(tmp, FLAG_UNIQUE) && (QUERY_FLAG(tmp, FLAG_STARTEQUIP) 
       || QUERY_FLAG(tmp,FLAG_NO_DROP) || !(RANDOM()%3))) {
      if(tmp->nrof>1) {
	tmp2=get_split_ob(tmp,1+RANDOM()%(tmp->nrof-1));
	free_object(tmp2);
	insert_ob_in_map(tmp,op->map,NULL);
      } else
	free_object(tmp);
    } else
      insert_ob_in_map(tmp,op->map,NULL);
  }
}

/*
 * fix_weight(): Check recursively the weight of all players, and fix
 * what needs to be fixed.  Refresh windows and fix speed if anything
 * was changed.
 */

void fix_weight() {
  player *pl;
  for (pl = first_player; pl != NULL; pl = pl->next) {
    int old = pl->ob->carrying, sum = sum_weight(pl->ob);
    if(old == sum)
      continue;
    fix_player(pl->ob);
    esrv_update_item(UPD_WEIGHT, pl->ob, pl->ob);
    LOG(llevDebug,"Fixed inventory in %s (%d -> %d)\n",
	pl->ob->name, old, sum);
  }
}

void fix_luck() {
  player *pl;
  for (pl = first_player; pl != NULL; pl = pl->next)
    if (!pl->ob->contr->state)
      change_luck(pl->ob, 0);
}


/* cast_dust() - handles op throwing objects of type 'DUST' */
 
void cast_dust (object *op, object *throw_ob, int dir) {
  archetype *arch = find_archetype(spells[throw_ob->stats.sp].archname);
 
  /* casting POTION 'dusts' is really a use_magic_item skill */
  if(op->type==PLAYER&&throw_ob->type==POTION
	&&!change_skill(op,SK_USE_MAGIC_ITEM)) {
      LOG(llevError,"Player %s lacks critical skill use_magic_item!\n",
                op->name);
      return;
  }
 
  if(throw_ob->type==POTION&&arch!= NULL)
    cast_cone(op,throw_ob,dir,10,throw_ob->stats.sp,arch,1);
  else if((arch=find_archetype("dust_effect"))!=NULL) { /* dust_effect */
    cast_cone(op,throw_ob,dir,1,0,arch,0);
  } else /* problem occured! */ 
    LOG(llevError,"cast_dust() can't find an archetype to use!\n");
 
  if (op->type==PLAYER&&arch)
    new_draw_info_format(NDI_UNIQUE, 0,op,"You cast %s.",query_name(throw_ob));
  if(!QUERY_FLAG(throw_ob,FLAG_REMOVED)) remove_ob(throw_ob);
  free_object(throw_ob);
}

void make_visible (object *op) {
    op->hide = 0;
    op->invisible = 0;
    if(op->type==PLAYER) 
      op->contr->tmp_invis = 0;
    if(QUERY_FLAG(op, FLAG_UNDEAD)&&!is_true_undead(op)) 
      CLEAR_FLAG(op, FLAG_UNDEAD);
    update_object(op);
}

int is_true_undead(object *op) {
  object *tmp=NULL;
  
  if(QUERY_FLAG(&op->arch->clone,FLAG_UNDEAD)) return 1;

  if(op->type==PLAYER)
    for(tmp=op->inv;tmp;tmp=tmp->below)
       if(tmp->type==EXPERIENCE && tmp->stats.Wis)
	  if(QUERY_FLAG(tmp,FLAG_UNDEAD)) return 1;
  return 0;
}

/* look at the surrounding terrain to determine
 * the hideability of this object. Positive levels
 * indicate greater hideability.
 */

int hideability(object *ob) {
  int i,x,y,level=0;

  if(!ob||!ob->map) return 0;

  /* so, on normal lighted maps, its hard to hide */
  level=ob->map->darkness - 2;

  /* this also picks up whether the object is glowing.
   * If you carry a light on a non-dark map, its not
   * as bad as carrying a light on a pitch dark map */
  if(has_carried_lights(ob)) level =- (10 + (2*ob->map->darkness));

  /* scan through all nearby squares for terrain to hide in */
  for(i=0,x=ob->x,y=ob->y;i<9;i++,x=ob->x+freearr_x[i],y=ob->y+freearr_y[i]) { 
    if(out_of_map(ob->map,x,y)) { continue; }
    if(blocks_view(ob->map,x,y)) /* something to hide near! */
      level += 2;
    else /* open terrain! */
      level -= 1;
  }
  
#if 0
  LOG(llevDebug,"hideability of %s is %d\n",ob->name,level);
#endif
  return level;
}

/* For Hidden creatures - a chance of becoming 'unhidden'
 * every time they move - as we subtract off 'invisibility'
 * AND, for players, if they move into a ridiculously unhideable
 * spot (surrounded by clear terrain in broad daylight). -b.t.
 */

void do_hidden_move (object *op) {
    int hide=0, num=RANDOM()%20;
    
    if(!op || !op->map) return;

    /* its *extremely* hard to run and sneak/hide at the same time! */
    if(op->type==PLAYER && op->contr->run_on) {
      if(num >= SK_level(op)) {
        new_draw_info(NDI_UNIQUE,0,op,"You ran too much! You are no longer hidden!");
        make_visible(op);
        return;
      } else num += 20;
    }
    num += op->map->difficulty;
    hide=hideability(op); /* modify by terrain hidden level */
    num -= hide;
    if((op->type==PLAYER&&hide<-10) || ((op->invisible-=num)<=0)) {
      make_visible(op);
      if(op->type==PLAYER) new_draw_info(NDI_UNIQUE, 0,op,
          "You moved out of hiding! You are visible!");
    }
}

/* determine if who is standing near a hostile creature. */

int stand_near_hostile( object *who ) {
  object *tmp=NULL;
  int i,friendly=0,player=0;

  if(!who) return 0;

  if(who->type==PLAYER) player=1; 
  else friendly = QUERY_FLAG(who,FLAG_FRIENDLY);

  /* search adjacent squares */
  for(i=1;i<9;i++) {
    if (out_of_map(who->map, who->x+freearr_x[i],who->y+freearr_y[i])) continue;
    for(tmp=get_map_ob(who->map,who->x+freearr_x[i],who->y+freearr_y[i]);
	tmp;tmp=tmp->above) {
	
	if((player||friendly)
	   &&QUERY_FLAG(tmp,FLAG_MONSTER)&&!QUERY_FLAG(tmp,FLAG_UNAGGRESSIVE)) 
	    return 1;
	else if(tmp->type==PLAYER) return 1;
    }
  }

  return 0;
}

/* check the player los field for viewability of the 
 * object op. This function works fine for monsters,
 * but we dont worry if the object isnt the top one in 
 * a pile (say a coin under a table would return "viewable"
 * by this routine). Another question, should we be
 * concerned with the direction the player is looking 
 * in? Realistically, most of use cant see stuff behind
 * our backs...on the other hand, does the "facing" direction
 * imply the way your head, or body is facing? Its possible
 * for them to differ. Sigh, this fctn could get a bit more complex.
 * -b.t. */

int player_can_view (object *pl,object *op) {

  if(pl->type!=PLAYER) {
    LOG(llevError,"player_can_view() called for non-player object\n");
    return -1;
  }

  if(pl&&op&&pl->map==op->map) { 
    /* starting with the 'head' part, lets loop
     * through the object and find if it has any
     * part that is in the los array but isnt on 
     * a blocked los square. */
    if(op->head) { op = op->head; }
    while(op) {
      if(pl->y + WINUPPER <= op->y && pl->y + WINLOWER >= op->y 
          && pl->x + WINLEFT <= op->x && pl->x + WINRIGHT >= op->x
          && !pl->contr->blocked_los[op->x-pl->x+5][op->y-pl->y+5] ) 
        return 1;
      op = op->more;
    }
  }
  return 0;
}

/* routine for both players and monsters. We call this when
 * there is a possibility for our action distrubing our hiding
 * place or invisiblity spell. Artefact invisiblity is not
 * effected by this. If we arent invisible to begin with, we 
 * return 0. 
 */
int action_makes_visible (object *op) {

  if(op->invisible && QUERY_FLAG(op,FLAG_ALIVE)) {
    if(!QUERY_FLAG(op,FLAG_MAKE_INVIS)) 
      return 0; 
    else if(op->hide || (op->contr&&op->contr->tmp_invis)) { 
      new_draw_info_format(NDI_UNIQUE, 0,op,"You become %!",op->hide?"unhidden":"visible");
      return 1; 
    } else if(op->contr && !op->contr->shoottype==range_magic) { 
	  /* improved invis is lost EXCEPT for case of casting of magic */
          new_draw_info(NDI_UNIQUE, 0,op,"Your invisibility spell is broken!");
          return 1;
    }
  }

  return 0;
}

/* op_on_battleground - checks if the given object op (usually
 * a player) is standing on a valid battleground-tile,
 * function returns TRUE/FALSE. If true x, y returns the battleground
 * -exit-coord. (and if x, y not NULL)
 */
int op_on_battleground (object *op, int *x, int *y) {
  object *tmp;
  int floor;
  
  /* A battleground-tile needs the following attributes to be valid:
   * is_floor 1 (has to be the FIRST floor beneath the player's feet),
   * name="battleground", no_pick 1, type=58 (type BATTLEGROUND)
   * and the exit-coordinates sp/hp must both be > 0.
   * => The intention here is to prevent abuse of the battleground-
   * feature (like pickable or hidden battleground tiles). */
  for (tmp=op->below, floor=0; tmp!=NULL && !floor; tmp=tmp->below) {
    if (QUERY_FLAG (tmp, FLAG_IS_FLOOR)) {
      if (QUERY_FLAG (tmp, FLAG_NO_PICK) &&
	  strcmp(tmp->name, "battleground")==0 &&
	  tmp->type == BATTLEGROUND && EXIT_X(tmp) && EXIT_Y(tmp)) {
	if (x != NULL && y != NULL)
	  *x=EXIT_X(tmp), *y=EXIT_Y(tmp);
	return 1;
      } else
	floor++; /* onle the uppermost IS_FLOOR object is processed */
    }
  }
  
  tmp = NULL;
  return 0;
}

