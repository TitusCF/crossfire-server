/*
 * static char *rcsid_item_c =
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

#include <global.h>
#include <funcpoint.h>
#include <living.h>
#include <spells.h>

static char numbers[21][20] = {
  "no","","two","three","four","five","six","seven","eight","nine","ten",
  "eleven","twelve","thirteen","fourteen","fifteen","sixteen","seventeen",
  "eighteen","nineteen","twenty"
};

static char numbers_10[10][20] = {
  "zero","ten","twenty","thirty","fourty","fifty","sixty","seventy",
  "eighty","ninety"
};

static char levelnumbers[21][20] = {
  "zeroth","first", "second", "third", "fourth", "fifth", "sixth", "seventh",
  "eighth", "ninth", "tenth", "eleventh", "twelfth", "thirteenth",
  "fourteenth", "fifteenth", "sixteenth", "seventeenth", "eighteen",
  "nineteen", "twentieth"
};

static char levelnumbers_10[11][20] = {
  "zeroth","tenth","twentieth","thirtieth","fortieth","fiftieth","sixtieth",
  "seventieth","eightieth","ninetieth"
};

/* describe_resistance generates the visible naming for resistances.
 * returns a static array of the description.  This can return
 * a big buffer.
 * if newline is true, we don't put parens around the description
 * but do put a newline at the end.  Useful when dumping to files
 */
char *describe_resistance(object *op, int newline)
{
    static char buf[VERY_BIG_BUF];
    char    buf1[VERY_BIG_BUF];
    int tmpvar;

    buf[0]=0;
    for (tmpvar=0; tmpvar<NROFATTACKS; tmpvar++) {
       if (op->resist[tmpvar]) {
	    if (!newline)
		sprintf(buf1,"(%s %+d)", resist_plus[tmpvar], op->resist[tmpvar]);
	    else
		sprintf(buf1,"%s %d\n", resist_plus[tmpvar], op->resist[tmpvar]);

	    strcat(buf, buf1);
       }
    }
    return buf;
}


/*
 * query_weight(object) returns a character pointer to a static buffer
 * containing the text-representation of the weight of the given object.
 * The buffer will be overwritten by the next call to query_weight().
 */

char *query_weight(object *op) {
  static char buf[10];
  int i=op->nrof?op->nrof*op->weight:op->weight+op->carrying;

  if(op->weight<0)
    return "      ";
  if(i%1000)
    sprintf(buf,"%6.1f",i/1000.0);
  else
    sprintf(buf,"%4d  ",i/1000);
  return buf;
}

/*
 * Returns the pointer to a static buffer containing
 * the number requested (of the form first, second, third...)
 */

char *get_levelnumber(int i) {
  static char buf[MAX_BUF];
  if (i > 99) {
    sprintf(buf, "%d.", i);
    return buf;
  }
  if(i < 21)
    return levelnumbers[i];
  if(!(i%10))
    return levelnumbers_10[i/10];
  strcpy(buf, numbers_10[i/10]);
  strcat(buf, levelnumbers[i%10]);
  return buf;
}


/*
 * get_number(integer) returns the text-representation of the given number
 * in a static buffer.  The buffer might be overwritten at the next
 * call to get_number().
 * It is currently only used by the query_name() function.
 */

char *get_number(int i) {
  if(i<=20)
    return numbers[i];
  else {
    static char buf[MAX_BUF];
    sprintf(buf,"%d",i);
    return buf;
  }
}

/*
 *  Returns pointer to static buffer containing ring's or amulet's
 *  abilities
 *  These are taken from old query_name(), but it would work better
 *  if describle_item() would be called to get this information and
 *  caller would handle FULL_RING_DESCRIPTION definition.
 *  Or make FULL_RING_DESCRIPTION standard part of a game and let
 *  client handle names.
 */
/* Aug 95 modified this slightly so that Skill tools don't have magic bonus
 * from stats.sp - b.t.
 */
