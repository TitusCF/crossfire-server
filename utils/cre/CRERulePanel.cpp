#include "CRERulePanel.h"
#include "MessageFile.h"
#include <QtGui>

CRERulePanel::CRERulePanel(QWidget* parent) : QTabWidget(parent)
{
    QWidget* matches = new QWidget(this);
    QGridLayout* layout = new QGridLayout(matches);
    myMatches = new QListWidget(this);
    connect(myMatches, SIGNAL(currentRowChanged(int)), this, SLOT(onCurrentMatchChanged(int)));
    layout->addWidget(new QLabel(tr("Matches:"), this), 0, 0, 1, 2);
    layout->addWidget(myMatches, 1, 0, 1, 2);

    QPushButton* add = new QPushButton(tr("add"), this);
    connect(add, SIGNAL(clicked(bool)), this, SLOT(onAddMatch(bool)));
    layout->addWidget(add, 2, 0);

    QPushButton* remove = new QPushButton(tr("remove"), this);
    connect(remove, SIGNAL(clicked(bool)), this, SLOT(onDeleteMatch(bool)));
    layout->addWidget(remove, 2, 1);

    layout->addWidget(new QLabel(tr("Match:"), this), 3, 0);
    myMatch = new QLineEdit(this);
    layout->addWidget(myMatch, 3, 1);

    addTab(matches, tr("matches"));
    addTab(new QLabel(tr("pre")), tr("pre"));
    addTab(new QLabel(tr("message")), tr("message"));
    addTab(new QLabel(tr("post")), tr("post"));
    addTab(new QLabel(tr("replies")), tr("replies"));
    addTab(new QLabel(tr("includes")), tr("includes"));

    myRule = NULL;
    myCurrentMatch = -1;
}

CRERulePanel::~CRERulePanel()
{
}

void CRERulePanel::setMessageRule(MessageRule* rule)
{
    commitMatch();

    myRule = rule;

    myMatches->clear();
    myMatch->clear();

    if (rule != NULL)
    {
        myMatches->addItems(rule->match());
    }
}

void CRERulePanel::onAddMatch(bool)
{
    myRule->match().append("<match>");
    myMatches->addItem("<match>");
    emit currentRuleModified();
}

void CRERulePanel::onDeleteMatch(bool)
{
    if (myCurrentMatch == -1 || myCurrentMatch >= myRule->match().size())
        return;

    myRule->match().removeAt(myCurrentMatch);
    myCurrentMatch = -1;
    myMatches->clear();
    myMatches->addItems(myRule->match());
    myMatch->setText("");
    emit currentRuleModified();
}

void CRERulePanel::commitMatch()
{
    if (myCurrentMatch == -1)
        return;

    myRule->match()[myCurrentMatch] = myMatch->text();
    myMatches->item(myCurrentMatch)->setText(myMatch->text());
    myCurrentMatch = -1;
    myMatch->setText("");
    emit currentRuleModified();
}

void CRERulePanel::onCurrentMatchChanged(int currentRow)
{
    commitMatch();
    if (currentRow == -1)
        return;
    myCurrentMatch = currentRow;
    myMatch->setText(myRule->match()[currentRow]);
}
