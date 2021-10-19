#include <QtWidgets>
#include "CREPrePostPanel.h"
#include "QuestConditionScript.h"
#include "assets.h"
#include "AssetsManager.h"
extern "C" {
#include "quest.h"
}

CRESubItemList::CRESubItemList(QWidget* parent) : CRESubItemWidget(parent)
{
    QGridLayout* layout = new QGridLayout(this);

    mySubItems = new QListWidget(this);
    mySubItems->setViewMode(QListView::ListMode);
    layout->addWidget(mySubItems, 0, 0, 1, 2);

    QPushButton* addSubItem = new QPushButton(tr("add"), this);
    connect(addSubItem, SIGNAL(clicked(bool)), this, SLOT(onAddSubItem(bool)));
    layout->addWidget(addSubItem, 1, 0);

    QPushButton* delSubItem = new QPushButton(tr("delete"), this);
    connect(delSubItem, SIGNAL(clicked(bool)), this, SLOT(onDeleteSubItem(bool)));
    layout->addWidget(delSubItem, 1, 1);

    connect(mySubItems->itemDelegate(), SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), this, SLOT(endEdition(QWidget*, QAbstractItemDelegate::EndEditHint)));
}

void CRESubItemList::addItem(const QString& item)
{
    QListWidgetItem* wi = new QListWidgetItem(item);
    wi->setFlags(wi->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    mySubItems->addItem(wi);
}

void CRESubItemList::setData(const QStringList& data)
{
    QStringList d(data);
    d.takeFirst();
    mySubItems->clear();
    for (const QString& item : d)
        addItem(item);
}

QStringList CRESubItemList::data() const
{
    QStringList values;
    for (int i = 0; i < mySubItems->count(); i++)
        values.append(mySubItems->item(i)->text());
    return values;
}

void CRESubItemList::endEdition(QWidget*, QAbstractItemDelegate::EndEditHint)
{
    emit dataModified(data());
}

void CRESubItemList::onAddSubItem(bool)
{
    addItem("(item)");
    mySubItems->setCurrentRow(mySubItems->count() - 1);
    mySubItems->editItem(mySubItems->currentItem());

    emit dataModified(data());
}

void CRESubItemList::onDeleteSubItem(bool)
{
    if (mySubItems->currentRow() < 0)
        return;

    delete mySubItems->takeItem(mySubItems->currentRow());
    mySubItems->setCurrentRow(0);
    emit dataModified(data());
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

CRESubItemQuest::CRESubItemQuest(CREPrePostList::Mode mode, QWidget* parent) : CRESubItemWidget(parent), myMode(mode)
{
    myInit = true;

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Quest:"), this));

    myQuestList = new QComboBox(this);
    layout->addWidget(myQuestList);

    if (mode != CREPrePostList::PostConditions)
    {
        myAtStep = new QRadioButton(tr("at step"), this);
        layout->addWidget(myAtStep);
        myBelowStep = new QRadioButton(tr("below step"), this);
        layout->addWidget(myBelowStep);
        myBelowStep->setVisible(mode == CREPrePostList::SetWhen);
        myFromStep = new QRadioButton(tr("from step"), this);
        layout->addWidget(myFromStep);
        myFromStep->setVisible(mode == CREPrePostList::PreConditions);
        myStepRange = new QRadioButton(tr("from step to step"), this);
        layout->addWidget(myStepRange);

        connect(myAtStep, SIGNAL(toggled(bool)), this, SLOT(checkToggled(bool)));
        connect(myBelowStep, SIGNAL(toggled(bool)), this, SLOT(checkToggled(bool)));
        connect(myFromStep, SIGNAL(toggled(bool)), this, SLOT(checkToggled(bool)));
        connect(myStepRange, SIGNAL(toggled(bool)), this, SLOT(checkToggled(bool)));
    }
    else
    {
        layout->addWidget(new QLabel(tr("new step:"), this));
        myAtStep = NULL;
        myBelowStep = nullptr;
        myFromStep = NULL;
    }

    myFirstStep = new QComboBox(this);
    layout->addWidget(myFirstStep);

    if (mode != CREPrePostList::PostConditions)
    {
        mySecondStep = new QComboBox(this);
        layout->addWidget(mySecondStep);
    }
    else
        mySecondStep = NULL;

    layout->addStretch();

    connect(myQuestList, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedQuestChanged(int)));
    getManager()->quests()->each([&] (auto quest) {
        myQuestList->addItem(QString(quest->quest_title) + " [" + quest->quest_code + "]", quest->quest_code);
    });
    connect(myFirstStep, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedStepChanged(int)));
    if (mySecondStep)
        connect(mySecondStep, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedStepChanged(int)));

    myInit = false;
}

