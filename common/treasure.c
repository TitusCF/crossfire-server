
/*
 * static char *rcs_treasure_c =
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

#define ALLOWED_COMBINATION

/* TREASURE_DEBUG does some checking on the treasurelists after loading.
 * It is useful for finding bugs in the treasures file.  Since it only
 * slows the startup some (and not actual game play), it is by default
 * left on
 */
#define TREASURE_DEBUG

/* TREASURE_VERBOSE enables copious output concerning artifact generation */
/* #define TREASURE_VERBOSE */
 
#include <global.h>
#include <treasure.h>
#include <spellist.h>
#include <funcpoint.h>
#include <loader.h>

/*
 * Initialize global archtype pointers:
 */

void init_archetype_pointers() {
  int prev_warn = warn_archetypes;
  warn_archetypes = 1;
  if (ring_arch == NULL)
    ring_arch = find_archetype("ring");
  if (amulet_arch == NULL)
    amulet_arch = find_archetype("amulet");
  if (staff_arch == NULL)
    staff_arch = find_archetype("staff");
  if (crown_arch == NULL)
    crown_arch = find_archetype("crown");
  warn_archetypes = prev_warn;
}

/*
 * Allocate and return the pointer to an empty treasurelist structure.
 */

static treasurelist *get_empty_treasurelist() {
  treasurelist *tl = (treasurelist *) malloc(sizeof(treasurelist));
  if(tl==NULL)
    fatal(OUT_OF_MEMORY);
  tl->name=NULL;
  tl->next=NULL;
  tl->items=NULL;
  tl->total_chance=0;
  return tl;
}

/*
 * Allocate and return the pointer to an empty treasure structure.
 */

static treasure *get_empty_treasure() {
  treasure *t = (treasure *) malloc(sizeof(treasure));
  if(t==NULL)
    fatal(OUT_OF_MEMORY);
  t->item=NULL;
  t->name=NULL;
  t->next=NULL;
  t->next_yes=NULL;
  t->next_no=NULL;
  t->chance=100;
  t->magic=0;
  t->nrof=0;
  return t;
}

/*
 * Reads the lib/treasure file from disk, and parses the contents
 * into an internal treasure structure (very linked lists)
 */

static treasure *load_treasure(FILE *fp) {
  char buf[MAX_BUF], *cp, variable[MAX_BUF];
  treasure *t=get_empty_treasure();
  int value;

  nroftreasures++;
  while(fgets(buf,MAX_BUF,fp)!=NULL) {
    if(*buf=='#')
      continue;
    if((cp=strchr(buf,'\n'))!=NULL)
      *cp='\0';
    cp=buf;
    while(*cp==' ') /* Skip blanks */
      cp++;
    if(sscanf(cp,"arch %s",variable)) {
      if((t->item=find_archetype(variable))==NULL)
        LOG(llevError,"Treasure lacks archetype: %s\n",variable);
    } else if (sscanf(cp, "list %s", variable))
	t->name = add_string(variable);
    else if(sscanf(cp,"chance %d",&value))
      t->chance=(uint8) value;
    else if(sscanf(cp,"nrof %d",&value))
      t->nrof=(uint16) value;
    else if(sscanf(cp,"magic %d",&value))
      t->magic=(uint8) value;
    else if(!strcmp(cp,"yes"))
      t->next_yes=load_treasure(fp);
    else if(!strcmp(cp,"no"))
      t->next_no=load_treasure(fp);
    else if(!strcmp(cp,"end"))
      return t;
    else if(!strcmp(cp,"more")) {
      t->next=load_treasure(fp);
      return t;
    } else
      LOG(llevError,"Unknown treasure-command: '%s', last entry %s\n",cp,t->name?t->name:"null");
  }
  LOG(llevError,"treasure lacks 'end'.\n");
  return t;
}

#ifdef TREASURE_DEBUG
/* recursived checks the linked list.  Treasurelist is passed only
 * so that the treasure name can be printed out
 */
static void check_treasurelist(treasure *t, treasurelist *tl)
{
    if (t->item==NULL && t->name==NULL)
	LOG(llevError,"Treasurelist %s has element with no name or archetype\n", tl->name);
    if (t->chance>=100 && t->next_yes && (t->next || t->next_no))
	LOG(llevError,"Treasurelist %s has element that has 100% generation, next_yes field as well as next or next_no\n",
		tl->name);
    /* find_treasurelist will print out its own error message */
    if (t->name && strcmp(t->name,"NONE"))
	(void) find_treasurelist(t->name);
    if (t->next) check_treasurelist(t->next, tl);
    if (t->next_yes) check_treasurelist(t->next_yes,tl);
    if (t->next_no) check_treasurelist(t->next_no, tl);
}
#endif

/*
 * Opens LIBDIR/treasure and reads all treasure-declarations from it.
 * Each treasure is parsed with the help of load_treasure().
 */

void load_treasures() {
  FILE *fp;
  char filename[MAX_BUF], buf[MAX_BUF], name[MAX_BUF];
  treasurelist *previous=NULL;
  treasure *t;
  int comp;

  sprintf(filename,"%s/%s",settings.datadir,settings.treasures);
  if((fp=open_and_uncompress(filename,0,&comp))==NULL) {
    LOG(llevError,"Can't open treasure file.\n");
    return;
  }
  while(fgets(buf,MAX_BUF,fp)!=NULL) {
    if(*buf=='#')
      continue;
    if(sscanf(buf,"treasureone %s\n",name) || sscanf(buf,"treasure %s\n",name)) {
      treasurelist *tl=get_empty_treasurelist();
      tl->name=add_string(name);
      if(previous==NULL)
        first_treasurelist=tl;
      else
        previous->next=tl;
      previous=tl;
      tl->items=load_treasure(fp);
      /* This is a one of the many items on the list should be generated.
       * Add up the chance total, and check to make sure the yes & no
       * fields of the treasures are not being used.
       */
      if (!strncmp(buf,"treasureone",11)) {
	for (t=tl->items; t!=NULL; t=t->next) {
#ifdef TREASURE_DEBUG
	  if (t->next_yes || t->next_no) {
	    LOG(llevError,"Treasure %s is one item, but on treasure %s\n",
		tl->name, t->item ? t->item->name : t->name);
	    LOG(llevError,"  the next_yes or next_no field is set");
	  }
#endif
	  tl->total_chance += t->chance;
	}
#if 0
        LOG(llevDebug, "Total chance for list %s is %d\n", tl->name, tl->total_chance);
#endif
      }
    } else
      LOG(llevError,"Treasure-list didn't understand: %s\n",buf);
  }
  close_and_delete(fp, comp);

#ifdef TREASURE_DEBUG
/* Perform some checks on how valid the treasure data actually is.
 * verify that list transitions work (ie, the list that it is supposed
 * to transition to exists).  Also, verify that at least the name
 * or archetype is set for each treasure element.
 */
  for (previous=first_treasurelist; previous!=NULL; previous=previous->next)
    check_treasurelist(previous->items, previous);
#endif
}

