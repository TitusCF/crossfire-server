/*
 * static char *rcsid_input_c =
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

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <version.h>
#include <spells.h>
#include <skills.h>
#include <newclient.h>
#include <commands.h>

int onoff_value(char *line)
{
  int i;

  if (sscanf(line, "%d", &i))
    return (i != 0);
  switch (line[0]) {
  case 'o':
    switch (line[1]) {
    case 'n': return 1;		/* on */
    default:  return 0;		/* o[ff] */
    }
  case 'y':			/* y[es] */
  case 'k':			/* k[ylla] */
  case 's':
  case 'd':
    return 1;
  case 'n':			/* n[o] */
  case 'e':			/* e[i] */
  case 'u':
  default:
    return 0;
  }
}

/*
 * flags:
 *  0 - inv & below
 *  1 - below & inv
 *  2 - inv
 *  3 - below
 */
object **find_objects(object *op, char *line, int flags)
{

  return NULL;
}


void drop_inventory(object *op) {
  object *inv,*n=NULL;
  if(op->contr->last_used!=NULL) {
    inv=op->contr->last_used;
    if(QUERY_FLAG(inv, FLAG_FREED)||inv->count!=op->contr->last_used_id||
       inv->env==NULL||(inv->env!=op&&inv->env->env!=op)) {
      op->contr->last_used=NULL;
      op->contr->last_used_id=0;
      drop_inventory(op);
      return;
    }
    inv=op->contr->last_used;
    if(inv->below!=NULL)
      n=inv->below;
    else if(inv->above!=NULL)
      n=inv->above;
    if(n!=NULL) {
      op->contr->last_used=n;
      op->contr->last_used_id=n->count;
    }
  } else {
    inv=op->inv;
    while(inv&&inv->invisible)
      inv=inv->below;
  }
  if(inv)
    drop(op,inv);
  else
    new_draw_info(NDI_UNIQUE, 0,op,"You have nothing to drop!");
}


/* op is the player
 * tmp is the monster being examined.
 */
void examine_monster(object *op,object *tmp) {
    object *mon=tmp->head?tmp->head:tmp;
    archetype *at=tmp->arch;

    if(QUERY_FLAG(mon,FLAG_UNDEAD))
	new_draw_info(NDI_UNIQUE, 0,op,"It is an undead force.");
    if(mon->level>op->level)
	new_draw_info(NDI_UNIQUE, 0,op,"It is likely more powerful than you.");
    else if(mon->level<op->level)
	new_draw_info(NDI_UNIQUE, 0,op,"It is likely less powerful than you.");
    else
	new_draw_info(NDI_UNIQUE, 0,op,"It is probably as powerful as you.");
    if(mon->attacktype&AT_ACID)
	new_draw_info(NDI_UNIQUE, 0,op,"You seem to smell an acrid odor.");

    /* Anyone know why this used to use the clone value instead of the
     * maxhp field?  This seems that it should give more accurate results.
     */
    switch((mon->stats.hp+1)*4/(mon->stats.maxhp+1)) { /* From 1-4 */
	case 1:
	    new_draw_info(NDI_UNIQUE, 0,op,"It is in a bad shape.");
	    break;
	case 2:
	    new_draw_info(NDI_UNIQUE, 0,op,"It is hurt.");
	    break;
	case 3:
	    new_draw_info(NDI_UNIQUE, 0,op,"It is somewhat hurt.");
	    break;
	case 4:
	    new_draw_info(NDI_UNIQUE, 0,op,"It is in excellent shape.");
	    break;
    }
    if(present_in_ob(POISONING,mon)!=NULL)
	new_draw_info(NDI_UNIQUE, 0,op,"It looks very ill.");
}

char *long_desc(object *tmp) {
  static char buf[VERY_BIG_BUF];
  char *cp;

  if(tmp==NULL)
    return "";
  buf[0]='\0';
  switch(tmp->type) {
  case RING:
  case SKILL:
  case WEAPON:
  case ARMOUR:
  case BRACERS:
  case HELMET:
  case SHIELD:
  case BOOTS:
  case GLOVES:
  case AMULET:
  case GIRDLE:
  case BOW:
  case ARROW:
  case CLOAK:
  case FOOD:
  case DRINK:
  case FLESH:
    if(*(cp=describe_item(tmp))!='\0') {
	int len;

	strncpy(buf,query_name(tmp), VERY_BIG_BUF-1);
	buf[VERY_BIG_BUF-1]=0;
	len=strlen(buf);
	if (len<VERY_BIG_BUF-5) {
	    /* Since we know the length, we save a few cpu cycles by using
	     * it instead of calling strcat */
	    strcpy(buf+len," ");
	    len++;
	    strncpy(buf+len, cp, VERY_BIG_BUF-len-1);
	    buf[VERY_BIG_BUF-1]=0;
	}
    }
  }
  if(buf[0]=='\0') {
	strncpy(buf,query_name(tmp), VERY_BIG_BUF-1);
	buf[VERY_BIG_BUF-1]=0;
  }

  return buf;
}

