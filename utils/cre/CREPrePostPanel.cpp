#include "CREPrePostPanel.h"
#include "CRERulePanel.h"
#include <QtGui>
#include "QuestConditionScript.h"

CRESubItemList::CRESubItemList(QWidget* parent) : CRESubItemWidget(parent)
{
    QGridLayout* layout = new QGridLayout(this);

    mySubItems = new QListWidget(this);
    connect(mySubItems, SIGNAL(currentRowChanged(int)), this, SLOT(currentSubItemChanged(int)));
    layout->addWidget(mySubItems, 0, 0, 1, 2);

    QPushButton* addSubItem = new QPushButton(tr("add"), this);
    connect(addSubItem, SIGNAL(clicked(bool)), this, SLOT(onAddSubItem(bool)));
    layout->addWidget(addSubItem, 1, 0);

    QPushButton* delSubItem = new QPushButton(tr("delete"), this);
    connect(delSubItem, SIGNAL(clicked(bool)), this, SLOT(onDeleteSubItem(bool)));
    layout->addWidget(delSubItem, 1, 1);

    myItemEdit = new QLineEdit(this);
    connect(myItemEdit, SIGNAL(textChanged(const QString&)), this, SLOT(subItemChanged(const QString&)));
    layout->addWidget(myItemEdit, 2, 0, 1, 2);
}

void CRESubItemList::setData(const QStringList& data)
{
    myData = data;
    myData.takeFirst();
    mySubItems->clear();
    mySubItems->addItems(myData);
    myItemEdit->clear();
}

void CRESubItemList::currentSubItemChanged(int)
{
    if (mySubItems->currentItem())
        myItemEdit->setText(mySubItems->currentItem()->text());
}


void CRESubItemList::onAddSubItem(bool)
{
    myData.append("(item)");
    mySubItems->addItem("(item)");
    mySubItems->setCurrentRow(myData.size() - 1);

    emit dataModified(myData);
}

void CRESubItemList::onDeleteSubItem(bool)
{
    if (mySubItems->currentRow() < 0)
        return;

    myData.removeAt(mySubItems->currentRow());
    delete mySubItems->takeItem(mySubItems->currentRow());
    mySubItems->setCurrentRow(0);
    emit dataModified(myData);
}

void CRESubItemList::subItemChanged(const QString& text)
{
    if (mySubItems->currentRow() < 0)
        return;

    myData[mySubItems->currentRow()] = text;
    mySubItems->currentItem()->setText(text);
    emit dataModified(myData);
}

CRESubItemConnection::CRESubItemConnection(QWidget* parent) : CRESubItemWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Connection number:"), this));
    myEdit = new QLineEdit(this);
    myEdit->setValidator(new QIntValidator(1, 65000, myEdit));
    connect(myEdit, SIGNAL(textChanged(const QString&)), this, SLOT(editChanged(const QString&)));
    layout->addWidget(myEdit);
    myWarning = new QLabel(this);
    myWarning->setVisible(false);
    layout->addWidget(myWarning);
    layout->addStretch();
}

void CRESubItemConnection::setData(const QStringList& data)
{
    if (data.size() < 2)
    {
        showWarning(tr("Not enough arguments"));
        return;
    }

    bool ok = false;
    int value = data[1].toInt(&ok);
    if (!ok || value <= 0 || value > 65000)
    {
        showWarning(tr("Invalid number %1, must be a number between 1 and 65000").arg(data[1]));
        value = 1;
    }

    myWarning->setVisible(false);
    myEdit->setText(QString::number(value));
}

void CRESubItemConnection::showWarning(const QString& warning)
{
    myWarning->setText(warning);
    myWarning->setVisible(true);
}

void CRESubItemConnection::editChanged(const QString& text)
{
    bool ok = false;
    int value = text.toInt(&ok);
    if (!ok || value <= 0 || value > 65000)
    {
        showWarning(tr("Invalid number %1, must be a number between 1 and 65000").arg(text));
        return;
    }

    myWarning->setVisible(false);
    emit dataModified(QStringList(text));
}


CREPrePostPanel::CREPrePostPanel(bool isPre, const QList<QuestConditionScript*> scripts, QWidget* parent) : QWidget(parent)
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

    mySubItemsStack = new QStackedWidget(this);

    for(int script = 0; script < scripts.size(); script++)
    {
        myChoices->addItem(scripts[script]->name());
        myChoices->setItemData(script, scripts[script]->comment(), Qt::ToolTipRole);
        mySubWidgets.append(createSubItemWidget(isPre, scripts[script]));
        mySubItemsStack->addWidget(mySubWidgets.last());
        connect(mySubWidgets.last(), SIGNAL(dataModified(const QStringList&)), this, SLOT(subItemChanged(const QStringList&)));
    }

    layout->addWidget(myChoices, 0, 3);

    layout->addWidget(mySubItemsStack, 1, 2, 3, 2);
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

void CREPrePostPanel::currentItemChanged(int index)
{
    if (index < 0 || index >= myData.size())
        return;

    QStringList data = myData[index];
    if (data.size() == 0)
        return;

    myChoices->setCurrentIndex(myChoices->findText(data[0]));

    mySubWidgets[myChoices->currentIndex()]->setData(data);
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

    mySubItemsStack->setCurrentIndex(myChoices->currentIndex());
    mySubWidgets[myChoices->currentIndex()]->setData(data);

    emit dataModified();
}

void CREPrePostPanel::subItemChanged(const QStringList& data)
{
    QStringList& item = myData[myItems->currentRow()];
    while (item.size() > 1)
        item.removeLast();
    item.append(data);

    emit dataModified();
}

CRESubItemWidget* CREPrePostPanel::createSubItemWidget(bool isPre, const QuestConditionScript* script)
{
    if (!isPre && script->name() == "connection")
        return new CRESubItemConnection(this);

    return new CRESubItemList(this);
}
