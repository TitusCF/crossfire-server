

/* Basic stuff for use with the alchemy code. Clearly some of this stuff
 * could go into server/alchemy, but I left it here just in case it proves
 * more generally useful. 
 *
 * Nov 1995 - file created by b.t. thomas@astro.psu.edu 
 */


/* Our definition of 'formula' is any product of an alchemical process.
 * Ingredients are just comma delimited list of archetype (or object) 
 * names. 
 */

/* Example 'formula' entry in libdir/formulae:
 * 	Object transparency
 *	chance 10
 *	ingred dust of beholdereye,gem
 *	arch potion_generic
 */

#include <global.h>
#include <object.h>
#include <ctype.h>

static void build_stringlist(const char *str, char ***result_list, size_t *result_size);

static recipelist *formulalist;

static recipelist *init_recipelist(void) {
  recipelist *tl = (recipelist *) malloc(sizeof(recipelist));
  if(tl==NULL)
    fatal(OUT_OF_MEMORY);
  tl->total_chance=0;
  tl->number=0;
  tl->items=NULL;
  tl->next=NULL;
  return tl;
}

static recipe *get_empty_formula(void) {
  recipe *t = (recipe *) malloc(sizeof(recipe));
  if(t==NULL)
    fatal(OUT_OF_MEMORY);
  t->chance = 0;
  t->index = 0;
  t->transmute = 0;
  t->yield=0;
  t->diff=0;
  t->exp=0;
  t->keycode = 0;
  t->title = NULL;
  t->arch_names = 0;
  t->arch_name = NULL;
  t->skill = NULL;
  t->cauldron = NULL;
  t->ingred = NULL;
  t->next=NULL;
  return t;
}
 
/* get_formulalist() - returns pointer to the formula list
 * i is the number of ingredients.
 */

recipelist * get_formulalist ( int i ) {
  recipelist *fl=formulalist;
  int number=i;

  while(fl && number>1) { 
         if(!(fl=fl->next)) break;
         number--;
     }   
  return fl;
}

/* check_recipe() - makes sure we actually have the requested artifact 
 * and archetype. */

static int check_recipe(const recipe *rp) {
    size_t i;
    int result;

    result = 1;
    for (i = 0; i < rp->arch_names; i++) {
        if (find_archetype(rp->arch_name[i]) != NULL) { 
            artifact *art = locate_recipe_artifact(rp, i);
            if (!art && strcmp(rp->title, "NONE") != 0) {
                LOG(llevError,"\nWARNING: Formula %s of %s has no artifact.\n", rp->arch_name[i], rp->title); 
                result = 0;
            }
        } else { 
            LOG(llevError,"\nWARNING: Can't find archetype %s for formula %s\n", rp->arch_name[i], rp->title); 
            result = 0;
        }
    }
   
    return result;
}


/*
 * init_formulae() - Builds up the lists of formula from the file in 
 * the libdir. -b.t. 
 */
 
