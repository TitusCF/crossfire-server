#ifndef _CREMESSAGEPANEL_H
#define _CREMESSAGEPANEL_H

#include <QObject>
#include <QBrush>
#include "CREPanel.h"

class MessageFile;
class MessageRule;
class QLineEdit;
class QTableView;
class QTextEdit;
class QTableWidgetItem;
class CRERulePanel;
class MessageManager;
class QuestManager;
class CREMessageItemModel;

/**
 * Display information about a NPC message file, and allow edition.
 */
class CREMessagePanel : public CRETPanel<MessageFile>
{
    Q_OBJECT

    public:
        CREMessagePanel(const MessageManager* manager, const QuestManager* quests, QWidget* parent);
        virtual ~CREMessagePanel();

        virtual void setItem(MessageFile* message);
        virtual void commitData();

    protected:

    private:
        const MessageManager* myMessageManager;
        MessageFile* myMessage;
        MessageFile* myOriginal;
        QLineEdit* myPath;
        QLineEdit* myLocation;
        QTableView* myRules;
        CREMessageItemModel* myModel;
        QBrush myDefaultBackground;
        QTreeWidget* myUse;

    private slots:
        void currentRowChanged(const QModelIndex& current, const QModelIndex& previous);
        void onAddRule(bool);
        void onDeleteRule(bool);
        void onMoveUp(bool);
        void onMoveDown(bool);
        void onDuplicate(bool);
        void onReset(bool);
};

#endif /* _CREMESSAGEPANEL_H */
