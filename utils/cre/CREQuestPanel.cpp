#include "CREQuestPanel.h"
#include "CRERegionPanel.h"
#include "Quest.h"

CREQuestPanel::CREQuestPanel()
{
    QGridLayout* layout = new QGridLayout(this);

    int line = 1;
    layout->addWidget(new QLabel(tr("Code:"), this), line, 1);
    myCode = new QLineEdit();
    layout->addWidget(myCode, line++, 2);

    layout->addWidget(new QLabel(tr("Title:"), this), line, 1);
    myTitle = new QLineEdit();
    layout->addWidget(myTitle, line++, 2);

    layout->addWidget(new QLabel(tr("Description:"), this), line++, 1, 1, 2);
    myDescription = new QTextEdit();
    layout->addWidget(myDescription, line++, 1, 1, 2);

    layout->addWidget(new QLabel(tr("Steps:"), this), line++, 1, 1, 2);
    mySteps = new QTreeWidget(this);
    QStringList labels;
    labels << tr("Step") << tr("title");
    mySteps->setHeaderLabels(labels);
    layout->addWidget(mySteps, line++, 1, 1, 2);
}

CREQuestPanel::~CREQuestPanel()
{
}

void CREQuestPanel::setQuest(const Quest* quest)
{
    myCode->setText(quest->code());
    myTitle->setText(quest->title());
    myDescription->setText(quest->description());

    mySteps->clear();

    foreach(const QuestStep* step, quest->steps())
    {
        QStringList data;
        data << QString::number(step->step()) << step->description();
        new QTreeWidgetItem(mySteps, data);
    }
}