void init_formulae(void) {
  static int has_been_done=0;
  FILE *fp;
  char filename[MAX_BUF], buf[MAX_BUF], *cp, *next;
  recipe *formula=NULL;
  recipelist *fl=init_recipelist();
  linked_char *tmp;
  int value, comp;

  if(!formulalist) formulalist = fl;
 
  if (has_been_done) return;
  else has_been_done = 1;
 
  sprintf(filename, "%s/formulae", settings.datadir);
  LOG(llevDebug, "Reading alchemical formulae from %s...",filename);
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
 
    if (!strncmp(cp, "Object", 6)) {
      formula=get_empty_formula();
      formula->title = add_string(strchr(cp,' ') + 1);
    } else if (!strncmp(cp, "keycode", 7)) {
      formula->keycode = add_string(strchr(cp,' ') + 1);
    } else if (sscanf(cp, "trans %d", &value)) {
        formula->transmute = (uint16)value;
    } else if (sscanf(cp, "yield %d", &value)) {
	formula->yield = (uint16)value;
    } else if (sscanf(cp, "chance %d", &value)) {
        formula->chance = (uint16)value;
    } else if (sscanf(cp, "exp %d", &value)) {
	formula->exp = (uint16)value;
    } else if (sscanf(cp, "diff %d", &value)) {
	formula->diff = (uint16)value;
    } else if (!strncmp(cp, "ingred",6)) {
      int numb_ingred = 1;
      cp = strchr(cp,' ') + 1;
      do {
        if ((next=strchr(cp,','))!=NULL)
          {*(next++) = '\0'; numb_ingred++;}
        tmp = (linked_char*) malloc(sizeof(linked_char));
        tmp->name = add_string(cp);
        tmp->next = formula->ingred;
        formula->ingred = tmp;
	/* each ingredient's ASCII value is coadded. Later on this 
 	 * value will be used allow us to search the formula lists 
	 * quickly for the right recipe. 
	 */ 
	formula->index += strtoint(cp);
      } while ((cp=next)!=NULL);
      /* now find the correct (# of ingred ordered) formulalist */
      fl=formulalist;
      while(numb_ingred!=1) {
	if(!fl->next)
	  fl->next = init_recipelist();
	fl = fl->next;
        numb_ingred--;
      }
      fl->total_chance += formula->chance;
      fl->number++;
      formula->next = fl->items; 
      fl->items = formula;
    } else if (!strncmp(cp, "arch",4)) { 
        build_stringlist(strchr(cp, ' ')+1, &formula->arch_name, &formula->arch_names);
        (void) check_recipe(formula);
    } else if (!strncmp(cp, "skill", 5)) {
	formula->skill = add_string(strchr(cp, ' ')+1);
    } else if (!strncmp(cp, "cauldron", 8)) {
	formula->cauldron = add_string(strchr(cp, ' ')+1);
    } else
        LOG(llevError,"Unknown input in file %s: %s\n", filename, buf);
  }
  LOG(llevDebug,"done.\n");
  close_and_delete(fp, comp);
  /* Lastly, lets check for problems in formula we got */
  check_formulae();
}

/* check_formulae()- since we are doing a squential search on the
 * formulae lists now, we have to be carefull that we dont have 2
 * formula with the exact same index value. Under the new nbatches
 * code, it is possible to have multiples of ingredients in a cauldron
 * which could result in an index formula mismatch. We *don't* check for
 * that possibility here. -b.t.
 */
void check_formulae( void ) {
  recipelist *fl;
  recipe *check, *formula;
  int numb = 1;

  LOG(llevDebug,"Checking formulae lists...");

  for(fl=formulalist; fl!=NULL; fl = fl->next) { 
    for (formula=fl->items; formula!=NULL; formula=formula->next) 
      for (check=formula->next; check!=NULL; check=check->next) 
        if(check->index==formula->index) { 
          LOG(llevError," ERROR: On %d ingred list: ", numb);
          LOG(llevError, "Formulae [%s] of %s and [%s] of %s have matching index id (%d)\n",
            formula->arch_name[0],formula->title,check->arch_name[0],check->title,formula->index);
        }
    numb++;
  }

  LOG(llevDebug,"done.\n");

}

/* Borrowed (again) from the artifacts code for this  */

void dump_alchemy( void ) {
  recipelist *fl=formulalist;
  recipe *formula=NULL;
  linked_char *next;
  int num_ingred=1;

  fprintf(logfile, "\n");
  while(fl) { 
    fprintf(logfile, "\n Formulae with %d ingredient%s  %d Formulae with total_chance=%d\n", 
	num_ingred, num_ingred>1?"s.":".",fl->number,fl->total_chance);
    for (formula=fl->items; formula!=NULL; formula=formula->next) {
      artifact *art=NULL;
      char buf[MAX_BUF];
      size_t i;

      for (i = 0; i < formula->arch_names; i++) {
        const char *string = formula->arch_name[i];
	if(find_archetype(string)!=NULL) { 
          art = locate_recipe_artifact(formula, i);
          if (!art && strcmp(formula->title,"NONE")) 
		LOG(llevError,"Formula %s has no artifact\n",formula->title);
	  else {
	     if(strcmp(formula->title,"NONE"))
                sprintf(buf,"%s of %s",string,formula->title);
	     else
                sprintf(buf,"%s",string);
             fprintf(logfile,"%-30s(%d) bookchance %3d  ",buf,formula->index,
		formula->chance);
             fprintf(logfile,"skill %s",formula->skill);
             fprintf(logfile,"\n");
             if (formula->ingred !=NULL) {
		int nval=0,tval=0;
                fprintf(logfile,"\tIngred: ");
                for (next=formula->ingred; next!=NULL; next=next->next) { 
		  if(nval!=0) fprintf(logfile,","); 
                  fprintf(logfile,"%s(%d)",next->name,(nval=strtoint(next->name)));
		  tval += nval;
		}
                fprintf(logfile,"\n");
		if(tval!=formula->index) fprintf(logfile, "WARNING:ingredient list and formula values not equal.\n");
             }
	     if (formula->skill != NULL)
		 fprintf(logfile, "\tSkill Required: %s", formula->skill);
	     if (formula->cauldron != NULL)
		 fprintf(logfile, "\tCauldron: %s\n", formula->cauldron);
	     fprintf(logfile, "\tDifficulty: %d\t Exp: %d\n", formula->diff,
		 formula->exp);
	  }
	} else 
	   LOG(llevError,"Can't find archetype:%s for formula %s\n", string,
		formula->title); 
      }
    }  
  fprintf(logfile,"\n");
  fl = fl->next;
  num_ingred++;
  }
}

