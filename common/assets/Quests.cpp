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

#include "Quests.h"

Quests::Quests() : visibleQuests(0) {
}

Quests::~Quests() {
}

quest_definition *Quests::create(const std::string& name) {
    return quest_create(name.data());
}

void Quests::destroy(quest_definition *quest) {
    quest_destroy(quest);
}

void Quests::replace(quest_definition *existing, quest_definition *update) {
    if (!existing->quest_is_system) {
        visibleQuests--;
    }
    quest_clear(existing);
    (*existing) = (*update);
    free(update);
    if (!existing->quest_is_system) {
        visibleQuests++;
    }
}

void Quests::added(quest_definition *quest) {
    quest->client_code = m_assets.size();
    if (!quest->quest_is_system)
        visibleQuests++;
}

quest_step_definition *quest_create_step(void) {
    quest_step_definition *step = static_cast<quest_step_definition *>(calloc(1, sizeof(quest_step_definition)));
    if (!step)
        fatal(OUT_OF_MEMORY);
    return step;
}

quest_condition *quest_create_condition(void) {
    quest_condition *cond = static_cast<quest_condition *>(calloc(1, sizeof(quest_condition)));
    if (!cond)
        fatal(OUT_OF_MEMORY);
    return cond;
}

quest_definition *quest_create(const char *name) {
    quest_definition *quest = static_cast<quest_definition *>(calloc(1, sizeof(quest_definition)));
    quest->quest_code = add_string(name);
    return quest;
}

void quest_destroy_condition(quest_condition *condition) {
    free_string(condition->quest_code);
    free(condition);
}

void quest_destroy_step(quest_step_definition *step) {
    FREE_AND_CLEAR_STR_IF(step->step_description);
    auto condition = step->conditions;
    while (condition != NULL) {
        auto next_condition = condition->next;
        quest_destroy_condition(condition);
        condition = next_condition;
    }
    free(step);
}

void quest_destroy_steps(quest_step_definition *step) {
    while (step) {
        auto next_step = step->next;
        quest_destroy_step(step);
        step = next_step;
    }
}

void quest_clear(quest_definition *quest) {
    free_string(quest->quest_code);
    FREE_AND_CLEAR_STR_IF(quest->quest_description);
    FREE_AND_CLEAR_STR_IF(quest->quest_title);
    FREE_AND_CLEAR_STR_IF(quest->quest_comment);
    quest_destroy_steps(quest->steps);
}

void quest_destroy(quest_definition *quest) {
    quest_clear(quest);
    free(quest);
}

int quest_condition_from_string(quest_condition *condition, const char *buffer) {
    int condition_parsed = 0;
    condition->minstep = 0;
    condition->maxstep = 0;
    condition->quest_code = nullptr;

    char namedquest[500];
    namedquest[0] = '\0';
    if (sscanf(buffer, "%s %d-%d", namedquest, &condition->minstep, &condition->maxstep) != 3) {
        if (sscanf(buffer, "%s <=%d", namedquest, &condition->maxstep) == 2) {
            condition->minstep = 0;
            condition_parsed = 1;
        } else if (sscanf(buffer, "%s %d", namedquest, &condition->minstep) == 2) {
            condition->maxstep = condition->minstep;
            condition_parsed = 1;
        } else if (strstr(buffer, "finished")) {
            if (sscanf(buffer, "%s finished", namedquest) == 1) {
                condition->minstep = condition->maxstep = -1;
                condition_parsed = 1;
            }
        }
    } else
        condition_parsed = 1;

    if (condition_parsed)
        condition->quest_code = add_string(namedquest);

    return condition_parsed;
}

void quest_write_condition(char *buf, size_t len, const quest_condition *cond) {
    if (cond->maxstep == -1 && cond->minstep == -1) {
        snprintf(buf, len, "%s finished", cond->quest_code);
    } else {
        if (cond->minstep == 0) {
            snprintf(buf, len, "%s <=%d", cond->quest_code, cond->maxstep);
        } else if (cond->minstep == cond->maxstep) {
            snprintf(buf, len, "%s %d", cond->quest_code, cond->maxstep);
        } else {
            snprintf(buf, len, "%s %d-%d", cond->quest_code, cond->minstep, cond->maxstep);
        }
    }
}
