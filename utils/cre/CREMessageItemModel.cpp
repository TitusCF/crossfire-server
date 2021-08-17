#include <QtWidgets>

#include "CREMessageItemModel.h"

CREMessageItemModel::CREMessageItemModel(QObject* parent) : QAbstractItemModel(parent), myMessage(nullptr)
{
}

CREMessageItemModel::~CREMessageItemModel()
{
}

void CREMessageItemModel::setMessage(MessageFile* message)
{
    beginResetModel();
    myMessage = message;
    endResetModel();
}

void CREMessageItemModel::setSelectedRule(const QModelIndex& index)
{
    auto blue(myBlue), red(myRed);
    myBlue.clear();
    myRed.clear();

    int row = index.row();
    if (row < 0 || row >= myMessage->rules().size())
        return;

    MessageRule* rule = myMessage->rules()[row];

    foreach(QStringList pre, rule->preconditions())
    {
        if (pre.size() < 3)
            continue;

        if (pre[0] != "token" && pre[0] != "npctoken")
            continue;

        QStringList acceptable = pre;
        acceptable.removeFirst();
        acceptable.removeFirst();

        for (int c = 0; c < myMessage->rules().size(); c++)
        {
            MessageRule* check = myMessage->rules()[c];

            if (check == rule)
                continue;

            bool match = false;
            foreach(QStringList post, check->postconditions())
            {
                if (post.size() < 3)
                    continue;
                if ((post[0] != "settoken" && post[0] != "setnpctoken") || post[1] != pre[1] || !acceptable.contains(post[2]))
                    continue;
                match = true;
                break;
            }

            if (match)
                myRed.insert(c);
        }
    }

    foreach(QStringList post, rule->postconditions())
    {
        if (post.size() < 3)
            continue;

        if (post[0] != "settoken" && post[0] != "setnpctoken")
            continue;

        for (int c = 0; c < myMessage->rules().size(); c++)
        {
            MessageRule* check = myMessage->rules()[c];

            if (check == rule)
                continue;

            bool match = false;
            foreach(QStringList pre, check->preconditions())
            {
                if (pre.size() < 3)
                    continue;
                if ((pre[0] != "token" && pre[0] != "npctoken") || pre[1] != post[1])
                    continue;

                QStringList acceptable = pre;
                acceptable.removeFirst();
                acceptable.removeFirst();
                if (!acceptable.contains(post[2]))
                    continue;

                match = true;
                break;
            }

            if (match)
                myBlue.insert(c);
        }
    }

    for (int b : myBlue)
    {
        emit dataChanged(createIndex(b, 0), createIndex(b, 6));
        blue.remove(b);
        red.remove(b);
    }
    for (int r : myRed)
    {
        emit dataChanged(createIndex(r, 0), createIndex(r, 6));
        blue.remove(r);
        red.remove(r);
    }

    for (int old : red + blue)
    {
        emit dataChanged(createIndex(old, 0), createIndex(old, 6));
    }
}

int CREMessageItemModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return 6;
}

QModelIndex CREMessageItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex CREMessageItemModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int CREMessageItemModel::rowCount(const QModelIndex& index) const
{
    if (!myMessage || index.isValid())
        return 0;

    return myMessage->rules().size();
}

static QString toDisplay(const QList<QStringList>& list)
{
    QStringList data;
    foreach(QStringList item, list)
        data.append(item.join(" "));
    return data.join("\n");
}

