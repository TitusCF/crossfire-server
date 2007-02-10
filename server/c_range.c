/*
 * static char *rcsid_c_range_c =
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

/* This file deals with range related commands (casting, shooting,
 * throwing, etc.
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <skills.h>
#include <newclient.h>
#include <commands.h>


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

/* Shows all spells that op knows.  If params is supplied, the must match
 * that.  Given there is more than one skill, we can't supply break
 * them down to cleric/wizardry.
 */
static void show_matching_spells(object *op, char *params)
{
    object *spell;
    char    spell_sort[NROFREALSPELLS][MAX_BUF], tmp[MAX_BUF], *cp;
    int	    num_found=0, i;

    /* We go and see what spells the player has.  We put them
     * into the spell_sort array so that we can sort them -
     * we prefix the skill in the name so that the sorting
     * works better.
     */
    for (spell=op->inv; spell!=NULL; spell=spell->below) {
	/* If it is a spell, and no params are passed, or they
	 * match the name, process this spell.
	 */
	if (spell->type == SPELL &&
	    (!params || !strncmp(params, spell->name, strlen(params)))) {
	    if (spell->path_attuned & op->path_denied) {
		snprintf(spell_sort[num_found++], sizeof(spell_sort[0]),
			"%s:%-22s %3s %3s", spell->skill?spell->skill:"generic",
			spell->name, "den", "den");
	    } else {
		snprintf(spell_sort[num_found++], sizeof(spell_sort[0]),
			"%s:%-22s %3d %3d", spell->skill?spell->skill:"generic",
			spell->name, spell->level,
			SP_level_spellpoint_cost(op,spell, SPELL_HIGHEST));
	    }
	}
    }
    if (!num_found) {
	/* If a matching string was passed along, now try it without that
	 * string.  It is odd to do something like 'cast trans',
	 * and it say you have no spells, when really, you do, but just
	 * nothing that matches.
	 */
	if (params)
	    show_matching_spells(op, NULL);
	else
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  "You know no spells", NULL);
    } else {
	/* Note in the code below that we make some
	 * presumptions that there will be a colon in the
	 * string.  given the code above, this is always
	 * the case.
	 */
	qsort(spell_sort, num_found, MAX_BUF, (int (*)(const void*, const void*))strcmp);
	strcpy(tmp,"asdfg");	/* Dummy string so initial compare fails */
	for (i=0; i<num_found; i++) {
	    /* Different skill name, so print banner */
	    if (strncmp(tmp, spell_sort[i], strlen(tmp))) {
		strcpy(tmp, spell_sort[i]);
		cp = strchr(tmp, ':');
		*cp = '\0';

		draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
				     "\n[fixed]%s spells %.*s [lvl] [sp]",
				     "\n%s spells %.*s [lvl] [sp]",
				     tmp, 12-strlen(tmp), "              ");
	    }
	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
				 "[fixed]%s",
				 "%s",
				 strchr(spell_sort[i], ':') + 1);
	}
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
    char *cp;
    object *spob;

    if (command=='i') castnow = 1;

    /* Remove control of the golem */
    if(op->contr->ranges[range_golem]!=NULL) {
	if (op->contr->golem_count == op->contr->ranges[range_golem]->count) {
	    remove_friendly_object(op->contr->ranges[range_golem]);
	    remove_ob(op->contr->ranges[range_golem]);
	    free_object(op->contr->ranges[range_golem]);
	}
        op->contr->ranges[range_golem]=NULL;
	op->contr->golem_count = 0;
    }

    if(params!=NULL) {
	tag_t spellnumber = 0;
	if ((spellnumber = atoi(params))!=0)
	    for (spob = op->inv; spob && spob->count != spellnumber; spob=spob->below);
	else spob = lookup_spell_by_name(op, params);

	if (spob && spob->type == SPELL) {
	    /* Now grab any extra data, if there is any.  Forward pass
	     * any 'of' delimiter
	     */
	    if (spellnumber) {
		/* if we passed a number, the options start at the second word */
		cp = strchr(params, ' ');
		if (cp) {
		    cp++;
		    if (!strncmp(cp, "of ", 3)) cp+=3;
		}
	    }
	    else if (strlen(params) > strlen(spob->name)) {
		cp = params + strlen(spob->name);
		*cp = 0;
		cp++;
		if (!strncmp(cp, "of ", 3)) cp+=3;
	    } else
		cp = NULL;

	    if (spob->skill && !find_skill_by_name(op, spob->skill)) {
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
		    "You need the skill %s to cast %s!",
		    "You need the skill %s to cast %s!",
		    spob->skill, spob->name);
		return 1;
	    }

	    if (castnow) {
		cast_spell(op,op,op->facing,spob,cp);
	    } else {
		op->contr->ranges[range_magic] = spob;
		op->contr->shoottype = range_magic;
		if(cp != NULL) {
		  strncpy(op->contr->spellparam, cp, MAX_BUF);
		  op->contr->spellparam[MAX_BUF-1] = '\0';
		} else {
		  op->contr->spellparam[0] = '\0';
		}
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
				     "You ready the spell %s",
				     "You ready the spell %s",
				     spob->name);
	    }
	    return 0;
	} /* else fall through to below and print spells */
    } /* params supplied */

    /* We get here if cast was given without options or we could not find
     * the requested spell.  List all the spells the player knows.
     */
    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		  "Cast what spell?  Choose one of:", NULL);
    show_matching_spells(op, params);
    return 1;
}