void examine(object *op, object *tmp) {
    char buf[VERY_BIG_BUF];
    int i;

    if (tmp == NULL || tmp->type == CLOSE_CON)
	return;

    /* Eneq(csd.uu.se): If NO_PRETEXT is defined we should only print the name. */
    if (QUERY_FLAG(tmp, FLAG_NO_PRETEXT)) {
	strncpy(buf, long_desc(tmp), VERY_BIG_BUF-1);
	buf[VERY_BIG_BUF-1]=0;
    }
    else {
	strcpy(buf,"That is ");
	strncat(buf, long_desc(tmp), VERY_BIG_BUF-strlen(buf)-1);
	buf[VERY_BIG_BUF-1]=0;
    }

    new_draw_info(NDI_UNIQUE, 0,op,buf);
    buf[0]='\0';
    switch(tmp->type) {
	case SPELLBOOK:
	    if(QUERY_FLAG(tmp, FLAG_IDENTIFIED) && tmp->stats.sp > 0 && tmp->stats.sp <= NROFREALSPELLS ) {
		if(!strcmp(tmp->arch->name,"cleric_book"))
		    sprintf(buf,"%s is a %d level prayer.",
			    spells[tmp->stats.sp].name,spells[tmp->stats.sp].level);
		else
		    sprintf(buf,"%s is a %d level spell.",
			    spells[tmp->stats.sp].name,spells[tmp->stats.sp].level);
	    }
	break;

	case BOOK:
	    if(tmp->msg!=NULL)
		strcpy(buf,"Something is written in it.");
	break;

	case CONTAINER:
	    if(tmp->race!=NULL) {
		if(tmp->weight_limit && tmp->stats.Str<100)
		    sprintf (buf,"It can hold only %s and its weight limit is %.1f kg.", 
			 tmp->race, tmp->weight_limit/(10.0 * (100 - tmp->stats.Str)));
		else
		    sprintf (buf,"It can hold only %s.", tmp->race);
	    } else
		if(tmp->weight_limit && tmp->stats.Str<100)
		    sprintf (buf,"Its weight limit is %.1f kg.", 
			     tmp->weight_limit/(10.0 * (100 - tmp->stats.Str)));
	    break;

	case WAND:
	    if(QUERY_FLAG(tmp, FLAG_IDENTIFIED))
		sprintf(buf,"It has %d charges left.",tmp->stats.food);
	    break;
    }

    if(buf[0]!='\0')
	new_draw_info(NDI_UNIQUE, 0,op,buf);

    if(tmp->material && !tmp->msg) {
	strcpy(buf,"It is made of: ");
	for(i=0; i < NROFMATERIALS; i++) {
	  if(tmp->material & (1<<i)) {
	    strcat(buf, material[i].name);
	    strcat(buf, " ");
	  }
	}
	new_draw_info(NDI_UNIQUE, 0,op,buf);
    }

    if(tmp->weight) {
	sprintf(buf,tmp->nrof>1?"They weigh %3.3f kg.":"It weighs %3.3f kg.",
            (tmp->nrof?tmp->weight*tmp->nrof:tmp->weight)/1000.0);
	new_draw_info(NDI_UNIQUE, 0,op,buf);
    }

    if(tmp->value&&!QUERY_FLAG(tmp, FLAG_STARTEQUIP)) {
	if(QUERY_FLAG(tmp, FLAG_UNPAID))
	    sprintf(buf,"%s would cost you %s.",
		    tmp->nrof>1?"They":"It",query_cost_string(tmp,op,F_BUY));
	else
	    sprintf(buf,"You would get %s for %s.",
		    query_cost_string(tmp,op,F_SELL), tmp->nrof>1?"them":"it");
	new_draw_info(NDI_UNIQUE, 0,op,buf);
    }

    if(QUERY_FLAG(tmp, FLAG_MONSTER))
	examine_monster(op,tmp);
   

    /* Does the object have a message?  Don't show message for all object
     * types - especially if the first entry is a match
     */
    if(tmp->msg && tmp->type != EXIT && tmp->type != BOOK && 
       tmp->type != CORPSE && !QUERY_FLAG(tmp, FLAG_WALK_ON) && 
       strncasecmp(tmp->msg, "@match",7)) {

	/* This is just a hack so when identifying hte items, we print
	 * out the extra message
	 */
	if (need_identify(tmp) && QUERY_FLAG(tmp, FLAG_IDENTIFIED))
	    new_draw_info(NDI_UNIQUE, 0,op, "The object has a story:");

	new_draw_info(NDI_UNIQUE, 0,op,tmp->msg);
    }
    new_draw_info(NDI_UNIQUE, 0,op," "); /* Blank line */
}

/*
 * inventory prints object's inventory. If inv==NULL then print player's
 * inventory. 
 * [ Only items which are applied are showed. Tero.Haatanen@lut.fi ]
 */
