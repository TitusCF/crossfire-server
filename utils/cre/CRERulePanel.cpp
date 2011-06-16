#include "CRERulePanel.h"
#include "MessageFile.h"
#include <QtGui>
#include "CREStringListPanel.h"
#include "CREPrePostPanel.h"
#include "CREReplyPanel.h"
#include "MessageManager.h"

CRERulePanel::CRERulePanel(const MessageManager* manager, const QuestManager* quests, QWidget* parent) : QTabWidget(parent)
{
    myMatches = new CREStringListPanel(true, this);
    connect(myMatches, SIGNAL(dataModified()), this, SLOT(onMatchModified()));
    addTab(myMatches, tr("matches"));
    myPre = new CREPrePostPanel(true, manager->preConditions(), quests, this);
    connect(myPre, SIGNAL(dataModified()), this, SLOT(onPreModified()));
    addTab(myPre, tr("pre"));
    myMessages = new CREStringListPanel(false, this);
    connect(myMessages, SIGNAL(dataModified()), this, SLOT(onMessageModified()));
    addTab(myMessages, tr("message"));
    myPost = new CREPrePostPanel(false, manager->postConditions(), quests, this);
    connect(myPost, SIGNAL(dataModified()), this, SLOT(onPostModified()));
    addTab(myPost, tr("post"));

    myReplies = new CREReplyPanel(this);
    connect(myReplies, SIGNAL(dataModified()), this, SLOT(onRepliesModified()));
    addTab(myReplies, tr("replies"));

    QWidget* w = new QWidget(this);
    QVBoxLayout* l = new QVBoxLayout(w);
    l->addWidget(new QLabel(tr("Includes (one per line, path can be absolute or relative to the current message file):"), this));
    myInclude = new QTextEdit(this);
    myInclude->setAcceptRichText(false);
    connect(myInclude, SIGNAL(textChanged()), this, SLOT(onIncludeModified()));
    l->addWidget(myInclude);
    addTab(w, tr("includes"));

    myRule = NULL;
}

CRERulePanel::~CRERulePanel()
{
}

void CRERulePanel::setMessageRule(MessageRule* rule)
{
    myRule = NULL;

    myMatches->clearData();
    myMessages->clearData();
    myInclude->clear();

    myRule = rule;

    if (rule != NULL)
    {
        myMatches->setData(rule->match());
        myPre->setData(rule->preconditions());
        myMessages->setData(rule->messages());
        myPost->setData(rule->postconditions());
        myReplies->setData(rule->replies());
        myInclude->setText(rule->include().join("\n"));
    }
}

void CRERulePanel::onMatchModified()
{
    if (myRule == NULL)
        return;
    myRule->setMatch(myMatches->getData());
    emit currentRuleModified();
}

void CRERulePanel::onPreModified()
{
    if (myRule == NULL)
        return;
    myRule->setPreconditions(myPre->getData());
    emit currentRuleModified();
}

void CRERulePanel::onMessageModified()
{
    if (myRule == NULL)
        return;
    myRule->setMessages(myMessages->getData());
    emit currentRuleModified();
}

void CRERulePanel::onPostModified()
{
    if (myRule == NULL)
        return;
    myRule->setPostconditions(myPost->getData());
    emit currentRuleModified();
}

void CRERulePanel::onRepliesModified()
{
    if (myRule == NULL)
        return;
    myRule->setReplies(myReplies->getData());
    emit currentRuleModified();
}

void CRERulePanel::onIncludeModified()
{
    if (myRule != NULL)
    {
        QStringList include;
        include = myInclude->toPlainText().split("\n");
        for (int i = include.length() - 1; i >= 0; i--)
        {
            if (include[i].isEmpty())
                include.removeAt(i);
        }

        myRule->setInclude(include);
        emit currentRuleModified();
    }
}
