#ifndef _QUESTMANAGER_H
#define	_QUESTMANAGER_H

#include <QList>
class Quest;

class QuestManager
{
    public:
        QuestManager();
        virtual ~QuestManager();

        void loadQuests();
        QList<const Quest*> quests() const;
        Quest* getByCode(const QString& code);

    private:
        QList<Quest*> myQuests;

        void loadQuestFile(const QString& filename);
};

#endif	/* _QUESTMANAGER_H */