void inventory(object *op,object *inv) {
  object *tmp;
  char buf[MAX_BUF], *in;
  int items = 0, length;

  if (inv==NULL && op==NULL) {
    new_draw_info(NDI_UNIQUE, 0,op,"Inventory of what object?");
    return;
  }
  tmp = inv ? inv->inv : op->inv;

  while (tmp) {
    if ((!tmp->invisible && 
        (inv==NULL || inv->type == CONTAINER || QUERY_FLAG(tmp, FLAG_APPLIED)))
         || (!op || QUERY_FLAG(op, FLAG_WIZ)))
      items++;
    tmp=tmp->below;
  }
  if (inv==NULL) { /* player's inventory */
    if (items==0) {
      new_draw_info(NDI_UNIQUE, 0,op,"You carry nothing.");
      return;
    } else {
      length = 30;
      in = "";
      if (op)
        clear_win_info(op);
      new_draw_info(NDI_UNIQUE, 0,op,"Inventory:");
    }
  } else {
    if (items==0) 
      return;
    else { 
      length = 30;
      in = "  ";
    }
  }
  for (tmp=inv?inv->inv:op->inv; tmp; tmp=tmp->below) {
    if((!op||!QUERY_FLAG(op, FLAG_WIZ)) && (tmp->invisible || 
       (inv && inv->type != CONTAINER && !QUERY_FLAG(tmp, FLAG_APPLIED))))
      continue;
    if((!op || QUERY_FLAG(op, FLAG_WIZ)))
      (void) sprintf(buf,"%s- %-*s (%5d) %-8s", in, length, query_name(tmp),
                     tmp->count,query_weight(tmp));
    else
      (void) sprintf(buf,"%s- %-*s %-8s", in, length+8, query_name(tmp),
                     query_weight(tmp));
    new_draw_info(NDI_UNIQUE, 0,op,buf);
  }
  if(!inv && op) {
    sprintf(buf,"%-*s %-8s",
            41,"Total weight :",query_weight(op));
    new_draw_info(NDI_UNIQUE, 0,op,buf);
  }
}


void receive_player_name(object *op,char k) {

  if(strlen(op->contr->write_buf)<=1) {
    get_name(op);
    return;
  }
  if(!check_name(op->contr,op->contr->write_buf+1)) {
      get_name(op);
      return;
  }
  if(op->name!=NULL)
    free_string(op->name);
  op->name=add_string(op->contr->write_buf+1);
  new_draw_info(NDI_UNIQUE, 0,op,op->contr->write_buf);
  op->contr->last_value= -1; /* Flag: redraw all stats */
  op->contr->name_changed=1;
  get_password(op);
}

void receive_player_password(object *op,char k) {

  if(strlen(op->contr->write_buf)<=1) {
    unlock_player(op->name);
    get_name(op);
    return;
  }
  new_draw_info(NDI_UNIQUE, 0,op,"          "); /* To hide the password better */
  if(op->contr->state==ST_CONFIRM_PASSWORD) {
    if(!check_password(op->contr->write_buf+1,op->contr->password)) {
      new_draw_info(NDI_UNIQUE, 0,op,"The passwords did not match.");
      unlock_player(op->name);
      get_name(op);
      return;
    }
    clear_win_info(op);
    display_motd(op);
    new_draw_info(NDI_UNIQUE, 0,op," ");
    new_draw_info(NDI_UNIQUE, 0,op,"Welcome, Brave New Warrior!");
    new_draw_info(NDI_UNIQUE, 0,op," ");
    Roll_Again(op);
    op->contr->state=ST_ROLL_STAT;
    return;
  }
  strcpy(op->contr->password,crypt_string(op->contr->write_buf+1,NULL));
  op->contr->state=ST_ROLL_STAT;
  check_login(op);
  return;
}


void set_pickup_mode(object *op,int i) {
  switch(op->contr->mode=i) {
    case 0:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Don't pick up.");
      break;
    case 1:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up one item.");
      break;
    case 2:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up one item and stop.");
      break;
    case 3:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Stop before picking up.");
      break;
    case 4:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up all items.");
      break;
    case 5:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up all items and stop.");
      break;
    case 6:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up all magic items.");
      break;
    case 7:
      new_draw_info(NDI_UNIQUE, 0,op,"Mode: Pick up all coins and gems");
      break;
    }
}

int explore_mode() {
#ifdef EXPLORE_MODE
  player *pl;
  for (pl = first_player; pl != (player *) NULL; pl = pl->next)
    if (pl->explore)
      return 1;
#endif
  return 0;
}

/*
 * Actual commands.
 * Those should be in small separate files (c_object.c, c_wiz.c, cmove.c,...)
 */


static void help_topics(object *op, int what)
{
    DIR *dirp;
    struct dirent *de;
    char filename[MAX_BUF], line[80];
    int namelen, linelen=0;
  
    switch (what) {
	case 1:
	    sprintf(filename, "%s/wizhelp", settings.datadir);
	    new_draw_info(NDI_UNIQUE, 0,op, "      Wiz commands:");
	    break;
	case 3:
	    sprintf(filename, "%s/mischelp", settings.datadir);
	    new_draw_info(NDI_UNIQUE, 0,op, "      Misc help:");
	    break;
	default:
	    sprintf(filename, "%s/help", settings.datadir);
	    new_draw_info(NDI_UNIQUE, 0,op, "      Commands:");
	    break;
    }
    if (!(dirp=opendir(filename)))
	return;

    line[0] ='\0';
    for (de = readdir(dirp); de; de = readdir(dirp)) {
	namelen = NAMLEN(de);
	if (namelen <= 2 && *de->d_name == '.' &&
		(namelen == 1 || de->d_name[1] == '.' ) )
	    continue;
	linelen +=namelen+1;
	if (linelen > 42) {
	    new_draw_info(NDI_UNIQUE, 0,op, line);
	    sprintf(line, " %s", de->d_name);
	    linelen =namelen+1;
	    continue;
	}
	strcat(line, " ");
	strcat(line, de->d_name);
    }
    new_draw_info(NDI_UNIQUE, 0,op, line);
    closedir(dirp);
}

