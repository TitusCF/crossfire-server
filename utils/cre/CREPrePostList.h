#ifndef CREPREPOSTLIST_H
#define CREPREPOSTLIST_H

#include <QDialog>

class QListWidget;
class MessageManager;

/**
 * Display and allow edition of a list of pre- or post- conditions for a NPC message,
 * or quest states for a quest step.
 */
class CREPrePostList : public QDialog
{
    Q_OBJECT

public:
    enum Mode { PreConditions, PostConditions, SetWhen };

    CREPrePostList(QWidget* parent, Mode mode, const MessageManager* manager);
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
    Mode myMode;
};

#endif /* CREPREPOSTLIST_H */

