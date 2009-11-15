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
 * The highscore table. Unused entries are set to zero (except for position).
 */
static score hiscore_table[HIGHSCORE_LENGTH];

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
 * Saves the highscore_table into the highscore file.
 */
static void hiscore_save(void) {
    FILE *fp;
    char fname[MAX_BUF];
    size_t i;
    char buf[MAX_BUF];

    snprintf(fname, sizeof(fname), "%s/%s", settings.localdir, HIGHSCORE);
    LOG(llevDebug, "Writing highscore files %s\n", fname);

    fp = fopen(fname, "w");
    if (fp == NULL) {
        LOG(llevError, "Cannot create highscore file %s: %s\n", fname, strerror_local(errno, buf, sizeof(buf)));
        return;
    }

    for (i = 0; i < HIGHSCORE_LENGTH; i++) {
        if (hiscore_table[i].name[0] == '\0')
            break;

        put_score(&hiscore_table[i], buf, sizeof(buf));
        fprintf(fp, "%s\n", buf);
    }
    if (ferror(fp)) {
        LOG(llevError, "Cannot write to highscore file %s: %s\n", fname, strerror_local(errno, buf, sizeof(buf)));
        fclose(fp);
    } else if (fclose(fp) != 0) {
        LOG(llevError, "Cannot write to highscore file %s: %s\n", fname, strerror_local(errno, buf, sizeof(buf)));
    }
}

/**
 * The opposite of put_score(), get_score reads from the given buffer into
 * a static score structure, and returns a pointer to it.
 *
 * @param bp
 * string to parse.
 * @param sc
 * returns the parsed score.
 * @return
 * whether parsing was successful
 */

static int get_score(char *bp, score *sc) {
    char *cp;
    char *tmp[8];

    cp = strchr(bp, '\n');
    if (cp != NULL)
        *cp = '\0';

    if (split_string(bp, tmp, 8) != 8)
        return 0;

    strncpy(sc->name, tmp[0], BIG_NAME);
    sc->name[BIG_NAME-1] = '\0';

    strncpy(sc->title, tmp[1], BIG_NAME);
    sc->title[BIG_NAME-1] = '\0';

    sscanf(tmp[2], "%"FMT64, &sc->exp);

    strncpy(sc->killer, tmp[3], BIG_NAME);
    sc->killer[BIG_NAME-1] = '\0';

    strncpy(sc->maplevel, tmp[4], BIG_NAME);
    sc->maplevel[BIG_NAME-1] = '\0';

    sscanf(tmp[5], "%d", &sc->maxhp);

    sscanf(tmp[6], "%d", &sc->maxsp);

    sscanf(tmp[7], "%d", &sc->maxgrace);
    return 1;
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
    const char *s1;
    const char *s2;

    if (strcmp(sc->killer, "quit") == 0 || strcmp(sc->killer, "left") == 0) {
        s1 = sc->killer;
        s2 = "the game";
    } else {
        s1 = "was killed by";
        s2 = sc->killer;
    }
    snprintf(buf, size, "[fixed]%3d %10"FMT64"[print] %s %s %s %s on map %s <%d><%d><%d>.",
        sc->position, sc->exp, sc->name, sc->title, s1, s2, sc->maplevel, sc->maxhp, sc->maxsp, sc->maxgrace);
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
    static score old_score;
    size_t i;

    new_score->position = HIGHSCORE_LENGTH+1;
    memset(&old_score, 0, sizeof(old_score));
    old_score.position = -1;

    /* find existing entry by name */
    for (i = 0; i < HIGHSCORE_LENGTH; i++) {
        if (hiscore_table[i].name[0] == '\0') {
            hiscore_table[i] = *new_score;
            hiscore_table[i].position = i+1;
            break;
        }
        if (strcmp(new_score->name, hiscore_table[i].name) == 0) {
            old_score = hiscore_table[i];
            if (hiscore_table[i].exp <= new_score->exp) {
                hiscore_table[i] = *new_score;
                hiscore_table[i].position = i+1;
            }
            break;
        }
    }

    if (i >= HIGHSCORE_LENGTH) {
        /* entry for unknown name */

        if (new_score->exp < hiscore_table[i-1].exp) {
            /* new exp is less than lowest hiscore entry => drop */
            return &old_score;
        }

        /* new exp is not less than lowest hiscore entry => add */
        i--;
        hiscore_table[i] = *new_score;
        hiscore_table[i].position = i+1;
    }

    /* move entry to correct position */
    while (i > 0 && new_score->exp >= hiscore_table[i-1].exp) {
        score tmp;

        tmp = hiscore_table[i-1];
        hiscore_table[i-1] = hiscore_table[i];
        hiscore_table[i] = tmp;

        hiscore_table[i-1].position = i;
        hiscore_table[i].position = i+1;

        i--;
    }

    new_score->position = hiscore_table[i].position;
    hiscore_save();
    return &old_score;
}