static void show_commands(object *op, int what)
{
  char line[80];
  int i, size, namelen, linelen=0;
  CommArray_s *ap;
  extern CommArray_s Commands[], WizCommands[], SocketCommands[];
  extern const int CommandsSize, WizCommandsSize, SocketCommandsSize;
  
  switch (what) {
  case 1:
    ap =WizCommands;
    size =WizCommandsSize;
    new_draw_info(NDI_UNIQUE, 0,op, "      Wiz commands:");
    break;
  default:
    ap =Commands;
    size =CommandsSize;
    new_draw_info(NDI_UNIQUE, 0,op, "      Commands:");
    break;
  }

  line[0] ='\0';
  for (i=0; i<size; i++) {
    namelen = strlen(ap[i].name);
    linelen +=namelen+1;
    if (linelen > 42) {
      new_draw_info(NDI_UNIQUE, 0,op, line);
      sprintf(line, " %s", ap[i].name);
      linelen =namelen+1;
      continue;
    }
    strcat(line, " ");
    strcat(line, ap[i].name);
  }	       
  new_draw_info(NDI_UNIQUE, 0,op, line);
}

int command_help (object *op, char *params)
{
  struct stat st;
  FILE *fp;
  char filename[MAX_BUF], line[MAX_BUF];
  int len;

  if(op != NULL)
    clear_win_info(op);

/*
   * Main help page?
 */
  if (!params) {
    sprintf(filename, "%s/def_help", settings.datadir);
    if ((fp=fopen(filename, "r")) == NULL) {
      LOG(llevError, "Can't open %s\n", filename);
      perror("Can't read default help");
      return 0;
    }
    while (fgets(line, MAX_BUF, fp)) {
      line[MAX_BUF-1] ='\0';
      len =strlen(line)-1;
      if (line[len] == '\n')
	line[len] ='\0';
      new_draw_info(NDI_UNIQUE, 0,op, line);
    }
    fclose(fp);
    return 0;
  }

  /*
   * Topics list
   */
  if (!strcmp(params, "topics")) {
    help_topics(op, 3);
    help_topics(op, 0);
    if (QUERY_FLAG(op, FLAG_WIZ))
      help_topics(op, 1);
    return 0;
    }
  
  /*
   * Commands list
   */
  if (!strcmp(params, "commands")) {
    show_commands(op, 0);
    if (QUERY_FLAG(op, FLAG_WIZ))
      show_commands(op, 1);
    return 0;
  }

  /*
   * User wants info about command
   */
  if (strchr(params, '.') || strchr(params, ' ') || strchr(params, '/')) {
    sprintf(line, "Illegal characters in '%s'", params);
    new_draw_info(NDI_UNIQUE, 0,op, line);
    return 0;
  }

  sprintf(filename, "%s/mischelp/%s", settings.datadir, params);
  if (stat(filename, &st) || !S_ISREG(st.st_mode)) {
    if (op) {
      sprintf(filename, "%s/help/%s", settings.datadir, params);
      if (stat(filename, &st) || !S_ISREG(st.st_mode)) {
	if (QUERY_FLAG(op, FLAG_WIZ)) {
	  sprintf(filename, "%s/wizhelp/%s", settings.datadir, params);
	  if (stat(filename, &st) || !S_ISREG(st.st_mode))
	    goto nohelp;
	} else
	  goto nohelp;
      }
  }
  }

  /*
   * Found that. Just cat it to screen.
   */
  if ((fp=fopen(filename, "r")) == NULL) {
    LOG(llevError, "Can't open %s\n", filename);
    perror("Can't read helpfile");
    return 0;
      }
  sprintf(line, "Help about '%s'", params);
  new_draw_info(NDI_UNIQUE, 0,op, line);
  while (fgets(line, MAX_BUF, fp)) {
    line[MAX_BUF-1] ='\0';
    len =strlen(line)-1;
    if (line[len] == '\n')
      line[len] ='\0';
    new_draw_info(NDI_UNIQUE, 0,op, line);
    }
  fclose(fp);
  return 0;

  /*
   * No_help -escape
   */
 nohelp:
  sprintf(line, "No help availble on '%s'", params);
  new_draw_info(NDI_UNIQUE, 0,op, line);
  return 0;
}


int command_invoke(object *op, char *params)
{
	return command_cast_spell(op, params, 'i');
}

int command_cast(object *op, char *params)
{
	return command_cast_spell(op, params, 'c');
}

int command_prepare(object *op, char *params)
{
	return command_cast_spell(op, params, 'p');
}

/* object *op is the caster, params is the spell name.  We return the index
 * value of the spell in the spells array for a match, -1 if there is no
 * match, -2 if there are multiple matches.  Note that 0 is a valid entry, so
 * we can't use that as failure.
 *
 * Modified 03/24/98 - extra parameter 'options' specifies if the search is
 * done with the length of the input spell name, or the length of the stored
 * spell name.  This allows you to find out if the spell name entered had
 * extra optional parameters at the end (ie: marking rune <text>)
 *
 */
