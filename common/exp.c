/*
 * static char *rcsid_arch_c =
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

#include <stdio.h>
#include <global.h>

uint32 levels[MAXLEVEL+1]={
0,
0,2000,4000, 8000,
16000,32000,64000,125000,250000,                /* 9 */
500000,900000,1400000,2000000,2600000,
3300000,4100000,4900000,5700000,6600000,        /* 19 */
7500000,8400000,9300000,10300000,11300000,
12300000,13300000,14400000,15500000,16600000,   /* 29 */
17700000,18800000,19900000,21100000,22300000,
23500000,24700000,25900000,27100000,28300000,   /* 39 */
29500000,30800000,32100000,33400000,34700000,
36000000,37300000,38600000,39900000,41200000,   /* 49 */
42600000,44000000,45400000,46800000,48200000,
49600000,51000000,52400000,53800000,55200000,   /* 59 */
56600000,58000000,59400000,60800000,62200000,
63700000,65200000,66700000,68200000,69700000,   /* 69 */
71200000,72700000,74200000,75700000,77200000,
78700000,80200000,81700000,83200000,84700000,   /* 79 */
86200000,87700000,89300000,90900000,92500000,
94100000,95700000,97300000,98900000,100500000,  /* 89 */
102100000,103700000,105300000,106900000,108500000,
110100000,111700000,113300000,114900000,116500000,      /* 99 */
118100000,119700000,121300000,122900000,124500000,
126100000,127700000,129300000,130900000,785400000,
1570800000      /* 110 */
};


#define TRUE	1
#define FALSE	0

float exp_att_mult[NROFATTACKS+2] = {
	0.0,				/* AT_PHYSICAL	*/
	0.0,				/* AT_MAGIC	*/
	0.0,				/* AT_FIRE	*/
	0.0,				/* AT_ELECTRICITY */
	0.0,				/* AT_COLD	*/
	0.0,				/* AT_WATER	*//*AT_CONFUSION!*/
	0.4,				/* AT_ACID	*/
	1.5,				/* AT_DRAIN	*/
	0.0,				/* AT_WEAPONMAGIC */
	0.1,				/* AT_GHOSTHIT	*/
	0.3,				/* AT_POISON	*/
	0.2,				/* AT_DISEASE	*/
	0.3,				/* AT_PARALYZE	*/
	0.0,				/* AT_TURN_UNDEAD */
	0.0,				/* AT_FEAR	*/
	0.0,				/* AT_CANCELLATION */
	0.0,				/* AT_DEPLETE */
	0.0,				/* AT_DEATH */
	0.0,				/* AT_CHAOS */
	0.0				/* AT_COUNTERSPELL */
};

float exp_prot_mult[NROFATTACKS+2] = {
	0.4,				/* AT_PHYSICAL	*/
	0.5,				/* AT_MAGIC	*/
	0.1,				/* AT_FIRE	*/
	0.1,				/* AT_ELECTRICITY */
	0.1,				/* AT_COLD	*/
	0.1,				/* AT_WATER	*/
	0.1,				/* AT_ACID	*/
	0.1,				/* AT_DRAIN	*/
	0.1,				/* AT_WEAPONMAGIC */
	0.1,				/* AT_GHOSTHIT	*/
	0.1,				/* AT_POISON	*/
	0.1,				/* AT_DISEASE	*/
	0.1,				/* AT_PARALYZE	*/
	0.1,				/* AT_TURN_UNDEAD */
	0.1,				/* AT_FEAR	*/
	0.0,				/* AT_CANCELLATION */
	0.0,				/* AT_DEPLETE */
	0.0,				/* AT_DEATH */
	0.0,				/* AT_CHAOS */
	0.0				/* AT_COUNTERSPELL */

};

/*
 * new_exp() is an alternative way to calculate experience based
 * on the ability of a monster.
 * It's far from perfect, and doesn't consider everything which
 * can be considered, thus it's only used in debugging.
 */
	
