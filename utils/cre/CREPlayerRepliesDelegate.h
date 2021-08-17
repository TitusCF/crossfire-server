#ifndef CREPLAYERREPLIESDELEGATE_H
#define CREPLAYERREPLIESDELEGATE_H

#include <QStyledItemDelegate>

/**
 * Delegate handling edition of player replies in message file.
 */
class CREPlayerRepliesDelegate : public QStyledItemDelegate
{
public:
    CREPlayerRepliesDelegate(QObject* parent);
    virtual ~CREPlayerRepliesDelegate();

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:

};

#endif /* CREPLAYERREPLIESDELEGATE_H */