static int find_spell_byname(object *op, char *params, int options)
{
    int numknown; /* number of spells known by op */
    int spnum;  /* number of spell that is being cast */
    int match=-1,i;
    int paramlen;

    if(QUERY_FLAG(op, FLAG_WIZ))
	numknown = NROFREALSPELLS;
    else 
	numknown = op->contr->nrofknownspells;

    for(i=0;i<numknown;i++){
	if (QUERY_FLAG(op, FLAG_WIZ)) spnum = i;
	else spnum = op->contr->known_spells[i];

        if (!options)
          paramlen=strlen(params);
          
	if (!strncmp(params, spells[spnum].name, options?strlen(spells[spnum].name):paramlen)) {
	    /* We already found a match previously - thus params is not
	     * not unique, so return -2 stating this.
	     */
	    if (match>=0) return -2;
	    else match=spnum;
	}
    }
    return match;
}


/* Shows all spells that op knows.  If params is supplied, the must match
 * that.  If cleric is 1, show cleric spells, if not set, show mage
 * spells.
 */
static void show_matching_spells(object *op, char *params, int cleric)
{
    int i,spnum,first_match=0;
    char lev[80], cost[80];

    for (i=0; i<(QUERY_FLAG(op, FLAG_WIZ)?NROFREALSPELLS:op->contr->nrofknownspells); i++) {	
	if (QUERY_FLAG(op,FLAG_WIZ)) spnum=i;
	else spnum = op->contr->known_spells[i];

	if (spells[spnum].cleric != cleric) continue;
	if (params && strncmp(spells[spnum].name,params, strlen(params)))
		continue;
	if (!first_match) {
	    first_match=1;
	    if (!cleric)
		new_draw_info(NDI_UNIQUE, 0, op, "Mage spells");
	    else
		new_draw_info(NDI_UNIQUE, 0, op, "Priest spells");
	    new_draw_info(NDI_UNIQUE, 0,op,"[ sp] [lev] spell name");
	}
	if (spells[spnum].path & op->path_denied) {
	    strcpy(lev,"den");
            strcpy(cost,"den");
	} else {
	    sprintf(lev,"%3d",spells[spnum].level);
            sprintf(cost,"%3d",SP_level_spellpoint_cost(op,op,spnum));
        }

	new_draw_info_format(NDI_UNIQUE,0,op,"[%s] [%s] %s",
		cost, lev, spells[spnum].name);
    }
}



/* sets up to cast a spell.  op is the caster, params is the spell name,
 * and command is the first letter of the spell type (c=cast, i=invoke, 
 * p=prepare).  Invoke casts a spell immediately, where as cast (and I believe
 * prepare) just set up the range type.
 */

int command_cast_spell (object *op, char *params, char command)
{
    int castnow=0;
    char *cp=NULL;
    int spnum=-1, spnum2=-1;  /* number of spell that is being cast */

    if(!op->contr->nrofknownspells&&!QUERY_FLAG(op, FLAG_WIZ)) {
	new_draw_info(NDI_UNIQUE, 0,op,"You don't know any spells.");
        return 1;
    }
    /* Remove control of the golem */
    if(op->contr->golem!=NULL) {
        remove_friendly_object(op->contr->golem);
        remove_ob(op->contr->golem);
        free_object(op->contr->golem);
        op->contr->golem=NULL;
    }

    if (command=='i') castnow = 1;
    if(params!=NULL) {

#if 0
	/* rune of fire, rune of ... are special cases, break it into 'rune' and
	 * then put 'fire, marking, whatever' in cp. */
	if (strncmp(params,"rune",4)) {
	    cp =strstr(params, " of ");
	    if (cp) {
		*cp='\0';
		cp +=4;
	    }
	}
#endif
        /* This replaces the above.  It assumes simply that if the name of
         * the spell being cast as input by the player is shorter than or
         * equal to the length of the spell name, then there is no options
         * but if it is longer, then everything after the spell name is
         * an option.  It determines if the spell name is shorter or
         * longer by first iterating through the actual spell names, checking
         * to the length of the typed in name.  If that fails, then it checks
         * to the length of each spell name.  If that passes, it assumes that
         * anything after the length of the actual spell name is extra options
         * typed in by the player (ie: marking rune Hello there) */
	if ( ((spnum2 = spnum = find_spell_byname(op, params, 0)) < 0) && 
	    ((spnum = find_spell_byname(op, params, 1)) >= 0) ) {
          params[strlen(spells[spnum].name)] = '\0';
          cp = &params[strlen(spells[spnum].name)+1];
          if (strncmp(cp,"of ",3) == 0)
            cp += 3;
        }	         

	if (spnum>=0) {
	    rangetype orig_rangetype=op->contr->shoottype;
	    op->contr->shoottype=range_magic;
#ifdef ALLOW_SKILLS 
	    if(op->type==PLAYER&&!QUERY_FLAG(op,FLAG_WIZ)) { 
		 /* if we don't change to the correct spell numb,
		  * check_skill_to_fire will be confused as to which
		  * spell casting skill to ready for the player!
		  * I set the code to change back to the old spellnum
		  * after we check, but is this really needed?? -b.t. */

		int orig_spn = op->contr->chosen_spell;
		op->contr->chosen_spell=spnum;
		if(!check_skill_to_fire(op)) {  
		    op->contr->shoottype=orig_rangetype;
		    return 0; 
		    }
		op->contr->chosen_spell=orig_spn;
	    }
#endif
	    if (castnow) { 
		int value;

		    /* Need to switch shoottype to range_magic - otherwise 
                     * cast_spell doesn't check to see if the character 
                     * has enough spellpoints. 
		     * Note: now done above this -b.t. */
		    /* op->contr->shoottype=range_magic; */ 

		value = cast_spell(op,op,op->facing,spnum,0,spellNormal,cp);
		op->contr->shoottype=orig_rangetype;

		if(spells[spnum].cleric) 
			op->stats.grace -= value;
		else 
			op->stats.sp -= value;
	    } 
	    /* We are not casting now */
	    else op->contr->chosen_spell=spnum; 
	    
	    return 1;
	} /* found a matching spell */
    } /* params supplied */

    /* We get here if cast was given without options or we could not find
     * the requested spell.  List all the spells the player knows (if
     * spnum = -1) or spells matching params if spnum=-2
     */

    new_draw_info(NDI_UNIQUE, 0,op,"Cast what spell?  Choose one of:");
    show_matching_spells(op, (spnum2==-2)?params:NULL, 0);
    new_draw_info(NDI_UNIQUE,0,op,"");
    show_matching_spells(op, (spnum2==-2)?params:NULL, 1);

    return 1;
}