char *ring_desc (object *op) 
{
    static char buf[VERY_BIG_BUF];
    int attr, val,len;
    
    buf[0] = 0;

    if (! QUERY_FLAG(op, FLAG_IDENTIFIED))
	return buf;

    for (attr=0; attr<7; attr++) {
	if ((val=get_attr_value(&(op->stats),attr))!=0) {
	    sprintf (buf+strlen(buf), "(%s%+d)", short_stat_name[attr], val);
	}
    }
    if(op->stats.exp)
	sprintf(buf+strlen(buf), "(speed %+d)", op->stats.exp);
    if(op->stats.wc)
	sprintf(buf+strlen(buf), "(wc%+d)", op->stats.wc);
    if(op->stats.dam)
	sprintf(buf+strlen(buf), "(dam%+d)", op->stats.dam);
    if(op->stats.ac)
	sprintf(buf+strlen(buf), "(ac%+d)", op->stats.ac);

    strcat(buf,describe_resistance(op, 0));

    if (op->stats.food != 0)
	sprintf(buf+strlen(buf), "(sustenance%+d)", op->stats.food);
	 /*    else if (op->stats.food < 0)
			 sprintf(buf+strlen(buf), "(hunger%+d)", op->stats.food); */
    if(op->stats.grace)
	sprintf(buf+strlen(buf), "(grace%+d)", op->stats.grace);
    if(op->stats.sp && op->type!=SKILL)
	sprintf(buf+strlen(buf), "(magic%+d)", op->stats.sp);
    if(op->stats.hp)
	sprintf(buf+strlen(buf), "(regeneration%+d)", op->stats.hp);
    if(op->stats.luck)
	sprintf(buf+strlen(buf), "(luck%+d)", op->stats.luck);
    if(QUERY_FLAG(op,FLAG_LIFESAVE))
	strcat(buf,"(lifesaving)");
    if(QUERY_FLAG(op,FLAG_REFL_SPELL))
	strcat(buf,"(reflect spells)");
    if(QUERY_FLAG(op,FLAG_REFL_MISSILE))
	strcat(buf,"(reflect missiles)");
    if(QUERY_FLAG(op,FLAG_STEALTH))
	strcat(buf,"(stealth)");
    /* Shorten some of the names, so they appear better in the windows */
    len=strlen(buf);
    DESCRIBE_PATH_SAFE(buf, op->path_attuned, "Attuned", &len, VERY_BIG_BUF);
    DESCRIBE_PATH_SAFE(buf, op->path_repelled, "Repelled", &len, VERY_BIG_BUF);
    DESCRIBE_PATH_SAFE(buf, op->path_denied, "Denied", &len, VERY_BIG_BUF);
    if(buf[0] == 0 && op->type!=SKILL)
	strcpy(buf,"of adornment");
    return buf;
}

/*
 * query_short_name(object) is similar to query_name, but doesn't 
 * contain any information about object status (worn/cursed/etc.)
 */
