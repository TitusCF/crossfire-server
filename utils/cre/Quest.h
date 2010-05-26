#ifndef _QUEST_H
#define	_QUEST_H

#include <QObject>

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

    private:
        int myStep;
        QString myDescription;
        bool myIsCompletion;
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

        void markModified();
};

#endif	/* _QUEST_H */

