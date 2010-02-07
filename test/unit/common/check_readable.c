/*
 * static char *rcsid_check_readable_c =
 *   "$Id$";
 */

/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2002 Mark Wedel & Crossfire Development Team
 * Copyright (C) 1992 Frank Tore Johansen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * The authors can be reached via e-mail at crossfire-devel@real-time.com
 */

/*
 * This is the unit tests file for common/readable.c
 */

#include <stdlib.h>
#include <check.h>
#include <global.h>

void setup(void) {
    cctk_setdatadir(BUILD_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/readable.out");
/*    printf("set log to %s\n", LOGDIR"/unit/common/readable.out");*/
    cctk_init_std_archetypes();
    init_gods();
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

static char *old_god_info_msg(int level, char *retbuf, size_t booksize) {
    const char *name;
    char buf[BOOK_BUF];
    int i;
    size_t retlen, buflen;
    size_t introlen;
    const object *god = pntr_to_god_obj(get_rand_god());
    char en[BOOK_BUF];

    if (booksize > BOOK_BUF) {
        LOG(llevError, "common/readable.c:god_info_msg() - passed in booksize (%lu) is larger than book buffer (%d)\n", (unsigned long)booksize, BOOK_BUF);
        booksize = BOOK_BUF;
    }

    if (!god)
        return (char *)NULL; /* oops, problems... */
    name = god->name;

    /* preamble.. */
    snprintf(retbuf, BOOK_BUF, "This document contains knowledge concerning the diety %s", name);

    retlen = strlen(retbuf);

    /* Always have as default information the god's descriptive terms. */
    if (nstrtok(god->msg, ",") > 0) {
        safe_strcat(retbuf, ", known as", &retlen, BOOK_BUF);
        safe_strcat(retbuf, strtoktolin(god->msg, ",", buf, sizeof(buf)), &retlen, BOOK_BUF);
    } else
        safe_strcat(retbuf, "...", &retlen, BOOK_BUF);

        safe_strcat(retbuf, "\n ---\n", &retlen, BOOK_BUF);

        introlen = retlen; /* so we will know if no new info is added later */

    /* Information about the god is random, and based on the level of the
        * 'book'. This goes through this loop 'level' times, reducing level by
        * 1 each time.  If the info provided is filled up, we exit the loop.
        * otherwise, buf is appended to the existing book buffer.
    */

        while (level > 0) {
            buf[0]=' ';
            buf[1]='\0';
            if (level == 2 && RANDOM()%2) {
                /* enemy god */

                if (god->title)
                    snprintf(buf, BOOK_BUF, "The gods %s and %s are enemies.\n ---\n", name, god->title);
            }
            if (level == 3 && RANDOM()%2) {
                /* enemy race, what the god's holy word effects */
                const char *enemy = god->slaying;

                if (enemy && !(god->path_denied&PATH_TURNING)
                    && (i = nstrtok(enemy, ",")) > 0) {
                    char tmpbuf[MAX_BUF];

                    snprintf(buf, BOOK_BUF, "The holy words of %s have the power to slay creatures belonging to the ", name);
                    if (i > 1)
                        snprintf(tmpbuf, MAX_BUF, "following races:%s\n ---\n", strtoktolin(enemy, ",", en, sizeof(en)));
                    else
                        snprintf(tmpbuf, MAX_BUF, "race of%s\n ---\n", strtoktolin(enemy, ",", en, sizeof(en)));

                    buflen = strlen(buf);
                    safe_strcat(buf, tmpbuf, &buflen, BOOK_BUF);
                    }
            }
            if (level == 4 && RANDOM()%2) {
                /* Priest of god gets these protect,vulnerable... */

                char cp[BOOK_BUF];
                describe_resistance(god, 1, cp, BOOK_BUF);

                if (*cp) {  /* This god does have protections */
                    snprintf(buf, BOOK_BUF, "%s has a potent aura which is extended to faithful priests. The effects of this aura include:\n%s\n ---\n", name, cp);
                }
            }
            if (level == 5 && RANDOM()%2) {
                /* aligned race, summoning */
                const char *race = god->race; /* aligned race */

                if (race && !(god->path_denied&PATH_SUMMON)) {
                    i = nstrtok(race, ",");
                    if (i > 0) {
                        char tmpbuf[MAX_BUF];

                        snprintf(buf, BOOK_BUF, "Creatures sacred to %s include the\n", name);

                        if (i > 1)
                            snprintf(tmpbuf, MAX_BUF, "following races:%s\n ---\n", strtoktolin(race, ",", en, sizeof(en)));
                        else
                            snprintf(tmpbuf, MAX_BUF, "race of %s\n ---\n", strtoktolin(race, ",", en, sizeof(en)));

                        buflen = strlen(buf);
                        safe_strcat(buf, tmpbuf, &buflen, BOOK_BUF);
                    }
                }
            }
            if (level == 6 && RANDOM()%2) {
                /* blessing,curse properties of the god */

                char cp[MAX_BUF];
                describe_resistance(god, 1, cp, MAX_BUF);

                if (*cp) {  /* This god does have protections */
                    snprintf(buf, MAX_BUF, "\nThe priests of %s are known to be able to bestow a blessing which makes the recipient %s\n ---\n", name, cp);
                }
            }
            if (level == 8 && RANDOM()%2) {
                /* immunity, holy possession */
                int has_effect = 0, tmpvar;
                char tmpbuf[MAX_BUF];

                snprintf(buf, MAX_BUF, "\nThe priests of %s are known to make cast a mighty prayer of possession which gives the recipient", name);

                for (tmpvar = 0; tmpvar < NROFATTACKS; tmpvar++) {
                    if (god->resist[tmpvar] == 100) {
                        has_effect = 1;
                        snprintf(tmpbuf, MAX_BUF, "Immunity to %s", attacktype_desc[tmpvar]);
                    }
                }
                if (has_effect) {
                    buflen = strlen(buf);
                    safe_strcat(buf, tmpbuf, &buflen, BOOK_BUF);
                    safe_strcat(buf, "\n ---\n", &buflen, BOOK_BUF);
                } else {
                    buf[0]=' ';
                    buf[1]='\0';
                }
            }
            if (level == 12 && RANDOM()%2) {
                /* spell paths */
                int has_effect = 0;

                snprintf(buf, MAX_BUF, "It is rarely known fact that the priests of %s are mystically transformed. Effects of this include:\n", name);
                buflen = strlen(buf);

                if (god->path_attuned) {
                    has_effect = 1;
                    DESCRIBE_PATH_SAFE(buf, god->path_attuned, "Attuned", &buflen, BOOK_BUF);
                }
                if (god->path_repelled) {
                    has_effect = 1;
                    DESCRIBE_PATH_SAFE(buf, god->path_repelled, "Repelled", &buflen, BOOK_BUF);
                }
                if (god->path_denied) {
                    has_effect = 1;
                    DESCRIBE_PATH_SAFE(buf, god->path_denied, "Denied", &buflen, BOOK_BUF);
                }
                if (has_effect) {
                    safe_strcat(buf, "\n ---\n", &buflen, BOOK_BUF);
                } else {
                    buf[0]=' ';
                    buf[1]='\0';
                }
            }

        /* check to be sure new buffer size dont exceed either
            * the maximum buffer size, or the 'natural' size of the
            * book...
        */
            if (book_overflow(retbuf, buf, booksize))
                break;
            if (strlen(buf) > 1)
                safe_strcat(retbuf, buf, &retlen, BOOK_BUF);

            level--;
        }
        if (retlen == introlen) {
            /* we got no information beyond the preamble! */
            safe_strcat(retbuf, " Unfortunately the rest of the information is hopelessly garbled!\n ---\n", &retlen, BOOK_BUF);
        }
#ifdef BOOK_MSG_DEBUG
        LOG(llevDebug, "\n god_info_msg() created strng: %d\n", strlen(retbuf));
        fprintf(logfile, " MADE THIS:\n%s", retbuf);
#endif
        return retbuf;
}


START_TEST(test_god_info_msg_rewrite) {
    /* todo: write */
    char old[HUGE_BUF], new[HUGE_BUF];
    int todo = 10000, seed, what;

    while (todo-- > 0) {
        old[0] = '\0';
        new[0] = '\0';
        seed = RANDOM();
        SRANDOM(seed);
        old_god_info_msg(RANDOM() % 100, old, sizeof(old));
        SRANDOM(seed);
        god_info_msg(RANDOM() % 100, new, sizeof(new));
        if (strcmp(old, new)) {
            int match = 0;
            while (old[match] == new[match] && old[match] != '\0') {
                match++;
            }
            if (old[match] == '\0')
                fail_unless(1 == 0, "test %d new longer than old, extra %s\n", todo, new + match);
            fail_unless(1 == 0, "test %d wrong data ***\n%s\n********************\n%s\n\n *** %s => %s\n", todo, new, old, new + match, old + match);
        }
    }

}
END_TEST

Suite *readable_suite(void) {
    Suite *s = suite_create("readable");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_god_info_msg_rewrite);

    return s;
}

int main(void) {
    int nf;
    Suite *s = readable_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/common/readable.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/readable.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
