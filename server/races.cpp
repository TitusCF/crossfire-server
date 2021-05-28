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
std::map<std::string, std::vector<object *> > races;

/**
 * Get a random monster of specified race and level at most the specified one.
 * @param race race, must not be NULL. If invalid, then logs as an error.
 * @param level maximum number, included.
 * @return random monster, NULL if none available for the level.
 */
object *races_get_random_monster(const char *race, int level) {
    auto r = races.find(race);
    if (r == races.end()) {
        LOG(llevError, "races_get_random_monster: requested non-existent aligned race %s!\n", race);
        return NULL;
    }

    std::vector<object *> valid;
    for (auto it = (*r).second.begin(); it != (*r).second.end(); it++) {
        if ((*it)->level <= level) {
            valid.push_back(*it);
        }
    }
    if (valid.empty()) {
        return NULL;
    }
    return valid[rndm(0, valid.size() - 1)];
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
            /* Remove trailing spaces */
            for (cp1 = cp+strlen(cp)-1; *cp1 == ' '; cp1--) {
                *cp1 = '\0';
                if (cp == cp1)
                    break;
            }

            addToRace[race].push_back(cp);
        }
    }
    LOG(llevDebug, "loaded races\n");
}

/**
 * Dumps all race information to stderr.
 */
void dump_races(void) {
    for (auto race = races.cbegin(); race != races.cend(); race++) {
        fprintf(stderr, "\nRACE %s:\t", (*race).first.c_str());
        for (auto mon = (*race).second.cbegin(); mon != (*race).second.cend(); mon++) {
            fprintf(stderr, "%s (%d), ", (*mon)->arch->name, (*mon)->level);
        }
    }
}

/**
 * Frees all race-related information.
 */
void free_races(void) {
    races.clear();
    LOG(llevDebug, "Freeing race information.\n");
}

void finish_races() {
    for (const auto& add : addToRace) {
        for (const auto& name : add.second) {
            auto mon = getManager()->archetypes()->find(name);
            if (mon && QUERY_FLAG(&mon->clone, FLAG_MONSTER)) {
                races[add.first].push_back(&mon->clone);
            } else {
                LOG(llevError, "races: %s %s\n", name.c_str(), mon ? "is not a monster" : "does not exist");
            }
        }
    }
    addToRace.clear();
}