int new_exp(object *ob) {
  double	att_mult, prot_mult, spec_mult;
  double	exp;
  int		i;
  long		mask = 1;

  att_mult = prot_mult =spec_mult = 1.0;
  for(i=0;i<NROFATTACKS;i++) {
    mask = 1<<i;
    att_mult += (exp_att_mult[i] * ((ob->attacktype&mask) != FALSE));
    /* We multiply & then divide to prevent roundoffs on the floats.
     * the doubling is to take into account the table and resistances
     * are lower than they once were.
     */
    prot_mult += (exp_prot_mult[i] * 200*ob->resist[i]) / 100.0;
  }
  spec_mult += 	(0.3*(QUERY_FLAG(ob,FLAG_SEE_INVISIBLE)!= FALSE)) +
  		(0.5*(QUERY_FLAG(ob,FLAG_SPLITTING)!= FALSE))+
		(0.3*(QUERY_FLAG(ob,FLAG_HITBACK)!= FALSE)) + 
		(0.1*(QUERY_FLAG(ob,FLAG_REFL_MISSILE)!= FALSE)) +
		(0.3*(QUERY_FLAG(ob,FLAG_REFL_SPELL)!= FALSE)) +
		(1.0*(QUERY_FLAG(ob,FLAG_NO_MAGIC)!= FALSE)) +
		(0.1*(QUERY_FLAG(ob,FLAG_PICK_UP)!= FALSE)) + 
		(0.1*(QUERY_FLAG(ob,FLAG_USE_SCROLL)!= FALSE)) +
		(0.2*(QUERY_FLAG(ob,FLAG_USE_RANGE)!= FALSE)) +
		(0.1*(QUERY_FLAG(ob,FLAG_USE_BOW)!= FALSE));

  exp = (ob->stats.maxhp<5) ? 5 : ob->stats.maxhp;
  exp *= (QUERY_FLAG(ob,FLAG_CAST_SPELL) && has_ability(ob)) 
  	 ? (40+(ob->stats.maxsp>80?80:ob->stats.maxsp))/40 : 1;
  exp *= (80.0/(70.0+ob->stats.wc)) * (80.0/(70.0+ob->stats.ac)) * (50.0+ob->stats.dam)/50.0;
  exp *= att_mult * prot_mult * spec_mult;
  exp *= 2.0/(2.0-((FABS(ob->speed)<0.95)?FABS(ob->speed):0.95));
  exp *= (20.0+ob->stats.Con)/20.0;
  if (QUERY_FLAG(ob, FLAG_STAND_STILL))
    exp /= 2;

  return (int) exp;
}

/*
 * Returns true if the monster specified has any innate abilities.
 */

int has_ability(object *ob) {
  object *tmp;

  for(tmp=ob->inv;tmp!=NULL;tmp=tmp->below)
    if(tmp->type==ABILITY||tmp->type==SPELLBOOK) 
      return TRUE;

  return FALSE;
}

/* This loads the experience table from the exp_table
 * file.  This tends to exit on any errors, since it
 * populates the table as it goes along, so if there
 * are errors, the table is likely in an inconsistent
 * state.
 */
void init_experience()
{
    char buf[MAX_BUF], *cp;
    int lastlevel=0, lastexp=-1, tmpexp, comp;
    FILE *fp;
    

    sprintf(buf,"%s/exp_table",settings.confdir);

    if ((fp = open_and_uncompress(buf, 0, &comp)) == NULL) {
        return;
    }
    while (fgets(buf, MAX_BUF-1, fp) != NULL) {
	if (buf[0] == '#') continue;

	/* eliminate newline */
	if ((cp=strrchr(buf,'\n'))!=NULL) *cp='\0';

	/* Skip over empty lines */
	if (buf[0] == 0) continue;
	cp = buf;
	while (isspace(*cp) && *cp!=0) cp++;
	while (isdigit(*cp) && *cp!=0) {
	    tmpexp = atoi(cp);
	    /* Do some sanity checking - if value is bogus, just exit because
	     * the table otherwise is probably in an inconsistent state
	     */
	    if (tmpexp <= lastexp) {
		LOG(llevError,"Experience for level %d is lower than previous level (%d <= %d)\n",
		    lastlevel + 1, tmpexp, lastexp);
		exit(1);
	    }
	    lastlevel++;
	    if (lastlevel > MAXLEVEL) {
		LOG(llevError,"Too many levels specified in table (%d > %d)\n", 
		    lastlevel, MAXLEVEL);
		exit(1);
	    }
	    levels[lastlevel] = tmpexp;
	    lastexp = tmpexp;
	    /* First, skip over the number we just processed. Then skip over
	     * any spaces, commas, etc.
	     */
	    while (isdigit(*cp) && *cp!=0) cp++;
	    while (!isdigit(*cp) && *cp!=0) cp++;
	}
    }
    close_and_delete(fp, comp);
    if (lastlevel != MAXLEVEL && lastlevel != 0) {
	LOG(llevError,"Warning: exp_table does not have %d entries (%d)\n",
	    MAXLEVEL, lastlevel);
	exit(1);
    }
}

/* Dump the table - useful in terms of debugging to make sure the
 * format of the exp_table is correct.
 */

void dump_experience()
{
    int i;

    for (i=1; i<= MAXLEVEL; i++) {
	fprintf(logfile,"%4d %20d\n", i, levels[i]);
    }
    exit(0);
}