#ifdef SET_TITLE
int command_title (object *op, char *params)
{
    char buf[MAX_BUF];

    if(params == NULL) {
	if(op->contr->own_title[0]=='\0')
	    sprintf(buf,"Your title is '%s'.", op->contr->title);
	else
	    sprintf(buf,"Your title is '%s'.", op->contr->own_title);
	new_draw_info(NDI_UNIQUE, 0,op,buf);
	return 1;
    }
    if(strcmp(params, "clear")==0 || strcmp(params, "default")==0) {
	if(op->contr->own_title[0]=='\0')
	    new_draw_info(NDI_UNIQUE, 0,op,"Your title is the default title.");
	else
	    new_draw_info(NDI_UNIQUE, 0,op,"Title set to default.");
	op->contr->own_title[0]='\0';
	return 1;
    }

    if((int)strlen(params) >= MAX_NAME) {
	new_draw_info(NDI_UNIQUE, 0,op,"Title too long.");
	return 1;
    }
    strcpy(op->contr->own_title, params);
    return 1;
}
#endif /* SET_TITLE */

int command_save (object *op, char *params)
{
    if (blocks_cleric(op->map, op->x, op->y)) {
	new_draw_info(NDI_UNIQUE, 0, op, "You can not save on unholy ground");
    } else {
	if(save_player(op,1))
	    new_draw_info(NDI_UNIQUE, 0,op,"You have been saved.");
	else
	    new_draw_info(NDI_UNIQUE, 0,op,"SAVE FAILED!");
    }
    return 1;
}

#ifdef SEARCH_ITEMS
int command_search_items (object *op, char *params)
{
      char buf[MAX_BUF];
  if(params == NULL) {
	if(op->contr->search_str[0]=='\0') {
	  new_draw_info(NDI_UNIQUE, 0,op,"Example: search magic+1");
	  new_draw_info(NDI_UNIQUE, 0,op,"Would automatically pick up all");
	  new_draw_info(NDI_UNIQUE, 0,op,"items containing the word 'magic+1'.");
	  return 1;
	}
	op->contr->search_str[0]='\0';
	new_draw_info(NDI_UNIQUE, 0,op,"Search mode turned off.");
	fix_player(op);
	return 1;
  }
  if((int)strlen(params) >= MAX_BUF) {
	new_draw_info(NDI_UNIQUE, 0,op,"Search string too long.");
	return 1;
      }
  strcpy(op->contr->search_str, params);
      sprintf(buf,"Searching for '%s'.",op->contr->search_str);
      new_draw_info(NDI_UNIQUE, 0,op,buf);
  fix_player(op);
      return 1;
    }
#endif /* SEARCH_ITEMS */

int command_peaceful (object *op, char *params)
{
      if((op->contr->peaceful=!op->contr->peaceful))
        new_draw_info(NDI_UNIQUE, 0,op,"You will not attack other players.");
      else
        new_draw_info(NDI_UNIQUE, 0,op,"You will attack other players.");
      return 1;
    }

int command_strength (object *op, char *params)
{
      int i;
  if(params==NULL || !sscanf(params, "%d", &i) ||
         (!QUERY_FLAG(op, FLAG_WIZ)&&(i<5||i>op->stats.maxsp))) {
        new_draw_info(NDI_UNIQUE, 0,op,"Set which strength?");
        return 1;
      }
      op->contr->shootstrength=i;
      new_draw_info(NDI_UNIQUE, 0,op,"OK.");
      return 1;
    }

int command_pickup (object *op, char *params)
{
      int i;

  if(!params) {
    op->contr->count_left=0;
    set_pickup_mode(op, (op->contr->mode > 6)? 0: op->contr->mode+1);
    return 0;
  }
  if(params==NULL || !sscanf(params, "%d", &i) || i<0 ) {
        new_draw_info(NDI_UNIQUE, 0,op,"Usage: pickup <0-7> or <value_density> .");
        return 1;
      }
      set_pickup_mode(op,i);
      return 1;
}


