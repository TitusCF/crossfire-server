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
    quest_definition *quest = static_cast<quest_definition *>(calloc(1, sizeof(quest_definition)));
    quest->quest_code = add_string(name.c_str());
    return quest;
}

void destroy_steps(quest_step_definition *step) {
    while (step) {
        auto next_step = step->next;
        FREE_AND_CLEAR_STR_IF(step->step_description);
        auto condition = step->conditions;
        while (condition != NULL) {
            auto next_condition = condition->next;
            free_string(condition->quest_code);
            free(condition);
            condition = next_condition;
        }
        free(step);
        step = next_step;
    }
}

void clear_quest(quest_definition *quest) {
    free_string(quest->quest_code);
    FREE_AND_CLEAR_STR_IF(quest->quest_description);
    FREE_AND_CLEAR_STR_IF(quest->quest_title);
    FREE_AND_CLEAR_STR_IF(quest->quest_comment);
    destroy_steps(quest->steps);
}

void Quests::destroy(quest_definition *quest) {
    clear_quest(quest);
    free(quest);
}

void Quests::replace(quest_definition *existing, quest_definition *update) {
    if (!existing->quest_is_system) {
        visibleQuests--;
    }
    clear_quest(existing);
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