char *query_short_name(object *op) 
{
    static char buf[HUGE_BUF];
    char buf2[HUGE_BUF];
    int len=0;

    if(op->name == NULL)
	return "(null)";
    if(!op->nrof && !op->weight && !op->title && !is_magical(op)) 
	return op->name; /* To speed things up (or make things slower?) */
    if(op->nrof) {
	safe_strcat(buf, get_number(op->nrof), &len, HUGE_BUF);

	if (op->nrof!=1) safe_strcat(buf, " ", &len, HUGE_BUF);
	safe_strcat(buf,op->name, &len, HUGE_BUF);

	if (op->nrof != 1) {
	    char *buf3 = strstr(buf, " of ");
	    if (buf3!=NULL) {
		strcpy(buf2, buf3);
		*buf3 = '\0';	/* also changes value in buf */
	    }
	    len=strlen(buf);
	    if(QUERY_FLAG(op,FLAG_NEED_IE)) {
		char *cp=strrchr(buf,'y');

		if(cp!=NULL) {
		    *cp='\0'; /* Strip the 'y' */
		    len--;
		}
		safe_strcat(buf,"ies", &len, HUGE_BUF);
	    } else if (buf[strlen(buf)-1]!='s') 
		/* if the item ends in 's', then adding another one is 
		 * not the way to pluralize it.  The only item where this 
		 * matters (that I know of) is bracers, as they start of 
		 * plural 
		 */
		safe_strcat(buf,"s", &len, HUGE_BUF);

	    /* If buf3 is set, then this was a string that contained
	     * something of something (potion of dexterity.)  The part before
	     * the of gets made plural, so now we need to copy the rest
	     * (after and including the " of "), to the buffer string.
	     */
	    if (buf3)
		safe_strcat(buf, buf2, &len, HUGE_BUF);
	}
    } else {
	/* if nrof is 0, the object is not mergable, and thus, op->name
	   should contain the name to be used. */
	safe_strcat(buf,op->name, &len, HUGE_BUF);
    }
    if (op->title && QUERY_FLAG(op,FLAG_IDENTIFIED)) {
	safe_strcat(buf, " ", &len, HUGE_BUF);
	safe_strcat(buf, op->title, &len, HUGE_BUF);
    }

    switch(op->type) {
      case SPELLBOOK:
	if (QUERY_FLAG(op,FLAG_IDENTIFIED)||QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
	    if(!op->title) {
		safe_strcat(buf," of ", &len, HUGE_BUF);
		if(op->slaying) safe_strcat(buf,op->slaying, &len, HUGE_BUF);
		else
		  safe_strcat(buf,spells[op->stats.sp].name, &len, HUGE_BUF);
		if(op->type != SPELLBOOK) {
		    sprintf(buf2, " (lvl %d)", op->level);
		    safe_strcat(buf, buf2, &len, HUGE_BUF);
		}
	    }
	}
	
	break;
      case SCROLL:
      case WAND:
      case ROD:
	if (QUERY_FLAG(op,FLAG_IDENTIFIED)||QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
	    if(!op->title) {
		safe_strcat(buf," of ", &len, HUGE_BUF);
		safe_strcat(buf,spells[op->stats.sp].name, &len, HUGE_BUF);
		if(op->type != SPELLBOOK) {
		    sprintf(buf2, " (lvl %d)", op->level);
		    safe_strcat(buf, buf2, &len, HUGE_BUF);
		}
	    }
	}
	break;

      case SKILL:
      case AMULET:
      case RING:
#ifdef FULL_RING_DESCRIPTION
	if (!op->title) {
	    /* If ring has a title, full description isn't so useful */ 
	    char *s = ring_desc(op);
	    if (s[0]) {
		safe_strcat (buf, " ", &len, HUGE_BUF);
		safe_strcat(buf, s, &len, HUGE_BUF);
	    }
	}
#endif
	break;
      default:
	if(op->magic && ((QUERY_FLAG(op,FLAG_BEEN_APPLIED) && 
	   need_identify(op)) || QUERY_FLAG(op,FLAG_IDENTIFIED))) {
	    sprintf(buf2, " %+d", op->magic);
	    safe_strcat(buf, buf2, &len, HUGE_BUF);
	}
    }
    return buf;
}

/*
 * query_name(object) returns a character pointer pointing to a static
 * buffer which contains a verbose textual representation of the name
 * of the given object.
 * cf 0.92.6:  Put in 5 buffers that it will cycle through.  In this way,
 * you can make several calls to query_name before the bufs start getting
 * overwritten.  This may be a bad thing (it may be easier to assume the value
 * returned is good forever.)  However, it makes printing statements that
 * use several names much easier (don't need to store them to temp variables.)
 *
 */
