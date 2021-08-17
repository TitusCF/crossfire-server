#ifndef CREPREPOSTLIST_H
#define CREPREPOSTLIST_H

#include <QDialog>

class QListWidget;
class MessageManager;
class QuestManager;

/**
 * Display and allow edition of a list of pre- or post- conditions for a NPC message.
 */
class CREPrePostList : public QDialog
{
    Q_OBJECT

public:
    CREPrePostList(QWidget* parent, bool isPre, const MessageManager* manager, const QuestManager* quests);
    virtual ~CREPrePostList();

    QList<QStringList> data() const;
    void setData(const QList<QStringList>& data);

private slots:
    void onAddCondition(bool);
    void onDeleteCondition(bool);
    void onReset(bool);

private:
    void addItem(const QStringList& item);
    QListWidget* myList;
    QList<QStringList> myOriginal;
};

#endif /* CREPREPOSTLIST_H */

