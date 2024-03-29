/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Hiscore handling functions.
 */
#define _GNU_SOURCE // strcasestr() is a GNU extension in string.h

#include "global.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "sproto.h"
#include "output_file.h"

/**
 * The score structure is used when treating new high-scores.
 */
typedef struct scr {
    char name[BIG_NAME];      /**< Name.  */
    char title[BIG_NAME];     /**< Title. */
    char killer[BIG_NAME];    /**< Name (+ title) or "left". */
    int64_t exp;               /**< Experience. */
    char maplevel[BIG_NAME];  /**< Killed on what level. */
    int maxhp,      /**< Max hp when killed. */
        maxsp,      /**< Max sp when killed. */
        maxgrace;   /**< Max grace when killed. */
    int position;   /**< Position in the highscore list. */
} score;

/**
 * A highscore table.
 */
typedef struct {
    char fname[MAX_BUF];      /**< Filename of the backing file. */
    char skill_name[MAX_BUF]; /**< The name of the skill or "Overall". */
    score entry[HIGHSCORE_LENGTH]; /**< The entries in decreasing exp order. */
} score_table;

/**
 * The highscore table. Unused entries are set to zero (except for position).
 */
static score_table hiscore_tables[MAX_SKILLS + 1]; // One for each skill, plus one for overall

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
 *
 * @param table
 * the highscore table to save.
 */
static void hiscore_save(const score_table *table) {
    FILE *fp;
    OutputFile of;
    size_t i;
    char buf[MAX_BUF];

    LOG(llevDebug, "Writing highscore files %s\n", table->fname);

    fp = of_open(&of, table->fname);
    if (fp == NULL)
        return;

    for (i = 0; i < HIGHSCORE_LENGTH; i++) {
        if (table->entry[i].name[0] == '\0')
            break;

        put_score(&table->entry[i], buf, sizeof(buf));
        fprintf(fp, "%s\n", buf);
    }
    of_close(&of);
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

    if (split_string(bp, tmp, 8, ':') != 8)
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
    } else if (strcmp(sc->killer,"a dungeon collapse") == 0) {
        s1 = "was last";
        s2 = "seen";
    } else {
        s1 = "was killed by";
        s2 = sc->killer;
    }
    snprintf(buf, size, "[fixed]%3d %10"FMT64"[print] %s%s%s %s %s on map %s <%d><%d><%d>.",
             sc->position, sc->exp, sc->name, sc->title[0]==',' ? "" : " ", sc->title, s1, s2, sc->maplevel, sc->maxhp, sc->maxsp, sc->maxgrace);
    return buf;
}

/**
 * Adds the given score-structure to the high-score list, but
 * only if it was good enough to deserve a place.
 *
 * @param table
 * the highscore table to add to.
 * @param new_score
 * score to add.
 * @param old_score
 * returns the old player score.
 */
static void add_score(score_table *table, score *new_score, score *old_score) {
    size_t i;

    new_score->position = HIGHSCORE_LENGTH+1;
    memset(old_score, 0, sizeof(*old_score));
    old_score->position = -1;

    /* find existing entry by name */
    for (i = 0; i < HIGHSCORE_LENGTH; i++) {
        if (table->entry[i].name[0] == '\0') {
            table->entry[i] = *new_score;
            table->entry[i].position = i+1;
            break;
        }
        if (strcmp(new_score->name, table->entry[i].name) == 0) {
            *old_score = table->entry[i];
            if (table->entry[i].exp <= new_score->exp) {
                table->entry[i] = *new_score;
                table->entry[i].position = i+1;
            }
            break;
        }
    }

    if (i >= HIGHSCORE_LENGTH) {
        /* entry for unknown name */

        if (new_score->exp < table->entry[i-1].exp) {
            /* new exp is less than lowest hiscore entry => drop */
            return;
        }

        /* new exp is not less than lowest hiscore entry => add */
        i--;
        table->entry[i] = *new_score;
        table->entry[i].position = i+1;
    }

    /* move entry to correct position */
    while (i > 0 && new_score->exp >= table->entry[i-1].exp) {
        score tmp;

        tmp = table->entry[i-1];
        table->entry[i-1] = table->entry[i];
        table->entry[i] = tmp;

        table->entry[i-1].position = i;
        table->entry[i].position = i+1;

        i--;
    }

    new_score->position = table->entry[i].position;
    hiscore_save(table);
}

/**
 * Loads the hiscore_table from the highscore file.
 *
 * @param table
 * the highscore table to load.
 */
