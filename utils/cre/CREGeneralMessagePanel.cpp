#include <Qt>
#include <QtWidgets>

#include "CREGeneralMessagePanel.h"

#define LINE(label, exp) \
    layout->addWidget(new QLabel(tr(label), this), line, 0); \
    layout->addWidget(exp, line++, 1);

CREGeneralMessagePanel::CREGeneralMessagePanel(QWidget* parent) : CRETPanel(parent)
{
    QGridLayout* layout = new QGridLayout(this);
    setLayout(layout);

    int line = 0;

    LINE("Title:", myTitle = new QLineEdit(this));
    LINE("Identifier:", myIdentifier = new QLineEdit(this));
    LINE("Quest:", myQuest = new QLineEdit(this));
    LINE("Chance:", myChance = new QLineEdit(this));

    layout->addWidget(new QLabel(tr("Message:"), this), line++, 0, 1, 2);
    layout->addWidget(myBody = new QTextEdit(this), line++, 0, 1, 2);

    myTitle->setReadOnly(true);
    myIdentifier->setReadOnly(true);
    myQuest->setReadOnly(true);
    myChance->setReadOnly(true);
    myBody->setReadOnly(true);
}

void CREGeneralMessagePanel::setItem(const GeneralMessage* message)
{
    myTitle->setText(message->title ? message->title : "");
    myIdentifier->setText((message->identifier && message->identifier[0] != '\n') ? message->identifier : "");
    myQuest->setText(message->quest_code);
    myChance->setText(QString::number(message->chance));
    myBody->setText(message->message);
}
