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

#include "QuestWriter.h"

#define W(x, n) { if (quest->x) { stringbuffer_append_printf(buf, n "\n", quest->x); } }

void QuestWriter::write(const quest_definition *quest, StringBuffer *buf) {
    W(quest_code, "quest %s");
    W(quest_title, "title %s");
    if (quest->face) {
        stringbuffer_append_printf(buf, "face %s\n", quest->face->name);
    }
    if (quest->quest_description) {
        stringbuffer_append_multiline_block(buf, "description", quest->quest_description, NULL);
    }
    if (quest->parent) {
        stringbuffer_append_printf(buf, "parent %s\n", quest->parent->quest_code);
    }
    W(quest_restart, "restart %d");
    W(quest_is_system, "is_system %d");
    if (quest->quest_comment) {
        stringbuffer_append_multiline_block(buf, "comment", quest->quest_comment, NULL);
    }
    auto step = quest->steps;
    while (step) {
        stringbuffer_append_printf(buf, "step %d\n", step->step);
        if (step->is_completion_step) {
            stringbuffer_append_string(buf, "finishes_quest\n");
        }
        if (step->step_description) {
            stringbuffer_append_multiline_block(buf, "description", step->step_description, NULL);
        }
        if (step->conditions) {
            stringbuffer_append_string(buf, "setwhen\n");
            auto cond = step->conditions;
            while (cond) {
                stringbuffer_append_printf(buf, "%s ", cond->quest_code);
                if (cond->maxstep == -1 && cond->minstep == -1) {
                    stringbuffer_append_printf(buf, "finished");
                } else {
                    if (cond->minstep == 0) {
                        stringbuffer_append_printf(buf, "<=%d", cond->maxstep);
                    } else if (cond->minstep == cond->maxstep) {
                        stringbuffer_append_printf(buf, " %d", cond->maxstep);
                    } else {
                        stringbuffer_append_printf(buf, "%d-%d", cond->minstep, cond->maxstep);
                    }
                }
                stringbuffer_append_string(buf, "\n");
                cond = cond->next;
            }
            stringbuffer_append_string(buf, "end_setwhen\n");
        }
        stringbuffer_append_string(buf, "end_step\n");
        step = step->next;
    }
    stringbuffer_append_string(buf, "end_quest\n");
    /*

    foreach(QuestStep* step, quest->steps())
    {
        if (step->setWhen().size() > 0)
        {
            stream << "setwhen\n";
            foreach(QString when, step->setWhen())
            {
                stream << when << "\n";
            }
            stream << "end_setwhen\n";
        }
        stream << "end_step\n";

    }

    stream << "end_quest\n\n";*/
}
