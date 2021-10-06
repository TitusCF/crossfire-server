#ifndef CREPREPOSTCONDITIONDELEGATE_H
#define CREPREPOSTCONDITIONDELEGATE_H

#include "CREPrePostList.h"

class MessageManager;

/**
 * Allow edition a single pre- or post condition.
 */
class CREPrePostSingleConditionDelegate : public QStyledItemDelegate
{
public:  
    /**
     * Standard constructor.
     * @param parent parent of this item.
     * @param mode list mode to use.
     * @param manager messages manager.
     */
    CREPrePostSingleConditionDelegate(QObject* parent, CREPrePostList::Mode mode, const MessageManager* manager);

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    CREPrePostList::Mode myMode;
    const MessageManager* myMessages;
};

/**
 * Allow edition of a pre- or post- list in the message table.
 */
class CREPrePostConditionDelegate : public QStyledItemDelegate
{
public:
    /**
     * Standard constructor.
     * @param parent parent of this item.
     * @param mode list mode to use.
     * @param manager messages manager.
     */
    CREPrePostConditionDelegate(QObject* parent, CREPrePostList::Mode mode, const MessageManager* manager);
    virtual ~CREPrePostConditionDelegate();

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    CREPrePostList::Mode myMode;  /**< List mode we're working in. */
    const MessageManager* myMessages; /**< Available message scripts. */
};

#endif /* CREPREPOSTCONDITIONDELEGATE_H */