/*
 * Searches for the given treasurelist in the globally linked list
 * of treasurelists which has been built by load_treasures().
 */

treasurelist *find_treasurelist(char *name) {
  char *tmp=find_string(name);
  treasurelist *tl;

  /* Special cases - randomitems of none is to override default.  If
   * first_treasurelist is null, it means we are on the first pass of
   * of loading archetyps, so for now, just return - second pass will
   * init these values.
   */
  if (!strcmp(name,"none") || (!first_treasurelist)) return NULL;
  if(tmp!=NULL)
    for(tl=first_treasurelist;tl!=NULL;tl=tl->next)
      if(tmp==tl->name)
        return tl;
  LOG(llevError,"Couldn't find treasurelist %s\n",name);
  return NULL;
}


/*
 * Generates the objects specified by the given treasure.
 * It goes recursively through the rest of the linked list.
 * If there is a certain percental chance for a treasure to be generated,
 * this is taken into consideration.
 * The second argument specifies for which object the treasure is
 * being generated.
 * If flag is GT_INVISIBLE, only invisible objects are generated (ie, only
 * abilities.  This is used by summon spells, thus no summoned monsters
 * start with equipment, but only their abilities).
 */


static void put_treasure (object *op, object *creator, int flags)
{
    object *tmp;

    if (flags & GT_ENVIRONMENT) {
        op->x = creator->x;
        op->y = creator->y;
        insert_ob_in_map_simple (op, creator->map);
    } else {
        op = insert_ob_in_ob (op, creator);
        if ((flags & GT_APPLY) && QUERY_FLAG (creator, FLAG_MONSTER))
            (void) (*monster_check_apply_func) (creator, op);
        if ((flags & GT_UPDATE_INV) && (tmp = is_player_inv (creator)) != NULL)
            (*esrv_send_item_func) (tmp, op);
    }
}

void create_all_treasures(treasure *t, object *op, int flag, int difficulty, int tries) {
  object *tmp;


  if((int)t->chance >= 100 || (RANDOM()%100 + 1) < (int) t->chance) {
    if (t->name) {
	if (strcmp(t->name,"NONE") && difficulty>=t->magic)
	  create_treasure(find_treasurelist(t->name), op, flag, difficulty, tries);
    }
    else {
      if(t->item->clone.invisible != 0 || ! (flag & GT_INVISIBLE)) {
        tmp=arch_to_object(t->item);
        if(t->nrof&&tmp->nrof<=1)
          tmp->nrof = RANDOM()%((int) t->nrof) + 1;
        fix_generated_item (tmp, op, difficulty, t->magic, flag);
        put_treasure (tmp, op, flag);
      }
    }
    if(t->next_yes!=NULL)
      create_all_treasures(t->next_yes,op,flag,difficulty, tries);
  } else
    if(t->next_no!=NULL)
      create_all_treasures(t->next_no,op,flag,difficulty,tries);
  if(t->next!=NULL)
    create_all_treasures(t->next,op,flag,difficulty, tries);
}

void create_one_treasure(treasurelist *tl, object *op, int flag, int difficulty,
	int tries)
{
    int value = RANDOM() % tl->total_chance;
    treasure *t;

    if (tries++>100) return;
    for (t=tl->items; t!=NULL; t=t->next) {
	value -= t->chance;
	if (value<0) break;
    }

    if (!t || value>=0) {
	LOG(llevError, "create_one_treasure: got null object or not able to find treasure\n");
        abort();
	return;
    }
    if (t->name) {
	if (!strcmp(t->name,"NONE")) return;
	if (difficulty>=t->magic)
	    create_treasure(find_treasurelist(t->name), op, flag, difficulty, tries);
	else if (t->nrof)
	    create_one_treasure(tl, op, flag, difficulty, tries);
	return;
    }
    if(t->item->clone.invisible != 0 || flag != GT_INVISIBLE) {
	object *tmp=arch_to_object(t->item);
        if(t->nrof&&tmp->nrof<=1)
          tmp->nrof = RANDOM()%((int) t->nrof) + 1;
        fix_generated_item (tmp, op, difficulty, t->magic, flag);
        put_treasure (tmp, op, flag);
    }
}

/* This calls the appropriate treasure creation function.  tries is passed
 * to determine how many list transitions or attempts to create treasure
 * have been made.  It is really in place to prevent infinite loops with
 * list transitions, or so that excessively good treasure will not be
 * created on weak maps, because it will exceed the number of allowed tries
 * to do that.
 */
void create_treasure(treasurelist *t, object *op, int flag, int difficulty,
	int tries)
{

    if (tries++>100) return;
    if (t->total_chance) 
	create_one_treasure(t, op, flag,difficulty, tries);
    else
  	create_all_treasures(t->items, op, flag, difficulty, tries);
}

/* This is similar to the old generate treasure function.  However,
 * it instead takes a treasurelist.  It is really just a wrapper around
 * create_treasure.  We create a dummy object that the treasure gets
 * inserted into, and then return that treausre
 */
object *generate_treasure(treasurelist *t, int difficulty)
{
	object *ob = get_object(), *tmp;

	create_treasure(t, ob, 0, difficulty, 0);

	/* Don't want to free the object we are about to return */
	tmp = ob->inv;
	if (tmp!=NULL) remove_ob(tmp);
	if (ob->inv) {
	    LOG(llevError,"In generate treasure, created multiple objects.\n");
	}
	free_object(ob);
	return tmp;
}

/*
 * This is a new way of calculating the chance for an item to have
 * a specific magical bonus.
 * The array has two arguments, the difficulty of the level, and the
 * magical bonus "wanted".
 */

