#include <QtWidgets>
#include "CREPrePostConditionDelegate.h"
#include "CREMessageItemModel.h"
#include "CREPrePostPanel.h"
#include "CREPrePostList.h"

CREPrePostSingleConditionDelegate::CREPrePostSingleConditionDelegate(QObject* parent, bool isPre, const MessageManager* manager, const QuestManager* quests) :
  QStyledItemDelegate(parent), myIsPre(isPre), myMessages(manager), myQuests(quests)
{
}

QWidget* CREPrePostSingleConditionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    return new CREPrePostPanel(myIsPre, myIsPre ? myMessages->preConditions() : myMessages->postConditions(), myQuests, parent);
}

void CREPrePostSingleConditionDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    CREPrePostPanel* edit = qobject_cast<CREPrePostPanel*>(editor);
    if (!edit)
        return;
    edit->setData(index.data(Qt::UserRole).value<QStringList>());
}

void CREPrePostSingleConditionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    CREPrePostPanel* edit = qobject_cast<CREPrePostPanel*>(editor);
    if (!edit)
        return;
    model->setData(index, edit->getData(), Qt::UserRole);
    model->setData(index, edit->getData().join(" "), Qt::DisplayRole);
}

void CREPrePostSingleConditionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem&, const QModelIndex&) const
{
    if (!qApp->activeWindow())
        return;
    QRect r(qApp->activeWindow()->geometry());
    r.setLeft(r.left() + r.width() * .1);
    r.setWidth(r.width() * .8);
    r.setTop(r.top() + r.height() * .1);
    r.setHeight(r.height() * .8);
    editor->setGeometry(r);
}


CREPrePostConditionDelegate::CREPrePostConditionDelegate(QObject* parent, bool isPre, const MessageManager* manager, const QuestManager* quests)
 : QStyledItemDelegate(parent), myIsPre(isPre), myMessages(manager), myQuests(quests)
{
}

CREPrePostConditionDelegate::~CREPrePostConditionDelegate()
{
}

QWidget* CREPrePostConditionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    return new CREPrePostList(parent, myIsPre, myMessages, myQuests);
}

void CREPrePostConditionDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (!index.isValid())
        return;
    CREPrePostList* list = qobject_cast<CREPrePostList*>(editor);
    if (list == NULL)
        return;

    QVariant data = index.model()->data(index, Qt::EditRole);
    if (!data.canConvert<QList<QStringList>>())
        return;

    list->setData(data.value<QList<QStringList>>());
}

void CREPrePostConditionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (!index.isValid())
        return;
    CREPrePostList* list = qobject_cast<CREPrePostList*>(editor);
    if (list == NULL)
        return;

    model->setData(index, QVariant::fromValue(list->data()));
}

void CREPrePostConditionDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem&, const QModelIndex&) const
{
    if (!qApp->activeWindow())
        return;
    QRect r(qApp->activeWindow()->geometry());
    r.setLeft(r.left() + r.width() * .1);
    r.setWidth(r.width() * .8);
    r.setTop(r.top() + r.height() * .1);
    r.setHeight(r.height() * .8);
    editor->setGeometry(r);
}
