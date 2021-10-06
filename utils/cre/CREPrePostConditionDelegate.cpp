#include <QtWidgets>
#include "CREPrePostConditionDelegate.h"
#include "CREMessageItemModel.h"
#include "CREPrePostPanel.h"

CREPrePostSingleConditionDelegate::CREPrePostSingleConditionDelegate(QObject* parent, CREPrePostList::Mode mode, const MessageManager* manager) :
  QStyledItemDelegate(parent), myMode(mode), myMessages(manager)
{
}

QWidget* CREPrePostSingleConditionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    return new CREPrePostPanel(myMode, myMode == CREPrePostList::PreConditions ? myMessages->preConditions() : myMessages->postConditions(), parent);
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
    QStringList data = edit->getData();
    model->setData(index, data, Qt::UserRole);
    if (myMode == CREPrePostList::SetWhen)
        data.pop_front();
    model->setData(index, data.join(" "), Qt::DisplayRole);
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


CREPrePostConditionDelegate::CREPrePostConditionDelegate(QObject* parent, CREPrePostList::Mode mode, const MessageManager* manager)
 : QStyledItemDelegate(parent), myMode(mode), myMessages(manager)
{
}

CREPrePostConditionDelegate::~CREPrePostConditionDelegate()
{
}

QWidget* CREPrePostConditionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    return new CREPrePostList(parent, myMode, myMessages);
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