static int difftomagic_list[DIFFLEVELS][MAXMAGIC+1] =
{
/*chance of magic    difficulty*/
/* +0  +1 +2 +3 +4 */
  { 95, 2, 2, 1, 0 }, /*1*/
  { 92, 5, 2, 1, 0 }, /*2*/
  { 85,10, 4, 1, 0 }, /*3*/
  { 80,14, 4, 2, 0 }, /*4*/
  { 75,17, 5, 2, 1 }, /*5*/
  { 70,18, 8, 3, 1 }, /*6*/
  { 65,21,10, 3, 1 }, /*7*/
  { 60,22,12, 4, 2 }, /*8*/
  { 55,25,14, 4, 2 }, /*9*/
  { 50,27,16, 5, 2 }, /*10*/
  { 45,28,18, 6, 3 }, /*11*/
  { 42,28,20, 7, 3 }, /*12*/
  { 40,27,21, 8, 4 }, /*13*/
  { 38,25,22,10, 5 }, /*14*/
  { 36,23,23,12, 6 }, /*15*/
  { 33,21,24,14, 8 }, /*16*/
  { 31,19,25,16, 9 }, /*17*/
  { 27,15,30,18,10 }, /*18*/
  { 20,12,30,25,13 }, /*19*/
  { 15,10,28,30,17 }, /*20*/
  { 13, 9,27,28,23 }, /*21*/
  { 10, 8,25,28,29 }, /*22*/
  {  8, 7,23,26,36 }, /*23*/
  {  6, 6,20,22,46 }, /*24*/
  {  4, 5,17,18,56 }, /*25*/
  {  2, 4,12,14,68 }, /*26*/
  {  0, 3, 7,10,80 }, /*27*/
  {  0, 0, 3, 7,90 }, /*28*/
  {  0, 0, 0, 3,97 }, /*29*/
  {  0, 0, 0, 0,100}, /*30*/
  {  0, 0, 0, 0,100}, /*31*/
};

/*
 * Based upon the specified difficulty and upon the difftomagic_list array,
 * a random magical bonus is returned.  This is used when determine
 * the magical bonus created on specific maps.
 */

int magic_from_difficulty(int difficulty)
{
  int percent,loop;

  difficulty--;
  if(difficulty<0)
    difficulty=0;

  if (difficulty>=DIFFLEVELS)
    difficulty=DIFFLEVELS-1;

  percent = RANDOM()%100;

  for(loop=0;loop<(MAXMAGIC+1);++loop) {
    percent -= difftomagic_list[difficulty][loop];
    if (percent<0)
      break;
  }
  if (loop==(MAXMAGIC+1)) {
    LOG(llevError,"Warning, table for difficulty %d bad.\n",difficulty);
    loop=0;
  }
/*  printf("Chose magic %d for difficulty %d\n",loop,difficulty);*/
  return (RANDOM()%3)?loop:-loop;
}

/*
 * Sets magical bonus in an object, and recalculates the effect on
 * the armour variable, and the effect on speed of armour.
 * This function doesn't work properly, should add use of archetypes
 * to make it truly absolute.
 */

void set_abs_magic(object *op, int magic) {
  if(!magic)
    return;

  op->magic=magic;
  if (op->arch) {
    if (op->type == ARMOUR)
      ARMOUR_SPEED(op)=(ARMOUR_SPEED(&op->arch->clone)*(100+magic*10))/100;

    if (magic < 0 && !(RANDOM()%3)) /* You can't just check the weight always */
      magic = (-magic);
    op->weight = (op->arch->clone.weight*(100-magic*10))/100;
  } else {
    if(op->type==ARMOUR)
      ARMOUR_SPEED(op)=(ARMOUR_SPEED(op)*(100+magic*10))/100;
    if (magic < 0 && !(RANDOM()%3)) /* You can't just check the weight always */
      magic = (-magic);
    op->weight=(op->weight*(100-magic*10))/100;
  }
}

/*
 * Sets a random magical bonus in the given object based upon
 * the given difficulty, and the given max possible bonus.
 */

static void set_magic (int difficulty, object *op, int max_magic, int flags)
{
  int i;
  i = magic_from_difficulty(difficulty);
  if ((flags & GT_ONLY_GOOD) && i < 0)
      i = -i;
  if(i > max_magic)
    i = max_magic;
  set_abs_magic(op,i);
  if (i < 0)
    SET_FLAG(op, FLAG_CURSED);
}

/*
 * Randomly adds one magical ability to the given object.
 * Modified for Partial Resistance in many ways:
 * 1) Since rings can have multiple bonuses, if the same bonus
 *  is rolled again, increase it - the bonuses now stack with
 *  other bonuses previously rolled and ones the item might natively have.
 * 2) Add code to deal with new PR method.
 */

void set_ring_bonus(object *op,int bonus) {

    int r=RANDOM()%(bonus>0?25:11);

    if(op->type==AMULET) {
	if(!(RANDOM()%21))
	    r=20+RANDOM()%2;
	else {
	    if(RANDOM()&2)
		r=10;
	    else
		r=11+RANDOM()%9;
	}
    }

    switch(r) {
	/* Redone by MSW 2000-11-26 to have much less code.  Also,
	 * bonuses and penalties will stack and add to existing values.
	 * of the item.
	 */
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	    set_attr_value(&op->stats, r, bonus + get_attr_value(&op->stats,r));
	    break;

	case 7:
	    op->stats.dam+=bonus;
	    break;

	case 8:
	    op->stats.wc+=bonus;
	    break;

	case 9:
	    op->stats.food+=bonus; /* hunger/sustenance */
	    break;

	case 10:
	    op->stats.ac+=bonus;
	    break;

	/* Item that gives protections/vulnerabilities */
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19: 
	    {
	    int b=5+FABS(bonus),val,resist=RANDOM() % num_resist_table;

	    /* Roughly generate a bonus between 100 and 35 (depending on the bonus) */
	    val = 10 + RANDOM() % b + RANDOM() % b + RANDOM() % b + RANDOM() % b;

	    /* Cursed items need to have higher negative values to equal out with
	     * positive values for how protections work out.  Put another
	     * little random element in since that they don't always end up with
	     * even values.
	     */
	    if (bonus<0) val = 2*-val - RANDOM() % b;
	    if (val>35) val=35;	/* Upper limit */
	    b=0;
	    while (op->resist[resist_table[resist]]!=0 && b<4) {
		resist=RANDOM() % num_resist_table;
	    }
	    if (b==4) return;	/* Not able to find a free resistance */
	    op->resist[resist_table[resist]] = val;
	    /* We should probably do something more clever here to adjust value
	     * based on how good a resistance we gave.
	     */
	    break;
	}
	case 20:
	    if(op->type==AMULET) {
		SET_FLAG(op,FLAG_REFL_SPELL);
		op->value*=11;
	    } else {
		op->stats.hp=1; /* regenerate hit points */
		op->value*=4;
	    }
	    break;

	case 21:
	    if(op->type==AMULET) {
		SET_FLAG(op,FLAG_REFL_MISSILE);
		op->value*=9;
	    } else {
		op->stats.sp=1; /* regenerate spell points */
		op->value*=3;
	    }
	break;

	case 22:
	    op->stats.exp+=bonus; /* Speed! */
	    op->value=(op->value*2)/3;
	    break;
    }
    if(bonus>0)
	op->value*=2*bonus;
    else
	op->value= -(op->value*2*bonus)/3;
}

