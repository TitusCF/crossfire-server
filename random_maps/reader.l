%{
/*
 * static char *reader_l =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to mark@pyramid.com
*/

#include <global.h>
#include <loader.h>
#include <newserver.h>
#include <random_map.h>

#define YY_DECL int rmap_lex_read(RMParms *RP)

static char *rmap_yval(void);
static int rmap_lex_read(RMParms *RP);

static int rmap_lex_error;

#define IVAL atoi(rmap_yval())
#define FVAL atof(rmap_yval())

/**
 * Error handler.
 *
 * @param s
 * lex-generated error message.
 */
static int yyerror(const char *s)
{
    LOG(llevError, "%s: %s\n", s, yytext);
    return -1;
}

%}

S       [ \t]+.+
WS      [ \t]*

%x MESSAGE

/* Don't have to link with -lfl with this */
%option noyywrap

/* those functions are unused, not sure of compatibility with Flex versions. */
%option nounput
%option noyy_top_state
%option noinput

%%

%{
/* Declare some local variables */

    rmap_lex_error = 0;

%}

^wallstyle{S}            strncpy(RP->wallstyle, rmap_yval(), RM_SIZE);
^floorstyle{S}           strncpy(RP->floorstyle, rmap_yval(), RM_SIZE);
^monsterstyle{S}         strncpy(RP->monsterstyle, rmap_yval(), RM_SIZE);
^treasurestyle{S}        strncpy(RP->treasurestyle, rmap_yval(), RM_SIZE);
^layoutstyle{S}          strncpy(RP->layoutstyle, rmap_yval(), RM_SIZE);
^doorstyle{S}            strncpy(RP->doorstyle, rmap_yval(), RM_SIZE);
^decorstyle{S}           strncpy(RP->decorstyle, rmap_yval(), RM_SIZE);
^cheststyle{S}           strncpy(RP->cheststyle, rmap_yval(), RM_SIZE);
^xsize{S}                RP->Xsize = IVAL;
^ysize{S}                RP->Ysize = IVAL;
^expand2x{S}             RP->expand2x = IVAL;
^layoutoptions1{S}       RP->layoutoptions1 = IVAL;
^layoutoptions2{S}       RP->layoutoptions2 = IVAL;
^symmetry{S}             RP->symmetry = IVAL;
^difficulty{S}           RP->difficulty = IVAL;
^difficulty_increase{S}  RP->difficulty_increase = FVAL;
^decoroptions{S}         RP->decoroptions = IVAL;
^exitstyle{S}            strncpy(RP->exitstyle, rmap_yval(), RM_SIZE);
^dungeon_level{S}        RP->dungeon_level = IVAL;
^dungeon_name{S}         strncpy(RP->dungeon_name, rmap_yval(), RM_SIZE);
^dungeon_depth{S}        RP->dungeon_depth = IVAL;
^final_map{S}            strncpy(RP->final_map, rmap_yval(), RM_SIZE);
^final_exit_archetype{S} strncpy(RP->final_exit_archetype, rmap_yval(), RM_SIZE);
^orientation{S}          RP-> orientation = IVAL;
^origin_x{S}             RP->origin_x = IVAL;
^origin_y{S}             RP-> origin_y = IVAL;
^origin_map{S}           strncpy(RP->origin_map, rmap_yval(), RM_SIZE);
^random_seed{S}          RP->random_seed = IVAL;
^treasureoptions{S}      RP->treasureoptions = IVAL;
^exit_on_final_map{S}    strncpy(RP->exit_on_final_map, rmap_yval(), RM_SIZE);
^multiple_floors{S}      RP->multiple_floors = IVAL;
^darkness{S}             RP->darkness = IVAL;
^outdoor{S}              RP->outdoor = IVAL;

<*>(^{WS}$)|\n           {/* ignore empty lines, newlines we don't do above */}
#.*\n                    { }

<<EOF>>                  {/* If we got an error, return the error.  Otherwise, return that we got EOF */
                             if (rmap_lex_error != 0)
                                 return rmap_lex_error;
                             else
                                 return LL_EOF;
                         }
.*                       {
                             yyerror("Unrecognized string");
                             rmap_lex_error= -1;
                         }
%%

/* Our save file syntax is very simple, so we can use a very simple
 * processing mechanism here instead using something like bison
 * This skips over the space and returns the value, or "" if no value
 * is found.
 */
static char *rmap_yval(void) {
    static char em[] = "";
    char *cp;

    cp = strchr(yytext, ' ');
    if (cp)
        return cp+1;
    else
        return em;
}

int load_parameters(FILE *fp, int bufstate, RMParms *RP) {
    int retval;
    char inbuf[MAX_BUF];

    if (bufstate == LO_NEWFILE || bufstate == LO_NOREAD) {
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE));
        if (bufstate == LO_NOREAD)
            return LL_NORMAL;
    }
    if (bufstate == LO_LINEMODE) {
        YY_BUFFER_STATE yybufstate;
        while (fgets(inbuf, MAX_BUF-3, fp)) {
            yybufstate = yy_scan_string(inbuf);
            retval = rmap_lex_read(RP);
            yy_delete_buffer(yybufstate);
            if (retval == LL_NORMAL)
                return retval;
        }
        return LL_EOF;
    }

    retval = rmap_lex_read(RP);
/*    LOG(llevDebug, " load completed, object=%s\n", op->name);*/
    return retval;
}


/* This takes a buffer, scans it for variables, and sets those variables
 * as appropriate in op.
 *
 * This function appears to be used in only 2 places - in crossedit to
 * override values and in c_wiz to mutate values.
 */
int set_random_map_variable(RMParms *rp, const char *buf) {
    YY_BUFFER_STATE yybufstate;
    int retval;

    yybufstate = yy_scan_string(buf);
    retval = rmap_lex_read(rp);
    yy_delete_buffer(yybufstate);
    return retval;
}
