extern "C" {
#include "global.h"
#include "compat.h"
#include "string.h"
#include "malloc.h"
}
#include <string>
#include <map>
#include <vector>

#include "sproto.h"
#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"

std::map<std::string, std::vector<std::string> > addToRace;

/**
 * Find the race information for the specified name.
 *
 * @param name
 * race to search for.
 * @return
 * race structure, NULL if not found.
 */
racelink *find_racelink(const char *name) {
    racelink *test = NULL;

    if (name && first_race)
        for (test = first_race; test && test != test->next; test = test->next)
            if (!test->name || !strcmp(name, test->name))
                break;

    return test;
}

/**
 * Create a new ::racelink structure.
 *
 * @note
 * will call fatal() in case of memory allocation failure.
 * @return
 * empty structure.
 */
static racelink *get_racelist(void) {
    racelink *list;

    list = (racelink *)malloc(sizeof(racelink));
    if (!list)
        fatal(OUT_OF_MEMORY);
    list->name = NULL;
    list->nrof = 0;
    list->member = get_objectlink();
    list->next = NULL;

    return list;
}

/**
 * Add an object to the racelist.
 *
 * @param race_name
 * race name.
 * @param op
 * what object to add to the race.
 */
static void add_to_racelist(const char *race_name, object *op) {
    racelink *race;

    if (!op || !race_name)
        return;
    race = find_racelink(race_name);

    if (!race) { /* add in a new race list */
        race = get_racelist();
        race->next = first_race;
        first_race = race;
        race->name = add_string(race_name);
    }

    if (race->member->ob) {
        objectlink *tmp = get_objectlink();

        tmp->next = race->member;
        race->member = tmp;
    }
    race->nrof++;
    race->member->ob = op;
}

/**
 * Reads the races file in the lib/ directory, then
 * overwrites old 'race' entries. This routine allow us to quickly
 * re-configure the 'alignment' of monsters, objects. Useful for
 * putting together lists of creatures, etc that belong to gods.
 */
void load_races(BufferReader *reader, const char *) {
    char race[MAX_BUF], *buf, *cp, variable[MAX_BUF];

    while ((buf = bufferreader_next_line(reader)) != NULL) {
        if (*buf == '#')
            continue;
        cp = buf;
        while (*cp == ' ') {
            cp++;
        }
        if (sscanf(cp, "RACE %s", variable)) { /* set new race value */
            strcpy(race, variable);
        } else {
            char *cp1;

            /* Take out beginning spaces */
            for (cp1 = cp; *cp1 == ' '; cp1++)
                ;
            /* Remove newline and trailing spaces */
            for (cp1 = cp+strlen(cp)-1; *cp1 == '\n' || *cp1 == ' '; cp1--) {
                *cp1 = '\0';
                if (cp == cp1)
                    break;
            }

            if (cp[strlen(cp)-1] == '\n')
                cp[strlen(cp)-1] = '\0';
            addToRace[race].push_back(cp);
        }
    }
    LOG(llevDebug, "loaded races\n");
}

/**
 * Dumps all race information to stderr.
 */
void dump_races(void) {
    racelink *list;
    objectlink *tmp;

    for (list = first_race; list; list = list->next) {
        fprintf(stderr, "\nRACE %s:\t", list->name);
        for (tmp = list->member; tmp; tmp = tmp->next)
            fprintf(stderr, "%s (%d), ", tmp->ob->arch->name, tmp->ob->level);
    }
    fprintf(stderr, "\n");
}

/**
 * Frees all race-related information.
 */
void free_races(void) {
    racelink *race;
    objectlink *link;

    LOG(llevDebug, "Freeing race information.\n");
    while (first_race) {
        race = first_race->next;
        while (first_race->member) {
            link = first_race->member->next;
            free(first_race->member);
            first_race->member = link;
        }
        free_string(first_race->name);
        free(first_race);
        first_race = race;
    }
}

void finish_races() {
    for (const auto& add : addToRace) {
        for (const auto& name : add.second) {
            auto mon = getManager()->archetypes()->find(name);
            if (mon && QUERY_FLAG(&mon->clone, FLAG_MONSTER)) {
                add_to_racelist(add.first.c_str(), &mon->clone);
            } else {
                LOG(llevError, "races: %s %s\n", name.c_str(), mon ? "is not a monster" : "does not exist");
            }
        }
    }
    addToRace.clear();
}