/* Find a treasure with a matching name.  The 'depth' parameter is
 * only there to prevent infinite loops in treasure lists (a list
 * referencing another list pointing back to the first one). */
archetype *find_treasure_by_name (const treasure *t, const char *name, int depth)
{
  treasurelist *tl;
  archetype    *at;

  if (depth > 10)
    return NULL;
  while (t != NULL)
    {
      if (t->name != NULL)
	{
	  tl = find_treasurelist (t->name);
	  at = find_treasure_by_name (tl->items, name, depth + 1);
	  if (at != NULL)
	    return at;
	}
      else
	{
	  if (! strcasecmp (t->item->clone.name, name))
	    return t->item;
	}
      if (t->next_yes != NULL)
	{
	  at = find_treasure_by_name (t->next_yes, name, depth);
	  if (at != NULL)
	    return at;
	}
      if (t->next_no != NULL)
	{
	  at = find_treasure_by_name (t->next_no, name, depth);
	  if (at != NULL)
	    return at;
	}
      t = t->next;
    }
  return NULL;
}

/* If several archetypes have the same name, the value of the first
 * one with that name will be returned.  This happens for the
 * mushrooms (mushroom_1, mushroom_2 and mushroom_3).  For the
 * monsters' body parts, there may be several monsters with the same
 * name.  This is not a problem if these monsters have the same level
 * (e.g. sage & c_sage) or if only one of the monsters generates the
 * body parts that we are looking for (e.g. big_dragon and
 * big_dragon_worthless). */
long find_ingred_cost (const char *name)
{
  archetype    *at;
  archetype    *at2;
  artifactlist *al;
  artifact     *art;
  long          mult;
  char         *cp;
  char          part1[100];
  char          part2[100];

  /* same as atoi(), but skip number */
  mult = 0;
  while (isdigit (*name))
    {
      mult = 10 * mult + (*name - '0');
      name++;
    }
  if (mult > 0)
    name++;
  else
    mult = 1;
  /* first, try to match the name of an archetype */
  for (at = first_archetype; at != NULL; at = at->next) 
    {
      if (at->clone.title != NULL)
	{
	  /* inefficient, but who cares? */
	  sprintf (part1, "%s %s", at->clone.name, at->clone.title);
	  if (! strcasecmp (part1, name))
	    return mult * at->clone.value;
	}
      if (! strcasecmp (at->clone.name, name))
	return mult * at->clone.value;
    }
  /* second, try to match an artifact ("arch of something") */
  cp = strstr (name, " of ");
  if (cp != NULL)
    {
      strcpy (part1, name);
      part1[cp - name] = '\0';
      strcpy (part2, cp + 4);
      /* find the first archetype matching the first part of the name */
      for (at = first_archetype; at != NULL; at = at->next) 
	if (! strcasecmp (at->clone.name, part1) && at->clone.title == NULL)
	  break;
      if (at != NULL)
	{
	  /* find the first artifact derived from that archetype (same type) */
	  for (al = first_artifactlist; al != NULL; al = al->next)
	    if (al->type == at->clone.type)
	      {
		for (art = al->items; art != NULL; art = art->next)
		  if (! strcasecmp (art->item->name, part2))
		    return mult * at->clone.value * art->item->value;
	      }
	}
    }
  /* third, try to match a body part ("arch's something") */
  cp = strstr (name, "'s ");
  if (cp != NULL)
    {
      strcpy (part1, name);
      part1[cp - name] = '\0';
      strcpy (part2, cp + 3);
      /* examine all archetypes matching the first part of the name */
      for (at = first_archetype; at != NULL; at = at->next) 
	if (! strcasecmp (at->clone.name, part1) && at->clone.title == NULL)
	  {
	    if (at->clone.randomitems != NULL)
	      {
		at2 = find_treasure_by_name (at->clone.randomitems->items,
					     part2, 0);
		if (at2 != NULL)
		  return mult * at2->clone.value * isqrt (at->clone.level * 2);
	      }
	  }
    }
  /* failed to find any matching items -- formula should be checked */
  return -1;
}

