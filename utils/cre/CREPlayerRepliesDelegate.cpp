#include "CREPlayerRepliesDelegate.h"
#include "CREPlayerRepliesPanel.h"

CREPlayerRepliesDelegate::CREPlayerRepliesDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

CREPlayerRepliesDelegate::~CREPlayerRepliesDelegate()
{
}

QWidget* CREPlayerRepliesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    return new CREPlayerRepliesPanel(parent);
}

void CREPlayerRepliesDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    CREPlayerRepliesPanel* edit = qobject_cast<CREPlayerRepliesPanel*>(editor);
    if (!edit)
        return;
    edit->setData(index.data(Qt::EditRole).value<QList<QStringList>>());
}

void CREPlayerRepliesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    CREPlayerRepliesPanel* edit = qobject_cast<CREPlayerRepliesPanel*>(editor);
    if (!edit)
        return;
    model->setData(index, QVariant::fromValue(edit->data()), Qt::EditRole);
}

void CREPlayerRepliesDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem&, const QModelIndex&) const
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
