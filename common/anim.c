/*
 * static char *rcsid_anim_c =
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

/* This file contains animation related code. */

#include <global.h>
#include <stdio.h>

void free_all_anim() {
    int i;

    for (i=0; i<=num_animations; i++) {
	free_string(animations[i].name);
	free(animations[i].faces);
    }
    free(animations);
}

void init_anim() {
    char buf[MAX_BUF];
    FILE *fp;
    static int anim_init=0;
    int num_frames=0,faces[MAX_ANIMATIONS],i;

    if (anim_init) return;
    animations_allocated=9;
    num_animations=0;
    /* Make a default.  New animations start at one, so if something
     * thinks it is animated but hasn't set the animation_id properly,
     * it will have a default value that should be pretty obvious.
     */
    animations = malloc(10*sizeof(Animations));
    /* set the name so we don't try to dereferance null.
     * Put # at start so it will be first in alphabetical
     * order.
     */
    animations[0].name=add_string("###none");
    animations[0].num_animations=1;
    animations[0].faces = malloc(sizeof(Fontindex));
    animations[0].faces[0]=0;
    animations[0].facings=0;

    sprintf(buf,"%s/animations", settings.datadir);
    LOG(llevDebug,"Reading animations from %s...", buf);
    if ((fp=fopen(buf,"r")) ==NULL) {
	perror("Can not open animations file\n");
	fprintf(stderr,"Filename=%s\n", buf);
	exit(-1);
    }
    while (fgets(buf, MAX_BUF-1, fp)!=NULL) {
	if (*buf=='#') continue;
	/* Kill the newline */
	buf[strlen(buf)-1] = '\0';
	if (!strncmp(buf,"anim ", 5)) {
	    if (num_frames) {
		LOG(llevError,"Didn't get a mina before %s\n", buf);
		num_frames=0;
	    }
	    num_animations++;
	    if (num_animations==animations_allocated) {
		animations=realloc(animations, sizeof(Animations)*(animations_allocated+10));
		animations_allocated+=10;
		}
	    animations[num_animations].name = add_string(buf+5);
	    animations[num_animations].num = num_animations;	/* for bsearch */
	    animations[num_animations].facings = 1;
	}
	else if (!strncmp(buf,"mina",4)) {
	    animations[num_animations].faces = malloc(sizeof(Fontindex)*num_frames);
	    for (i=0; i<num_frames; i++)
		animations[num_animations].faces[i]=faces[i];
	    animations[num_animations].num_animations = num_frames;
	    if (num_frames % animations[num_animations].facings) {
		LOG(llevDebug,"Animation %s frame numbers (%d) is not a multiple of facings (%d)\n",
		    animations[num_animations].name, num_frames, animations[num_animations].facings);
	    }
	    num_frames=0;
	}
	else if (!strncmp(buf,"facings",7)) {
	    if (!(animations[num_animations].facings = atoi(buf+7))) {
		LOG(llevDebug,"Animation %s has 0 facings, line=%s\n",
		    animations[num_animations].name, buf);
		animations[num_animations].facings=1;
	    }

	} else {
	    if (!(faces[num_frames++] = FindFace(buf,0)))
		LOG(llevDebug,"Could not find face %s for animation %s\n",
		    buf, animations[num_animations].name);
	}
    }
    fclose(fp);
    LOG(llevDebug,"done. got (%d)\n", num_animations);
}

static int anim_compare(Animations *a, Animations *b) {
    return strcmp(a->name, b->name);
}

/* Tries to find the animation id that matches name.  Returns an integer match 
 * 0 if no match found (animation 0 is initialized as the 'bug' face
 */
int find_animation(char *name)
{
    Animations search, *match;

    search.name = name;

    match = (Animations*)bsearch(&search, animations, (num_animations+1), 
		sizeof(Animations), (int (*)())anim_compare);


    if (match) return match->num;
    LOG(llevError,"Unable to find animation %s\n", name);
    return 0;
}

/*
 * animate_object(object) updates the face-variable of an object.
 * If the object is the head of a multi-object, all objects are animated.
 */

void animate_object(object *op) {
    int max_state;  /* Max animation state object should be drawn in */
    int base_state; /* starting index # to draw from */
    int	dir=op->direction;

    if(!op->animation_id || !NUM_ANIMATIONS(op)) {
	LOG(llevError,"Object lacks animation.\n");
	dump_object(op);
	return;
    }
    ++op->state;    /* increase draw state */

    if (op->head) dir=op->head->direction;

    /* If object is turning, then max animation state is half through the
     * animations.  Otherwise, we can use all the animations.
     */
    max_state=NUM_ANIMATIONS(op)/ NUM_FACINGS(op);
    base_state=0;
    /* at least in the older aniamtions that used is_turning, the first half
     * of the animations were left facing, the second half right facing.
     * Note in old the is_turning, it was set so that the animation for a monster
     * was always towards the enemy - now it is whatever direction the monster
     * is facing.
     */
    if (NUM_FACINGS(op)==2) {
	if (dir<5) base_state=0;
	else base_state=NUM_ANIMATIONS(op)/2;
    }
    else if (NUM_FACINGS(op)==4) {
	if (dir==0) base_state=0;
	else base_state = ((dir-1)/2) * (NUM_ANIMATIONS(op)/4); 
    }
    else if (NUM_FACINGS(op)==8) {
	if (dir==0) base_state=0;
	else base_state = (dir-1)*(NUM_ANIMATIONS(op)/4); 
    }

    /* If beyond drawable states, reset */
    if (op->state>=max_state) op->state=0;

    SET_ANIMATION(op, op->state + base_state);

    if(op->face==blank_face)
	op->invisible=1;
#if 1
    /* Anyone know what this code here is for?  MSW 980503 */
    else if(QUERY_FLAG((&op->arch->clone),FLAG_ALIVE)) {
	if(op->face->number==0) {
	    op->invisible=1;
	    CLEAR_FLAG(op, FLAG_ALIVE);
	} else {
	    op->invisible=0;
	    SET_FLAG(op, FLAG_ALIVE);
	}
    }
#endif
    if(op->more)
	animate_object(op->more);
    /* update_object will also recursively update all the pieces.
     * as such, we call it last, and only call it for the head
     * piece, and not for the other tail pieces.
     */
    if (!op->head)
	update_object(op, UP_OBJ_FACE);
}