/*
 * get_magic(diff) will return a random number between 0 and 4.
 * diff can be any value above 2.  The higher the diff-variable, the
 * higher is the chance of returning a low number.
 * It is only used in fix_generated_treasure() to set bonuses on
 * rings and amulets.
 * Another scheme is used to calculate the magic of weapons and armours.
 */

int get_magic(int diff) {
  int i;
  if(diff<3)
    diff=3;
  for(i=0;i<4;i++)
    if(RANDOM()%diff) return i;
  return 4;
}

#define DICE2	(get_magic(2)==2?2:1)
#define DICESPELL (RANDOM()%3+RANDOM()%3+RANDOM()%3+RANDOM()%3+RANDOM()%3)

/*
 * fix_generated_item():  This is called after an item is generated, in
 * order to set it up right.  This produced magical bonuses, puts spells
 * into scrolls/books/wands, makes it unidentified, hides the value, etc.
 */
/* 4/28/96 added creator object from which op may now inherit properties based on
 * op->type. Right now, which stuff the creator passes on is object type 
 * dependant. I know this is a spagetti manuever, but is there a cleaner 
 * way to do this? b.t. */
/*
 * ! (flags & GT_ENVIRONMENT):
 *     Automatically calls fix_flesh_item().
 *
 * flags & FLAG_STARTEQUIP:
 *     Sets FLAG_STARTEQIUP on item if appropriate, or clears the item's
 *     value.
 */
