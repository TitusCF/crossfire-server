#ifndef CREGENERALMESSAGEPANEL_H
#define CREGENERALMESSAGEPANEL_H

#include "CREPanel.h"

class GeneralMessage;
class QLineEdit;

class CREGeneralMessagePanel : public CRETPanel<const GeneralMessage>
{
public:
    CREGeneralMessagePanel(QWidget* parent);
    virtual void setItem(const GeneralMessage* item);

private:
    QLineEdit* myTitle;
    QLineEdit* myIdentifier;
    QLineEdit* myQuest;
    QLineEdit* myChance;
    QTextEdit* myBody;
};

#endif /* CREGENERALMESSAGEPANEL_H */

