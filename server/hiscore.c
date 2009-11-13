/*
 * static char *rcsid_hiscore_c =
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
 * @file
 * Hiscore handling functions.
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/**
 * The score structure is used when treating new high-scores.
 */

typedef struct scr {
    char name[BIG_NAME];      /**< Name.  */
    char title[BIG_NAME];     /**< Title. */
    char killer[BIG_NAME];    /**< Name (+ title) or "left". */
    sint64 exp;               /**< Experience. */
    char maplevel[BIG_NAME];  /**< Killed on what level. */
    int maxhp, maxsp, maxgrace; /**< Max hp, sp, grace when killed. */
    int position;             /**< Position in the highscore list. */
} score;

/**
 * Does what it says, copies the contents of the first score structure
 * to the second one.
 *
 * @param sc1
 * what to copy.
 * @param sc2
 * where to copy to.
 */
static void copy_score(const score *sc1, score *sc2) {
    strncpy(sc2->name, sc1->name, BIG_NAME);
    sc2->name[BIG_NAME-1] = '\0';
    strncpy(sc2->title, sc1->title, BIG_NAME);
    sc2->title[BIG_NAME-1] = '\0';
    strncpy(sc2->killer, sc1->killer, BIG_NAME);
    sc2->killer[BIG_NAME-1] = '\0';
    sc2->exp = sc1->exp;
    strcpy(sc2->maplevel, sc1->maplevel);
    sc2->maxhp = sc1->maxhp;
    sc2->maxsp = sc1->maxsp;
    sc2->maxgrace = sc1->maxgrace;
}

/**
 * Writes the given score structure to specified buffer.
 *
 * @param sc
 * score to write.
 * @param buf
 * buffer to write to.
 * @param size
 * buf's size.
 */
static void put_score(const score *sc, char *buf, size_t size) {
    snprintf(buf, size, "%s:%s:%"FMT64":%s:%s:%d:%d:%d", sc->name, sc->title, sc->exp, sc->killer, sc->maplevel, sc->maxhp, sc->maxsp, sc->maxgrace);
}

/**
 * The opposite of put_score(), get_score reads from the given buffer into
 * a static score structure, and returns a pointer to it.
 *
 * @param bp
 * string to parse.
 * @return
 * parsed score.
 * @todo make thread-safe, remove static stuff.
 */

static score *get_score(char *bp) {
    static score sc;
    char *cp;
    char *tmp[8];

    if ((cp = strchr(bp, '\n')) != NULL)
        *cp = '\0';

    if (split_string(bp, tmp, 8) != 8)
        return NULL;

    strncpy(sc.name, tmp[0], BIG_NAME);
    sc.name[BIG_NAME-1] = '\0';

    strncpy(sc.title, tmp[1], BIG_NAME);
    sc.title[BIG_NAME-1] = '\0';

    sscanf(tmp[2], "%"FMT64, &sc.exp);

    strncpy(sc.killer, tmp[3], BIG_NAME);
    sc.killer[BIG_NAME-1] = '\0';

    strncpy(sc.maplevel, tmp[4], BIG_NAME);
    sc.maplevel[BIG_NAME-1] = '\0';

    sscanf(tmp[5], "%d", &sc.maxhp);

    sscanf(tmp[6], "%d", &sc.maxsp);

    sscanf(tmp[7], "%d", &sc.maxgrace);
    return &sc;
}

/**
 * Formats one score to display to a player.
 *
 * @param sc
 * score to format.
 * @param buf
 * buffer to write to. Will contain suitably formatted score.
 * @param size
 * length of buf.
 * @return
 * buf.
 */