static void hiscore_load(score_table *table) {
    FILE *fp;
    size_t i;

    i = 0;

    fp = fopen(table->fname, "r");
    if (fp == NULL) {
        if (errno == ENOENT) {
            LOG(llevDebug, "Highscore file %s does not exist\n", table->fname);
        } else {
            LOG(llevError, "Cannot open highscore file %s: %s\n", table->fname, strerror(errno));
        }
    } else {
        LOG(llevDebug, "Reading highscore file %s\n", table->fname);
        while (i < HIGHSCORE_LENGTH) {
            char buf[MAX_BUF];

            if (fgets(buf, MAX_BUF, fp) == NULL) {
                break;
            }

            if (!get_score(buf, &table->entry[i]))
                break;
            table->entry[i].position = i+1;
            i++;
        }

        fclose(fp);
    }

    while (i < HIGHSCORE_LENGTH) {
        memset(&table->entry[i], 0, sizeof(table->entry[i]));
        // This cannot be ++i due the right-to-left association of assignment.
        table->entry[i].position = i + 1;
        i++;
    }
}

/**
 * Initializes the module.
 *
 * @note
 * There is one table per skill, as well as the "Overall" table, each saved in a file
 * in .../var/crossfire/hiscores/[skill_name] or as configured in config.h.
 */
void hiscore_init(void) {
    char dirname[MAX_BUF];

    snprintf(dirname, sizeof(dirname), "%s/%s", settings.localdir, HIGHSCORE_DIR);
#ifdef WIN32
    mkdir(dirname);
#else
    mkdir(dirname,0755);
#endif
    memset(hiscore_tables,0,sizeof(hiscore_tables));
    for (int i =- 1; i < MAX_SKILLS; ++i) {
        const char *name;
        int subtype;

        /*
         * This gets complicated because the skills are defined internally by the subtype in
         * the skill object, but our list of skill names is in the order the skills are
         * initialized in.
         */
        if (i == -1) {
            name = "Overall";
            subtype = 0;
        } else {
            name = skill_names[i];
            if (!name || !*name) continue; // No such skill
            subtype = get_skill_client_code(name) + 1;
        }
        snprintf(hiscore_tables[subtype].fname, sizeof(hiscore_tables[subtype].fname), "%s/%s/%s", settings.localdir, HIGHSCORE_DIR,name);
        for ( char *c = hiscore_tables[subtype].fname; *c; ++c ) {
            if (*c == ' ')
                *c = '_'; /* avoid spaces in file names */
        }
        strncpy(hiscore_tables[subtype].skill_name, name, sizeof(hiscore_tables[subtype].skill_name));
        hiscore_load(&hiscore_tables[subtype]);
    }
    /* Load legacy highscore file if new one was blank */
    if (hiscore_tables[0].entry[0].exp == 0) {
        snprintf(hiscore_tables[0].fname, sizeof(hiscore_tables[0].fname), "%s/%s", settings.localdir, OLD_HIGHSCORE);
        hiscore_load(&hiscore_tables[0]);
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
 */
void hiscore_check(object *op, int quiet) {
    score new_score;
    score old_score;
    char bufscore[MAX_BUF];
    const char *message;

    if (op->stats.exp == 0 || !op->contr->name_changed)
        return;

    if (QUERY_FLAG(op, FLAG_WAS_WIZ)) {
        if (!quiet)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                          "Since you have been in wizard mode, "
                          "you can't enter the high-score list.");
        return;
    }
    if (!op->stats.exp) {
        if (!quiet)
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_APPLY, MSG_TYPE_APPLY_ERROR,
                          "You don't deserve to save your character yet.");
        return;
    }

    strncpy(new_score.name, op->name, BIG_NAME);
    new_score.name[BIG_NAME-1] = '\0';
    player_get_title(op->contr, new_score.title, sizeof(new_score.title));
    strncpy(new_score.killer, op->contr->killer, BIG_NAME);
    if (new_score.killer[0] == '\0') {
        strcpy(new_score.killer, "a dungeon collapse");
    }
    new_score.killer[BIG_NAME-1] = '\0';
    if (op->map == NULL) {
        *new_score.maplevel = '\0';
    } else {
        strncpy(new_score.maplevel, op->map->name ? op->map->name : op->map->path, BIG_NAME-1);
        new_score.maplevel[BIG_NAME-1] = '\0';
    }
    new_score.maxhp = (int)op->stats.maxhp;
    new_score.maxsp = (int)op->stats.maxsp;
    new_score.maxgrace = (int)op->stats.maxgrace;
    FOR_INV_PREPARE(op, tmp) {
        if (tmp->type != SKILL) continue;
        if (!tmp->stats.exp) continue;
        new_score.exp = tmp->stats.exp;
        add_score(&hiscore_tables[get_skill_client_code(tmp->name) + 1], &new_score, &old_score);
#if 0
        if (!quiet && new_score.exp > old_score.exp) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                                 "You improved your rating in %s: %" FMT64, tmp->name, new_score.exp);
            if (old_score.position != -1)
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                              draw_one_high_score(&old_score, bufscore, sizeof(bufscore)));
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                          draw_one_high_score(&new_score, bufscore, sizeof(bufscore)));
        }
