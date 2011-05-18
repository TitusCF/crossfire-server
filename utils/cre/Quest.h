#ifndef _QUEST_H
#define	_QUEST_H

#include <QObject>
#include <QStringList>

class CREMapInformation;

class QuestStep : public QObject
{
    Q_OBJECT

    public:
        QuestStep();

        int step() const;
        void setStep(int step);
        const QString& description() const;
        void setDescription(const QString& description);
        bool isCompletion() const;
        void setCompletion(bool completion);
        const QStringList& setWhen() const;
        QStringList& setWhen();

    private:
        int myStep;
        QString myDescription;
        bool myIsCompletion;
        QStringList mySetWhen;
};

class Quest : public QObject
{
    Q_OBJECT

    public:
        Quest();
        virtual ~Quest();

        const QString& code() const;
        void setCode(const QString& code);
        const QString& title() const;
        void setTitle(const QString& title);
        const QString& description()const;
        void setDescription(const QString& description);
        bool canRestart() const;
        void setRestart(bool restart);
        QList<const QuestStep*> steps() const;
        QList<QuestStep*>& steps();

        bool isModified() const;
        void setModified(bool modified);
        const Quest* parent() const;
        void setParent(Quest* parent);

        QList<CREMapInformation*>& maps();

    signals:
        void modified();

    private:
        QString myCode;
        QString myTitle;
        QString myDescription;
        bool myCanRestart;
        QList<QuestStep*> mySteps;
        bool myModified;
        Quest* myParent;
        QList<CREMapInformation*> myMaps;

        void markModified();
};

#endif	/* _QUEST_H */

