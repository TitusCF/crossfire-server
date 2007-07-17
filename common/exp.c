/*
 * static char *rcsid_arch_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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

/**
 * @file exp.c
 * Experience management. reading data from files and such.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <global.h>

sint64 *levels; /**< Number of levels for which we have experience. */

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

/**
 * Alternative way to calculate experience based
 * on the ability of a monster.
 *
 * It's far from perfect, and doesn't consider everything which
 * can be considered, thus it's only used in debugging.
 * this is only used with one of the dumpflags,
 * and not anyplace in the code.
 *
 * @param ob
 * object for which to return experience
 * @return
 * experience computed from object's properties.
 */
uint64 new_exp(const object *ob) {
    double att_mult, prot_mult, spec_mult;
    double exp;
    int i;
    long mask = 1;

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
    spec_mult += (0.3*(QUERY_FLAG(ob,FLAG_SEE_INVISIBLE)!= FALSE)) +
        (0.5*(QUERY_FLAG(ob,FLAG_SPLITTING)!= FALSE))+
        (0.3*(QUERY_FLAG(ob,FLAG_HITBACK)!= FALSE)) +
        (0.1*(QUERY_FLAG(ob,FLAG_REFL_MISSILE)!= FALSE)) +
        (0.3*(QUERY_FLAG(ob,FLAG_REFL_SPELL)!= FALSE)) +
        (1.0*(QUERY_FLAG(ob,FLAG_NO_MAGIC)!= FALSE)) +
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

/**
 * Checks whether object has innate abilities (spell/spellbook in inventory).
 * @return
 * 1 if monster has any innate abilities, 0 else
 */
int has_ability(const object *ob) {
    object *tmp;

    for(tmp=ob->inv;tmp!=NULL;tmp=tmp->below)
        if(tmp->type==SPELL||tmp->type==SPELLBOOK)
            return TRUE;
    return FALSE;
}

/**
 * This loads the experience table from the exp_table
 * file.  This tends to exit on any errors, since it
 * populates the table as it goes along, so if there
 * are errors, the table is likely in an inconsistent
 * state.
 *
 * @note
 * will call exit() if file is invalid.
  @todo should probably call fatal() instead of exit()
 */
void init_experience(void)
{
    char buf[MAX_BUF], *cp;
    int lastlevel=0, comp;
    sint64 lastexp=-1, tmpexp;
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
        if (!strncasecmp(cp, "max_level",9)) {
            if (settings.max_level) {
                LOG(llevDebug, "Got more than one max_level value from exp_table file?\n");
                free(levels);
            }
            settings.max_level = atoi(cp+9);
            if (!settings.max_level) {
                LOG(llevDebug, "Got invalid max_level from exp_table file? %s\n", buf);
            } else {
                levels = calloc(settings.max_level +1, sizeof(sint64));
            }
        }
        while (isdigit(*cp) && *cp!=0) {
            if (!settings.max_level) {
                LOG(llevError, "max_level is not set in exp_table file.  Did you remember to update it?\n");
                exit(1);
            }

            tmpexp = atoll(cp);
	        /* Do some sanity checking - if value is bogus, just exit because
             * the table otherwise is probably in an inconsistent state
             */
            if (tmpexp <= lastexp) {
#ifndef WIN32
                LOG(llevError,"Experience for level %d is lower than previous level (%lld <= %lld)\n",
                    lastlevel + 1, tmpexp, lastexp);
#else
                LOG(llevError,"Experience for level %d is lower than previous level (%I64d <= %I64d)\n",
                    lastlevel + 1, tmpexp, lastexp);
#endif
                exit(1);
            }
            lastlevel++;
            if (lastlevel > settings.max_level) {
                LOG(llevError,"Too many levels specified in table (%d > %d)\n",
                    lastlevel, settings.max_level);
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
    if (lastlevel != settings.max_level && lastlevel != 0) {
        LOG(llevError,"Warning: exp_table does not have %d entries (%d)\n",
            settings.max_level, lastlevel);
        exit(1);
    }
}

/**
 * Dump the experience table, then calls exit() - useful in terms of debugging to make sure the
 * format of the exp_table is correct.
 */
void dump_experience(void)
{
    int i;

    for (i=1; i<= settings.max_level; i++) {
        fprintf(logfile,"%4d %20"  FMT64  "\n", i, levels[i]);
    }
    exit(0);
}

/**
 * Frees experience-related memory.
 */
void free_experience(void) {
    FREE_AND_CLEAR(levels);
}
