/*
 * static char *rcsid_encounter_c
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

#include <global.h>
#include <ctype.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

#ifdef RANDOM_ENCOUNTERS

struct encounter_alt {
  int num;
  char *path;
  struct encounter_alt *next;
};

static struct encounter_alt *encounter_alternatives = NULL;

/*
 * Read the directory to find the number of available terrain maps,
 * remember values in encounter_alternatives.
 * 
 * Beware! "path" is assumed to be a shared string!
 */
int find_encounter_alternatives(const char *lib, char *path) {
  struct encounter_alt *enc = encounter_alternatives;

  while (enc) {
    if (enc->path == path) {
      return enc->num;
    }
    enc = enc->next;
  }

  {
    DIR *dp;
    struct dirent *direntry;
    char buf[MAX_BUF], *basename;
    int baselen;

    enc = malloc(sizeof(struct encounter_alt));
    /* add_refcount() is declared wrongly, but fixing it is a lot
     * work. Therefore we must live with a warning on the next line.
     */
    enc->path = add_refcount(path);
    enc->next = encounter_alternatives;
    enc->num = 0;

    sprintf(buf, "%s/%s", lib, path);

    basename = strrchr(buf, '/');
    *basename++ = 0;
    baselen = strlen(basename);
  
    dirp = xopendir(buf);
    if (dirp == NULL) {
      LOG(llevError, "Can't open terrain map directory (%s)\n", buf);
      free(enc);
      return 0;
    }
    while ((direntry = readdir(dirp, 0))!=NULL) {
      /* The length of the filename under consideration should be at least
       * two characters longer than basename, namely "_1"
       */
      if (baselen + 2 <= NAMLEN(direntry)
         && !strncmp(basename, direntry->d_name, baselen)
         && direntry->d_name[baselen] == '_'
         && isdigit(direntry->d_name[baselen+1])) {
       enc->num += 1;
      }
    }
    closedir(dirp);
    encounter_alternatives = enc;

    return enc->num;
  }
}
  
static int encounter_count = 0;