char *query_name(object *op) {
    static char buf[5][HUGE_BUF];
    static int use_buf=0;
    int len=0;

    use_buf++;
    use_buf %=5;

    safe_strcat(buf[use_buf], query_short_name(op), &len, HUGE_BUF);

    if (QUERY_FLAG(op,FLAG_INV_LOCKED))
	safe_strcat(buf[use_buf], " *", &len, HUGE_BUF);
    if (op->type == CONTAINER && ((op->env && op->env->container == op) || 
	(!op->env && QUERY_FLAG(op,FLAG_APPLIED))))
	safe_strcat(buf[use_buf]," (open)", &len, HUGE_BUF);

    if (QUERY_FLAG(op,FLAG_KNOWN_CURSED)) {
	if(QUERY_FLAG(op,FLAG_DAMNED))
	    safe_strcat(buf[use_buf], " (damned)", &len, HUGE_BUF);
	else if(QUERY_FLAG(op,FLAG_CURSED))
	    safe_strcat(buf[use_buf], " (cursed)", &len, HUGE_BUF);
    }
    /* Basically, if the object is known magical (detect magic spell on it),
     * and it isn't identified,  print out the fact that
     * it is magical.  Assume that the detect magical spell will only set
     * KNOWN_MAGICAL if the item actually is magical.
     *
     * Changed in V 0.91.4 - still print that the object is magical even
     * if it has been applied.  Equipping an item does not tell full
     * abilities, especially for artifact items.
     */
    if (QUERY_FLAG(op,FLAG_KNOWN_MAGICAL) && !QUERY_FLAG(op,FLAG_IDENTIFIED))
	safe_strcat(buf[use_buf], " (magic)", &len, HUGE_BUF);
    if(QUERY_FLAG(op,FLAG_APPLIED)) {
	switch(op->type) {
	  case BOW:
	  case WAND:
	  case ROD:
	  case HORN:
	    safe_strcat(buf[use_buf]," (readied)", &len, HUGE_BUF);
	    break;
	  case WEAPON:
	    safe_strcat(buf[use_buf]," (wielded)", &len, HUGE_BUF);
	    break;
	  case ARMOUR:
	  case HELMET:
	  case SHIELD:
	  case RING:
	  case BOOTS:
	  case GLOVES:
	  case AMULET:
	  case GIRDLE:
	  case BRACERS:
	  case CLOAK:
	    safe_strcat(buf[use_buf]," (worn)", &len, HUGE_BUF);
	    break;
	  case CONTAINER:
	    safe_strcat(buf[use_buf]," (active)", &len, HUGE_BUF);
	    break;
	  case SKILL:
	  default:
	    safe_strcat(buf[use_buf]," (applied)", &len, HUGE_BUF);
	}
    }
    if(QUERY_FLAG(op, FLAG_UNPAID))
	safe_strcat(buf[use_buf]," (unpaid)", &len, HUGE_BUF);

    return buf[use_buf];
}

/*
 * query_base_name(object) returns a character pointer pointing to a static
 * buffer which contains a verbose textual representation of the name
 * of the given object.  The buffer will be overwritten at the next
 * call to query_base_name().   This is a lot like query_name, but we
 * don't include the item count or item status.  Used for inventory sorting
 * and sending to client.
 * If plural is set, we generate the plural name of this.
 */
char *query_base_name(object *op, int plural) {
    static char buf[MAX_BUF];
    char buf2[MAX_BUF];
    int len;

    if(op->name == NULL)
	return "(null)";
    if(!op->nrof && !op->weight && !op->title && !is_magical(op)) 
	return op->name; /* To speed things up (or make things slower?) */

    strcpy(buf,op->name);
    len=strlen(buf);

    /* This code pretty much taken directly from query_short_name */
    if (plural) {
	char *buf3 = strstr(buf, " of ");
	if (buf3!=NULL) {
	    strcpy(buf2, buf3);
	    *buf3 = '\0';   /* also changes value in buf */
	    len=strlen(buf);
	}

	if(QUERY_FLAG(op,FLAG_NEED_IE)) {
	    char *cp=strrchr(buf,'y');

	    if(cp!=NULL) {
		*cp='\0'; /* Strip the 'y' */
		len--;
	    }
	    safe_strcat(buf,"ies", &len, MAX_BUF);
	} else if (buf[strlen(buf)-1]!='s') 
	    /* if the item ends in 's', then adding another one is
	     * not the way to pluralize it.  The only item where this
	     * matters (that I know of) is bracers, as they start of
	     * plural
	     */
	    safe_strcat(buf,"s", &len, MAX_BUF);

	/* If buf3 is set, then this was a string that contained
	 * something of something (potion of dexterity.)  The part before
	 * the of gets made plural, so now we need to copy the rest
	 * (after and including the " of "), to the buffer string.
	 */
	if (buf3)
	    safe_strcat(buf, buf2, &len, MAX_BUF);
    }


    if (op->title && QUERY_FLAG(op,FLAG_IDENTIFIED)) {
	safe_strcat(buf, " ", &len, MAX_BUF);
	safe_strcat(buf, op->title, &len, MAX_BUF);
    }

    switch(op->type) {
      case SPELLBOOK:
	if (QUERY_FLAG(op,FLAG_IDENTIFIED)||QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
	    if(!op->title) {
		safe_strcat(buf," of ", &len, MAX_BUF);
		if(op->slaying) safe_strcat(buf,op->slaying, &len, MAX_BUF);
		else
		  safe_strcat(buf,spells[op->stats.sp].name, &len, MAX_BUF);
		if(op->type != SPELLBOOK) 
		    sprintf(buf+strlen(buf), " (lvl %d)", op->level);
	    }
	}
	break;

      case SCROLL:
      case WAND:
      case ROD:
	if (QUERY_FLAG(op,FLAG_IDENTIFIED)||QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
	    if(!op->title) {
		safe_strcat(buf," of ", &len, MAX_BUF);
		safe_strcat(buf,spells[op->stats.sp].name, &len, MAX_BUF);
		if(op->type != SPELLBOOK)
		    sprintf(buf+strlen(buf), " (lvl %d)", op->level);
	    }
	}
	break;

      case SKILL:
      case AMULET:
      case RING:
#ifdef FULL_RING_DESCRIPTION
	if (!op->title) {
	    /* If ring has a title, full description isn't so useful */ 
	    char *s = ring_desc(op);
	    if (s[0]) {
		safe_strcat (buf, " ", &len, MAX_BUF);
		safe_strcat (buf, s, &len, MAX_BUF);
	    }
	}
#endif
	break;
      default:
	if(op->magic && ((QUERY_FLAG(op,FLAG_BEEN_APPLIED) && 
	   need_identify(op)) || QUERY_FLAG(op,FLAG_IDENTIFIED))) {
	    sprintf(buf + strlen(buf), " %+d", op->magic);
	}
    }
    return buf;
}
/*
 * Returns a pointer to a static buffer which contains a
 * description of the given object.
 * If it is a monster, lots of information about its abilities
 * will be returned.
 * If it is an item, lots of information about which abilities
 * will be gained about its user will be returned.
 * If it is a player, it writes out the current abilities
 * of the player, which is usually gained by the items applied.
 */