QVariant CREMessageItemModel::data(const QModelIndex& index, int role) const
{
    if (myMessage == NULL || !index.isValid())
        return QVariant();

    if (role == Qt::BackgroundColorRole)
    {
        if (myBlue.contains(index.row()))
            return QVariant::fromValue(QBrush(Qt::blue));
        if (myRed.contains(index.row()))
            return QVariant::fromValue(QBrush(Qt::red));
        return QVariant();
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    Q_ASSERT(index.row() < myMessage->rules().size());
    const MessageRule* rule = myMessage->rules()[index.row()];

    switch(index.column())
    {
        case 0:
            return role == Qt::DisplayRole ? QVariant::fromValue(rule->match().join("\n")) : QVariant::fromValue(rule->match());
        case 1:
            return role == Qt::DisplayRole ? QVariant::fromValue(toDisplay(rule->preconditions())) : QVariant::fromValue(rule->preconditions());
        case 2:
            return role == Qt::DisplayRole ? QVariant::fromValue(toDisplay(rule->replies())) : QVariant::fromValue(rule->replies());
        case 3:
            return role == Qt::DisplayRole ? QVariant::fromValue(rule->messages().join("\n")) : QVariant::fromValue(rule->messages());
        case 4:
            return role == Qt::DisplayRole ? QVariant::fromValue(toDisplay(rule->postconditions())) : QVariant::fromValue(rule->postconditions());
        case 5:
            return role == Qt::DisplayRole ? QVariant::fromValue(rule->include().join("\n")) : QVariant::fromValue(rule->include());
    }

    return QVariant();;
}

QVariant CREMessageItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
        case 0:
            return tr("match");
        case 1:
            return tr("pre-conditions");
        case 2:
            return tr("player suggested replies");
        case 3:
            return tr("NPC messages");
        case 4:
            return tr("post-conditions");
        case 5:
            return tr("includes");
    }

    return QVariant();
}

Qt::ItemFlags CREMessageItemModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool CREMessageItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || myMessage == nullptr || myMessage->rules().size() <= index.row())
        return false;
    if (role != Qt::EditRole)
        return false;

    MessageRule* rule = myMessage->rules()[index.row()];

    switch (index.column())
    {
        case 0:
            rule->setMatch(value.value<QStringList>());
            break;
        case 1:
            rule->setPreconditions(value.value<QList<QStringList>>());
            break;
        case 2:
            rule->setReplies(value.value<QList<QStringList>>());
            break;
        case 3:
            rule->setMessages(value.value<QStringList>());
            break;
        case 4:
            rule->setPostconditions(value.value<QList<QStringList>>());
            break;
        case 5:
            rule->setInclude(value.value<QStringList>());
            break;
        default:
            return false;
    }

    myMessage->setModified(true);
    emit dataChanged(index, index);

    return true;
}

bool CREMessageItemModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    while (count > 0)
    {
        myMessage->rules().insert(row, new MessageRule());
        count--;
    }
    endInsertRows();
    myMessage->setModified(true);

    return true;
}

bool CREMessageItemModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid())
        return false;
    if (row < 0 || row >= myMessage->rules().size())
        return false;

    beginRemoveRows(parent, row, row + count);
    while (count > 0 && row < myMessage->rules().size())
    {
        myMessage->rules().removeAt(row);
        count--;
    }
    endRemoveRows();
    myMessage->setModified(true);

    return true;
}

void CREMessageItemModel::moveUpDown(int row, bool up)
{
    if (!myMessage)
        return;

    const int shift = up ? -1 : 1;
    if (row < 0 || row >= myMessage->rules().size() || (row + shift) < 0 || (row + shift) >= myMessage->rules().size())
        return;

    MessageRule* moved = myMessage->rules()[row];
    myMessage->rules()[row] = myMessage->rules()[row + shift];
    myMessage->rules()[row + shift] = moved;

    emit dataChanged(createIndex(up ? row - 1 : row, 0), createIndex(up ? row : row + 1, 6));
    myMessage->setModified(true);
}

void CREMessageItemModel::duplicateRow(int row)
{
    if (!myMessage || row < 0 || row >= myMessage->rules().count())
        return;

    beginInsertRows(QModelIndex(), row + 1, row + 1);
    myMessage->rules().insert(row + 1, new MessageRule(*myMessage->rules()[row]));
    endInsertRows();
    myMessage->setModified(true);
}
