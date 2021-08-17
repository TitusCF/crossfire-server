#ifndef CRESTRINGLISTDELEGATE_H
#define CRESTRINGLISTDELEGATE_H

#include <QtWidgets>

/**
 * Delegate allowing edition of a list of strings.
 */
class CREStringListDelegate : public QStyledItemDelegate
{
public:
    CREStringListDelegate(QObject* parent);
    virtual ~CREStringListDelegate();

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif /* CRESTRINGLISTDELEGATE_H */
