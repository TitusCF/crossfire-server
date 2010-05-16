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

    myCanRestart = new QCheckBox(tr("this quest can be done multiple times"));
    layout->addWidget(myCanRestart, line++, 1, 1, 2);

    layout->addWidget(new QLabel(tr("Description:"), this), line++, 1, 1, 2);
    myDescription = new QTextEdit();
    layout->addWidget(myDescription, line++, 1, 1, 2);

    myStepDescription = new QTextEdit(this);
    QGroupBox* steps = new QGroupBox(tr("Steps"));

    layout->addWidget(steps, line++, 1, 1, 2);

    QGridLayout* stepsLayout = new QGridLayout();
    steps->setLayout(stepsLayout);

    mySteps = new QListWidget(this);
    connect(mySteps, SIGNAL(currentRowChanged(int)), this, SLOT(stepChanged(int)));
    stepsLayout->addWidget(mySteps, 0, 0, 4, 1);
    stepsLayout->addWidget(new QLabel(tr("Step:"), this), 0, 1);
    myStep = new QLineEdit();
    stepsLayout->addWidget(myStep, 0, 2);
    stepsLayout->addWidget(new QLabel(tr("Description"), this), 1, 1, 1, 2);
    stepsLayout->addWidget(myStepDescription, 2, 1, 1, 2);
    myStepEnd = new QCheckBox(tr("end"));
    stepsLayout->addWidget(myStepEnd, 3, 1);

    myQuest = NULL;
    myCurrentStep = NULL;
}

CREQuestPanel::~CREQuestPanel()
{
}

void CREQuestPanel::setQuest(Quest* quest)
{
    myQuest = quest;
    myCurrentStep = NULL;

    myCode->setText(quest->code());
    myTitle->setText(quest->title());
    myCanRestart->setChecked(quest->canRestart());
    myDescription->setText(quest->description());
    myStepDescription->setText("");
    myStepEnd->setChecked(false);

    mySteps->clear();

    QString display;
    foreach(const QuestStep* step, quest->steps())
    {
        display = QString::number(step->step());
        if (step->isCompletion())
            display += tr(" (end)");
        new QListWidgetItem(display, mySteps);
    }
}

void CREQuestPanel::commitData()
{
    if (!myQuest)
        return;

    myQuest->setCode(myCode->text());
    myQuest->setTitle(myTitle->text());
    myQuest->setRestart(myCanRestart->isChecked());
    myQuest->setDescription(myDescription->toPlainText());
    commitStep();
}

void CREQuestPanel::commitStep()
{
    if (myCurrentStep == NULL)
        return;

    myCurrentStep->setStep(myStep->text().toInt());
    myCurrentStep->setDescription(myStepDescription->toPlainText());
    myCurrentStep->setCompletion(myStepEnd->isChecked());
    /** @todo shouldn't be required */
    myQuest->setModified(true);
}

void CREQuestPanel::stepChanged(int newStep)
{
    commitStep();
    if (newStep >= 0 && newStep < myQuest->steps().size())
    {
        myCurrentStep = myQuest->steps()[newStep];
        myStep->setText(QString::number(myCurrentStep->step()));
        myStepDescription->setText(myCurrentStep->description());
        myStepEnd->setChecked(myCurrentStep->isCompletion());
    }
    else
    {
        myCurrentStep = NULL;
        myStep->setText("");
        myStepDescription->setText("");
        myStepEnd->setChecked(false);
    }
}