/**
 * Initializes the module.
 */
void hiscore_init(void) {
    FILE *fp;
    char fname[MAX_BUF];
    size_t i;

    i = 0;

    snprintf(fname, sizeof(fname), "%s/%s", settings.localdir, HIGHSCORE);
    fp = fopen(fname, "r");
    if (fp == NULL) {
        if (errno == ENOENT) {
            LOG(llevInfo, "Highscore file %s does not exist\n", fname);
        } else {
            char err[MAX_BUF];

            LOG(llevError, "Cannot open highscore file %s: %s\n", fname, strerror_local(errno, err, sizeof(err)));
        }
    } else {
        LOG(llevInfo, "Reading highscore file %s\n", fname);
        while (i < HIGHSCORE_LENGTH) {
            char buf[MAX_BUF];

            if (fgets(buf, MAX_BUF, fp) == NULL) {
                break;
            }

            if (!get_score(buf, &hiscore_table[i]))
                break;
            hiscore_table[i].position = i+1;
            i++;
        }

        fclose(fp);
    }

    while (i < HIGHSCORE_LENGTH) {
        memset(&hiscore_table[i], 0, sizeof(hiscore_table[i]));
        hiscore_table[i].position = i+1;
        i++;
    }
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
 *
 * @note
 * check_score() has been renamed to hiscore_check()
 */
void hiscore_check(object *op, int quiet) {
    score new_score;
    score *old_score;
    char bufscore[MAX_BUF];
    const char *message;

    if (op->stats.exp == 0 || !op->contr->name_changed)
        return;

    if (QUERY_FLAG(op, FLAG_WAS_WIZ)) {
        if (!quiet)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "Since you have been in wizard mode, "
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
    old_score = add_score(&new_score);

    /* Everything below here is just related to print messages
     * to the player.  If quiet is set, we can just return
     * now.
     */
    if (quiet)
        return;

    if (old_score->position == -1) {
        if (new_score.position > HIGHSCORE_LENGTH)
            message = "You didn't enter the highscore list:";
        else
            message = "You entered the highscore list:";
    } else {
        if (new_score.position > HIGHSCORE_LENGTH)
            message = "You left the highscore list:";
        else if (new_score.exp  > old_score->exp)
            message = "You beat your last score:";
        else
            message = "You didn't beat your last score:";
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE, message, NULL);
    if (old_score->position != -1)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
            draw_one_high_score(old_score, bufscore, sizeof(bufscore)), NULL);
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
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
 *
 * @note
 * display_high_score() has been renamed to hiscore_display()
 */
void hiscore_display(object *op, int max, const char *match) {
    int printed_entries;
    size_t j;

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                  "[fixed]Nr    Score   [print] Who <max hp><max sp><max grace>",
                  "Nr    Score    Who <max hp><max sp><max grace>");

    printed_entries = 0;
    for (j = 0; j < HIGHSCORE_LENGTH && hiscore_table[j].name[0] != '\0' && printed_entries < max; j++) {
        char scorebuf[MAX_BUF];

        if (match != NULL
        && !strcasestr_local(hiscore_table[j].name, match)
        && !strcasestr_local(hiscore_table[j].title, match))
            continue;

        draw_one_high_score(&hiscore_table[j], scorebuf, sizeof(scorebuf));
        printed_entries++;

        if (op == NULL)
            LOG(llevDebug, "%s\n", scorebuf);
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE, scorebuf, NULL);
    }
}