/* code copied from dump_alchemy() and modified by Raphael Quinet */
void dump_alchemy_costs (void)
{
  recipelist *fl=formulalist;
  recipe *formula=NULL;
  linked_char *next;
  int num_ingred=1;
  int num_errors=0;
  long cost;
  long tcost;

  fprintf (logfile, "\n");
  while (fl) { 
    fprintf(logfile, "\n Formulae with %d ingredient%s  %d Formulae with total_chance=%d\n", 
	num_ingred, num_ingred>1?"s.":".",fl->number,fl->total_chance);
    for (formula = fl->items; formula != NULL; formula = formula->next) {
      artifact *art=NULL;
      archetype *at=NULL;
      char buf[MAX_BUF];
      size_t i;

      for (i = 0; i < formula->arch_names; i++) {
        const char *string = formula->arch_name[i];
	if ((at = find_archetype (string)) != NULL) {
          art = locate_recipe_artifact (formula, i);
          if (!art && strcmp (formula->title,"NONE")) 
	    LOG (llevError, "Formula %s has no artifact\n", formula->title);
	  else
	    {
	      if (! strcmp (formula->title, "NONE"))
		sprintf (buf, "%s", string);
	      else
		sprintf (buf, "%s of %s", string, formula->title);
	      fprintf (logfile, "\n%-40s bookchance %3d  skill %s\n",
		       buf, formula->chance, formula->skill);
	      if (formula->ingred !=NULL)
		{
		  tcost = 0;
		  for (next = formula->ingred; next != NULL; next = next->next)
		    { 
		      cost = find_ingred_cost (next->name);
		      if (cost < 0)
			num_errors++;
		      fprintf (logfile,"\t%-33s%5ld\n", next->name, cost);
		      if (cost < 0 || tcost < 0)
			tcost = -1;
		      else
			tcost += cost;
		    }
		  if (art != NULL && art->item != NULL)
		    cost = at->clone.value * art->item->value;
		  else
		    cost = at->clone.value;
		  fprintf (logfile, "\t\tBuying result costs: %5ld", cost);
		  if (formula->yield > 1)
		    {
		      fprintf (logfile, " to %ld (max %d items)\n",
			       cost * formula->yield, formula->yield);
		      cost = cost * (formula->yield + 1L) / 2L;
		    }
		  else
		    fprintf (logfile, "\n");
		  fprintf (logfile, "\t\tIngredients cost:    %5ld\n\t\tComment: ", tcost);
		  if (tcost < 0)
		    fprintf (logfile, "Could not find some ingredients.  Check the formula!\n");
		  else if (tcost > cost)
		    fprintf (logfile, "Ingredients are much too expensive.  Useless formula.\n");
		  else if (tcost * 2L > cost)
		    fprintf (logfile, "Ingredients are too expensive.\n");
		  else if (tcost * 10L < cost)
		    fprintf (logfile, "Ingredients are too cheap.\n");
		  else
		    fprintf (logfile, "OK.\n");
		}
	    }
	}
	else 
	  LOG(llevError, "Can't find archetype:%s for formula %s\n", string,
	      formula->title); 
      }
    }
  fprintf (logfile,"\n");
  fl = fl->next;
  num_ingred++;
  }
  if (num_errors > 0)
    fprintf (logfile, "WARNING: %d objects required by the formulae do not exist in the game.\n",
	     num_errors);
}

