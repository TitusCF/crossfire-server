#include <QTextEdit>
#include <qstyleditemdelegate.h>
#include "CREMultilineItemDelegate.h"
#include "CREQuestItemModel.h"

CREMultilineItemDelegate::CREMultilineItemDelegate(QObject* parent, bool asStringList, bool trimEmpty)
: QStyledItemDelegate(parent), myAsStringList(asStringList), myTrimEmpty(trimEmpty)
{
}

CREMultilineItemDelegate::~CREMultilineItemDelegate()
{
}

QWidget* CREMultilineItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
    QTextEdit* edit = new QTextEdit(parent);
    edit->setAcceptRichText(false);
    return edit;
}

void CREMultilineItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (!index.isValid())
        return;
    QTextEdit* edit = qobject_cast<QTextEdit*>(editor);
    if (edit == NULL)
        return;

    if (myAsStringList)
        edit->setPlainText(index.model()->data(index, Qt::EditRole).value<QStringList>().join("\n"));
    else
        edit->setPlainText(index.model()->data(index, Qt::EditRole).toString());
}

QStringList convert(const QString& source, bool trimEmpty)
{
    QStringList value;
    for (QString line : source.split("\n"))
    {
        if (!line.isEmpty() || !trimEmpty)
            value.append(line);
    }
    return value;
}

void CREMultilineItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (!index.isValid())
        return;
    QTextEdit* edit = qobject_cast<QTextEdit*>(editor);
    if (edit == NULL)
        return;

    if (myAsStringList)
        model->setData(index, convert(edit->toPlainText(), myTrimEmpty));
    else
        model->setData(index, edit->toPlainText());
}
