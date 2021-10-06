#include "CREQuestPanel.h"
#include "CRERegionPanel.h"
#include "CREQuestItemModel.h"
#include "CREMultilineItemDelegate.h"
#include "CRETreeItemQuest.h"
#include "CREMapInformationManager.h"
#include "CREMapInformation.h"
#include "MessageManager.h"
#include "CREMessagePanel.h"
#include "MessageFile.h"
#include "assets.h"
#include "AssetsManager.h"
#include "FaceComboBox.h"
#include "ResourcesManager.h"
#include "CREPrePostList.h"
#include "CREPrePostConditionDelegate.h"

CREQuestPanel::CREQuestPanel(CREMapInformationManager* mapManager, MessageManager* messageManager, ResourcesManager *resources, QWidget* parent) : CRETPanel(parent)
{
    Q_ASSERT(mapManager);
    Q_ASSERT(messageManager);
    myMapManager = mapManager;
    myMessageManager = messageManager;
    myResources = resources;

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

    layout->addWidget(new QLabel(tr("Face:"), this), line, 1);
    myFace = new FaceComboBox(this, true);
    layout->addWidget(myFace, line++, 2);

    myCanRestart = new QCheckBox(tr("this quest can be done multiple times"));
    layout->addWidget(myCanRestart, line++, 1, 1, 2);

    myIsSystem = new QCheckBox(tr("System quest, not listed to players"));
    layout->addWidget(myIsSystem, line++, 1, 1, 2);

    layout->addWidget(new QLabel(tr("Parent:"), this), line, 1);
    myParent = new QComboBox(this);
    layout->addWidget(myParent, line++, 2);
    myParent->addItem(tr("(none)"));

    QStringList codes;
    getManager()->quests()->each([&] (auto quest) { codes.append(quest->quest_code); });
    codes.sort();
    myParent->addItems(codes);

    layout->addWidget(new QLabel(tr("Quest file:"), this), line, 1);
    myFile = new QComboBox(this);
    layout->addWidget(myFile, line++, 2);

    myFile->setInsertPolicy(QComboBox::InsertAlphabetically);
    myFile->setEditable(true);
    myFile->addItem("");
//    QStringList files = myQuestManager->getFiles();
    //files.sort();
//    myFile->addItems(files);

    QTabWidget *dc = new QTabWidget(details);
    myDescription = new QTextEdit(this);
    dc->addTab(myDescription, "Player description");
    myComment = new QPlainTextEdit(this);
    dc->addTab(myComment, "Developer description");

    layout->addWidget(dc, line++, 1, 1, 2);

    layout->addWidget(new QLabel(tr("Steps:"), this), line++, 1, 1, 2);

    myStepsModel = new CREQuestItemModel(this);
    connect(myStepsModel, SIGNAL(questModified(quest_definition *)), resources, SLOT(questModified(quest_definition *)));
    mySteps = new QTreeView(this);
    mySteps->setRootIsDecorated(false);
    mySteps->setWordWrap(true);
    mySteps->setModel(myStepsModel);
    mySteps->setItemDelegateForColumn(3, new CREPrePostConditionDelegate(mySteps, CREPrePostList::SetWhen, messageManager));
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
    myUse->setHeaderLabel(tr("Used by..."));

    myQuest = NULL;
    myCurrentStep = NULL;
}

CREQuestPanel::~CREQuestPanel()
{
}

void CREQuestPanel::setItem(quest_definition *quest)
{
    myQuest = quest;
    myCurrentStep = NULL;

    myCode->setText(quest->quest_code);
    myTitle->setText(quest->quest_title);
    myFace->setFace(quest->face);
    myCanRestart->setChecked(quest->quest_restart);
    myIsSystem->setChecked(quest->quest_is_system);
    myDescription->setText(quest->quest_description);
    myComment->setPlainText(quest->quest_comment);

    auto origin = myResources->originOfQuest(myQuest);
    myFile->setEditText(QString::fromStdString(origin));
    myFile->setEnabled(origin.empty());

    if (quest->parent)
    {
        int idx = myParent->findText(quest->parent->quest_code);
        if (idx != -1)
            myParent->setCurrentIndex(idx);
    }
    else
        myParent->setCurrentIndex(0);

    displaySteps();

    myUse->clear();
    QTreeWidgetItem* root = NULL;
    auto maps = myMapManager->getMapsForQuest(quest);
    if (!maps.empty())
    {
        root = new QTreeWidgetItem(myUse, QStringList(tr("Maps")));
        root->setExpanded(true);
        foreach(CREMapInformation* map, maps)
        {
            new QTreeWidgetItem(root, QStringList(map->path()));
        }
        root = NULL;
    }

    foreach(MessageFile* message, myMessageManager->messages())
    {
        bool got = false;
        foreach(MessageRule* rule, message->rules())
        {
            QList<QStringList> conditions = rule->preconditions();
            conditions.append(rule->postconditions());
            foreach(QStringList list, conditions)
            {
                if (list.size() > 1 && (list[0] == "quest" || list[0] == "questdone") && list[1] == quest->quest_code)
                {
                    if (root == NULL)
                    {
                        root = new QTreeWidgetItem(myUse, QStringList(tr("Messages")));
                        root->setExpanded(true);
                    }

                    new QTreeWidgetItem(root, QStringList(message->path()));
                    got = true;
                    break;
                }
            }

            if (got)
                break;
        }
    }
}

void CREQuestPanel::commitData()
{
    if (!myQuest)
        return;

    FREE_AND_COPY(myQuest->quest_code, myCode->text().toStdString().data());
    FREE_AND_COPY(myQuest->quest_title, myTitle->text().toStdString().data());
    myQuest->face = myFace->face();
    myQuest->quest_restart = myCanRestart->isChecked();
    myQuest->quest_is_system = myIsSystem->isChecked();
    FREE_AND_COPY(myQuest->quest_description, myDescription->toPlainText().toStdString().data());
    FREE_AND_COPY(myQuest->quest_comment, myComment->toPlainText().trimmed().toStdString().data());
    /*
    if (myQuestManager->getQuestFile(myQuest).isEmpty())
        myQuestManager->setQuestFile(myQuest, myFile->currentText());
     */
    if (myParent->currentIndex() == 0)
    {
        myQuest->parent = NULL;
    }
    else
        myQuest->parent = getManager()->quests()->get(myParent->currentText().toStdString());

    myResources->questModified(myQuest);
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
