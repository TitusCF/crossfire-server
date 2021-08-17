#include <QtWidgets>
#include "CREPrePostList.h"
#include "CREPrePostConditionDelegate.h"

CREPrePostList::CREPrePostList(QWidget* parent, bool isPre, const MessageManager* manager, const QuestManager* quests)
{
    setModal(true);
    setWindowTitle(isPre ? tr("Message pre-conditions") : tr("Message post-conditions"));

    myList = new QListWidget(parent);
    myList->setItemDelegate(new CREPrePostSingleConditionDelegate(myList, isPre, manager, quests));

    QPushButton* addCondition = new QPushButton(tr("add"), this);
    connect(addCondition, SIGNAL(clicked(bool)), this, SLOT(onAddCondition(bool)));

    QPushButton* delCondition = new QPushButton(tr("delete"), this);
    connect(delCondition, SIGNAL(clicked(bool)), this, SLOT(onDeleteCondition(bool)));

    QPushButton* reset = new QPushButton(tr("reset changes"), this);
    connect(reset, SIGNAL(clicked(bool)), this, SLOT(onReset(bool)));

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons->addWidget(addCondition);
    buttons->addWidget(delCondition);
    buttons->addWidget(reset);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(myList);
    l->addLayout(buttons);
    setLayout(l);
}

CREPrePostList::~CREPrePostList()
{
}

QList<QStringList> CREPrePostList::data() const
{
    QList<QStringList> value;
    for (int i = 0; i < myList->count(); i++)
    {
        QListWidgetItem* wi = myList->item(i);
        value.append(wi->data(Qt::UserRole).value<QStringList>());
    }
    return value;
}

void CREPrePostList::addItem(const QStringList& item)
{
    QListWidgetItem* wi = new QListWidgetItem(item.join(" "));
    wi->setData(Qt::UserRole, QVariant::fromValue(item));
    wi->setFlags(wi->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    myList->addItem(wi);
}

void CREPrePostList::setData(const QList<QStringList>& data)
{
    myList->clear();
    for (QStringList item : data)
    {
        addItem(item);
    }
    myOriginal = data;
}

void CREPrePostList::onAddCondition(bool)
{
    QStringList item;
    item << "quest";
    addItem(item);
    myList->setCurrentRow(myList->count() - 1);
    myList->edit(myList->currentIndex());
}

void CREPrePostList::onDeleteCondition(bool)
{
    if (myList->currentRow() == -1)
        return;

    delete myList->takeItem(myList->currentRow());
}

void CREPrePostList::onReset(bool)
{
    if (QMessageBox::question(this, "Confirm reset", "Reset the conditions to their initial values, losing all changes?") != QMessageBox::StandardButton::Yes)
        return;
    setData(myOriginal);
}