void CRESubItemQuest::setData(const QStringList& data)
{
    if (data.size() < 3)
    {
        if (myAtStep)
            myAtStep->setChecked(true);
        return;
    }

    int index = myQuestList->findData(data[1], Qt::UserRole);
    if (index == -1)
    {
        return;
    }

    myInit = true;
    myQuestList->setCurrentIndex(index);

    if (myMode == CREPrePostList::PostConditions)
    {
        myFirstStep->setCurrentIndex(myFirstStep->findData(data[2], Qt::UserRole));
        myInit = false;
        return;
    }

    QString steps = data[2];
    int idx = steps.indexOf('-');

    if (idx == -1)
    {
        int start = 0;
        if (steps.startsWith("<="))
        {
            myBelowStep->setChecked(true);
            start = 2;
        }
        else if (steps.startsWith('=') || myMode == CREPrePostList::SetWhen)
        {
            myAtStep->setChecked(true);
            start = myMode == CREPrePostList::SetWhen ? 0 : 1;
        }
        else
        {
            myFromStep->setChecked(true);
        }

        myFirstStep->setCurrentIndex(myFirstStep->findData(steps.mid(start), Qt::UserRole));
    }
    else
    {
        myStepRange->setChecked(true);
        myFirstStep->setCurrentIndex(myFirstStep->findData(steps.left(idx), Qt::UserRole));
        mySecondStep->setCurrentIndex(mySecondStep->findData(steps.mid(idx + 1), Qt::UserRole));
    }

    myInit = false;
}

void CRESubItemQuest::selectedQuestChanged(int index)
{
    myFirstStep->clear();
    if (myMode != CREPrePostList::PostConditions)
        myFirstStep->addItem("(not started)", "0");

    if (mySecondStep)
        mySecondStep->clear();

    if (index < 0 || index >= myQuestList->count())
        return;

    auto quest = getManager()->quests()->find(myQuestList->currentData().toString().toStdString());
    if (!quest)
        return;

    QString desc;
    auto step = quest->steps;
    while (step) {
        desc = tr("%1 (%2)").arg(QString::number(step->step), QString(step->step_description).left(30));
        if (step->is_completion_step)
            desc += " (end)";
        myFirstStep->addItem(desc, QString::number(step->step));
        if (mySecondStep)
            mySecondStep->addItem(desc, QString::number(step->step));

        step = step->next;
    }
}

void CRESubItemQuest::updateData()
{
    if (myInit)
        return;

    QStringList data;

    data << myQuestList->itemData(myQuestList->currentIndex(), Qt::UserRole).toString();

    if (myMode != CREPrePostList::PostConditions)
    {
        QString value;
        if (myStepRange->isChecked())
        {
            value = myFirstStep->itemData(myFirstStep->currentIndex(), Qt::UserRole).toString();
            value += "-";
            value += mySecondStep->itemData(mySecondStep->currentIndex(), Qt::UserRole).toString();
        }
        else
        {
            if (myAtStep->isChecked() && myMode != CREPrePostList::SetWhen)
                value = "=";
            else if (myBelowStep->isChecked())
                value = "<=";
            value += myFirstStep->itemData(myFirstStep->currentIndex(), Qt::UserRole).toString();
        }

        data << value;
    }
    else
    {
        data << myFirstStep->itemData(myFirstStep->currentIndex(), Qt::UserRole).toString();
    }

    emit dataModified(data);
}

void CRESubItemQuest::checkToggled(bool checked)
{
    if (checked == false)
        return;

    mySecondStep->setEnabled(myStepRange->isChecked());
    updateData();
}

void CRESubItemQuest::selectedStepChanged(int index)
{
    if (index == -1)
        return;

    updateData();
}

CRESubItemToken::CRESubItemToken(bool isPre, QWidget* parent) : CRESubItemWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Token:"), this));
    myToken = new QLineEdit(this);
    layout->addWidget(myToken);
    connect(myToken, SIGNAL(textChanged(const QString&)), this, SLOT(tokenChanged(const QString&)));

    if (isPre)
    {
        layout->addWidget(new QLabel(tr("Values the token can be (one per line):"), this));
        myValues = new QTextEdit(this);
        myValues->setAcceptRichText(false);
        layout->addWidget(myValues);
        connect(myValues, SIGNAL(textChanged()), this, SLOT(valuesChanged()));
        myValue = NULL;
    }
    else
    {
        layout->addWidget(new QLabel(tr("Value to set for the token:"), this));
        myValue = new QLineEdit(this);
        layout->addWidget(myValue);
        connect(myValue, SIGNAL(textChanged(const QString&)), this, SLOT(tokenChanged(const QString&)));
        myValues = NULL;
    }
    layout->addStretch();
}