const char * ingred_name (const char *name) {
  const char *cp=name;
 
  if(atoi(cp)) cp = strchr(cp,' ') + 1;
  return cp;
}

/* strtoint() - we use this to convert buf into an integer
 * equal to the coadded sum of the (lowercase) character 
 * ASCII values in buf (times prepended integers).
 */

int strtoint (const char *buf) {
  const char *cp = ingred_name(buf);
  int val=0, len=strlen(cp), mult=numb_ingred(buf);

  while (len) { 
    val += tolower(*cp);
    cp++; len--;
  }
  return val*mult;
}

artifact * locate_recipe_artifact(const recipe *rp, size_t idx) {
   object *item=create_archetype(rp->arch_name[idx]);
   artifactlist *at=NULL;
   artifact *art=NULL;

   if(!item) return (artifact *) NULL; 

   if((at=find_artifactlist(item->type)))
      for(art=at->items;art;art=art->next)
          if(!strcmp(art->item->name,rp->title)) break;
 
   free_object(item);

   return art;
}

int numb_ingred (const char *buf) {
  int numb;

  if((numb=atoi(buf))) return numb;
  else return 1;
}

recipelist * get_random_recipelist ( void ) {
  recipelist *fl=NULL;
  int number=0,roll=0;

     /* first, determine # of recipelist we have */
     for(fl=get_formulalist(1);fl;fl=fl->next) number++;
 
     /* now, randomly choose one */
     if(number>0) roll=RANDOM()%number;
 
     fl=get_formulalist(1);
     while(roll && fl) {
        if(fl->next) fl = fl->next;
        else break;
        roll--;
     }   
     if(!fl) /* failed! */
        LOG(llevError,"get_random_recipelist(): no recipelists found!\n");
     else if(fl->total_chance==0) fl=get_random_recipelist();

     return fl;
}

recipe * get_random_recipe ( recipelist *rpl ) {
  recipelist *fl=rpl;
  recipe *rp=NULL;
  int r=0;

  /* looks like we have to choose a random one */ 
  if(fl==NULL) if((fl=get_random_recipelist())==NULL) return rp;
  
  if (fl->total_chance > 0) {
    r=RANDOM()%fl->total_chance;
    for (rp=fl->items;rp;rp=rp->next) {
      r -= rp->chance;
      if (r<0) break;
    }
  }
  return rp;
}

void free_all_recipes(void)
{
    recipelist *fl=formulalist,*flnext;
    recipe *formula=NULL,*next;
    linked_char *lchar, *charnext;

    LOG(llevDebug,"Freeing all the recipes\n");
    for (fl=formulalist; fl!=NULL; fl=flnext) {
	flnext=fl->next;

	for (formula=fl->items; formula!=NULL; formula=next) {
	    next=formula->next;
      
	    free(formula->arch_name[0]);
	    free(formula->arch_name);
	    if (formula->title)
		free_string(formula->title);
	    if (formula->skill)
		free_string(formula->skill);
	    if (formula->cauldron)
		free_string(formula->cauldron);
	    for (lchar=formula->ingred; lchar; lchar=charnext) {
		charnext=lchar->next;
		free_string(lchar->name);
		free(lchar);
	    }
	    free(formula);
	}
	free(fl);
    }
}

/**
 * Split a comma separated string list into words.
 *
 * @param str the string to split
 *
 * @param result_list pointer to return value for the newly created list; the
 * caller is responsible for freeing both *result_list and **result_list.
 *
 * @param result_size pointer to return value for the size of the newly
 * created list
 */
static void build_stringlist(const char *str, char ***result_list, size_t *result_size)
{
    char *dup;
    char *p;
    size_t size;
    size_t i;

    dup = strdup_local(str);
    if (dup == NULL)
        fatal(OUT_OF_MEMORY);

    size = 0;
    for (p = strtok(dup, ","); p != NULL; p = strtok(NULL, ","))
        size++;

    *result_list = malloc(size*sizeof(*result_list));
    if (*result_list == NULL)
        fatal(OUT_OF_MEMORY);
    *result_size = size;

    for (i = 0; i < size; i++) {
        (*result_list)[i] = dup;
        dup = dup+strlen(dup)+1;
    }
}
