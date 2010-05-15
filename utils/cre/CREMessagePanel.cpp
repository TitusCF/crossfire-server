#include <QtGui>
#include "CREMessagePanel.h"
#include "MessageFile.h"

CREMessagePanel::CREMessagePanel()
{
    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(new QLabel(tr("Location:"), this), 0, 0);
    myLocation = new QLineEdit(this);
    layout->addWidget(myLocation, 0, 1);

    QGroupBox* box = new QGroupBox(tr("Rules"));
    layout->addWidget(box, 1, 0, 1, 2);

    QGridLayout* rules = new QGridLayout();
    box->setLayout(rules);

    myRules = new QTreeWidget();
    rules->addWidget(myRules, 0, 0, 4, 1);
    QStringList labels;
    labels << tr("match") << tr("pre") << tr("message") << tr("post") << tr("replies") << tr("include");
    myRules->setHeaderLabels(labels);

    myMessage = NULL;
}

CREMessagePanel::~CREMessagePanel()
{
}

QString toDisplay(const QList<QStringList>& list)
{
    QStringList data;
    foreach(QStringList item, list)
        data.append(item.join(" "));
    return data.join("\n");
}

void CREMessagePanel::setMessage(MessageFile* message)
{
    myMessage = message;

    myLocation->setText(message->location());

    myRules->clear();
    foreach(MessageRule* rule, myMessage->rules())
    {
        QStringList data;
        data << rule->match().join("\n");
        data << toDisplay(rule->preconditions());
        data << rule->messages().join("\n");
        data << toDisplay(rule->postconditions());
        data << toDisplay(rule->replies());
        data << rule->include();
        new QTreeWidgetItem(myRules, data);
    }
}