void CRESubItemToken::setData(const QStringList& data)
{
    QStringList copy(data);

    if (data.size() < 2)
    {
        myToken->clear();
        if (myValues != NULL)
            myValues->clear();
        if (myValue != NULL)
            myValue->clear();

        return;
    }
    copy.removeFirst();
    myToken->setText(copy.takeFirst());
    if (myValues != NULL)
        myValues->setText(copy.join("\n"));
    else if (copy.size() > 0)
        myValue->setText(copy[0]);
    else
        myValue->clear();
}

void CRESubItemToken::updateData()
{
    QStringList values;
    values.append(myToken->text());
    if (myValues != NULL)
        values.append(myValues->toPlainText().split("\n"));
    else
        values.append(myValue->text());
    emit dataModified(values);
}

void CRESubItemToken::tokenChanged(const QString&)
{
    updateData();
}

void CRESubItemToken::valuesChanged()
{
    updateData();
}

CREPrePostPanel::CREPrePostPanel(CREPrePostList::Mode mode, const QList<QuestConditionScript*> scripts, QWidget* parent) : QDialog(parent), myMode(mode)
{
    setModal(true);
    switch (mode) {
        case CREPrePostList::PreConditions:
            setWindowTitle(tr("Message pre-condition"));
            break;
        case CREPrePostList::PostConditions:
            setWindowTitle(tr("Message post-condition"));
            break;
        case CREPrePostList::SetWhen:
            setWindowTitle(tr("Step set when"));
            break;
    }

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Script:"), this));

    myChoices = new QComboBox(this);
    layout->addWidget(myChoices);

    mySubItemsStack = new QStackedWidget(this);

    for(int script = 0; script < scripts.size(); script++)
    {
        if (mode == CREPrePostList::SetWhen && scripts[script]->name() != "quest")
            continue;
        myChoices->addItem(scripts[script]->name());
        myChoices->setItemData(script, scripts[script]->comment(), Qt::ToolTipRole);
        mySubWidgets.append(createSubItemWidget(scripts[script]));
        mySubItemsStack->addWidget(mySubWidgets.last());
        connect(mySubWidgets.last(), SIGNAL(dataModified(const QStringList&)), this, SLOT(subItemChanged(const QStringList&)));
    }

    layout->addWidget(mySubItemsStack);

    QPushButton* reset = new QPushButton(tr("reset changes"), this);
    connect(reset, SIGNAL(clicked(bool)), this, SLOT(onReset(bool)));
    layout->addWidget(reset);

    connect(myChoices, SIGNAL(currentIndexChanged(int)), this, SLOT(currentChoiceChanged(int)));

    if (CREPrePostList::SetWhen == mode) {
        myChoices->setVisible(false);
        layout->itemAt(0)->widget()->setVisible(false);
    }
}

CREPrePostPanel::~CREPrePostPanel()
{
}

QStringList CREPrePostPanel::getData()
{
    return myData;
}

void CREPrePostPanel::setData(const QStringList& data)
{
    myData = data;
    if (myMode == CREPrePostList::SetWhen && !myData.empty() && myData[0] != "quest")
        myData.push_front("quest");
    myOriginal = myData;
    if (myData.isEmpty())
        return;

    myChoices->setCurrentIndex(myChoices->findText(myOriginal[0]));

    mySubWidgets[myChoices->currentIndex()]->setData(myOriginal);
}

void CREPrePostPanel::currentChoiceChanged(int)
{
    if (myData.size() == 0)
        myData.append(myChoices->currentText());
    else
        myData[0] = myChoices->currentText();

    mySubItemsStack->setCurrentIndex(myChoices->currentIndex());
    mySubWidgets[myChoices->currentIndex()]->setData(myData);
}

void CREPrePostPanel::subItemChanged(const QStringList& data)
{
    while (myData.size() > 1)
        myData.removeLast();
    myData.append(data);
}

CRESubItemWidget* CREPrePostPanel::createSubItemWidget(const QuestConditionScript* script)
{
    if (myMode == CREPrePostList::PostConditions && script->name() == "connection")
        return new CRESubItemConnection(this);

    if (script->name() == "quest")
        return new CRESubItemQuest(myMode, this);

    if (script->name() == "token" || script->name() == "settoken" || script->name() == "npctoken" || script->name() == "setnpctoken")
        return new CRESubItemToken(myMode == CREPrePostList::PreConditions, this);

    return new CRESubItemList(this);
}

void CREPrePostPanel::onReset(bool)
{
    if (QMessageBox::question(this, "Confirm reset", "Reset the condition to initial initial values, losing all changes?") != QMessageBox::StandardButton::Yes)
        return;
    setData(myOriginal);
}
