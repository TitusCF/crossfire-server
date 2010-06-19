#include "CREPrePostPanel.h"
#include "CRERulePanel.h"
#include <QtGui>
#include "QuestConditionScript.h"

CREPrePostPanel::CREPrePostPanel(const QList<QuestConditionScript*> scripts, QWidget* parent) : QWidget(parent)
{
    QGridLayout* layout = new QGridLayout(this);

    myItems = new QListWidget(this);
    connect(myItems, SIGNAL(currentRowChanged(int)), this, SLOT(currentItemChanged(int)));
    layout->addWidget(myItems, 0, 0, 3, 2);

    QPushButton* addItem = new QPushButton(tr("add"), this);
    connect(addItem, SIGNAL(clicked(bool)), this, SLOT(onAddItem(bool)));
    layout->addWidget(addItem, 3, 0);

    QPushButton* delItem = new QPushButton(tr("delete"), this);
    connect(delItem, SIGNAL(clicked(bool)), this, SLOT(onDeleteItem(bool)));
    layout->addWidget(delItem, 3, 1);

    layout->addWidget(new QLabel(tr("Script:"), this), 0, 2);
    myChoices = new QComboBox(this);
    connect(myChoices, SIGNAL(currentIndexChanged(int)), this, SLOT(currentChoiceChanged(int)));

    for(int script = 0; script < scripts.size(); script++)
    {
        myChoices->addItem(scripts[script]->name());
        myChoices->setItemData(script, scripts[script]->comment(), Qt::ToolTipRole);
    }
    
    layout->addWidget(myChoices, 0, 3);

    mySubItems = new QListWidget(this);
    connect(mySubItems, SIGNAL(currentRowChanged(int)), this, SLOT(currentSubItemChanged(int)));
    layout->addWidget(mySubItems, 1, 2, 1, 2);

    QPushButton* addSubItem = new QPushButton(tr("add"), this);
    connect(addSubItem, SIGNAL(clicked(bool)), this, SLOT(onAddSubItem(bool)));
    layout->addWidget(addSubItem, 2, 2);

    QPushButton* delSubItem = new QPushButton(tr("delete"), this);
    connect(delSubItem, SIGNAL(clicked(bool)), this, SLOT(onDeleteSubItem(bool)));
    layout->addWidget(delSubItem, 2, 3);

    myItemEdit = new QLineEdit(this);
    connect(myItemEdit, SIGNAL(textChanged(const QString&)), this, SLOT(subItemChanged(const QString&)));
    layout->addWidget(myItemEdit, 3, 2, 1, 2);
}

CREPrePostPanel::~CREPrePostPanel()
{
}

QList<QStringList> CREPrePostPanel::getData()
{
    return myData;
}

void CREPrePostPanel::setData(const QList<QStringList> data)
{
    myItems->clear();
    mySubItems->clear();
    myItemEdit->clear();

    myData = data;

    foreach(QStringList list, data)
    {
        if (list.size() > 0)
            myItems->addItem(list[0]);
        else
            myItems->addItem(tr("(empty)"));
    }
}

void CREPrePostPanel::onAddItem(bool)
{
    myData.append(QStringList("quest"));
    myItems->addItem("quest");
    myItems->setCurrentRow(myData.size() - 1);
    emit dataModified();
}

void CREPrePostPanel::onDeleteItem(bool)
{
    if (myItems->currentRow() < 0 || myItems->currentRow() >= myData.size())
        return;

    myData.removeAt(myItems->currentRow());
    delete myItems->takeItem(myItems->currentRow());
    myItems->setCurrentRow(0);
    emit dataModified();
}

void CREPrePostPanel::onAddSubItem(bool)
{
    if (myItems->currentRow() < 0 || myItems->currentRow() >= myData.size())
        return;

    QStringList& data = myData[myItems->currentRow()];
    data.append("(item)");
    mySubItems->addItem("(item)");
    mySubItems->setCurrentRow(data.size() - 1);
    emit dataModified();
}

void CREPrePostPanel::onDeleteSubItem(bool)
{
    if (myItems->currentRow() < 0 || myItems->currentRow() >= myData.size())
        return;
    if (mySubItems->currentRow() < 0)
        return;

    myData[myItems->currentRow()].removeAt(mySubItems->currentRow() + 1); /* 0 is script */
    delete mySubItems->takeItem(mySubItems->currentRow());
    mySubItems->setCurrentRow(0);
    emit dataModified();
}

void CREPrePostPanel::currentItemChanged(int index)
{
    if (index < 0 || index >= myData.size())
        return;

    QStringList data = myData[index];
    if (data.size() == 0)
        return;

    myChoices->setCurrentIndex(myChoices->findText(data[0]));
    mySubItems->clear();
    data.takeFirst();
    mySubItems->addItems(data);
    myItemEdit->clear();
}

void CREPrePostPanel::currentSubItemChanged(int)
{
    if (mySubItems->currentItem())
        myItemEdit->setText(mySubItems->currentItem()->text());
}

void CREPrePostPanel::currentChoiceChanged(int)
{
    if (myItems->currentRow() < 0 || myItems->currentRow() >= myData.size())
        return;

    QStringList& data = myData[myItems->currentRow()];
    if (data.size() == 0)
        data.append(myChoices->currentText());
    else
        data[0] = myChoices->currentText();
    myItems->currentItem()->setText(data[0]);

    emit dataModified();
}

void CREPrePostPanel::subItemChanged(const QString& text)
{
    if (myItems->currentRow() < 0 || myItems->currentRow() >= myData.size())
        return;

    QStringList& data = myData[myItems->currentRow()];

    if (mySubItems->currentRow() < 0)
        return;

    data[mySubItems->currentRow() + 1] = text;
    mySubItems->currentItem()->setText(text);
    emit dataModified();
}
