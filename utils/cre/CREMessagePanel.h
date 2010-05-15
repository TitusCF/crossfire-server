#ifndef _CREMESSAGEPANEL_H
#define	_CREMESSAGEPANEL_H

#include <QObject>
#include <QBrush>
#include "CREPanel.h"

class MessageFile;
class QLineEdit;
class QTreeWidget;
class QTextEdit;
class QTreeWidgetItem;

class CREMessagePanel : public CREPanel
{
    Q_OBJECT

    public:
        CREMessagePanel();
        virtual ~CREMessagePanel();

        void setMessage(MessageFile* message);
    private:
        MessageFile* myMessage;
        QLineEdit* myLocation;
        QTreeWidget* myRules;
        QBrush myDefaultBackground;
        //QTextEdit

    private slots:
        void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
};

#endif	/* _CREMESSAGEPANEL_H */

