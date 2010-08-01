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
#include <assert.h>
#include <spells.h>

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

#define arraysize(arrayname) (sizeof(arrayname)/sizeof(*(arrayname)))
/** special structure, used only by art_name_array[] */
typedef struct namebytype {
    const char *name;  /**< generic name to call artifacts of this type */
    int type;          /**< matching type */
} arttypename;

static const arttypename art_name_array[] = {
    { "Helmet", HELMET },
    { "Amulet", AMULET },
    { "Shield", SHIELD },
    { "Bracers", BRACERS },
    { "Boots", BOOTS },
    { "Cloak", CLOAK },
    { "Gloves", GLOVES },
    { "Gridle", GIRDLE },
    { "Ring", RING },
    { "Horn", ROD },
    { "Missile Weapon", BOW },
    { "Missile", ARROW },
    { "Hand Weapon", WEAPON },
    { "Artifact", SKILL },
    { "Food", FOOD },
    { "Body Armour", ARMOUR }
};

static char *old_artifact_msg(int level, char *retbuf, size_t booksize) {
    artifactlist *al;
    artifact *art;
    int chance, i, type, index;
    int book_entries = level > 5 ? RANDOM()%3+RANDOM()%3+2 : RANDOM()%level+1;
    char buf[BOOK_BUF], sbuf[MAX_BUF];
    object *tmp;

    /* values greater than 5 create msg buffers that are too big! */
    if (book_entries > 5)
        book_entries = 5;

    /* lets determine what kind of artifact type randomly.
    * Right now legal artifacts only come from those listed
    * in art_name_array. Also, we check to be sure an artifactlist
    * for that type exists!
    */
    i = 0;
    do {
        index = RANDOM()%arraysize(art_name_array);
        type = art_name_array[index].type;
        al = find_artifactlist(type);
        i++;
    } while (al == NULL && i < 10);

    if (i == 10) { /* Unable to find a message */
        snprintf(retbuf, booksize, "None");
        return retbuf;
    }

    /* There is no reason to start on the artifact list at the beginning. Lets
    * take our starting position randomly... */
    art = al->items;
    for (i = RANDOM()%level+RANDOM()%2+1; i > 0; i--) {
        if (art == NULL)
            art = al->items; /* hmm, out of stuff, loop back around */
        art = art->next;
    }

    /* Ok, lets print out the contents */
    snprintf(retbuf, booksize, "Herein %s detailed %s...\n", book_entries > 1 ? "are" : "is", book_entries > 1 ? "some artifacts" : "an artifact");

    /* artifact msg attributes loop. Lets keep adding entries to the 'book'
    * as long as we have space up to the allowed max # (book_entires)
    */
    while (book_entries > 0) {
        if (art == NULL)
            art = al->items;

        /* separator of items */
        snprintf(buf, sizeof(buf), "---\n");

        /* Name */
        if (art->allowed != NULL && strcmp(art->allowed->name, "All")) {
            archetype *arch;
            linked_char *temp = art->allowed;
            int inv = 0, w;

            assert(art->allowed_size > 0);
            if (art->allowed_size > 1)
                w = 1 + RANDOM() % art->allowed_size;
            else
                w = 1;

            while (w > 1) {
                assert(temp);
                temp = temp->next;
                w--;
            }

            if (temp->name[0] == '!')
                inv = 1;

            /** @todo check archetype when loading archetypes, not here */
            arch = try_find_archetype(temp->name + inv);
            if (!arch)
                arch = find_archetype_by_object_name(temp->name + inv);

            if (!arch)
                LOG(llevError, "artifact_msg: missing archetype %s for artifact %s (type %d)\n", temp->name + inv, art->item->name, art->item->type);
            else {
                if (inv)
                    snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), " A %s (excepted %s) of %s", art_name_array[index].name, arch->clone.name_pl, art->item->name);
                else
                    snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), " A %s of %s", arch->clone.name, art->item->name);
            }
        } else {  /* default name is used */
            /* use the base 'generic' name for our artifact */
            snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), " The %s of %s", art_name_array[index].name, art->item->name);
        }

        /* chance of finding */
        chance = 100*((float)art->chance/al->total_chance);
        if (chance >= 20)
            snprintf(sbuf, sizeof(sbuf), "an uncommon");
        else if (chance >= 10)
            snprintf(sbuf, sizeof(sbuf), "an unusual");
        else if (chance >= 5)
            snprintf(sbuf, sizeof(sbuf), "a rare");
        else
            snprintf(sbuf, sizeof(sbuf), "a very rare");
        snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), " is %s\n", sbuf);

        /* value of artifact */
        snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), " item with a value that is %d times normal.\n",
                 art->item->value);

        /* include the message about the artifact, if exists, and book
        * level is kinda high */
        if (art->item->msg && RANDOM()%4+1 < level
            && !(strlen(art->item->msg)+strlen(buf) > BOOK_BUF))
            snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "%s", art->item->msg);

        /* properties of the artifact */
        tmp = object_new();
        add_abilities(tmp, art->item);
        tmp->type = type;
        SET_FLAG(tmp, FLAG_IDENTIFIED);
        {
            char *desc = stringbuffer_finish(describe_item(tmp, NULL, NULL));
            if (strlen(desc) > 1)
                snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), " Properties of this artifact include:\n %s\n", tmp);
            free(desc);
        }
        object_free_drop_inventory(tmp);
        /* add the buf if it will fit */
        if (book_overflow(retbuf, buf, booksize))
            break;
        snprintf(retbuf+strlen(retbuf), booksize-strlen(retbuf), "%s", buf);

        art = art->next;
        book_entries--;
    }

