/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "QuestLoader.h"
#include "AssetsCollection.h"
#include "Quests.h"
#include "assets.h"
#include "AssetsManager.h"

/**
 * @defgroup QUESTFILE_xxx Quest file parsing state.
 *
 * This is the parsing state when loading a file through load_quests_from_file().
 */
/*@{*/
#define QUESTFILE_NEXTQUEST 0   /**< Waiting for next quest definition. */
#define QUESTFILE_QUEST 1       /**< In a quest definition. */
#define QUESTFILE_QUESTDESC 2   /**< In a quest description. */
#define QUESTFILE_STEP 3        /**< In a quest step. */
#define QUESTFILE_STEPDESC 4    /**< In a quest step description. */
#define QUESTFILE_STEPCOND 5    /**< In a quest step conditions. */
#define QUESTFILE_COMMENT  6    /**< In a quest comment. */
/*@}*/

QuestLoader::QuestLoader(Quests *quests, Faces *faces, AssetsTracker *tracker) : quests(quests), faces(faces), tracker(tracker) {
}

void QuestLoader::load(BufferReader *reader, const std::string &filename) {
    int i, in = QUESTFILE_NEXTQUEST;
    quest_definition *quest = NULL;
    quest_condition *cond = NULL;
    char includefile[MAX_BUF];
    quest_step_definition *step = NULL;
    char final[MAX_BUF], *read;
    StringBuffer *buf = NULL;

    while ((read = bufferreader_next_line(reader)) != NULL) {
        if (in == QUESTFILE_STEPCOND) {
            if (strcmp(read, "end_setwhen") == 0) {
                in = QUESTFILE_STEP;
                continue;
            }
            /*
             * We are reading in a list of conditions for the 'setwhen' block for a quest step
             * There will be one entry per line, containing the quest, and the steps that it applies to.
             */

            cond = quest_create_condition();
            if (!quest_condition_from_string(cond, read)) {
                LOG(llevError, "Invalid line '%s' in setwhen block for quest %s=n", read, quest->quest_code);
                free(cond);
                continue;
            }

            if (step->conditions) {
                auto c = step->conditions;
                while (c->next) {
                    c = c->next;
                }
                c->next = cond;
            } else {
                step->conditions = cond;
            }
            LOG(llevDebug, "condition added for step %d of quest %s, looking for quest %s between steps %d and %d\n",
                    step->step, quest->quest_code, cond->quest_code, cond->minstep, cond->maxstep);
            continue;
        }
        if (in == QUESTFILE_STEPDESC) {
            if (strcmp(read, "end_description") == 0) {
                char *message;

                in = QUESTFILE_STEP;

                message = stringbuffer_finish(buf);
                buf = NULL;

                step->step_description = (*message != '\0') ? add_string(message + 1) : NULL; // Skip initial newline
                free(message);

                continue;
            }

            stringbuffer_append_string(buf, "\n");
            stringbuffer_append_string(buf, read);
            continue;
        }

        if (in == QUESTFILE_STEP) {
            if (strcmp(read, "end_step") == 0) {
                step = NULL;
                in = QUESTFILE_QUEST;
                continue;
            }
            if (strcmp(read, "finishes_quest") == 0) {
                step->is_completion_step = 1;
                continue;
            }
            if (strcmp(read, "description") == 0) {
                buf = stringbuffer_new();
                in = QUESTFILE_STEPDESC;
                continue;
            }
            if (strcmp(read, "setwhen") == 0) {
                in = QUESTFILE_STEPCOND;
                continue;
            }
            LOG(llevError, "quests: invalid line %s in definition of quest %s in %s:%d!\n",
                    read, quest->quest_code, filename.c_str(), bufferreader_current_line(reader));
            continue;
        }

        if (in == QUESTFILE_QUESTDESC) {
            if (strcmp(read, "end_description") == 0) {
                in = QUESTFILE_QUEST;

                auto message = stringbuffer_finish(buf);
                buf = NULL;

                quest->quest_description = (*message != '\0') ? add_string(message + 1) : NULL; // Remove initial newline
                free(message);

                continue;
            }
            stringbuffer_append_string(buf, "\n");
            stringbuffer_append_string(buf, read);
            continue;
        }

        if (in == QUESTFILE_COMMENT) {
            // Quest comment is ignored here, only used in eg CRE.
            if (strcmp(read, "end_comment") == 0) {
                in = QUESTFILE_QUEST;
                auto comment = stringbuffer_finish(buf);
                buf = nullptr;
                quest->quest_comment = (*comment != '\0') ? add_string(comment + 1) : NULL; // Skip initial newline
                free(comment);
                continue;
            }
            stringbuffer_append_string(buf, "\n");
            stringbuffer_append_string(buf, read);
            continue;
        }

        if (in == QUESTFILE_QUEST) {
            if (strcmp(read, "end_quest") == 0) {
                quests->define(quest->quest_code, quest);
                if (tracker) {
                    tracker->assetDefined(quest, filename);
                }
                quest = NULL;
                in = QUESTFILE_NEXTQUEST;
                continue;
            }

            if (strcmp(read, "description") == 0) {
                in = QUESTFILE_QUESTDESC;
                buf = stringbuffer_new();
                continue;
            }

            if (strncmp(read, "title ", 6) == 0) {
                quest->quest_title = add_string(read + 6);
                continue;
            }

            if (sscanf(read, "step %d", &i)) {
                step = quest_create_step();
                step->step = i;
                if (quest->steps) {
                    auto l = quest->steps;
                    while (l->next) {
                        l = l->next;
                    }
                    l->next = step;
                } else {
                    quest->steps = step;
                }
                in = QUESTFILE_STEP;
                continue;
            }

            if (sscanf(read, "restart %d", &i)) {
                quest->quest_restart = i;
                continue;
            }
            if (strncmp(read, "parent ", 7) == 0) {
                quest->parent = quests->get(read + 7);
                continue;
            }

            if (strncmp(read, "face ", 5) == 0) {
                quest->face = faces->get(read + 5);
                continue;
            }

            if (strncmp(read, "comment", 7) == 0) {
                in = QUESTFILE_COMMENT;
                buf = stringbuffer_new();
                continue;
            }

            if (sscanf(read, "is_system %d", &i)) {
                quest->quest_is_system = (i ? true : false);
                continue;
            }
        }

        if (read[0] == '#')
            continue;

        if (strncmp(read, "quest ", 6) == 0) {
            if (quest) {
                LOG(llevError, "'quest' while in quest '%s' in file %s\n", quest->quest_code, filename.c_str());
            }
            quest = quest_create(read + 6);
            /* Set a default face, which will be overwritten if a face is defined. */
            quest->face = faces->get("quest_generic.111");
            in = QUESTFILE_QUEST;
            continue;
        }
        if (sscanf(read, "include %s\n", includefile)) {
#if 0
            char inc_path[HUGE_BUF];
            path_combine_and_normalize(filename, includefile, inc_path, sizeof (inc_path));
            found = load_quests_from_file(inc_path);
            if (found >= 0) {
                LOG(llevDebug, "loaded %d quests from file %s\n", found, inc_path);
                loaded_quests += found;
            } else {
                LOG(llevError, "Failed to load quests from file %s\n", inc_path);
            }
#endif
            continue;
        }

        if (strcmp(read, "\n") == 0)
            continue;

        LOG(llevError, "quest: invalid file format for %s, I don't know what to do with the line %s\n", final, read);
    }

    if (in != 0) {
        LOG(llevError, "quest: quest definition file %s read in, ends with state %d\n", final, in);

        /* The buffer may not have been freed. */
        if (buf != NULL) {
            stringbuffer_finish(buf);
        }
    }
}
