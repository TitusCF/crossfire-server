/*
 * static char *rcsid_arch_c =
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
#include <stdio.h>
#include <global.h>

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

float exp_imm_mult[NROFATTACKS+2] = {
	0.5,				/* AT_PHYSICAL	*/
	1.0,				/* AT_MAGIC	*/
	0.2,				/* AT_FIRE	*/
	0.2,				/* AT_ELECTRICITY */
	0.2,				/* AT_COLD	*/
	0.2,				/* AT_WATER	*/
	0.2,				/* AT_ACID	*/
	0.2,				/* AT_DRAIN	*/
	0.2,				/* AT_WEAPONMAGIC */
	0.2,				/* AT_GHOSTHIT	*/
	0.2,				/* AT_POISON	*/
	0.2,				/* AT_DISEASE	*/
	0.2,				/* AT_PARALYZE	*/
	0.2,				/* AT_TURN_UNDEAD */
	0.2,				/* AT_FEAR	*/
	0.0,				/* AT_CANCELLATION */
	0.0,				/* AT_DEPLETE */
	0.0,				/* AT_DEATH */
	0.0,				/* AT_CHAOS */
	0.0				/* AT_COUNTERSPELL */

};

float exp_vuln_mult[NROFATTACKS+2] = {
	0.8,				/* AT_PHYSICAL	*/
	0.8,				/* AT_MAGIC	*/
	0.9,				/* AT_FIRE	*/
	0.9,				/* AT_ELECTRICITY */
	0.9,				/* AT_COLD	*/
	0.99,				/* AT_WATER	*/
	0.9,				/* AT_ACID	*/
	0.9,				/* AT_DRAIN	*/
	0.9,				/* AT_WEAPONMAGIC */
	0.9,				/* AT_GHOSTHIT	*/
	0.9,				/* AT_POISON	*/
	0.9,				/* AT_DISEASE	*/
	0.9,				/* AT_PARALYZE	*/
	0.9,				/* AT_TURN_UNDEAD */
	0.9,				/* AT_FEAR	*/
	0.0,				/* AT_CANCELLATION */
	0.0,				/* AT_DEPLETE */
	0.0,				/* AT_DEATH */
	0.0,				/* AT_CHAOS */
	0.0				/* AT_COUNTERSPELL */

};

/*
 * nex_exp() is an alternative way to calculate experience based
 * on the ability of a monster.
 * It's far from perfect, and doesn't consider everything which
 * can be considered, thus it's only used in debugging.
 */
	
int new_exp(object *ob) {
  double	att_mult, prot_mult, imm_mult, vuln_mult, spec_mult;
  double	exp;
  int		i;
  long		mask = 1;

  att_mult = prot_mult = imm_mult = vuln_mult = spec_mult = 1.0;
  for(i=0;i<NROFATTACKS+2;i++) {
    att_mult += (exp_att_mult[i] * ((ob->attacktype&mask) != FALSE));
    prot_mult += (exp_prot_mult[i] * ((ob->protected&mask) != FALSE));
    imm_mult += (exp_imm_mult[i] * ((ob->immune&mask) != FALSE));
    vuln_mult *= (ob->vulnerable&mask) ? exp_vuln_mult[i] : 1.0;
    mask <<= 1;
  }
  spec_mult += 	(0.3*(QUERY_FLAG(ob,FLAG_SEE_INVISIBLE)!= FALSE)) +
  		(0.5*(QUERY_FLAG(ob,FLAG_SPLITTING)!= FALSE))+
		(0.3*(QUERY_FLAG(ob,FLAG_HITBACK)!= FALSE)) + 
		(0.1*(QUERY_FLAG(ob,FLAG_REFL_MISSILE)!= FALSE)) +
		(0.3*(QUERY_FLAG(ob,FLAG_REFL_SPELL)!= FALSE)) +
		(1.0*(QUERY_FLAG(ob,FLAG_NO_MAGIC)!= FALSE)) +
		(0.1*(QUERY_FLAG(ob,FLAG_PICK_UP)!= FALSE)) + 
		(0.1*(QUERY_FLAG(ob,FLAG_USE_SCROLL)!= FALSE)) +
		(0.2*(QUERY_FLAG(ob,FLAG_USE_WAND)!= FALSE)) +
		(0.1*(QUERY_FLAG(ob,FLAG_USE_BOW)!= FALSE)) +
		(0.1*(ob->can_apply != FALSE));
  exp = (ob->stats.maxhp<5) ? 5 : ob->stats.maxhp;
  exp *= (QUERY_FLAG(ob,FLAG_CAST_SPELL) && has_ability(ob)) 
  	 ? (40+(ob->stats.maxsp>80?80:ob->stats.maxsp))/40 : 1;
  exp *= (80.0/(70.0+ob->stats.wc)) * (80.0/(70.0+ob->stats.ac)) * (50.0+ob->stats.dam)/50.0;
  exp *= att_mult * prot_mult * imm_mult * vuln_mult * spec_mult;
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