static char *draw_one_high_score(const score *sc, char *buf, size_t size) {
    if (!strncmp(sc->killer, "quit", MAX_NAME))
        snprintf(buf, size, "[Fixed]%3d %10"FMT64"[Print] %s %s quit the game on map %s <%d><%d><%d>.",
                 sc->position, sc->exp, sc->name, sc->title, sc->maplevel, sc->maxhp, sc->maxsp, sc->maxgrace);
    else if (!strncmp(sc->killer, "left", MAX_NAME))
        snprintf(buf, size, "[Fixed]%3d %10"FMT64"[Print] %s %s left the game on map %s <%d><%d><%d>.",
                 sc->position, sc->exp, sc->name, sc->title, sc->maplevel, sc->maxhp, sc->maxsp, sc->maxgrace);
    else
        snprintf(buf, size, "[Fixed]%3d %10"FMT64"[Print] %s %s was killed by %s on map %s <%d><%d><%d>.",
                 sc->position, sc->exp, sc->name, sc->title, sc->killer, sc->maplevel, sc->maxhp, sc->maxsp, sc->maxgrace);
    return buf;
}

/**
 * Adds the given score-structure to the high-score list, but
 * only if it was good enough to deserve a place.
 *
 * @param new_score
 * score to add.
 * @return
 * old player score.
 * @todo remove static buffer.
 */
static score *add_score(score *new_score) {
    FILE *fp;
    static score old_score;
    score *tmp_score, pscore[HIGHSCORE_LENGTH];
    char buf[MAX_BUF], filename[MAX_BUF], bp[MAX_BUF];
    int nrofscores = 0, flag = 0, i, comp;

    new_score->position = HIGHSCORE_LENGTH+1;
    old_score.position = -1;
    snprintf(filename, sizeof(filename), "%s/%s", settings.localdir, HIGHSCORE);
    if ((fp = open_and_uncompress(filename, 1, &comp)) != NULL) {
        while (fgets(buf, MAX_BUF, fp) != NULL && nrofscores < HIGHSCORE_LENGTH) {
            if ((tmp_score = get_score(buf)) == NULL) {
                LOG(llevError, "Corrupt highscore file %s\n", filename);
                break;
            }
            if (!flag && new_score->exp >= tmp_score->exp) {
                copy_score(new_score, &pscore[nrofscores]);
                new_score->position = nrofscores;
                flag = 1;
                if (++nrofscores >= HIGHSCORE_LENGTH)
                    break;
            }
            if (!strcmp(new_score->name, tmp_score->name)) { /* Another entry */
                copy_score(tmp_score, &old_score);
                old_score.position = nrofscores;
                if (flag)
                    continue;
            }
            copy_score(tmp_score, &pscore[nrofscores++]);
        }
        close_and_delete(fp, comp);
    }
    if (old_score.position != -1 && old_score.exp >= new_score->exp)
        return &old_score; /* Did not beat old score */
    if (!flag && nrofscores < HIGHSCORE_LENGTH)
        copy_score(new_score, &pscore[nrofscores++]);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot write to highscore file %s: %s\n", filename, strerror_local(errno, buf, sizeof(buf)));
        return NULL;
    }
    for (i = 0; i < nrofscores; i++) {
        put_score(&pscore[i], bp, sizeof(bp));
        fprintf(fp, "%s\n", bp);
    }
    fclose(fp);
    if (flag) {
        /* Eneq(@csd.uu.se): Patch to fix error in adding a new score to the
           hiscore-list */
        if (old_score.position == -1)
            return new_score;
        return &old_score;
    }
    new_score->position = -1;
    if (old_score.position != -1)
        return &old_score;
    if (nrofscores) {
        copy_score(&pscore[nrofscores-1], &old_score);
        return &old_score;
    }
    LOG(llevError, "Highscore error.\n");
    return NULL;
}

/**
 * Checks if player should enter the hiscore, and if so writes her into the list.
 *
 * @param op
 * player to check.
 * @param quiet
 * If set, don't print anything out - used for periodic updates during game
 * play or when player unexpected quits - don't need to print anything
 * in those cases
 */
