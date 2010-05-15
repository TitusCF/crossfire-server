#include "QuestManager.h"
#include "CREMainWindow.h"
#include "Quest.h"

extern "C" {
#include <global.h>
}

QuestManager::QuestManager()
{
}

QuestManager::~QuestManager()
{
}

void QuestManager::loadQuests()
{
    loadQuestFile("world.quests");
    qDebug() << "found quests:" << myQuests.size();
}

void QuestManager::loadQuestFile(const QString& filename)
{
    int i, in = 0; /* 0: quest file, 1: one quest, 2: quest description, 3: quest step, 4: step description */
    Quest *quest = NULL;
    char includefile[MAX_BUF];
    QuestStep *step = NULL;
    char final[MAX_BUF], read[MAX_BUF];
    FILE *file;
    StringBuffer *buf;

    snprintf(final, sizeof(final), "%s/%s/%s", settings.datadir, settings.mapdir, qPrintable(filename));
    file = fopen(final, "r");
    if (!file) {
        LOG(llevError, "Can't open %s for reading quests", qPrintable(filename));
        return;
    }

    while (fgets(read, sizeof(read), file) != NULL) {
        if (in == 4) {
            if (strcmp(read, "end_description\n") == 0) {
                char *message;

                in = 3;

                message = stringbuffer_finish(buf);
                buf = NULL;

                step->setDescription(add_string(message));
                free(message);

                continue;
            }

            stringbuffer_append_string(buf, read);
            continue;
        }

        if (in == 3) {
            if (strcmp(read, "end_step\n") == 0) {
                step = NULL;
                in = 1;
                continue;
            }
            if (strcmp(read, "finishes_quest\n") == 0) {
                step->setCompletion(true);
                continue;
            }
            if (strcmp(read, "description\n") == 0) {
                buf = stringbuffer_new();
                in = 4;
                continue;
            }
            LOG(llevError, "quests: invalid line %s in definition of quest %s in file %s!\n",
                    read, qPrintable(quest->code()), qPrintable(filename));
            continue;
        }

        if (in == 2) {
            if (strcmp(read, "end_description\n") == 0) {
                char *message;

                in = 1;

                message = stringbuffer_finish(buf);
                buf = NULL;

                quest->setDescription(message);
                free(message);

                continue;
            }
            stringbuffer_append_string(buf, read);
            continue;
        }

        if (in == 1) {
            if (strcmp(read, "end_quest\n") == 0) {
                quest = NULL;
                in = 0;
                continue;
            }

            if (strcmp(read, "description\n") == 0) {
                in = 2;
                buf = stringbuffer_new();
                continue;
            }

            if (strncmp(read, "title ", 6) == 0) {
                read[strlen(read) - 1] = '\0';
                quest->setTitle(read + 6);
                continue;
            }

            if (sscanf(read, "step %d\n", &i)) {
                step = new QuestStep();
                step->setStep(i);
                quest->steps().append(step);
                in = 3;
                continue;
            }

            if (sscanf(read, "restart %d\n", &i)) {
                quest->setRestart(i != 0);
                continue;
            }
        }

        if (read[0] == '#')
            continue;

        if (strncmp(read, "quest ", 6) == 0) {
            quest = new Quest();
            read[strlen(read) - 1] = '\0';
            quest->setCode(read + 6);
            if (getByCode(quest->code()) != NULL) {
                LOG(llevError, "Quest %s is listed in file %s, but this quest has already been defined\n", read + 6, qPrintable(filename));
            }
            myQuests.append(quest);
            in = 1;
            continue;
        }
        if (sscanf(read, "include %s\n", includefile)) {
            char inc_path[HUGE_BUF], p[HUGE_BUF];
            snprintf(p, sizeof(p), qPrintable(filename));
            path_combine_and_normalize(p, includefile, inc_path, sizeof(inc_path));
            loadQuestFile(inc_path);
            continue;
        }

        if (strcmp(read, "\n") == 0)
            continue;

        LOG(llevError, "quest: invalid file format for %s, I don't know what to do with the line %s\n", final, read);
    }
}

Quest* QuestManager::getByCode(const QString& code)
{
    foreach(Quest* quest, myQuests)
    {
        if (quest->code() == code)
            return quest;
    }
    return NULL;
}

QList<const Quest*> QuestManager::quests() const
{
    QList<const Quest*> quests;
    foreach(const Quest* quest, myQuests)
        quests.append(quest);
    return quests;
}
