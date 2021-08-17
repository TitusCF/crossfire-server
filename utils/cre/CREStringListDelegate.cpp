#include "CREStringListDelegate.h"
#include "CREStringListPanel.h"

CREStringListDelegate::CREStringListDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

CREStringListDelegate::~CREStringListDelegate()
{
}

QWidget* CREStringListDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    return new CREStringListPanel(parent);
}

void CREStringListDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    CREStringListPanel* edit = qobject_cast<CREStringListPanel*>(editor);
    if (!edit)
        return;
    edit->setData(index.data(Qt::EditRole).value<QStringList>());
}

void CREStringListDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    CREStringListPanel* edit = qobject_cast<CREStringListPanel*>(editor);
    if (!edit)
        return;
    model->setData(index, QVariant::fromValue(edit->data()), Qt::EditRole);
}

void CREStringListDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem&, const QModelIndex&) const
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