int command_wimpy (object *op, char *params)
{
    int i;
  char buf[MAX_BUF];

  if (params==NULL || !sscanf(params, "%d", &i)) {
      sprintf(buf, "Your current wimpy level is %d.", op->run_away);
      new_draw_info(NDI_UNIQUE, 0,op, buf);
      return 1;
    }
    sprintf(buf, "Your new wimpy level is %d.", i);
    new_draw_info(NDI_UNIQUE, 0,op, buf);
    op->run_away = i;
    return 1;
  }

int command_quit (object *op, char *params)
{
    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,
	       "Quitting will delete your character.\nAre you sure you want to quit (y/n):");

    op->contr->state = ST_CONFIRM_QUIT;
    return 1;
  }

#ifdef EXPLORE_MODE
/*
 * don't allow people to exit explore mode.  It otherwise becomes
 * really easy to abuse this.
 */
int command_explore (object *op, char *params)
{
  /*
   * I guess this is the best way to see if we are solo or not.  Actually,
   * are there any cases when first_player->next==NULL and we are not solo?
   */
      if ((first_player!=op->contr) || (first_player->next!=NULL)) {
	  new_draw_info(NDI_UNIQUE, 0,op,"You can not enter explore mode if you are in a party");
      }
      else if (op->contr->explore)
              new_draw_info(NDI_UNIQUE, 0,op, "There is no return from explore mode");
      else {
		op->contr->explore=1;
		new_draw_info(NDI_UNIQUE, 0,op, "You are now in explore mode");
      }
      return 1;
    }
#endif

int command_sound (object *op, char *params)
{
    if (op->contr->socket.sound) {
        op->contr->socket.sound=0;
        new_draw_info(NDI_UNIQUE, 0,op, "Silence is golden...");
    }
    else {
        op->contr->socket.sound=1;
        new_draw_info(NDI_UNIQUE, 0,op, "The sounds are enabled.");
    }
    return 1;
}

int command_shout (object *op, char *params)
{
    char buf[MAX_BUF];
    if (params == NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Shout what?");
	return 1;
    }
    strcpy(buf,op->name);
    strcat(buf," shouts: ");
    strncat(buf, params, MAX_BUF-30);
    buf[MAX_BUF - 1] = '\0';
    new_draw_info(NDI_UNIQUE | NDI_ALL | NDI_RED, 1, NULL, buf);
    return 1;
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
	new_draw_info(NDI_UNIQUE, 0,op,"Tell whom what?");
	return 1;
    } else if ( msg == NULL){
	sprintf(buf, "Tell %s what?", name);
	new_draw_info(NDI_UNIQUE, 0,op,buf);
	return 1;
    }


    sprintf(buf,"%s tells you: %s",op->name,msg);
    for(pl=first_player;pl!=NULL;pl=pl->next)
      if(strncasecmp(pl->ob->name,name,MAX_NAME)==0)
      {
	new_draw_info(NDI_UNIQUE | NDI_WHITE, 0, pl->ob, buf);
        return 1;
      }
    new_draw_info(NDI_UNIQUE, 0,op,"No such player.");
    return 1;
  }

int command_bell (object *op, char *params)
{
    char buf[MAX_BUF];
    player *pl;
  if (params == NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"Bell whom?");
      return 1;
    }
    for(pl=first_player;pl!=NULL;pl=pl->next)
    if(strncasecmp(pl->ob->name, params, MAX_NAME)==0)
      {
        sprintf(buf,"%s bells you.",op->name);
        new_draw_info(NDI_UNIQUE, 0,pl->ob,buf);
	/* Need to play a bell sound here */
        return 1;
      }
  return 0;
}

/**************************************************************************/

/* Returns TRUE if the range specified (int r) is legal - that is,
 * the character has an item that is equipped for that range type.
 * return 0 if there is no item of that range type that is usable.
 */

int legal_range(object *op,int r) {
  int i;
  object *tmp;

  switch(r) {
  case range_none: /* "Nothing" is always legal */
    return 1;
  case range_bow: /* bows */
    for (tmp=op->inv; tmp!=NULL; tmp=tmp->below)
      if (tmp->type == BOW && QUERY_FLAG(tmp, FLAG_APPLIED))
	return 1;
    return 0;
  case range_magic: /* cast spells */
    if (op->contr->nrofknownspells == 0)
      return 0;
    for (i = 0; i < op->contr->nrofknownspells; i++)
      if (op->contr->known_spells[i] == op->contr->chosen_spell)
        return 1;
    op->contr->chosen_spell = op->contr->known_spells[0];
    return 1;
  case range_wand: /* use wands */
    for (tmp=op->inv; tmp!=NULL; tmp=tmp->below)
      if (tmp->type == WAND && QUERY_FLAG(tmp, FLAG_APPLIED)) {
        if (QUERY_FLAG(tmp, FLAG_BEEN_APPLIED) || QUERY_FLAG(tmp, FLAG_IDENTIFIED))
          op->contr->known_spell = 1;
        else
          op->contr->known_spell = 0;
        op->contr->chosen_item_spell=tmp->stats.sp;
        return 1;
      }
    return 0;
  case range_rod:
    for (tmp=op->inv; tmp!=NULL; tmp=tmp->below)
      if (tmp->type == ROD && QUERY_FLAG(tmp, FLAG_APPLIED)) {
        if (QUERY_FLAG(tmp,FLAG_BEEN_APPLIED) || QUERY_FLAG(tmp, FLAG_IDENTIFIED))
          op->contr->known_spell = 1;
        else
          op->contr->known_spell = 0;
        op->contr->chosen_item_spell=tmp->stats.sp;
        return 1;
      }
    return 0;
  case range_horn:
    for (tmp=op->inv; tmp!=NULL; tmp=tmp->below)
      if (tmp->type == HORN && QUERY_FLAG(tmp, FLAG_APPLIED)) {
        if (QUERY_FLAG(tmp,FLAG_BEEN_APPLIED) || QUERY_FLAG(tmp, FLAG_IDENTIFIED))
          op->contr->known_spell = 1;
        else
          op->contr->known_spell = 0;
        op->contr->chosen_item_spell=tmp->stats.sp;
        return 1;
      }
    return 0;
  case range_scroll: /* Use scrolls */
    return 0;
  case range_skill:
      for (tmp = op->inv; tmp!=NULL; tmp=tmp->below) {
	  if (tmp->type == SKILL) { 
	      return 1;
	  }
      }
      op->chosen_skill=NULL;	/* they have lost all skills :) */ 
      return 0;
  }
  return 0;
}