void fix_generated_item (object *op, object *creator, int difficulty,
                         int max_magic, int flags)
{
  int was_magic = op->magic;

  if(!creator||creator->type==op->type) creator=op; /*safety & to prevent polymorphed 
						     * objects giving attributes */ 

  if (difficulty<1) difficulty=1;
  if (op->arch == crown_arch) {
    set_magic(difficulty>25?30:difficulty+5, op, max_magic, flags);
    generate_artifact(op,difficulty);
  } else {
    if(!op->magic && max_magic)
      set_magic(difficulty,op,max_magic, flags);
    if ((!was_magic && !(RANDOM()%CHANCE_FOR_ARTIFACT)) || op->type == HORN ||
	difficulty >= 999 )
      generate_artifact(op, difficulty);
  }
  if (!op->title) /* Only modify object if not special */
    switch(op->type) {
    case WEAPON:
    case ARMOUR:
    case SHIELD:
    case HELMET:
    case CLOAK:
      if (QUERY_FLAG(op, FLAG_CURSED) && !(RANDOM()%4))
        set_ring_bonus(op, -DICE2);
      break;
    case BRACERS:
      if(!(RANDOM()%(QUERY_FLAG(op, FLAG_CURSED)?5:20))) {
        set_ring_bonus(op,QUERY_FLAG(op, FLAG_CURSED)?-DICE2:DICE2);
        if (!QUERY_FLAG(op, FLAG_CURSED))
          op->value*=3;
      }
      break;
    case POTION: {
	int too_many_tries=0,is_special=0;

	while(!(is_special=special_potion(op)) && !op->stats.sp) {
	   generate_artifact(op,difficulty);
	   if(too_many_tries++ > 10) break;
	}
	if(!is_special) { 
	   int nrof_potion=spells[op->stats.sp].scrolls-RANDOM()%3;
           /* these 'potions' have been generated as artifacts 
	    * and therefore need a few adjustments 
            */ 
           /* give fewer potions than scrolls */
           if(nrof_potion>0)
              op->nrof=RANDOM()%(nrof_potion)+1;
           /* value multiplier is same as for scrolls */
           op->value=(op->value*spells[op->stats.sp].level)/
                 (spells[op->stats.sp].level+4);
	}
	break; 
    }
    case AMULET:
      if(op->arch==amulet_arch)
        op->value*=5; /* Since it's not just decoration */
    case RING:
      if(op->arch==NULL) {
        remove_ob(op);
        free_object(op);
        op=NULL;
        break;
      }
      if(op->arch!=ring_arch&&op->arch!=amulet_arch) /* It's a special artefact!*/
        break;
      if ( ! (flags & GT_ONLY_GOOD) && ! (RANDOM() % 3))
        SET_FLAG(op, FLAG_CURSED);
      set_ring_bonus(op,QUERY_FLAG(op, FLAG_CURSED)?-DICE2:DICE2);
      if(op->type!=RING) /* Amulets have only one ability */
        break;
      if(!(RANDOM()%4)) {
        int d=(RANDOM()%2 || QUERY_FLAG(op, FLAG_CURSED))?-DICE2:DICE2;
        if(d>0)
          op->value*=3;
        set_ring_bonus(op,d);
        if(!(RANDOM()%4)) {
          int d=(RANDOM()%3 || QUERY_FLAG(op, FLAG_CURSED))?-DICE2:DICE2;
          if(d>0)
            op->value*=5;
          set_ring_bonus(op,d);
        }
      }
      if(GET_ANIM_ID(op))
	SET_ANIMATION(op, RANDOM()%((int) NUM_ANIMATIONS(op)));
      break;
    case BOOK:
      /* Is it an empty book?, if yes lets make a special 
       * msg for it, and tailor its properties based on the 
       * creator and/or map level we found it on.
       */
      if(!op->msg&&RANDOM()%10) { 
	/* set the book level properly */
	if(creator->level==0 || QUERY_FLAG(creator,FLAG_ALIVE)) {
            if(op->map&&op->map->difficulty) 
	      op->level=RANDOM()%(op->map->difficulty)+RANDOM()%10+1;
            else
	      op->level=RANDOM()%20+1;
	} else 
	    op->level=RANDOM()%creator->level;

	tailor_readable_ob(op,(creator&&creator->stats.sp)?creator->stats.sp:-1);
        /* books w/ info are worth more! */
      	op->value*=((op->level>10?op->level:(op->level+1)/2)*((strlen(op->msg)/250)+1));
	/* creator related stuff */
	if(QUERY_FLAG(creator,FLAG_NO_PICK)) /* for library, chained books! */
	    SET_FLAG(op,FLAG_NO_PICK);
	if(creator->slaying&&!op->slaying) /* for check_inv floors */
	    op->slaying = add_string(creator->slaying);
#ifdef ALLOW_SKILLS /* add exp so reading it gives xp (once)*/
        op->stats.exp = op->value>10000?op->value/5:op->value/10;
#endif
      }
      break;
    case SPELLBOOK:
      if (op->slaying
          && (op->stats.sp = look_up_spell_name (op->slaying)) >= 0)
      {
         free_string (op->slaying);
         op->slaying = NULL;
      }
      else if(!strcmp(op->arch->name,"cleric_book")) 
	 do { 
	     do
               op->stats.sp=RANDOM()%NROFREALSPELLS;
	     while(RANDOM()%10>=spells[op->stats.sp].books); 
	 } while (!spells[op->stats.sp].cleric);
      else
	 do {
             do   
               op->stats.sp=RANDOM()%NROFREALSPELLS;
             while(RANDOM()%10>=spells[op->stats.sp].books); 
         } while (spells[op->stats.sp].cleric);

      op->value=(op->value*spells[op->stats.sp].level)/
                 (spells[op->stats.sp].level+4);
      change_book(op,-1);
#ifdef ALLOW_SKILLS /* add exp so learning gives xp */
      op->level = spells[op->stats.sp].level;
      op->stats.exp = op->value;
#endif
      break;
    case WAND:
      do 
        op->stats.sp=RANDOM()%NROFREALSPELLS;
      while (!spells[op->stats.sp].charges||
             spells[op->stats.sp].level>DICESPELL);
      if (spells[op->stats.sp].cleric)
      { /* Make the wand into a staff */
        short i = op->stats.sp;
        if (staff_arch == NULL)
          staff_arch = find_archetype("staff");
        copy_object(&staff_arch->clone, op);
        op->stats.sp = i;
      }
      op->stats.food=RANDOM()%spells[op->stats.sp].charges+1;
      op->level = spells[op->stats.sp].level/2+ RANDOM()%difficulty + RANDOM()%difficulty;
      if (op->level<1) op->level=1;
      op->value=(op->value*spells[op->stats.sp].level)/
                 (spells[op->stats.sp].level+4);
      break;
    case ROD:
      if (op->stats.maxhp)
        op->stats.maxhp += RANDOM()%op->stats.maxhp;
      op->stats.hp = op->stats.maxhp;
      do
        op->stats.sp = RANDOM()%NROFREALSPELLS;
      while (!spells[op->stats.sp].charges||
             spells[op->stats.sp].sp > op->stats.maxhp ||
             spells[op->stats.sp].level>DICESPELL);
      op->level = spells[op->stats.sp].level/2+ RANDOM()%difficulty + RANDOM()%difficulty;
      op->value=(op->value*op->stats.hp*spells[op->stats.sp].level)/
                (spells[op->stats.sp].level+4);
      break;
    case SCROLL:
      do
        op->stats.sp=RANDOM()%NROFREALSPELLS;
      while (!spells[op->stats.sp].scrolls||
             spells[op->stats.sp].scroll_chance<=RANDOM()%10);
      op->nrof=RANDOM()%spells[op->stats.sp].scrolls+1;
      op->level = spells[op->stats.sp].level/2+ RANDOM()%difficulty +
	 RANDOM()%difficulty;
      if (op->level<1) op->level=1;
      op->value=(op->value*spells[op->stats.sp].level)/
                 (spells[op->stats.sp].level+4);
#ifdef ALLOW_SKILLS /* add exp so reading them properly gives xp */ 
      op->stats.exp = op->value/5;
#endif
      break;
    case RUNE:
      (*trap_adjust_func)(op,difficulty);
      break;
    }
  if (op->type == POTION && special_potion(op)) {
    /*if(op->face==blank_face) op->face = potion_face;*/
    free_string(op->name);
    op->name = add_string("potion");
    op->level = spells[op->stats.sp].level/2+ RANDOM()%difficulty + RANDOM()%difficulty;
    if ( ! (flags & GT_ONLY_GOOD) && RANDOM() % 2)
      SET_FLAG(op, FLAG_CURSED);
  }

  if (flags & GT_STARTEQUIP) {
      if (op->nrof < 2 && op->type != CONTAINER
          && op->type != MONEY && ! QUERY_FLAG (op, FLAG_IS_THROWN))
          SET_FLAG (op, FLAG_STARTEQUIP);
      else if (op->type != MONEY)
          op->value = 0;
  }

  if ( ! (flags & GT_ENVIRONMENT))
    fix_flesh_item (op, creator);
}

/*
 *
 *
 * CODE DEALING WITH ARTIFACTS STARTS HERE
 *
 *
 */

/*
 * Allocate and return the pointer to an empty artifactlist structure.
 */

static artifactlist *get_empty_artifactlist() {
  artifactlist *tl = (artifactlist *) malloc(sizeof(artifactlist));
  if(tl==NULL)
    fatal(OUT_OF_MEMORY);
  tl->next=NULL;
  tl->items=NULL;
  tl->total_chance=0;
  return tl;
}

/*
 * Allocate and return the pointer to an empty artifact structure.
 */

static artifact *get_empty_artifact() {
  artifact *t = (artifact *) malloc(sizeof(artifact));
  if(t==NULL)
    fatal(OUT_OF_MEMORY);
  t->item=NULL;
  t->next=NULL;
  t->chance=0;
  t->difficulty=0;
  t->allowed = NULL;
  return t;
}

