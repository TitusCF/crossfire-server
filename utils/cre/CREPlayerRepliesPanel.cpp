#include <QtWidgets>
#include "CREPlayerRepliesPanel.h"
extern "C"
{
#include "global.h"
#include "dialog.h"
#include "sproto.h"
}

/**
 * Small delegate to handle edition of the reply type field.
 */
class CREMessageTypeDelegate : public QStyledItemDelegate
{
public:
    CREMessageTypeDelegate(QObject* parent) : QStyledItemDelegate(parent)
    {
    }

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        QComboBox* box = new QComboBox(parent);
        box->addItem(get_reply_text_own(rt_say), QString::number(static_cast<int>(rt_say)));
        box->addItem(get_reply_text_own(rt_reply), QString::number(static_cast<int>(rt_reply)));
        box->addItem(get_reply_text_own(rt_question), QString::number(static_cast<int>(rt_question)));
        return box;
    }

    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        if (!box)
            return;

        box->setCurrentIndex(box->findData(index.data(Qt::UserRole).toString()));
    }

    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        if (!box)
            return;
        model->setData(index, box->currentData(Qt::UserRole).toString(), Qt::UserRole);
    }

    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const override
    {
        editor->setGeometry(option.rect);
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        if (!box)
            return;
        box->showPopup();
    }
};

CREPlayerRepliesPanel::CREPlayerRepliesPanel(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Player suggested replies"));

    myReplies = new QTableWidget(this);
    myReplies->setColumnCount(3);
    myReplies->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    myReplies->setItemDelegateForColumn(2, new CREMessageTypeDelegate(this));

    QPushButton* addReply = new QPushButton(tr("add reply"), this);
    connect(addReply, SIGNAL(clicked(bool)), this, SLOT(onAddCondition(bool)));

    QPushButton* deleteReply = new QPushButton(tr("delete reply"), this);
    connect(deleteReply, SIGNAL(clicked(bool)), this, SLOT(onDeleteCondition(bool)));

    QPushButton* reset = new QPushButton(tr("reset changes"), this);
    connect(reset, SIGNAL(clicked(bool)), this, SLOT(onReset(bool)));

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons->addWidget(addReply);
    buttons->addWidget(deleteReply);
    buttons->addWidget(reset);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(myReplies);
    l->addLayout(buttons);
    setLayout(l);
}

CREPlayerRepliesPanel::~CREPlayerRepliesPanel()
{
}

const char* convertType(const QString& type)
{
    int i = type.toInt();
    if (i < 0 || i > rt_question)
        i = 0;
    return get_reply_text_own(static_cast<reply_type>(i));
}

QTableWidgetItem* createItem(const QString& text, const QString& edit)
{
    QTableWidgetItem* wi = new QTableWidgetItem(text);
    wi->setData(Qt::UserRole, edit);
    wi->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    return wi;
}

void CREPlayerRepliesPanel::addItem(const QStringList& item)
{
    int row = myReplies->rowCount();
    myReplies->setRowCount(row + 1);
    myReplies->setItem(row, 0, createItem(item[0], item[0]));
    myReplies->setItem(row, 1, createItem(item[1], item[1]));
    myReplies->setItem(row, 2, createItem(convertType(item.size() > 2 ? item[2] : "0"), item.size() > 2 ? item[2] : "0"));
}

void CREPlayerRepliesPanel::setData(const QList<QStringList>& data)
{
    myReplies->clearContents();
    myReplies->setRowCount(0);
    myReplies->setHorizontalHeaderLabels(QStringList() << "What the player should say" << "What the player will be displayed as saying" << "Message type");
    for (QStringList item : data)
    {
        addItem(item);
    }
    myOriginal = data;
}

QList<QStringList> CREPlayerRepliesPanel::data() const
{
    QList<QStringList> value;
    for (int r = 0; r < myReplies->rowCount(); r++)
    {
        QStringList i;
        i << myReplies->item(r, 0)->data(Qt::DisplayRole).toString();
        i << myReplies->item(r, 1)->data(Qt::DisplayRole).toString();
        i << myReplies->item(r, 2)->data(Qt::UserRole).toString();
        value.append(i);
    }
    return value;
}

void CREPlayerRepliesPanel::onAddCondition(bool)
{
    QStringList item;
    item << "*" << "?" << "0";
    addItem(item);
    myReplies->setCurrentItem(myReplies->item(myReplies->rowCount() - 1, 0));
}

void CREPlayerRepliesPanel::onDeleteCondition(bool)
{
    if (myReplies->currentRow() == -1)
        return;

    myReplies->removeRow(myReplies->currentRow());
}

void CREPlayerRepliesPanel::onReset(bool)
{
    if (QMessageBox::question(this, "Confirm reset", "Reset the replies to the initial values of the message?") != QMessageBox::StandardButton::Yes)
        return;
    setData(myOriginal);
}
