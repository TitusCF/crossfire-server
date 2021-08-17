#include <QtWidgets>
#include "CREMessagePanel.h"
#include "CREFilterDefinition.h"
#include "MessageFile.h"
#include "CREMapInformation.h"
#include "MessageManager.h"
#include "CREMessageItemModel.h"
#include "CREMultilineItemDelegate.h"
#include "CREPrePostConditionDelegate.h"
#include "CREPlayerRepliesDelegate.h"
#include "CREStringListDelegate.h"

CREMessagePanel::CREMessagePanel(const MessageManager* manager, const QuestManager* quests, QWidget* parent) : CRETPanel(parent)
{
    Q_ASSERT(manager != NULL);
    myMessageManager = manager;

    QVBoxLayout* main = new QVBoxLayout(this);
    QTabWidget* tab = new QTabWidget(this);
    main->addWidget(tab);

    QWidget* details = new QWidget(this);
    tab->addTab(details, tr("Details"));

    QGridLayout* layout = new QGridLayout(details);

    int line = 0;

    layout->addWidget(new QLabel(tr("Path:"), this), line, 0);
    myPath = new QLineEdit(this);
    layout->addWidget(myPath, line++, 1);

    layout->addWidget(new QLabel(tr("Location:"), this), line, 0);
    myLocation = new QLineEdit(this);
    layout->addWidget(myLocation, line++, 1);

    layout->addWidget(new QLabel(tr("Message rules (blue: uses token set by current rule as pre-condition, red: rule that sets token for pre-condition of current rule)")), line++, 0, 1, 2);

    myModel = new CREMessageItemModel(this);
    myRules = new QTableView();
    myRules->setWordWrap(true);
    myRules->setModel(myModel);
    myRules->setSelectionMode(QAbstractItemView::SingleSelection);
    myRules->setSelectionBehavior(QAbstractItemView::SelectRows);
    myRules->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    myRules->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    myRules->setItemDelegateForColumn(0, new CREMultilineItemDelegate(myRules, true, true));
    myRules->setItemDelegateForColumn(1, new CREPrePostConditionDelegate(myRules, true, manager, quests));
    myRules->setItemDelegateForColumn(2, new CREPlayerRepliesDelegate(myRules));
    myRules->setItemDelegateForColumn(3, new CREStringListDelegate(myRules));
    myRules->setItemDelegateForColumn(4, new CREPrePostConditionDelegate(myRules, false, manager, quests));
    myRules->setItemDelegateForColumn(5, new CREMultilineItemDelegate(myRules, true, true));
    myRules->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(myRules->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentRowChanged(const QModelIndex&, const QModelIndex&)));
    layout->addWidget(myRules, line++, 0, 1, 2);

    QHBoxLayout* buttons = new QHBoxLayout();

    QPushButton* add = new QPushButton(tr("insert rule"), this);
    buttons->addWidget(add);
    connect(add, SIGNAL(clicked(bool)), this, SLOT(onAddRule(bool)));
    QPushButton* remove = new QPushButton(tr("remove rule"), this);
    buttons->addWidget(remove);
    connect(remove, SIGNAL(clicked(bool)), this, SLOT(onDeleteRule(bool)));

    QPushButton* up = new QPushButton(tr("move up"), this);
    buttons->addWidget(up);
    connect(up, SIGNAL(clicked(bool)), this, SLOT(onMoveUp(bool)));
    QPushButton* down = new QPushButton(tr("move down"), this);
    buttons->addWidget(down);
    connect(down, SIGNAL(clicked(bool)), this, SLOT(onMoveDown(bool)));

    QPushButton* copy = new QPushButton(tr("copy"), this);
    buttons->addWidget(copy);
    connect(copy, SIGNAL(clicked(bool)), this, SLOT(onDuplicate(bool)));

    QPushButton* reset = new QPushButton(tr("reset all changes"), this);
    buttons->addWidget(reset);
    connect(reset, SIGNAL(clicked(bool)), this, SLOT(onReset(bool)));

    layout->addLayout(buttons, line++, 0, 1, 2);

    myUse = new QTreeWidget(this);
    tab->addTab(myUse, tr("Use"));
    myUse->setHeaderLabel(tr("Referenced by..."));

    myMessage = nullptr;
    myOriginal = nullptr;
}