/*
 * Searches the artifact lists and returns one that has the same type
 * of objects on it.
 */

artifactlist *find_artifactlist(int type) {
  artifactlist *al;

  for (al=first_artifactlist; al!=NULL; al=al->next)
	if (al->type == type) return al;
  return NULL;
}

/*
 * For debugging purposes.  Dumps all tables.
 */

void dump_artifacts() {
  artifactlist *al;
  artifact *art;
  linked_char *next;

  fprintf(logfile,"\n");
  for (al=first_artifactlist; al!=NULL; al=al->next) {
    fprintf(logfile, "Artifact has type %d, total_chance=%d\n", al->type, al->total_chance);
    for (art=al->items; art!=NULL; art=art->next) {
      fprintf(logfile,"Artifact %-30s Difficulty %3d Chance %5d\n",
	art->item->name, art->difficulty, art->chance);
      if (art->allowed !=NULL) {
	fprintf(logfile,"\tAllowed combinations:");
	for (next=art->allowed; next!=NULL; next=next->next)
	   fprintf(logfile, "%s,", next->name);
	fprintf(logfile,"\n");
      }
    }
  }
  fprintf(logfile,"\n");
}

/*
 * For debugging purposes.  Dumps all treasures recursively (see below).
 */
void dump_monster_treasure_rec (char *name, treasure *t, int depth)
{
  treasurelist *tl;
  int           i;

  if (depth > 100)
    return;
  while (t != NULL)
    {
      if (t->name != NULL)
	{
	  for (i = 0; i < depth; i++)
	    fprintf (logfile, "  ");
	  fprintf (logfile, "{   (list: %s)\n", t->name);
	  tl = find_treasurelist (t->name);
	  dump_monster_treasure_rec (name, tl->items, depth + 2);
	  for (i = 0; i < depth; i++)
	    fprintf (logfile, "  ");
	  fprintf (logfile, "}   (end of list: %s)\n", t->name);
	}
      else
	{
	  for (i = 0; i < depth; i++)
	    fprintf (logfile, "  ");
	  if (t->item->clone.type == FLESH)
	    fprintf (logfile, "%s's %s\n", name, t->item->clone.name);
	  else
	    fprintf (logfile, "%s\n", t->item->clone.name);
	}
      if (t->next_yes != NULL)
	{
	  for (i = 0; i < depth; i++)
	    fprintf (logfile, "  ");
	  fprintf (logfile, " (if yes)\n");
	  dump_monster_treasure_rec (name, t->next_yes, depth + 1);
	}
      if (t->next_no != NULL)
	{
	  for (i = 0; i < depth; i++)
	    fprintf (logfile, "  ");
	  fprintf (logfile, " (if no)\n");
	  dump_monster_treasure_rec (name, t->next_no, depth + 1);
	}
      t = t->next;
    }
}

/*
 * For debugging purposes.  Dumps all treasures for a given monster.
 * Created originally by Raphael Quinet for debugging the alchemy code.
 */

void dump_monster_treasure (char *name)
{
  archetype *at;
  int        found;

  found = 0;
  fprintf (logfile, "\n");
  for (at = first_archetype; at != NULL; at = at->next) 
    if (! strcasecmp (at->clone.name, name) && at->clone.title == NULL)
      {
	fprintf (logfile, "treasures for %s (arch: %s)\n", at->clone.name,
		 at->name);
	if (at->clone.randomitems != NULL)
	  dump_monster_treasure_rec (at->clone.name,
				     at->clone.randomitems->items, 1);
	else
	  fprintf (logfile, "(nothing)\n");
	fprintf (logfile, "\n");
	found++;
      }
  if (found == 0)
    fprintf (logfile, "No objects have the name %s!\n\n", name);
}

/*
 * Builds up the lists of artifacts from the file in the libdir.
 */

void init_artifacts() {
  static int has_been_inited=0;
  FILE *fp;
  char filename[MAX_BUF], buf[MAX_BUF], *cp, *next;
  artifact *art=NULL;
  linked_char *tmp;
  int value, comp;
  artifactlist *al;

  if (has_been_inited) return;
  else has_been_inited = 1;

  sprintf(filename, "%s/artifacts", settings.datadir);
  LOG(llevDebug, "Reading artifacts from %s...",filename);
  if ((fp = open_and_uncompress(filename, 0, &comp)) == NULL) {
    LOG(llevError, "Can't open %s.\n", filename);
    return;
  }

  while (fgets(buf, MAX_BUF, fp)!=NULL) {
    if (*buf=='#') continue;
    if((cp=strchr(buf,'\n'))!=NULL)
      *cp='\0';
    cp=buf;
    while(*cp==' ') /* Skip blanks */
      cp++;
 
    if (!strncmp(cp, "Allowed", 7)) {
      art=get_empty_artifact();

      nrofartifacts++;
      cp = strchr(cp,' ') + 1;
      if (!strcmp(cp,"all")) continue;
      do {
	nrofallowedstr++;
	if ((next=strchr(cp, ','))!=NULL)
	  *(next++) = '\0';
	tmp = (linked_char*) malloc(sizeof(linked_char));
	tmp->name = add_string(cp);
	tmp->next = art->allowed;
	art->allowed = tmp;
      } while ((cp=next)!=NULL);
    }
    else if (sscanf(cp, "chance %d", &value))
	art->chance = (uint16) value;
    else if (sscanf(cp, "difficulty %d", &value))
	art->difficulty = (uint8) value;
    else if (!strncmp(cp, "Object",6)) {
	art->item = (object *) malloc(sizeof(object));
	reset_object(art->item);
        if (!load_object(fp, art->item,LO_LINEMODE,0))
	   LOG(llevError,"Init_Artifacts: Could not load object.\n");
	art->item->name = add_string((strchr(cp, ' ')+1));
	al=find_artifactlist(art->item->type);
	if (al==NULL) {
	  al = get_empty_artifactlist();
	  al->type = art->item->type;
	  al->next = first_artifactlist;
	  first_artifactlist = al;
	}
	art->next = al->items;
	al->items = art;
    }
    else
	LOG(llevError,"Unkown input in artifact file: %s\n", buf);
  }

  close_and_delete(fp, comp);

  for (al=first_artifactlist; al!=NULL; al=al->next) {
    for (art=al->items; art!=NULL; art=art->next) {
      if (!art->chance)
	LOG(llevError,"Warning: artifact with no chance: %s\n", art->item->name);
      else
	al->total_chance += art->chance;
    }
#if 0
    LOG(llevDebug,"Artifact list type %d has %d total chance\n",
	al->type, al->total_chance);
#endif
  }

  LOG(llevDebug,"done.\n");
}


