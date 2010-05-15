#ifndef _CREMESSAGEPANEL_H
#define	_CREMESSAGEPANEL_H

#include <QObject>
#include "CREPanel.h"

class MessageFile;
class QLineEdit;
class QTreeWidget;
class QTextEdit;

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
        //QTextEdit
};

#endif	/* _CREMESSAGEPANEL_H */