CREMessagePanel::~CREMessagePanel()
{
    delete myOriginal;
}

void CREMessagePanel::setItem(MessageFile* message)
{
    myPath->setText(message->path());
    /* can only change path when new file is created */
    myPath->setEnabled(message->path() == "<new file>");
    myLocation->setText(message->location());

    /* so the change handler won't do anything */
    myMessage = NULL;
    myModel->setMessage(message);
    myMessage = message;

    myUse->clear();

    QTreeWidgetItem* root = NULL;
    if (myMessage->maps().length() > 0)
    {
        root = new QTreeWidgetItem(myUse, QStringList(tr("Maps")));
        root->setExpanded(true);
        foreach(CREMapInformation* map, myMessage->maps())
        {
            new QTreeWidgetItem(root, QStringList(map->path()));
        }
        root = NULL;
    }

    foreach(MessageFile* file, myMessageManager->messages())
    {
        bool got = false;
        foreach(MessageRule* rule, file->rules())
        {
            QStringList includes = rule->include();
            foreach(QString include, includes)
            {
                if (include.isEmpty())
                    continue;

                if (!include.startsWith('/'))
                {
                    int last = file->path().lastIndexOf(QDir::separator());
                    if (last == -1)
                        continue;
                    include = file->path().left(last + 1) + include;
                }

                if (include == message->path())
                {
                    if (root == NULL)
                    {
                        root = new QTreeWidgetItem(myUse, QStringList(tr("Messages")));
                        root->setExpanded(true);
                    }

                    new QTreeWidgetItem(root, QStringList(file->path()));

                    got = true;
                    break;
                }

                if (got)
                    break;
            }
        }

        if (got)
            break;
    }

    delete myOriginal;
    myOriginal = myMessage->duplicate();
}

void CREMessagePanel::currentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    myModel->setSelectedRule(current);
}

void CREMessagePanel::onAddRule(bool)
{
    int row = myRules->selectionModel()->currentIndex().row() + 1;
    myModel->insertRows(row, 1);
    myRules->selectionModel()->select(myModel->index(row, 0, QModelIndex()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void CREMessagePanel::onDeleteRule(bool)
{
    myModel->removeRows(myRules->selectionModel()->currentIndex().row(), 1);
}

void CREMessagePanel::commitData()
{
    myMessage->setPath(myPath->text());
    myMessage->setLocation(myLocation->text());
}

void CREMessagePanel::onMoveUp(bool)
{
    int index = myRules->currentIndex().row();
    if (index <= 0 || index >= myMessage->rules().size())
        return;

    myModel->moveUpDown(index, true);
    myRules->setCurrentIndex(myModel->index(index - 1, 0, QModelIndex()));
}

void CREMessagePanel::onMoveDown(bool)
{
    int index = myRules->currentIndex().row();
    if (index < 0 || index >= myMessage->rules().size() - 1)
        return;

    myModel->moveUpDown(index, false);
    myRules->setCurrentIndex(myModel->index(index + 1, 0, QModelIndex()));
}

void CREMessagePanel::onDuplicate(bool)
{
    int index = myRules->currentIndex().row();
    if (index < 0 || index >= myMessage->rules().size())
        return;

    myModel->duplicateRow(index);
    myRules->setCurrentIndex(myModel->index(index + 1, 0, QModelIndex()));
}

void CREMessagePanel::onReset(bool)
{
    if (!myMessage)
        return;
    if (QMessageBox::question(this, "Confirm reset", "Reset message to its initial values?") != QMessageBox::StandardButton::Yes)
        return;

    myMessage->copy(myOriginal);
    setItem(myMessage);
}