/*
 * Used in artifact generation.  The bonuses of the first object
 * is modified by the bonuses of the second object.
 */

void add_abilities(object *op, object *change) {
  int i, tmp;
  if (change->face != blank_face) {
#ifdef TREASURE_VERBOSE
    LOG(llevDebug, "FACE: %d\n", change->face->number);
#endif
    op->face = change->face;
  }
  for (i = 0; i < 7; i++)
    change_attr_value(&(op->stats), i, get_attr_value(&(change->stats), i));
  if (QUERY_FLAG(change,FLAG_CURSED))
    SET_FLAG(op, FLAG_CURSED);
  if (QUERY_FLAG(change,FLAG_DAMNED))
    SET_FLAG(op, FLAG_DAMNED);
  if ((QUERY_FLAG(change,FLAG_CURSED) || QUERY_FLAG(change,FLAG_DAMNED))
	 && op->magic > 0)
    set_abs_magic(op, -op->magic);
  op->attacktype |= change->attacktype;
  op->path_attuned |= change->path_attuned;
  op->path_repelled |= change->path_repelled;
  op->path_denied |= change->path_denied;
  op->stats.luck += change->stats.luck;
  if (QUERY_FLAG(change,FLAG_LIFESAVE))
    SET_FLAG(op,FLAG_LIFESAVE);
  if (QUERY_FLAG(change,FLAG_REFL_SPELL))
    SET_FLAG(op,FLAG_REFL_SPELL);
  if (QUERY_FLAG(change,FLAG_STEALTH))
    SET_FLAG(op,FLAG_STEALTH);
  if (QUERY_FLAG(change,FLAG_FLYING))
    SET_FLAG(op,FLAG_FLYING);
  if (QUERY_FLAG(change,FLAG_XRAYS))
    SET_FLAG(op,FLAG_XRAYS);
  if (QUERY_FLAG(change,FLAG_BLIND))
    SET_FLAG(op,FLAG_BLIND);
#ifdef USE_LIGHTING
  if (QUERY_FLAG(change,FLAG_SEE_IN_DARK))
    SET_FLAG(op,FLAG_SEE_IN_DARK);
#endif
  if (QUERY_FLAG(change,FLAG_REFL_MISSILE))
    SET_FLAG(op,FLAG_REFL_MISSILE);
  if (QUERY_FLAG(change,FLAG_MAKE_INVIS))
    SET_FLAG(op,FLAG_MAKE_INVIS);	
  if (QUERY_FLAG(change,FLAG_STAND_STILL)) {
    CLEAR_FLAG(op,FLAG_ANIMATE);
    /*op->speed = 0.0; */  /* why was this done? */
    /* It was done so certain artifacts would join. */
    if(!QUERY_FLAG(op,FLAG_ALIVE)) op->speed = 0.0;
    update_ob_speed(op);
  }
  if(change->nrof) op->nrof=RANDOM()%((int) change->nrof) + 1;
  op->stats.exp += change->stats.exp; /* Speed modifier */
  op->stats.wc  += change->stats.wc;
  op->stats.ac  += change->stats.ac;
  if (change->stats.hp < 0)
     op->stats.hp = -change->stats.hp;
  else 
    op->stats.hp  += change->stats.hp;
  if (change->stats.maxhp < 0)
    op->stats.maxhp = -change->stats.maxhp;
  else
    op->stats.maxhp += change->stats.maxhp;
  if (change->stats.sp < 0)
    op->stats.sp = -change->stats.sp;
  else
    op->stats.sp  += change->stats.sp;
  if (change->stats.maxsp < 0)
    op->stats.maxsp = -change->stats.maxsp;
  else
    op->stats.maxsp  += change->stats.maxsp;
  if (change->stats.food < 0)
    op->stats.food = -(change->stats.food);
  else
    op->stats.food += change->stats.food;
  if (change->level < 0)
    op->level = -(change->level);
  else
    op->level += change->level;

  
  for (i=0; i<NROFATTACKS; i++) {
    if (change->resist[i]) {
	op->resist[i] += change->resist[i];
    }
  }
  if (change->stats.dam) {
    if (change->stats.dam < 0)
      op->stats.dam = (-change->stats.dam);
    else if (op->stats.dam) {
      tmp = (signed char) (((int)op->stats.dam * (int)change->stats.dam)/10);
      if (tmp == op->stats.dam) {
        if (change->stats.dam < 10)
          op->stats.dam--;
        else
          op->stats.dam++;
      }
      else
        op->stats.dam = tmp;
    }
  }
  if (change->weight) {
    if (change->weight < 0)
      op->weight = (-change->weight);
    else
      op->weight = (op->weight    * (change->weight)) / 100;
  }
  if (change->last_sp) {
    if (change->last_sp < 0)
      op->last_sp = (-change->last_sp);
    else
      op->last_sp = (signed char) (((int)op->last_sp * (int)change->last_sp) / (int) 100);
  }
  if (change->last_heal) {
    if (change->last_heal < 0)
      op->last_heal = (-change->last_heal);
    else
      op->last_heal = (signed char) (((int)op->last_heal * ((int)change->last_heal))
                      / (int)100);
  }
  op->value *= change->value;

  if(change->material)  op->material = change->material;

  if (change->slaying) {
    if (op->slaying)
      free_string(op->slaying);
    op->slaying = add_refcount(change->slaying);
  }
  if (change->race) {
    if (op->race)
      free_string(op->race);
    op->race = add_refcount(change->race);
  }
  if (change->msg) {
    if (op->msg)
      free_string(op->msg);
    op->msg = add_refcount(change->msg);
  }

}

