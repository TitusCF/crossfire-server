#include "CRERulePanel.h"
#include "MessageFile.h"
#include <QtGui>
#include "CREStringListPanel.h"

CRERulePanel::CRERulePanel(QWidget* parent) : QTabWidget(parent)
{
    myMatches = new CREStringListPanel(true, this);
    connect(myMatches, SIGNAL(dataModified()), this, SLOT(onMatchModified()));
    addTab(myMatches, tr("matches"));
    addTab(new QLabel(tr("pre")), tr("pre"));
    myMessages = new CREStringListPanel(false, this);
    connect(myMessages, SIGNAL(dataModified()), this, SLOT(onMessageModified()));
    addTab(myMessages, tr("message"));
    addTab(new QLabel(tr("post")), tr("post"));
    addTab(new QLabel(tr("replies")), tr("replies"));
    addTab(new QLabel(tr("includes")), tr("includes"));

    myRule = NULL;
}

CRERulePanel::~CRERulePanel()
{
}

void CRERulePanel::setMessageRule(MessageRule* rule)
{
    myRule = rule;

    myMatches->clearData();
    myMessages->clearData();

    if (rule != NULL)
    {
        myMatches->setData(rule->match());
        myMessages->setData(rule->messages());
    }
}

void CRERulePanel::onMatchModified()
{
    myRule->setMatch(myMatches->getData());
    emit currentRuleModified();
}

void CRERulePanel::onMessageModified()
{
    myRule->setMessages(myMessages->getData());
    emit currentRuleModified();
}
