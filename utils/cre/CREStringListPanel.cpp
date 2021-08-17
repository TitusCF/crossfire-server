#include <QtWidgets>
#include "CREStringListPanel.h"
#include "CREMultilineItemDelegate.h"

CREStringListPanel::CREStringListPanel(QWidget* parent) : QDialog(parent)
{
    setModal(true);
    setWindowTitle(tr("NPC possible messages"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Messages:"), this));

    myItems = new QListWidget(this);
    layout->addWidget(myItems);
    myItems->setItemDelegateForColumn(0, new CREMultilineItemDelegate(myItems, false));

    QHBoxLayout* buttons = new QHBoxLayout();

    QPushButton* add = new QPushButton(tr("add"), this);
    connect(add, SIGNAL(clicked(bool)), this, SLOT(onAddItem(bool)));
    buttons->addWidget(add);

    QPushButton* remove = new QPushButton(tr("remove"), this);
    connect(remove, SIGNAL(clicked(bool)), this, SLOT(onDeleteItem(bool)));
    buttons->addWidget(remove);

    QPushButton* reset = new QPushButton(tr("reset changes"), this);
    connect(reset, SIGNAL(clicked(bool)), this, SLOT(onReset(bool)));
    buttons->addWidget(reset);

    layout->addLayout(buttons);
}

CREStringListPanel::~CREStringListPanel()
{
}

void CREStringListPanel::setData(const QStringList& list)
{
    myItems->clear();
    for (const QString item : list)
        createItem(item);
    myOriginal = list;
}

QStringList CREStringListPanel::data() const
{
    QStringList data;
    for (int i = 0; i < myItems->count(); i++)
        data.append(myItems->item(i)->text().trimmed());
    return data;
}

QListWidgetItem* CREStringListPanel::createItem(const QString& text)
{
    QListWidgetItem* wi = new QListWidgetItem(text);
    wi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    myItems->addItem(wi);
    return wi;
}

void CREStringListPanel::onAddItem(bool)
{
    myItems->editItem(createItem("<message>"));
}

void CREStringListPanel::onDeleteItem(bool)
{
    if (myItems->currentRow() < 0 || myItems->currentRow() >= myItems->count())
        return;
    delete myItems->takeItem(myItems->currentRow());
}

void CREStringListPanel::onReset(bool)
{
    if (QMessageBox::question(this, "Confirm reset", "Reset the values, losing all changes?") != QMessageBox::StandardButton::Yes)
        return;
    setData(myOriginal);
}
