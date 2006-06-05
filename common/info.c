/*
 * static char *rcsid_info_c =
 *    "$Id$";
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

#include <global.h>

/*
 * The functions in this file are purely mean to generate information
 * in differently formatted output, mainly about monsters.
 */

/*
 * Dump to standard out the abilities of all monsters.
 */

void dump_abilities(void) {
  archetype *at;
  for(at = first_archetype; at; at=at->next) {
    char *ch;
    const char *gen_name = "";
    archetype *gen;

    if(!QUERY_FLAG(&at->clone,FLAG_MONSTER))
      continue;

    /* Get rid of e.g. multiple black puddings */
    if (QUERY_FLAG(&at->clone,FLAG_CHANGING))
      continue;

    for (gen = first_archetype; gen; gen = gen->next) {
      if (gen->clone.other_arch && gen->clone.other_arch == at) {
        gen_name = gen->name;
	break;
      }
    }

    ch = describe_item(&at->clone, NULL);
    printf("%-16s|%6" FMT64 "|%4d|%3d|%s|%s|%s\n",at->clone.name,at->clone.stats.exp,
           at->clone.stats.hp,at->clone.stats.ac,ch,at->name,gen_name);
  }
}

/*
 * As dump_abilities(), but with an alternative way of output.
 */

void print_monsters(void) {
    archetype *at;
    object *op;
    char   attbuf[34];
    int i;

    printf("               |     |   |    |    |      attack       |                        resistances                                                                       |\n");
    printf("monster        | hp  |dam| ac | wc |pmf ecw adw gpd ptf|phy mag fir ele cld cfs acd drn wmg ght poi slo par tud fer cnc dep dth chs csp gpw hwd bln int |  exp   | new exp |\n");
    printf("---------------------------------------------------------------------------------------------------------------------------------------------------\n");
    for(at=first_archetype;at!=NULL;at=at->next) {
	op = arch_to_object(at);
	if (QUERY_FLAG(op,FLAG_MONSTER)) {
	    bitstostring((long)op->attacktype, NROFATTACKS, attbuf);
	    printf("%-15s|%5d|%3d|%4d|%4d|%s|",
		   op->arch->name, op->stats.maxhp, op->stats.dam, op->stats.ac,
		   op->stats.wc,attbuf);
	    for (i=0; i<NROFATTACKS; i++)
		printf("%4d", op->resist[i]);
	    printf("|%8" FMT64 "|%9d|\n",op->stats.exp, new_exp(op));
    }
    free_object(op);
  }
}

/*
 * Writes <num> ones and zeros to the given string based on the
 * <bits> variable.
 */

void bitstostring(long bits, int num, char *str)
{
  int   i,j=0;

  if (num > 32)
    num = 32;

  for (i=0;i<num;i++) {
    if (i && (i%3)==0) {
      str[i+j] = ' ';
      j++;
    }
    if (bits&1)
      str[i+j] = '1';
    else
      str[i+j] = '0';
    bits >>= 1;
  }
  str[i+j] = '\0';
  return;
}