static int legal_artifact_combination(object *op, artifact *art) {
  int neg, success = 0;
  linked_char *tmp;
  char *name;

  if (art->allowed == (linked_char *) NULL)
    return 1; /* Ie, "all" */
  for (tmp = art->allowed; tmp; tmp = tmp->next) {
#ifdef TREASURE_VERBOSE
    LOG(llevDebug, "legal_art: %s\n", tmp->name);
#endif
    if (*tmp->name == '!')
      name = tmp->name + 1, neg = 1;
    else
      name = tmp->name, neg = 0;

    /* If we match name, then return the opposite of 'neg' */
    if (!strcmp(name,op->name) || (op->arch && !strcmp(name,op->arch->name)))
      return !neg;

    /* Set success as true, since if the match was an inverse, it means
     * everything is allowed except what we match
     */
    else if (neg)
      success = 1;
  }
  return success;
}

/*
 * Fixes the given object, giving it the abilities and titles
 * it should have due to the second artifact-template.
 */

void give_artifact_abilities(object *op, object *artifct) {
  char new_name[MAX_BUF];

  sprintf(new_name, "of %s", artifct->name);
  if (op->title)
    free_string(op->title);
  op->title = add_string(new_name);
  add_abilities(op, artifct); /* Give out the bonuses */

#if 0 /* Bit verbose, but keep it here until next time I need it... */
  {
    char identified = QUERY_FLAG(op, FLAG_IDENTIFIED);
    SET_FLAG(op, FLAG_IDENTIFIED);
    LOG(llevDebug, "Generated artifact %s %s [%s]\n",
      op->name, op->title, describe_item(op));
    if (!identified)
	  CLEAR_FLAG(op, FLAG_IDENTIFIED);
  }
#endif
  return;
}

/*
 * Decides randomly which artifact the object should be
 * turned into.  Makes sure that the item can become that
 * artifact (means magic, difficulty, and Allowed fields properly).
 * Then calls give_artifact_abilities in order to actually create
 * the artifact.
 */

/* Give 1 re-roll attempt per artifact */
#define ARTIFACT_TRIES 2

void generate_artifact(object *op, int difficulty) {
  artifactlist *al;
  artifact *art;
  int i;

  al = find_artifactlist(op->type);
  
  if (al==NULL) {
#if 0 /* This is too verbose, usually */
    LOG(llevDebug, "Couldn't change %s into artifact - no table.\n", op->name);
#endif
    return;
  }

  for (i = 0; i < ARTIFACT_TRIES; i++) {
    int roll = RANDOM()% al->total_chance;

    for (art=al->items; art!=NULL; art=art->next) {
      roll -= art->chance;
      if (roll<0) break;
    }
	
    if (art == NULL || roll>=0) {
#if 1
      LOG(llevError, "Got null entry and non zero roll in generate_artifact, type %d\n",
	op->type);
#endif
      return;
    }
    if (!strcmp(art->item->name,"NONE")) 
	return;
    if (FABS(op->magic) < art->item->magic)
      continue; /* Not magic enough to be this item */

    /* Map difficulty not high enough */
    if (difficulty<art->difficulty)
	continue;

    if (!legal_artifact_combination(op, art)) {
#ifdef TREASURE_VERBOSE
      LOG(llevDebug, "%s of %s was not a legal combination.\n",
          op->name, art->item->name);
#endif
      continue;
    }
    give_artifact_abilities(op, art->item);
    return;
  }
}

/* fix_flesh_item() - objects of type FLESH are similar to type
 * FOOD, except they inherit properties (name, food value, etc).
 * based on the original owner (or 'donor' if you like). -b.t.
 */

void fix_flesh_item(object *item, object *donor) {
    char tmpbuf[MAX_BUF];
    int i;

    if(item->type==FLESH && donor) {
	/* change the name */
	sprintf(tmpbuf,"%s's %s",donor->name,item->name);
	free_string(item->name);
	item->name=add_string(tmpbuf);
	/* weight is FLESH weight/100 * donor */
	if((item->weight = (float) (item->weight/100.0) * donor->weight)==0)
		item->weight=1;

	/* value is multiplied by level of donor */
	item->value *= isqrt(donor->level*2);

	/* food value */
	item->stats.food += (donor->stats.hp/100) + donor->stats.Con;

	/* flesh items inherit some abilities of donor, but not
	 * full effect.
	 */
	for (i=0; i<NROFATTACKS; i++)
	    item->resist[i] = donor->resist[i]/2;

	/* if donor has some attacktypes, the flesh is poisonous */
	if(donor->attacktype&AT_POISON)
	    item->type=POISON;
	if(donor->attacktype&AT_ACID) item->stats.hp = -1*item->stats.food;
	SET_FLAG(item,FLAG_NO_STEAL);
    }
}

/* special_potion() - so that old potion code is still done right. */

int special_potion (object *op) {

    int i;

    if(op->attacktype) return 1;

    if(op->stats.Str || op->stats.Dex || op->stats.Con || op->stats.Pow
 	|| op->stats.Wis || op->stats.Int || op->stats.Cha ) return 1;

    for (i=0; i<NROFATTACKS; i++)
	if (op->resist[i]) return 1;

    return 0;
}

void free_treasurestruct(treasure *t)
{
	if (t->next) free_treasurestruct(t->next);
	if (t->next_yes) free_treasurestruct(t->next_yes);
	if (t->next_no) free_treasurestruct(t->next_no);
	free(t);
}

void free_charlinks(linked_char *lc)
{
	if (lc->next) free_charlinks(lc->next);
	free(lc);
}

void free_artifact(artifact *at)
{

    if (at->next) free_artifact(at->next);
    if (at->allowed) free_charlinks(at->allowed);
    if (at->item) {
	if (at->item->name) free_string(at->item->name);
	if (at->item->msg) free_string(at->item->msg);
	if (at->item->title) free_string(at->item->title);
	free(at->item);
    }
    free(at);
}

void free_artifactlist(artifactlist *al)
{
    artifactlist *nextal;
    for (al=first_artifactlist; al!=NULL; al=nextal) {  
	nextal=al->next;
	if (al->items) {
		free_artifact(al->items);
	}
	free(al);
    }
}

void free_all_treasures() {
treasurelist *tl, *next;


    for (tl=first_treasurelist; tl!=NULL; tl=next) {
	next=tl->next;
	if (tl->name) free_string(tl->name);
	if (tl->items) free_treasurestruct(tl->items);
	free(tl);
    }
    free_artifactlist(first_artifactlist);
}