void check_score(object *op, int quiet) {
    score new_score;
    score *old_score;
    char bufscore[MAX_BUF];

    if (op->stats.exp == 0)
        return;

    if (!op->contr->name_changed) {
        if (op->stats.exp > 0) {
            if (!quiet)
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                              "As you haven't changed your name, you won't "
                              "get into the high-score list.", NULL);
        }
        return;
    }
    if (QUERY_FLAG(op, FLAG_WAS_WIZ)) {
        if (!quiet)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "Since you have been in wizard mode, "
                          "you can't enter the high-score list.", NULL);
        return;
    }
    if (op->contr->explore) {
        if (!quiet)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "Since you were in explore mode, "
                          "you can't enter the high-score list.", NULL);
        return;
    }
    if (!op->stats.exp) {
        if (!quiet)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                          "You don't deserve to save your character yet.", NULL);
        return;
    }

    strncpy(new_score.name, op->name, BIG_NAME);
    new_score.name[BIG_NAME-1] = '\0';
    player_get_title(op->contr, new_score.title, sizeof(new_score.title));
    strncpy(new_score.killer, op->contr->killer, BIG_NAME);
    if (new_score.killer[0] == '\0')
        strcpy(new_score.killer, "a dungeon collapse");
    new_score.killer[BIG_NAME-1] = '\0';
    new_score.exp = op->stats.exp;
    if (op->map == NULL)
        *new_score.maplevel = '\0';
    else {
        strncpy(new_score.maplevel, op->map->name ? op->map->name : op->map->path, BIG_NAME-1);
        new_score.maplevel[BIG_NAME-1] = '\0';
    }
    new_score.maxhp = (int)op->stats.maxhp;
    new_score.maxsp = (int)op->stats.maxsp;
    new_score.maxgrace = (int)op->stats.maxgrace;
    if ((old_score = add_score(&new_score)) == NULL) {
        if (!quiet)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "Error in the highscore list.", NULL);
        return;
    }
    /* Everything below here is just related to print messages
     * to the player.  If quiet is set, we can just return
     * now.
     */
    if (quiet)
        return;

    if (new_score.position == -1) {
        new_score.position = HIGHSCORE_LENGTH+1; /* Not strictly correct... */

        if (!strcmp(old_score->name, new_score.name))
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                          "You didn't beat your last highscore:", NULL);
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                          "You didn't enter the highscore list:", NULL);

        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                      draw_one_high_score(old_score, bufscore, sizeof(bufscore)), NULL);

        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                      draw_one_high_score(&new_score, bufscore, sizeof(bufscore)), NULL);
        return;
    }
    if (old_score->exp >= new_score.exp)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                      "You didn't beat your last score:", NULL);
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                      "You beat your last score:", NULL);

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                  draw_one_high_score(old_score, bufscore, sizeof(bufscore)), NULL);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                         draw_one_high_score(&new_score, bufscore, sizeof(bufscore)), NULL);
}



/**
 * Displays the high score file.
 *
 * @param op
 * player asking for the score file.
 * @param max
 * maximum number of scores to display.
 * @param match
 * if set, will only print players with name or title containing the string (non case-sensitive).
 */
void display_high_score(object *op, int max, const char *match) {
    FILE *fp;
    char buf[MAX_BUF], scorebuf[MAX_BUF];
    int i = 0, j = 0, comp;
    score *sc;

    snprintf(buf, sizeof(buf), "%s/%s", settings.localdir, HIGHSCORE);
    if ((fp = open_and_uncompress(buf, 0, &comp)) == NULL) {
        char err[MAX_BUF];

        LOG(llevError, "Cannot open highscore file %s: %s\n", buf, strerror_local(errno, err, sizeof(err)));
        if (op != NULL)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "There is no highscore file.", NULL);
        return;
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                  "[Fixed]Nr    Score    Who <max hp><max sp><max grace>",
                  "Nr    Score    Who <max hp><max sp><max grace>");

    while (fgets(buf, MAX_BUF, fp) != NULL) {
        if (j >= HIGHSCORE_LENGTH || i >= max)
            break;
        if ((sc = get_score(buf)) == NULL)
            break;
        sc->position = ++j;
        if (match != NULL
        && !strcasestr_local(sc->name, match)
        && !strcasestr_local(sc->title, match))
            continue;

        draw_one_high_score(sc, scorebuf, sizeof(scorebuf));
        i++;

        if (op == NULL)
            LOG(llevDebug, "%s\n", scorebuf);
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE, scorebuf, NULL);
    }
    close_and_delete(fp, comp);
}
