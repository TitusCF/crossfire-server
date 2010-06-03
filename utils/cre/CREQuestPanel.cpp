#include "CREQuestPanel.h"
#include "CRERegionPanel.h"
#include "Quest.h"
#include "QuestManager.h"

CREQuestPanel::CREQuestPanel(QuestManager* manager)
{
    Q_ASSERT(manager);
    myQuestManager = manager;
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

    layout->addWidget(new QLabel(tr("Parent:"), this), line, 1);
    myParent = new QComboBox(this);
    layout->addWidget(myParent, line++, 2);
    myParent->addItem(tr("(none)"));
    foreach(const Quest* quest, manager->quests())
    {
        myParent->addItem(quest->code());
    }

    layout->addWidget(new QLabel(tr("Quest file:"), this), line, 1);
    myFile = new QComboBox(this);
    layout->addWidget(myFile, line++, 2);

    myFile->setInsertPolicy(QComboBox::InsertAlphabetically);
    myFile->setEditable(true);
    myFile->addItem("");
    myFile->addItems(myQuestManager->getFiles());

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
    stepsLayout->addWidget(mySteps, 0, 0, 6, 2);

    QPushButton* add = new QPushButton(tr("add step"), this);
    connect(add, SIGNAL(clicked(bool)), this, SLOT(onAddStep(bool)));
    stepsLayout->addWidget(add, 6, 0);
    QPushButton* del = new QPushButton(tr("remove step"), this);
    connect(del, SIGNAL(clicked(bool)), this, SLOT(onDeleteStep(bool)));
    stepsLayout->addWidget(del, 6, 1);

    stepsLayout->addWidget(new QLabel(tr("Step:"), this), 0, 2);
    myStep = new QLineEdit();
    stepsLayout->addWidget(myStep, 0, 3);
    stepsLayout->addWidget(new QLabel(tr("Description"), this), 1, 2, 1, 2);
    stepsLayout->addWidget(myStepDescription, 2, 2, 1, 2);
    myStepEnd = new QCheckBox(tr("end"));
    stepsLayout->addWidget(myStepEnd, 3, 2);

    stepsLayout->addWidget(new QLabel(tr("Set when:"), this), 4, 2);
    mySetWhen = new QTextEdit(this);
    stepsLayout->addWidget(mySetWhen, 5, 2, 2, 2);

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

    QString file = myQuestManager->getQuestFile(myQuest);
    myFile->setEditText(file);
    myFile->setEnabled(file.isEmpty());

    if (quest->parent() != NULL)
    {
        int idx = myParent->findText(quest->parent()->code());
        if (idx != -1)
            myParent->setCurrentIndex(idx);
    }
    else
        myParent->setCurrentIndex(0);

    displaySteps();
}

void CREQuestPanel::commitData()
{
    if (!myQuest)
        return;

    myQuest->setCode(myCode->text());
    myQuest->setTitle(myTitle->text());
    myQuest->setRestart(myCanRestart->isChecked());
    myQuest->setDescription(myDescription->toPlainText());
    if (myQuestManager->getQuestFile(myQuest).isEmpty())
        myQuestManager->setQuestFile(myQuest, myFile->currentText());
    if (myParent->currentIndex() == 0)
    {
        myQuest->setParent(NULL);
    }
    else
        myQuest->setParent(myQuestManager->findByCode(myParent->currentText()));
    commitStep();
}

void CREQuestPanel::commitStep()
{
    if (myCurrentStep == NULL)
        return;

    myCurrentStep->setStep(myStep->text().toInt());
    myCurrentStep->setDescription(myStepDescription->toPlainText());
    myCurrentStep->setCompletion(myStepEnd->isChecked());

    myCurrentStep->setWhen() = mySetWhen->toPlainText().split("\n");
    for (int i = myCurrentStep->setWhen().size() - 1; i >= 0; i--)
    {
        if (myCurrentStep->setWhen()[i].isEmpty())
            myCurrentStep->setWhen().removeAt(i);
    }
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
        mySetWhen->setText(myCurrentStep->setWhen().join("\n"));
    }
    else
    {
        myCurrentStep = NULL;
        myStep->setText("");
        myStepDescription->setText("");
        myStepEnd->setChecked(false);
        mySetWhen->setText("");
    }
}

void CREQuestPanel::onAddStep(bool)
{
    if (myQuest == NULL)
        return;

    QuestStep* step = new QuestStep();
    step->setStep(-1);
    myQuest->steps().append(step);
    new QListWidgetItem("-1", mySteps);
}

void CREQuestPanel::onDeleteStep(bool)
{
    if (myQuest == NULL || myCurrentStep == NULL)
        return;

    int idx = myQuest->steps().indexOf(myCurrentStep);
    myQuest->steps().removeAt(idx);

    delete myCurrentStep;
    myCurrentStep = NULL;

    displaySteps();
}

void CREQuestPanel::displaySteps()
{
    mySteps->clear();

    QString display;
    foreach(const QuestStep* step, myQuest->steps())
    {
        display = QString::number(step->step());
        if (step->isCompletion())
            display += tr(" (end)");
        new QListWidgetItem(display, mySteps);
    }
}