#endif
    } FOR_INV_FINISH();
    new_score.exp = op->stats.exp;
    add_score(&hiscore_tables[0], &new_score, &old_score); // overall

    /* Everything below here is just related to print messages
     * to the player.  If quiet is set, we can just return
     * now.
     */
    if (quiet)
        return;

    if (old_score.position == -1) {
        if (new_score.position > HIGHSCORE_LENGTH)
            message = "You didn't enter the highscore list:";
        else
            message = "You entered the highscore list:";
    } else {
        if (new_score.position > HIGHSCORE_LENGTH)
            message = "You left the highscore list:";
        else if (new_score.exp  > old_score.exp)
            message = "You beat your last score:";
        else
            message = "You didn't beat your last score:";
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE, message);
    if (old_score.position != -1)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
            draw_one_high_score(&old_score, bufscore, sizeof(bufscore)));
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
        draw_one_high_score(&new_score, bufscore, sizeof(bufscore)));
}

/**
 * Displays the high score file.
 *
 * @param op
 * player asking for the score file.
 * @param max
 * maximum number of scores to display.
 * @param match
 * if non-empty, will only print players with name or title containing the string (non case-sensitive).
 * Other options: -s:[name] -- show the table for the skill 'name' instead of overall
 *                -s -- show a short list for each skill
 */
void hiscore_display(object *op, int max, const char *match) {
    int printed_entries;
    size_t j;
    int skill_match = 0;
    int skill_min,skill_max;
    int len;

    /* check for per-skill instead of overall report */
    if (strncmp(match, "-s", 2) == 0 ) {
        match += 2;
        if (*match == ':') {
            ++match;
            if (strchr(match,' ')) {
                len = strchr(match, ' ') - match;
            } else {
                len = strlen(match);
            }
            for (int i = 1; i < MAX_SKILLS; ++i) {
                if (strncmp(match,hiscore_tables[i].skill_name, len) == 0) {
                    skill_match = i;
                    break;
                }
            }
            if (!skill_match) {
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                                     "Could not match '%.*s' to a skill", len, match);
                return;
            }
            match += len;
        }
        else {
            skill_match = -1; // flag to show all skills
            if ( max < 100 && max > 10 ) max = 10; // Less output per skill
        }
    }
    while (*match == ' ') ++match;

    skill_min = skill_max = skill_match;
    if (skill_match == -1) {
        skill_min = 1;
        skill_max = MAX_SKILLS;
    }

    /*
     * Check all skills in skill_names[] order (which should be alphabetical)
     */
    for (int s = -1; s <= MAX_SKILLS; ++s) {
        int skill = s + 1;

        if (skill < skill_min || skill > skill_max) continue;

        if (hiscore_tables[skill].skill_name[0] == 0) {
            continue; // No such skill
        }
        if (hiscore_tables[skill].entry[0].exp == 0) {
            continue; // No entries for this skill
        }
        if (skill == 0) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                          "Overall high scores:");
        } else {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                                 "High scores for the skill [color=red]%s[/color]:", hiscore_tables[skill].skill_name);
        }
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE,
                      "[fixed]Rank     Score [print]Who <max hp><max sp><max grace>");

        printed_entries = 0;
        for (j = 0; j < HIGHSCORE_LENGTH && hiscore_tables[skill].entry[j].name[0] != '\0' && printed_entries < max; j++) {
            char scorebuf[MAX_BUF];

            if (*match != '\0'
                && !strcasestr_local(hiscore_tables[skill].entry[j].name, match)
                && !strcasestr_local(hiscore_tables[skill].entry[j].title, match))
                continue;

            draw_one_high_score(&hiscore_tables[skill].entry[j], scorebuf, sizeof(scorebuf));
            printed_entries++;

            if (op == NULL) {
                LOG(llevDebug, "%s\n", scorebuf);
            } else {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_HISCORE, scorebuf);
            }
        }
    }
}
