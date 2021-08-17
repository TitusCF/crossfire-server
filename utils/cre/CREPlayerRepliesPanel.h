#ifndef CREPLAYERREPLIESPANEL_H
#define CREPLAYERREPLIESPANEL_H

#include <QDialog>

class QTableWidget;

/**
 * Display and edition of player replies to NPC messages.
 */
class CREPlayerRepliesPanel : public QDialog
{
    Q_OBJECT

public:
    CREPlayerRepliesPanel(QWidget* parent);
    virtual ~CREPlayerRepliesPanel();

    void setData(const QList<QStringList>& data);
    QList<QStringList> data() const;

private slots:
    void onAddCondition(bool);
    void onDeleteCondition(bool);
    void onReset(bool);

private:
    void addItem(const QStringList& item);

    QTableWidget* myReplies;
    QList<QStringList> myOriginal;
};

#endif /* CREPLAYERREPLIESPANEL_H */