/**************************************************************************/

/* Returns TRUE if the range specified (int r) is legal - that is,
 * the character has an item that is equipped for that range type.
 * return 0 if there is no item of that range type that is usable.
 * This function could probably be simplified, eg, everything
 * should index into the ranges[] array.
 */

int legal_range(object *op,int r) {

    switch(r) {
	case range_none: /* "Nothing" is always legal */
	    return 1;
	case range_bow:
	case range_misc:
	case range_magic: /* cast spells */
	    if (op->contr->ranges[r]) return 1;
	    else return 0;

	case range_golem: /* Use scrolls */
	    if (op->contr->ranges[range_golem] &&
		op->contr->ranges[range_golem]->count == op->contr->golem_count)
		    return 1;
	    else
		return 0;

	case range_skill:
	    if (op->chosen_skill)
		return 1;
	    else
		return 0;
    }
    /* No match above, must not be valid */
    return 0;
}

void change_spell(object *op,char k) {

    char name[MAX_BUF];

    do {
	op->contr->shoottype += ((k == '+') ? 1 : -1);
	if(op->contr->shoottype >= range_size)
	    op->contr->shoottype = range_none;
	else if (op->contr->shoottype <= range_bottom)
	    op->contr->shoottype = (rangetype)(range_size-1);
    } while (!legal_range(op,op->contr->shoottype));

    /* Legal range has already checked that we have an appropriate item
     * that uses the slot, so we don't need to be too careful about
     * checking the status of the object.
     */
    switch(op->contr->shoottype) {
	case range_none:
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  "No ranged attack chosen.", NULL);
	    break;

	case range_golem:
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			  "You regain control of your golem.", NULL);
	    break;

	case range_bow:
        query_name(op->contr->ranges[range_bow], name, MAX_BUF);
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
			 "Switched to %s and %s.",
			 "Switched to %s and %s.",
			 name,
			 op->contr->ranges[range_bow]->race ? op->contr->ranges[range_bow]->race : "nothing");
	    break;

	case range_magic:
	    draw_ext_info_format(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
				 "Switched to spells (%s).",
				 "Switched to spells (%s).",
				 op->contr->ranges[range_magic]->name);
	    break;

	case range_misc:
        query_base_name(op->contr->ranges[range_misc], 0, name, MAX_BUF);
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
				 "Switched to %s.",
				 "Switched to %s.",
				 name);
	    break;

	case range_skill:
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
				 "Switched to skill: %s",
				 "Switched to skill: %s",
				 op->chosen_skill ? op->chosen_skill->name : "none");
	    break;

	default:
	    break;
    }
}


int command_rotateshoottype (object *op, char *params)
{
  if (!params)
      change_spell(op,'+');
  else
    change_spell(op, params[0]);
  return 0;
}

