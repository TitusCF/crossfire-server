/*
 * static char *rcsid_standalone_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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

#define LO_NEWFILE 2

/* the main routine for making a standalone version. */

#include <time.h>
#include <stdio.h>
#include <global.h>
#include <maze_gen.h>
#include <room_gen.h>
#include <random_map.h>
#include <rproto.h>
#include <sproto.h>

int main(int argc, char *argv[]) {
    char InFileName[1024], OutFileName[1024];
    mapstruct *newMap;
    RMParms rp;
    FILE *fp;

    if (argc < 3) {
        printf("\nUsage:  %s inputfile outputfile\n", argv[0]);
        exit(0);
    }

    strcpy(InFileName, argv[1]);
    strcpy(OutFileName, argv[2]);

    init_globals();
    init_library();
    init_archetypes();
    init_artifacts();
    init_formulae();
    init_readable();

    init_gods();
    memset(&rp, 0, sizeof(RMParms));
    rp.Xsize = -1;
    rp.Ysize = -1;
    if ((fp = fopen(InFileName, "r")) == NULL) {
        fprintf(stderr, "\nError: can not open %s\n", InFileName);
        exit(1);
    }
    load_parameters(fp, LO_NEWFILE, &rp);
    fclose(fp);
    newMap = generate_random_map(OutFileName, &rp, NULL);
    save_map(newMap, SAVE_MODE_INPLACE);
    exit(0);
}

void set_map_timeout(mapstruct *oldmap) {
    /* doesn't need to do anything */
}

#include <global.h>

/* some plagarized code from apply.c--I needed just these two functions
without all the rest of the junk, so.... */
int apply_auto(object *op) {
    object *tmp = NULL;
    int i;

    switch (op->type) {
    case SHOP_FLOOR:
        if (!HAS_RANDOM_ITEMS(op))
            return 0;
        do {
            i = 10; /* let's give it 10 tries */
            while ((tmp = generate_treasure(op->randomitems, op->stats.exp ? op->stats.exp : 5)) == NULL && --i)
                ;
            if (tmp == NULL)
                return 0;
            if (QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
                object_free_drop_inventory(tmp);
                tmp = NULL;
            }
        } while (!tmp);

        SET_FLAG(tmp, FLAG_UNPAID);
        object_insert_in_map_at(tmp, op->map, NULL, 0, op->x, op->y);
        CLEAR_FLAG(op, FLAG_AUTO_APPLY);
        tmp = identify(tmp);
        break;

    case TREASURE:
        if (HAS_RANDOM_ITEMS(op))
            while ((op->stats.hp--) > 0)
                create_treasure(op->randomitems, op, GT_ENVIRONMENT, op->stats.exp ? op->stats.exp : op->map == NULL ? 14 : op->map->difficulty, 0);
        object_remove(op);
        object_free_drop_inventory(op);
        break;
    }

    return tmp ? 1 : 0;
}

/* apply_auto_fix goes through the entire map (only the first time
 * when an original map is loaded) and performs special actions for
 * certain objects (most initialization of chests and creation of
 * treasures and stuff).  Calls apply_auto if appropriate.
 */
void apply_auto_fix(mapstruct *m) {
    int x, y;

    for (x = 0; x < MAP_WIDTH(m); x++)
        for (y = 0; y < MAP_HEIGHT(m); y++)
            FOR_MAP_PREPARE(m, x, y, tmp) {
                if (QUERY_FLAG(tmp, FLAG_AUTO_APPLY))
                    apply_auto(tmp);
                else if (tmp->type == TREASURE) {
                    if (HAS_RANDOM_ITEMS(tmp))
                        while ((tmp->stats.hp--) > 0)
                            create_treasure(tmp->randomitems, tmp, 0, m->difficulty, 0);
                }
                if (tmp && tmp->arch
                && tmp->type != PLAYER
                && tmp->type != TREASURE
                && tmp->randomitems) {
                    if (tmp->type == CONTAINER) {
                        if (HAS_RANDOM_ITEMS(tmp))
                            while ((tmp->stats.hp--) > 0)
                                create_treasure(tmp->randomitems, tmp, 0, m->difficulty, 0);
                    } else if (HAS_RANDOM_ITEMS(tmp))
                        create_treasure(tmp->randomitems, tmp, 0, m->difficulty, 0);
                }
            } FOR_MAP_FINISH();
    for (x = 0; x < MAP_WIDTH(m); x++)
        for (y = 0; y < MAP_HEIGHT(m); y++)
            FOR_MAP_PREPARE(m, x, y, tmp) {
                if (tmp->above
                && (tmp->type == TRIGGER_BUTTON || tmp->type == TRIGGER_PEDESTAL))
                    check_trigger(tmp, tmp->above);
            } FOR_MAP_FINISH();
}

/**
 * Those are dummy functions defined to resolve all symboles.
 * Added as part of glue cleaning.
 * Ryo 2005-07-15
 **/
void draw_ext_info(int flags, int pri, const object *pl, uint8 type, uint8 subtype, const char *message) {
    fprintf(logfile, "%s\n", message);
}

void draw_ext_info_format(int flags, int pri, const object *pl, uint8 type, uint8 subtype, const char *format, ...) {
    va_list ap;

    va_start(ap, format);
    vfprintf(logfile, format, ap);
    va_end(ap);
}


void ext_info_map(int color, const mapstruct *map, uint8 type, uint8 subtype, const char *str1) {
    fprintf(logfile, "ext_info_map: %s\n", str1);
}

void move_firewall(object *ob) {
}

void emergency_save(int x) {
}

void clean_tmp_files(void) {
}

void esrv_send_item(object *ob, object *obx) {
}

void esrv_update_item(int flags, object *pl, object *op) {
}

void dragon_ability_gain(object *ob, int x, int y) {
}

void set_darkness_map(mapstruct *m) {
}

object *find_skill_by_number(object *who, int skillno) {
    return NULL;
}

void esrv_del_item(player *pl, object *ob) {
}

void esrv_update_spells(player *pl) {
}

void rod_adjust(object *rod) {
}

int execute_event(object *op, int eventcode, object *activator, object *third, const char *message, int fix) {
    return 0;
}

int execute_global_event(int eventcode, ...) {
    return 0;
}
