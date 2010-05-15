#include "Quest.h"

QuestStep::QuestStep()
{
    myStep = 0;
    myIsCompletion = false;
}

int QuestStep::step() const
{
    return myStep;
}

void QuestStep::setStep(int step)
{
    myStep = step;
}

const QString& QuestStep::description() const
{
    return myDescription;
}

void QuestStep::setDescription(const QString& description)
{
    myDescription = description;
}

bool QuestStep::isCompletion() const
{
    return myIsCompletion;
}

void QuestStep::setCompletion(bool completion)
{
    myIsCompletion = completion;
}


Quest::Quest()
{
    myCanRestart = false;
}

Quest::~Quest()
{
    qDeleteAll(mySteps);
}

const QString& Quest::code() const
{
    return myCode;
}

void Quest::setCode(const QString& code)
{
    myCode = code;
}

const QString& Quest::title() const
{
    return myTitle;
}

void Quest::setTitle(const QString& title)
{
    myTitle = title;
}

const QString& Quest::description()const
{
    return myDescription;
}

void Quest::setDescription(const QString& description)
{
    myDescription = description;
}

bool Quest::canRestart() const
{
    return myCanRestart;
}

void Quest::setRestart(bool restart)
{
    myCanRestart = restart;
}

QList<const QuestStep*> Quest::steps() const
{
    QList<const QuestStep*> steps;
    foreach(const QuestStep* step, mySteps)
        steps.append(step);
    return steps;
}

QList<QuestStep*>& Quest::steps()
{
    return mySteps;
}
