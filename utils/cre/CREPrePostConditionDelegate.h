#ifndef CREPREPOSTCONDITIONDELEGATE_H
#define CREPREPOSTCONDITIONDELEGATE_H

class MessageManager;
class QuestManager;

/**
 * Allow edition a single pre- or post condition.
 */
class CREPrePostSingleConditionDelegate : public QStyledItemDelegate
{
public:  
    /**
     * Standard constructor.
     * @param parent parent of this item.
     * @param isPre true for pre-conditions, false for post-conditions.
     * @param manager messages manager.
     * @param quests quests manager.
     */
    CREPrePostSingleConditionDelegate(QObject* parent, bool isPre, const MessageManager* manager, const QuestManager* quests);

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    bool myIsPre;
    const MessageManager* myMessages;
    const QuestManager* myQuests;
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
     * @param isPre true for pre-conditions, false for post-conditions.
     * @param manager messages manager.
     * @param quests quests manager.
     */
    CREPrePostConditionDelegate(QObject* parent, bool isPre, const MessageManager* manager, const QuestManager* quests);
    virtual ~CREPrePostConditionDelegate();

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    bool myIsPre;                     /**< True if doing pre conditions, false for post conditions. */
    const MessageManager* myMessages; /**< Available message scripts. */
    const QuestManager* myQuests;     /**< Available quests. */
};

#endif /* CREPREPOSTCONDITIONDELEGATE_H */
