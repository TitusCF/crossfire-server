/*
 * static char *rcsid_resurrection_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to frankj@ifi.uio.no.
*/
/*  the contents of this file were create solely by peterm@soda.berkeley.edu
    all of the above disclaimers apply.  */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <errno.h>
#ifdef sequent
/* stoopid sequent includes don't do this like they should */
extern char * sys_errlist[];
extern int sys_nerr;
#endif
extern char **classname;
extern object *objects;



void dead_player(object *op)
{
  char filename[MAX_BUF];
  char newname[MAX_BUF];
  char path[MAX_BUF];

/*  set up our paths/strings...  */
  sprintf(path,"%s/%s/%s/%s",settings.localdir,settings.playerdir,op->name,
	  op->name);

  strcpy(filename,path);
  strcat(filename,".pl");
  strcpy(newname,filename);
  strcat(newname,".dead");

  if(!rename(filename,newname)) {
	perror(filename);
	perror(newname);
  }
}




/*  raise_dead by peterm and mehlhaff@soda.berkeley.edu  */
/*  *op  --  who is doing the resurrecting
    dir  --  direction the spell is cast
    spell_type  --  which spell was cast 
*/
int cast_raise_dead_spell(object *op,int dir,int spell_type, object * corpseobj)
{
  object *temp, *newob;
  char name_to_resurrect[MAX_BUF];
  int leveldead=25;


  if(corpseobj == NULL){
    if (out_of_map(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]))
	temp=NULL;
    else {
	/*  First we need to find a corpse, if any.  */
	/* If no object, temp will be set to NULL */
	for(temp=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);
	    temp!=NULL; temp=temp->above)
	    /* really need a better way to spot corpseobjects
	     * corpses immune to everything, so that they cannot be destroyed.
	     */
		if(temp->type == CORPSE && temp->immune==262143) 
		    break;
    } 
  } else temp = corpseobj;

   if(temp == NULL&&(spell_type==SP_RAISE_DEAD||spell_type==SP_RESURRECTION)){
        new_draw_info(NDI_UNIQUE, 0,op, "You need a body for this spell.");
        return 0;
   }

   strcpy(name_to_resurrect, temp->name );

     /* no matter what, we fry the corpse.  */
   if( temp) if(temp->map){
       /* replace corpse object with a burning object */
       newob = arch_to_object(find_archetype("burnout"));
       if(newob != NULL){
          newob->x = temp->x;
          newob->y = temp->y;
          insert_ob_in_map( newob, temp->map, op);
       }
	leveldead=temp->level;
       remove_ob(temp);
       free_object(temp);
   }
   /* chance it fails to resurrect? -- implement here */
   /* also, chances you accidentally summon death or worse... */

  switch(spell_type)
    {
    case SP_RAISE_DEAD:
      /*  see if this spell fails, if so then summon some
          undead. levels[1] is important, see below*/
      if(resurrection_fails(op->level,leveldead))
        {
          summon_hostile_monsters(op,5,"demon");
          summon_hostile_monsters(op,3,"skull");
          return 1;
	}
      return resurrect_player(op,name_to_resurrect,spell_type);


    case SP_RESURRECTION:
      if(resurrection_fails(op->level,leveldead))
        {
          summon_hostile_monsters(op,5,"skull");
          summon_hostile_monsters(op,3,"lich");
          return 1;
	}
      return resurrect_player(op,name_to_resurrect,spell_type);


    case SP_REINCARNATION:
      if(resurrection_fails(op->level,0))
        {
          summon_hostile_monsters(op,5,"lich");
          summon_hostile_monsters(op,3,"demilich");
          summon_hostile_monsters(op,1,"spectre");
          return 1;
	}
      return resurrect_player(op,name_to_resurrect,spell_type);
    }
   return 1;
}


int resurrection_fails(int levelcaster,int leveldead)
{
  int chance=9;
/*  scheme:  equal in level, 50% success.
    +5 % for each level below, -5% for each level above.
    minimum 20%
    */
  chance+=levelcaster-leveldead;
  if(chance<4) chance=4;
  if(chance>RANDOM()%20) return 0;  /* resurrection succeeds */
  return 1;
}


/*  name of the person to resurrect and which spell was used
to resurrect  */
int resurrect_player(object *op,char *playername,int rspell)
{
  FILE *deadplayer,*liveplayer;

  char oldname[MAX_BUF];
  char newname[MAX_BUF];
  char path[MAX_BUF];
  char buf[MAX_BUF];
  char buf2[MAX_BUF];

  static char *races[]= { "barbarian","cleric", "elf","human","mage","ninja","priest",
			"swashbuckler","thief","viking","warrior","wizard" };

  long int exp;
  int Con;

/*  set up our paths/strings...  */
  sprintf(path,"%s/%s/%s/%s",settings.localdir,settings.playerdir,playername,
	  playername);

  strcpy(newname,path);
  strcat(newname,".pl");

  strcpy(oldname,newname);
  strcat(oldname,".dead");

  if(! (deadplayer=fopen(oldname,"r"))) {
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"The soul of %s cannot be reached.",playername);
    return 0;
    }

  if(!access(newname,0)) {
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"The soul of %s has already been reborn!",playername);
    fclose(deadplayer);
    return 0;
    }

  if(! (liveplayer=fopen(newname,"w")))
    {
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"The soul of %s cannot be re-embodied at the moment.",playername);
    LOG(llevError,"Cannot write player in ressurect_player!\n");
    fclose(deadplayer);
    return 0;
    }

  while (!feof(deadplayer))
    {	fgets(buf,255,deadplayer);
	sscanf(buf,"%s",buf2);
	if( ! (strcmp(buf2,"exp"))) {
	    sscanf(buf,"%s %ld",buf2,&exp);
	    switch(rspell) {
		case SP_RAISE_DEAD:
		exp-=exp/5;
		break;
		case SP_RESURRECTION:
		exp-=exp/10;
		break;
		case SP_REINCARNATION:
		exp-=exp/20;
		break;
	    }
	    sprintf(buf,"exp %ld\n",exp);
	}
      if(! (strcmp(buf2,"Con"))) {
	sscanf(buf,"%s %d",buf2,&Con);	
	switch(rspell) {
	    case SP_RAISE_DEAD:
		Con-=2;
	    break;
	    case SP_RESURRECTION:
		Con-=1;
	    break;
	}
	sprintf(buf,"Con %d\n",Con);
      }
     if(rspell==SP_REINCARNATION) {
	if(! (strcmp(buf2,"race"))) {
	    sprintf(buf,"race %s\n",races[RANDOM()%12+1]);
	}
     }
  fputs(buf,liveplayer);
    }
  fclose(liveplayer);
  fclose(deadplayer);
  unlink(oldname);
  new_draw_info_format(NDI_UNIQUE, 0, op,
	"%s lives again!",playername);

  return 1;
}
	    

void dead_character(char *name) {
  char buf[MAX_BUF];
  char buf2[MAX_BUF];

  sprintf(buf,"%s/%s/%s/%s.pl",settings.localdir,settings.playerdir,name, name);
  /*  peterm:  create a .dead filename....  ***.pl.dead  */
  strcpy(buf2,buf);
  strcat(buf,".dead");
  if(rename(buf2,buf)== -1){
    LOG(llevDebug, "Crossfire character rename to dead: %s --", buf);
    perror("rename()");
  }
}


int dead_player_exists(char *name) {
  char buf[MAX_BUF];

  sprintf(buf,"%s/%s/%s/%s",settings.localdir,settings.playerdir,name, name);
  strcat(buf,".pl.dead");
  return !(access(buf,0));
}


