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


/* All the variable names.  Each entry should correspond to the 
 * V_ vlaue in loader.h (that is, the 20'th value in this array should
 * correspond to the 20'th V_(value) in loader.h)  The V_??? get
 * used as indexes into this array.
 */

/* If you change this list (add/delete/rename), also update the list
 * in crossedit/Attr.c, around line 360.  That list is the variables
 * that can be set in crossedit for various objects.
 */

#define YY_DECL int rmap_lex_read()

static char *rmap_yval();

static int rmap_lex_error;


#define IVAL	atoi(rmap_yval())
#define FVAL	atof(rmap_yval())

%}



S	[ \t]+.+
WS	[ \t]*

%x MESSAGE

/* Don't have to link with -lfl with this */
%option noyywrap

/* need yy_push_state, yy_pop_state */
%option stack

%%

%{
/* Declare some local variables */

    rmap_lex_error=0;

%}




^wallstyle{S}	    strcpy(wallstyle,rmap_yval());
^floorstyle{S}	    strcpy(floorstyle,rmap_yval());
^monsterstyle{S}    strcpy(monsterstyle,rmap_yval());
^treasurestyle{S}   strcpy(treasurestyle,rmap_yval());
^layoutstyle{S}	    strcpy(layoutstyle,rmap_yval());
^doorstyle{S}	    strcpy(doorstyle,rmap_yval());
^decorstyle{S}	    strcpy(decorstyle,rmap_yval());
^xsize{S}	    Xsize = IVAL;
^ysize{S}	    Ysize = IVAL;
^layoutoptions1{S}  layoutoptions1 = IVAL;
^layoutoptions2{S}  layoutoptions2 = IVAL;
^layoutoptions3{S}  layoutoptions3 = IVAL;
^symmetry{S}        symmetry = IVAL;
^difficulty{S}        difficulty = IVAL;
^decoroptions{S}      decoroptions = IVAL;
^exitstyle{S}       strcpy(exitstyle,rmap_yval());
^dungeon_level{S}   dungeon_level = IVAL;
^dungeon_depth{S}   dungeon_depth = IVAL;
^final_map{S}       strcpy(final_map,rmap_yval());
^orientation{S}     orientation = IVAL;
^origin_x{S}        origin_x = IVAL;
^origin_y{S}        origin_y = IVAL;
^origin_map{S}       strcpy(origin_map,rmap_yval());
^random_seed{S}        random_seed = IVAL;
^treasureoptions{S}    treasureoptions = IVAL;
^generate_treasure_now{S} generate_treasure_now = IVAL;


<*>(^{WS}$)|\n		{/* ignore empty lines, newlines we don't do above */}
#.*\n			{}

<<EOF>>			{/* If we got an error, return the error.  Otherwise, return that we got EOF */
			    if (rmap_lex_error!=0) return rmap_lex_error; else return LL_EOF;}
.*			{ yyerror( "Unrecognized string"); rmap_lex_error= -1; }
%%

/*int yyerror(char *s)
{
  return -1;
}
*/

/* Our save file syntax is very simple, so we can use a very simple
 * processing mechanism here instead using something like bison
 * This skips over the space and returns the value, or "" if no value
 * is found.
 */
static char *rmap_yval()
{
    static char *em="";
    char *cp;

    cp=strchr(yytext,' ');
    if (cp) return cp+1;
    else return em;
}




int load_parameters(FILE *fp, int bufstate) {
    int retval;
    char inbuf[MAX_BUF];

    if (bufstate==LO_NEWFILE || bufstate==LO_NOREAD) {
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE));
        if (bufstate==LO_NOREAD) return LL_NORMAL;
    } 
    if (bufstate==LO_LINEMODE) {
        YY_BUFFER_STATE  yybufstate;
        while (fgets(inbuf, MAX_BUF-3, fp)) {
            yybufstate=yy_scan_string(inbuf);
            retval=rmap_lex_read();
            yy_delete_buffer(yybufstate);
            if (retval==LL_NORMAL) return retval;
        }
        return LL_EOF;
    }

    retval=rmap_lex_read();
/*    LOG(llevDebug," load completed, object=%s\n",op->name);*/
    return retval;
}