void change_spell(object *op,char k) {
  char buf[MAX_BUF];
  if(op->contr->golem!=NULL) {
    remove_friendly_object(op->contr->golem);
    remove_ob(op->contr->golem);
    free_object(op->contr->golem);
    op->contr->golem=NULL;
  }
  do {
    op->contr->shoottype += ((k == '+') ? 1 : -1);
    if(op->contr->shoottype >= range_size)
      op->contr->shoottype = range_none;
    else if (op->contr->shoottype <= range_bottom)
      op->contr->shoottype = (rangetype)(range_size-1);
  } while (!legal_range(op,op->contr->shoottype));
  switch(op->contr->shoottype) {
  case range_none:
    strcpy(buf,"No ranged attack chosen.");
    break;
  case range_bow: {
	object *tmp;
	for (tmp = op->inv; tmp; tmp = tmp->below)
	  if (tmp->type == BOW && QUERY_FLAG (tmp, FLAG_APPLIED))
	    break;
	sprintf (buf, "Switched to %s and %s.", query_name(tmp),
		 tmp && tmp->race ? tmp->race : "nothing");
    }
    break;
  case range_magic:
    sprintf(buf,"Switched to spells (%s).",
            spells[op->contr->chosen_spell].name);
    new_draw_info(NDI_UNIQUE, 0,op,buf);
    break;
  case range_wand:
    sprintf(buf,"Switched to wand (%s).",
            op->contr->known_spell ?
              spells[op->contr->chosen_item_spell].name : "unknown");
    break;
  case range_rod:
    sprintf(buf, "Switched to rod (%s).",
            op->contr->known_spell ?
            spells[op->contr->chosen_item_spell].name : "unknown");
    break;
  case range_horn:
    sprintf(buf, "Switched to horn (%s).",
            op->contr->known_spell ?
            spells[op->contr->chosen_item_spell].name : "unknown");
    break;
  case range_skill: 
    sprintf (buf, "Switched to skill: %s", op->chosen_skill ?  
		 op->chosen_skill->name : "none");
    break;
  default:
    break;
  }
  new_draw_info(NDI_UNIQUE, 0,op,buf);
}


int command_invisible (object *op, char *params)
{
  if (!op)
    return 0;
      op->invisible+=100;
      update_object(op);
      new_draw_info(NDI_UNIQUE, 0,op,"You turn invisible.");
  return 0;
}

int command_rotateshoottype (object *op, char *params)
{
  if (!params)
      change_spell(op,'+');
  else
    change_spell(op, params[0]);
  return 0;
}

int command_throw (object *op, char *params)
{

#ifdef ALLOW_SKILLS
   if(!change_skill(op,SK_THROWING))
        return 0;
   else {
        int success = do_skill(op,op->facing,params);
        return success;
   }
#else
  return 0;
#endif
}

int command_brace (object *op, char *params)
{
  if (!params)
    op->contr->braced =!op->contr->braced;
  else
    op->contr->braced =onoff_value(params);

  if(op->contr->braced)
    new_draw_info(NDI_UNIQUE, 0,op, "You are braced.");
  else
    new_draw_info(NDI_UNIQUE, 0,op, "Not braced.");

      fix_player(op);
  return 0;
}

int command_rotatespells (object *op, char *params)
{
  player *pl=op->contr;
  int i, j;

  if(pl->shoottype != range_magic) {
    if(pl->nrofknownspells > 0) {
      pl->shoottype = range_magic;
      pl->chosen_spell = pl->known_spells[0];
    } else
          new_draw_info(NDI_UNIQUE, 0,op,"You know no spells.");
    return 0;
  }

  for(i=0;i<pl->nrofknownspells;i++)
    if(pl->known_spells[i]==pl->chosen_spell)
	    {
	j =1;
	if(params)
	  sscanf(params, "%d", &j);
	i +=j + (int)pl->nrofknownspells;
	i = i % (int)pl->nrofknownspells;
	pl->chosen_spell=pl->known_spells[i];
	return 1;
	    }
  pl->chosen_spell=pl->known_spells[0];
  return 1;
}

