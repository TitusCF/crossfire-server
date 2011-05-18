#include "CREQuestPanel.h"
#include "CRERegionPanel.h"
#include "Quest.h"
#include "QuestManager.h"
#include "CREQuestItemModel.h"
#include "CREMultilineItemDelegate.h"
#include "CRETreeItemQuest.h"
#include "CREMapInformation.h"

CREQuestPanel::CREQuestPanel(QuestManager* manager)
{
    Q_ASSERT(manager);
    myQuestManager = manager;

    QVBoxLayout* main = new QVBoxLayout(this);
    QTabWidget* tab = new QTabWidget(this);
    main->addWidget(tab);

    QWidget* details = new QWidget(this);
    tab->addTab(details, tr("Details"));

    QGridLayout* layout = new QGridLayout(details);

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


    layout->addWidget(new QLabel(tr("Steps:"), this), line++, 1, 1, 2);

    myStepsModel = new CREQuestItemModel(this);
    CREMultilineItemDelegate* delegate = new CREMultilineItemDelegate(this);
    mySteps = new QTreeView(this);
    mySteps->setRootIsDecorated(false);
    mySteps->setWordWrap(true);
    mySteps->setModel(myStepsModel);
    mySteps->setItemDelegateForColumn(1, delegate);
    mySteps->setItemDelegateForColumn(3, delegate);
    mySteps->setSelectionMode(QAbstractItemView::SingleSelection);

    layout->addWidget(mySteps, line++, 1, 1, 2);

    QHBoxLayout* buttons = new QHBoxLayout();

    QPushButton* add = new QPushButton(tr("add step"), this);
    connect(add, SIGNAL(clicked(bool)), myStepsModel, SLOT(addStep(bool)));
    buttons->addWidget(add);
    QPushButton* del = new QPushButton(tr("remove step"), this);
    connect(del, SIGNAL(clicked(bool)), this, SLOT(deleteStep(bool)));
    buttons->addWidget(del);

    QPushButton* up = new QPushButton(tr("move up"), this);
    connect(up, SIGNAL(clicked(bool)), this, SLOT(moveUp(bool)));
    buttons->addWidget(up);
    QPushButton* down = new QPushButton(tr("move down"), this);
    connect(down, SIGNAL(clicked(bool)), this, SLOT(moveDown(bool)));
    buttons->addWidget(down);

    layout->addLayout(buttons, line++, 1, 1, 2);

    myUse = new QTreeWidget(this);
    tab->addTab(myUse, tr("Use"));
    myUse->setHeaderLabel(tr("Map"));

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

    myUse->clear();
    foreach(CREMapInformation* map, quest->maps())
    {
        new QTreeWidgetItem(myUse, QStringList(map->path()));
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
    if (myQuestManager->getQuestFile(myQuest).isEmpty())
        myQuestManager->setQuestFile(myQuest, myFile->currentText());
    if (myParent->currentIndex() == 0)
    {
        myQuest->setParent(NULL);
    }
    else
        myQuest->setParent(myQuestManager->findByCode(myParent->currentText()));
}

void CREQuestPanel::displaySteps()
{
    myStepsModel->setQuest(myQuest);
}

void CREQuestPanel::deleteStep(bool)
{
    if (myQuest == NULL)
        return;

    if (!mySteps->currentIndex().isValid())
        return;

    myStepsModel->removeRow(mySteps->currentIndex().row());
}

void CREQuestPanel::moveUp(bool)
{
    if (myQuest == NULL)
        return;

    if (!mySteps->currentIndex().isValid())
        return;

    myStepsModel->moveUp(mySteps->currentIndex().row());
}

void CREQuestPanel::moveDown(bool)
{
    if (myQuest == NULL)
        return;

    if (!mySteps->currentIndex().isValid())
        return;

    myStepsModel->moveDown(mySteps->currentIndex().row());
}