void random_encounter(object *pl, object *rnd_ob) {
  char buf[MAX_BUF], buf2[MAX_BUF];
  mapstruct *m, *m2;
  object *exit_ob, *tmp, *op, *next;
  object *mapobs[3][3];
  int x,y,i;

  if (pl->type != PLAYER)
    return;

  if (pl->map->encounter)
    return; /* No encounters within encounters */
  if (rnd_ob->head != NULL)
    rnd_ob = rnd_ob->head;

  if (rnd_ob->slaying) {
    enter_exit(pl, rnd_ob);
    return;
  }
  if (RANDOM()%4)
    return;

  /*
   * Check if its possible to create an encounter map here.
   */
  for (x = (-1); x < 2; x++)
    for (y = (-1); y < 2; y++)
      if (out_of_map(pl->map, rnd_ob->x + x, rnd_ob->y + y) ||
          (mapobs[x+1][y+1] = get_map_ob(pl->map, x + rnd_ob->x, y + rnd_ob->y))
           == (object *) NULL ||
          mapobs[x+1][y+1]->type != ENCOUNTER ||
          mapobs[x+1][y+1]->slaying != (char *) NULL ||
          mapobs[x+1][y+1]->race == (char *) NULL ||
          (mapobs[x+1][y+1]->above != NULL &&
	   !QUERY_FLAG(mapobs[x+1][y+1]->above, FLAG_ALIVE)) ||
          (mapobs[x+1][y+1]->below != NULL &&
	   !QUERY_FLAG(mapobs[x+1][y+1]->below,FLAG_ALIVE))) return;

  sprintf(buf, "/encounter#%d", encounter_count++);
  m = ready_map_name("/terrain/empty", 0);
  m->encounter = 1;
  strcpy(m->path, buf);
#if 0
  if (m->map_object->name)
    free_string(m->map_object->name);
  m->map_object->name = add_string(buf);
#endif
  rnd_ob->slaying = add_refcount(m->map_object->name);

  /*
   * Read the terrains from the different squares of the map
   * which we are generating.  Then relink their objects onto
   * the new map.
   * Also, in the same loop, make the eight surrounding
   * terrain objects into exits which leads to the correct
   * spot on the map.
   */
  for (x = 0; x < 3; x++)
    for (y= 0; y < 3; y++)
    {
      int r;

      mapobs[x][y]->stats.hp = x*11 + 6;
      mapobs[x][y]->stats.sp = y*11 + 6;
      mapobs[x][y]->slaying = add_string(buf);
      sprintf(buf2, "%s/%s", settings.libdir, settings.mapdir);
      r = find_encounter_alternatives(buf2, mapobs[x][y]->race);
      sprintf(buf2, "%s_%ld", mapobs[x][y]->race, (long)RANDOM()%r+1);
      m2 = load_original_map(buf2, MAP_BLOCK); /* 1 so that the game blocks */
      if (m2 == (mapstruct *) NULL)
      {
        LOG(llevError, "Failed to read map %s\n", buf2);
	CLEAR_FLAG(rnd_ob, FLAG_WALK_ON);
        rnd_ob->type = 0;
        return;
      }
      relink_objs_offset(m2, m, x * 11, y * 11);
      free_map(m2, 1);
      delete_map(m2);
    }

  exit_ob = get_archetype("invis_exit");
  if (exit_ob == (object *) NULL)
  {
    LOG(llevError, "Couldn't find archetype invis_exit.\n");
    free_object(exit_ob);
    return;
  }
 /* We want dimension door to stop before it */
  SET_FLAG(exit_ob, FLAG_NO_MAGIC);
  exit_ob->slaying = add_string(rnd_ob->map->path);

  /*
   * Put exits around the edge of the map, and make them point
   * to the correct square outside.
   */
  for (x = 0; x < 3; x++)
    for (y = 0; y < 3; y++)
    {
      exit_ob->stats.hp = rnd_ob->x + x - 1;
      exit_ob->stats.sp = rnd_ob->y + y - 1;
      for (i = 0; i < 11; i++)
      {
        if (x != 1) {
          tmp = get_object();
          copy_object(exit_ob, tmp);
          if (x == 0)
            tmp->x = 2, tmp->y = i + y*11;
          else
            tmp->x = 31, tmp->y = i + y*11;
          insert_ob_in_map(tmp, m);
        }
        if (y != 1) {
          tmp = get_object();
          copy_object(exit_ob, tmp);
          if (y == 0)
            tmp->y = 2, tmp->x = i + x*11;
          else
            tmp->y = 31, tmp->x = i + x*11;
          insert_ob_in_map(tmp, m);
        }
      }
    }

  /*
   * To avoid some bugs, make the edges impassable.
   */
  for (i = 0; i < 33; i++)
  {
    if ((op = get_map_ob(m, i,  0)) != (object *) NULL)
      SET_FLAG(op, FLAG_NO_PASS);
    if ((op = get_map_ob(m, i, 32)) != (object *) NULL)
      SET_FLAG(op, FLAG_NO_PASS);
    if ((op = get_map_ob(m,  0, i)) != (object *) NULL)
      SET_FLAG(op, FLAG_NO_PASS);
    if ((op = get_map_ob(m, 32, i)) != (object *) NULL)
      SET_FLAG(op, FLAG_NO_PASS);
  }

  /*
   * Clear the centre of the map for monsters.
   */
  for (x = 11; x < 22; x++)
    for (y = 11; y < 22; y++)
      for (op = get_map_ob(m, x, y); op; op = next)
      {
        next = op->above;
        if (QUERY_FLAG(op,FLAG_MONSTER)) {
          remove_ob(op);
          free_object(op);
        }
      }

  free_object(exit_ob);

  /*
   * Now generate the treasure within the monsters
   */
  fix_auto_apply(m);

  /*
   * Go for it!
   */
  enter_exit(pl, rnd_ob);
}
#endif