#ifdef BOOK_MSG_DEBUG
    LOG(llevDebug, "artifact_msg() created strng: %d\n", strlen(retbuf));
    fprintf(logfile, " MADE THIS:\n%s", retbuf);
#endif
    return retbuf;
}

START_TEST(test_artifact_msg_rewrite) {
    char old[HUGE_BUF], new[HUGE_BUF];
    int todo = 10000, seed, what, size;
    const archetype *arch;

    const char* archs[] = { "book_clasp", "book_read", "checkbook", "letter", "note", "quarto", "scroll", "scroll_2", "tome" };
    const int arch_count = 9;
    object *dummy = object_new();

    while (todo-- > 0) {

        what = RANDOM() % arch_count;
        arch = find_archetype(archs[what]);
        fail_unless(arch != NULL, "missing arch %s", archs[what]);
        size = BOOKSIZE(&arch->clone);
        size -= strlen("\n"); /* Keep enough for final \n. */

        old[0] = '\0';
        new[0] = '\0';
        seed = RANDOM();
        SRANDOM(seed);
        old_artifact_msg(1 + RANDOM() % 100, old, size);
        SRANDOM(seed);
        artifact_msg(1 + RANDOM() % 100, new, size);
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

    object_free_drop_inventory(dummy);
}
END_TEST

static const uint32 spellpathdef[NRSPELLPATHS] = {
    PATH_PROT,
    PATH_FIRE,
    PATH_FROST,
    PATH_ELEC,
    PATH_MISSILE,
    PATH_SELF,
    PATH_SUMMON,
    PATH_ABJURE,
    PATH_RESTORE,
    PATH_DETONATE,
    PATH_MIND,
    PATH_CREATE,
    PATH_TELE,
    PATH_INFO,
    PATH_TRANSMUTE,
    PATH_TRANSFER,
    PATH_TURNING,
    PATH_WOUNDING,
    PATH_DEATH,
    PATH_LIGHT
};

static char *old_spellpath_msg(int level, char *retbuf, size_t booksize) {
    int path = RANDOM()%NRSPELLPATHS, prayers = RANDOM()%2;
    int did_first_sp = 0;
    uint32 pnum = spellpathdef[path];
    archetype *at;

    /* Preamble */
    snprintf(retbuf, booksize, "Herein are detailed the names of %s\n", prayers ? "prayers" : "incantations");

    snprintf(retbuf+strlen(retbuf), booksize-strlen(retbuf), "belonging to the path of %s:\n", spellpathnames[path]);

    for (at = first_archetype; at != NULL; at = at->next) {
        /* Determine if this is an appropriate spell.  Must
        * be of matching path, must be of appropriate type (prayer
        * or not), and must be within the valid level range.
        */
        if (at->clone.type == SPELL
            && at->clone.path_attuned&pnum
            && ((at->clone.stats.grace && prayers) || (at->clone.stats.sp && !prayers))
            && at->clone.level < level*8) {
            if (book_overflow(retbuf, at->clone.name, booksize))
                break;

            if (did_first_sp)
                snprintf(retbuf+strlen(retbuf), booksize-strlen(retbuf), ",\n");
            did_first_sp = 1;
            snprintf(retbuf+strlen(retbuf), booksize-strlen(retbuf), "%s", at->clone.name);
            }
    }
    /* Geez, no spells were generated. */
    if (!did_first_sp) {
        if (RANDOM()%4)  /* usually, lets make a recursive call... */
            return old_spellpath_msg(level, retbuf, booksize);
        /* give up, cause knowing no spells exist for path is info too. */
        snprintf(retbuf+strlen(retbuf), booksize-strlen(retbuf), "\n - no known spells exist -\n");
    } else {
        snprintf(retbuf+strlen(retbuf), booksize-strlen(retbuf), "\n");
    }
    return retbuf;
}

START_TEST(test_spellpath_msg_rewrite) {
    char old[HUGE_BUF], new[HUGE_BUF];
    int todo = 10000, seed, what, size;
    const archetype *arch;

    const char* archs[] = { "book_clasp", "book_read", "checkbook", "letter", "note", "quarto", "scroll", "scroll_2", "tome" };
    const int arch_count = 9;

    while (todo-- > 0) {

        what = RANDOM() % arch_count;
        arch = find_archetype(archs[what]);
        fail_unless(arch != NULL, "missing arch %s", archs[what]);
        size = BOOKSIZE(&arch->clone);
        size -= strlen("\n"); /* Keep enough for final \n. */

        old[0] = '\0';
        new[0] = '\0';
        seed = RANDOM();
        SRANDOM(seed);
        old_spellpath_msg(1 + RANDOM() % 100, old, size);
        SRANDOM(seed);
        spellpath_msg(1 + RANDOM() % 100, new, size);
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
    tcase_add_test(tc_core, test_artifact_msg_rewrite);
    tcase_add_test(tc_core, test_spellpath_msg_rewrite);

    return s;
}

int main(void) {
    int nf;
    Suite *s = readable_suite();
    SRunner *sr = srunner_create(s);

    /* to debug, uncomment this line */
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_set_xml(sr, LOGDIR "/unit/common/readable.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/readable.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