char *describe_item(object *op) {
  char buf[MAX_BUF];
  static char retbuf[VERY_BIG_BUF];

  retbuf[0]='\0';
  if(QUERY_FLAG(op,FLAG_MONSTER)) {
    if(FABS(op->speed)>MIN_ACTIVE_SPEED) {
      switch((int)((FABS(op->speed))*15)) {
      case 0:
        strcat(retbuf,"(very slow movement)");
        break;
      case 1:
        strcat(retbuf,"(slow movement)");
        break;
      case 2:
        strcat(retbuf,"(normal movement)");
        break;
      case 3:
      case 4:
        strcat(retbuf,"(fast movement)");
        break;
      case 5:
      case 6:
        strcat(retbuf,"(very fast movement)");
        break;
      case 7:
      case 8:
      case 9:
      case 10:
        strcat(retbuf,"(extremely fast movement)");
        break;
      default:
        strcat(retbuf,"(lightning fast movement)");
        break;
      }
    }
  } else switch(op->type) {
    case BOW:
    case ARROW:
    case WAND:
    case ROD:
    case HORN:
    case PLAYER:
    case WEAPON:
    case ARMOUR:
    case HELMET:
    case SHIELD:
    case BOOTS:
    case GLOVES:
    case GIRDLE:
    case BRACERS:
    case CLOAK:
	break;

    case FOOD:
    case FLESH:
    case DRINK:
      if(!need_identify(op) || QUERY_FLAG(op,FLAG_IDENTIFIED) || QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
	sprintf(buf,"(food+%d)", op->stats.food);
	strcat(retbuf, buf);
	if (!QUERY_FLAG(op,FLAG_CURSED)) {
	    if (op->stats.hp)
		strcat(retbuf,"(heals)");
	    if (op->stats.sp)
		strcat(retbuf,"(spellpoint regen)");
	}
	else {
	    if (op->stats.hp)
		strcat(retbuf,"(damages)");
	    if (op->stats.sp)
		strcat(retbuf,"(spellpoint depletion)");
	}
      }
	break;


    case SKILL:
    case RING:
    case AMULET:
#ifdef FULL_RING_DESCRIPTION
      if (op->title)
	strcat (retbuf, ring_desc(op));
#else
      strcat (retbuf, ring_desc(op));
#endif
      return retbuf;

    default:
      return retbuf;
    }
  if(op->type!=PLAYER && !QUERY_FLAG(op,FLAG_MONSTER)) {
    if(!need_identify(op) || QUERY_FLAG(op,FLAG_IDENTIFIED) || QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {

      int attr,val;

      for (attr=0; attr<7; attr++) {
        if ((val=get_attr_value(&(op->stats),attr))!=0) {
          sprintf(buf, "(%s%+d)", short_stat_name[attr], val);
          strcat(retbuf,buf);
      	}
      }

      if(op->stats.exp) {
        sprintf(buf,"(speed %+d)",op->stats.exp);
        strcat(retbuf,buf);
      }
      switch(op->type) {
      case BOW:
      case ARROW:
      case GIRDLE:
      case HELMET:
      case SHIELD:
      case BOOTS:
      case GLOVES:
      case WEAPON:
      case SKILL:
      case RING:
      case AMULET:
      case ARMOUR:
      case BRACERS:
      case FORCE:
      case CLOAK:
        if(op->stats.wc) {
          sprintf(buf,"(wc%+d)",op->stats.wc);
          strcat(retbuf,buf);
        }
        if(op->stats.dam) {
          sprintf(buf,"(dam%+d)",op->stats.dam);
          strcat(retbuf,buf);
        }
        if(op->stats.ac) {
          sprintf(buf,"(ac%+d)",op->stats.ac);
          strcat(retbuf,buf);
        }
	if (op->type==WEAPON && op->level>0) {
	  sprintf(buf,"(improved %d/%d)",op->last_eat,op->level);
          strcat(retbuf,buf);
	}
        break;
      default:
        break;
      }
    }
    if(!need_identify(op) || QUERY_FLAG(op,FLAG_IDENTIFIED)) {
      switch(op->type) {
        case ROD:  /* These use stats.sp for spell selection and stats.food */
        case HORN: /* and stats.hp for spell-point regeneration... */
        case BOW:
        case ARROW:
        case WAND:
	case FOOD:
	case FLESH:
	case DRINK:
          break;
        default:
          if(op->stats.food) {
            if(op->stats.food!=0)
              sprintf(buf,"(sustenance%+d)",op->stats.food);
				/*            else
								  sprintf(buf,"(hunger%+d)",op->stats.food); */
            strcat(retbuf,buf);
          }
          if(op->stats.grace) {
            sprintf(buf,"(grace%+d)",op->stats.grace);
            strcat(retbuf,buf);
          }
          if(op->stats.sp) {
            sprintf(buf,"(magic%+d)",op->stats.sp);
            strcat(retbuf,buf);
          }
          if(op->stats.hp) {
            sprintf(buf,"(regeneration%+d)",op->stats.hp);
            strcat(retbuf,buf);
          }
      }
    }
  } else if(op->type == PLAYER) {
    if(op->contr->digestion) {
      if(op->contr->digestion!=0)
        sprintf(buf,"(sustenance%+d)",op->contr->digestion);
		/*      else
				  sprintf(buf,"(hunger%+d)",op->contr->digestion); */
      strcat(retbuf,buf);
    }
    if(op->contr->gen_grace) {
      sprintf(buf,"(grace%+d)",op->contr->gen_grace);
      strcat(retbuf,buf);
    }
    if(op->contr->gen_sp) {
      sprintf(buf,"(magic%+d)",op->contr->gen_sp);
      strcat(retbuf,buf);
    }
    if(op->contr->gen_hp) {
      sprintf(buf,"(regeneration%+d)",op->contr->gen_hp);
      strcat(retbuf,buf);
    }
  }
  if(!need_identify(op) || QUERY_FLAG(op,FLAG_IDENTIFIED) || QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
    if(QUERY_FLAG(op,FLAG_XRAYS))
      strcat(retbuf,"(xray-vision)");
    if(QUERY_FLAG(op,FLAG_FLYING))
      strcat(retbuf,"(levitate)");
    if(QUERY_FLAG(op,FLAG_SEE_IN_DARK))
      strcat(retbuf,"(infravision)");
  }
  if(!need_identify(op) || QUERY_FLAG(op,FLAG_IDENTIFIED)) {
    if(op->stats.luck) {
      sprintf(buf,"(luck%+d)",op->stats.luck);
      strcat(retbuf,buf);
    }
    if(QUERY_FLAG(op,FLAG_LIFESAVE))
      strcat(retbuf,"(lifesaving)");
    if(QUERY_FLAG(op,FLAG_REFL_SPELL))
      strcat(retbuf,"(reflect spells)");
    if(QUERY_FLAG(op,FLAG_REFL_MISSILE))
      strcat(retbuf,"(reflect missiles)");
    if(QUERY_FLAG(op,FLAG_STEALTH))
      strcat(retbuf,"(stealth)");
    if(op->slaying!=NULL) {
      strcat(retbuf,"(slay ");
      strcat(retbuf,op->slaying);
      strcat(retbuf,")");
    }
  }
  if(QUERY_FLAG(op,FLAG_MONSTER)) {
    if(QUERY_FLAG(op,FLAG_UNDEAD))
      strcat(retbuf,"(undead)");
    if(QUERY_FLAG(op,FLAG_CAN_PASS_THRU))
      strcat(retbuf,"(pass through doors)");
    if(QUERY_FLAG(op,FLAG_SEE_INVISIBLE))
      strcat(retbuf,"(see invisible)");
    if(QUERY_FLAG(op,FLAG_USE_WEAPON))
      strcat(retbuf,"(wield weapon)");
    if(QUERY_FLAG(op,FLAG_USE_BOW))
      strcat(retbuf,"(archer)");
    if(QUERY_FLAG(op,FLAG_USE_ARMOUR))
      strcat(retbuf,"(wear armour)");
    if(QUERY_FLAG(op,FLAG_USE_RING))
      strcat(retbuf,"(wear ring)");
    if(QUERY_FLAG(op,FLAG_USE_SCROLL))
      strcat(retbuf,"(read scroll)");
    if(QUERY_FLAG(op,FLAG_USE_WAND))
      strcat(retbuf,"(fire wand)");
    if(QUERY_FLAG(op,FLAG_USE_ROD))
      strcat(retbuf,"(use rod)");
    if(QUERY_FLAG(op,FLAG_USE_HORN))
      strcat(retbuf,"(use horn)");
    if(QUERY_FLAG(op,FLAG_CAN_USE_SKILL))
      strcat(retbuf,"(skill user)");
    if(QUERY_FLAG(op,FLAG_CAST_SPELL))
      strcat(retbuf,"(spellcaster)");
    if(QUERY_FLAG(op,FLAG_FRIENDLY))
      strcat(retbuf,"(friendly)");
    if(QUERY_FLAG(op,FLAG_UNAGGRESSIVE))
      strcat(retbuf,"(unaggressive)");
    if(QUERY_FLAG(op,FLAG_HITBACK))
      strcat(retbuf,"(hitback)");
    if(op->randomitems != NULL) {
      treasure *t;
      int first = 1;
      for(t=op->randomitems->items; t != NULL; t=t->next)
        if(t->item && (t->item->clone.type == ABILITY)) {
          if(first) {
            first = 0;
            strcat(retbuf,"(Spell abilities:)");
          }
          strcat(retbuf,"(");
          strcat(retbuf,t->item->clone.name);
          strcat(retbuf,")");
        }
    }
  }
  if(!need_identify(op)||QUERY_FLAG(op,FLAG_IDENTIFIED)||
     QUERY_FLAG(op,FLAG_MONSTER)) {
/*    if (op->attacktype != 1)*/
      DESCRIBE_ABILITY(retbuf, op->attacktype, "Attacks");
      strcat(retbuf,describe_resistance(op, 0));
      DESCRIBE_PATH(retbuf, op->path_attuned, "Attuned");
      DESCRIBE_PATH(retbuf, op->path_repelled, "Repelled");
      DESCRIBE_PATH(retbuf, op->path_denied, "Denied");
  }
  return retbuf;
}

/* Return true if the item is magical.  A magical item is one that
 * increases/decreases any abilities, provides a resistance,
 * has a generic magical bonus, or is an artifact.
 * This function is used by detect_magic to determine if an item
 * should be marked as magical.
 */

int is_magical(object *op) {
    int i;

    /* living creatures are considered non magical */
    if (QUERY_FLAG(op, FLAG_ALIVE)) return 0;

    /* This is a test for it being an artifact, as artifacts have titles */
    if (op->title!=NULL) return 1;

    /* Handle rings and amulets specially.  If they change any of these
     * values, it means they are magical.
     */
    if ((op->type==AMULET || op->type==RING) && 
	(op->stats.ac || op->stats.food || op->stats.exp || op->stats.dam ||
	op->stats.wc || op->stats.sp || op->stats.hp || op->stats.luck))
		 return 1;

    /* Check for stealty, speed, flying, or just plain magic in the boots */
    if (op->type== BOOTS && 
	(QUERY_FLAG(op, FLAG_STEALTH) || QUERY_FLAG(op, FLAG_FLYING) ||
	 op->stats.exp))
		return 1;

    /* Take care of amulet/shield that reflects spells/missiles */
    if ((op->type==AMULET || op->type==SHIELD) && 
	(QUERY_FLAG(op, FLAG_REFL_SPELL) || QUERY_FLAG(op, FLAG_REFL_MISSILE)))
	    return 1;

    /* Take care of helmet of xrays */
    if (op->type==HELMET && QUERY_FLAG(op,FLAG_XRAYS)) return 1;

    /* Potions & rods are always magical.  Wands/staves are also magical,
     * assuming they still have any charges left.
     */
    if (op->type==POTION || op->type==ROD ||
	(op->type==WAND && op->stats.food))
	    return 1;

    /* if something gives a protection, either positive or negative, its magical */
    for (i=0; i<NROFATTACKS; i++)
	if (op->resist[i]) return 1;
	
   /* power crystal, spellbooks, and scrolls are always magical.  */
   if (op->magic || op->type==POWER_CRYSTAL || op->type==SPELLBOOK || 
	op->type==SCROLL || op->type==GIRDLE)
	    return 1;

    /* Check to see if it increases/decreases any stats */
    for (i=0; i<7; i++) 
	    if (get_attr_value(&(op->stats),i)!=0) return 1;

    /* If it doesn't fall into any of the above categories, must
     * be non magical.
     */
    return 0;
}

/* need_identify returns true if the item should be identified.  This 
 * function really should not exist - by default, any item not identified
 * should need it.
 */

int need_identify(object *op) {
  switch(op->type) {
  case RING:
  case WAND:
  case ROD:
  case HORN:
  case SCROLL:
  case SKILL:
  case SKILLSCROLL:
  case SPELLBOOK:
  case FOOD:
  case POTION:
  case BOW:
  case ARROW:
  case WEAPON:
  case ARMOUR:
  case SHIELD:
  case HELMET:
  case AMULET:
  case BOOTS:
  case GLOVES:
  case BRACERS:
  case GIRDLE:
  case CONTAINER:
  case DRINK:
  case FLESH:
  case INORGANIC:
  case CLOSE_CON:
  case CLOAK:
  case GEM:
  case POWER_CRYSTAL:
  case POISON:
  case BOOK:
    return 1;
  }
/* Try to track down some stuff that may show up here.  Thus, the
 * archetype file can be updated, and this function removed.
 */
#if 0
  LOG(llevDebug,"need_identify: %s does not need to be id'd\n", op->name);
#endif
  return 0;
}


/* 
 *  Return the number of the spell that whose name passes the pasesed string
 *  argument.   Return -1 if no such spell name match is found.
 */
int look_up_spell_name( char * spname ){
   register int i;
   for(i=0;i< NROFREALSPELLS;i++){
      if( strcmp(spname, spells[i].name) == 0) return i;
   }
   return -1;
}

/*
 * Supposed to fix face-values as well here, but later.
 */

void identify(object *op) {
  object *pl;

  SET_FLAG(op,FLAG_IDENTIFIED);
  CLEAR_FLAG(op, FLAG_KNOWN_MAGICAL);

/*
 * We want autojoining of equal objects:
 */
  if (QUERY_FLAG(op,FLAG_CURSED) || QUERY_FLAG(op,FLAG_DAMNED))
    SET_FLAG(op,FLAG_KNOWN_CURSED);

  if (op->type == POTION && op->arch != (archetype *) NULL) {
      op->face = op->arch->clone.face;
      free_string(op->name);
      op->name = add_refcount(op->arch->clone.name);
  } else if( op->type == SPELLBOOK && op->slaying != NULL){
       if((op->stats.sp = look_up_spell_name( op->slaying )) <0 ){
	  char buf[256];
          op->stats.sp = -1;  
          sprintf(buf, "Spell forumla for %s", op->slaying);
	  if(op->name != NULL) 
		free_string(op->name);
	  op->name = add_string(buf);
       } else {
         /* clear op->slaying since we no longer need it */
         free_string(op->slaying);
         op->slaying=NULL;
       }
  }

  if (op->map) /* The shop identifies items before they hit the ground */
    update_object(op);
  else {
    pl = is_player_inv(op->env);
    if (pl)
	/* A lot of the values can change from an update - might as well send
	 * it all.
	 */
	esrv_send_item_func(pl, op);
  }
}
