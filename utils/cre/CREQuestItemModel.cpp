#include "CREQuestItemModel.h"
#include "quest.h"

CREQuestItemModel::CREQuestItemModel(QObject* parent) : QAbstractItemModel(parent)
{
    myQuest = NULL;
    myStepCount = 0;
}

CREQuestItemModel::~CREQuestItemModel() {
}

quest_definition *CREQuestItemModel::quest() const
{
    return myQuest;
}

void CREQuestItemModel::setQuest(quest_definition *quest)
{
    emit beginResetModel();
    myQuest = quest;
    myStepCount = 0;
    auto step = myQuest->steps;
    while (step) {
        myStepCount++;
        step = step->next;
    }

    emit endResetModel();
}

int CREQuestItemModel::columnCount(const QModelIndex& parent) const
{
    if (myQuest == NULL)
        return 0;

    if (parent.isValid())
        return 0;

    return 4;
}

QModelIndex CREQuestItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid())
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex CREQuestItemModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int CREQuestItemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if (myQuest == NULL)
        return 0;

    return myStepCount;
}

QList<QStringList> conditionsToString(const quest_condition *condition) {
    QList<QStringList> ret;
    while (condition) {
        char buf[500];
        quest_write_condition(buf, sizeof(buf), condition);
        ret.append(QString(buf).split(' '));
        Q_ASSERT(ret.back().size() == 2);
        condition = condition->next;
    }
    return ret;
}

static QString toDisplay(const QList<QStringList>& list)
{
    QStringList data;
    foreach(QStringList item, list)
        data.append(item.join(" "));
    return data.join("\n");
}

QVariant CREQuestItemModel::data(const QModelIndex& index, int role) const
{
    if (myQuest == NULL || !index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole && (role != Qt::CheckStateRole || index.column() != 2))
        return QVariant();

    auto step = getStep(index.row());

    switch(index.column())
    {
        case 0:
            return step->step;

        case 1:
            return step->step_description;

        case 2:
            if (role == Qt::DisplayRole)
                return QVariant();
            return step->is_completion_step ? Qt::Checked : Qt::Unchecked;

        case 3:
        {
            auto conditions = conditionsToString(step->conditions);
            if (role == Qt::EditRole)
                return QVariant::fromValue(conditions);
            return toDisplay(conditions);
        }
    }

    return QVariant();;
}

QVariant CREQuestItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
        case 0:
            return tr("step");
        case 1:
            return tr("description");
        case 2:
            return tr("end?");
        case 3:
            return tr("set when");
    }

    return QVariant();
}

Qt::ItemFlags CREQuestItemModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    if (index.column() == 2)
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool CREQuestItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || myQuest == NULL || myStepCount <= index.row())
        return false;

    if (role != Qt::EditRole && index.column() != 2)
        return false;

    auto step = getStep(index.row());

    if (index.column() == 0)
        step->step = value.toInt();
    else if (index.column() == 1)
        FREE_AND_COPY(step->step_description, value.toString().toStdString().data())
    else if (index.column() == 2)
        step->is_completion_step = (value == Qt::Checked);
    else if (index.column() == 3) {
        auto when = value.value<QList<QStringList>>();
        auto cond = step->conditions;
        while (cond) {
            auto n = cond->next;
            quest_destroy_condition(cond);
            cond = n;
        }
        step->conditions = nullptr;
        quest_condition *last = nullptr;

        for (auto single : when) {
            auto cond = quest_create_condition();
            if (!quest_condition_from_string(cond, single.join(' ').toStdString().data())) {
                free(cond);
                continue;
            }
            if (last)
                last->next = cond;
            else
                step->conditions = cond;
            last = cond;
        }
    }

    emit dataChanged(index, index);
    emit questModified(myQuest);

    return true;
}

void CREQuestItemModel::addStep(bool)
{
    if (myQuest == NULL)
        return;

    beginInsertRows(QModelIndex(), myStepCount, myStepCount);

    if (myQuest->steps) {
        auto cur = myQuest->steps;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = quest_create_step();
        cur->next->step = cur->step + 10;
    } else {
        myQuest->steps = quest_create_step();
        myQuest->steps->step = 10;
    }
    myStepCount++;

    emit questModified(myQuest);

    endInsertRows();
}

bool CREQuestItemModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (myQuest == NULL || parent.isValid() || count != 1)
        return false;

    if (row < 0 || row >= myStepCount)
        return false;

    beginRemoveRows(parent, row, row);

    quest_step_definition *cur = myQuest->steps, *prev = nullptr;
    while (row > 0) {
        prev = cur;
        cur = cur->next;
        row--;
    }
    if (!prev) {
        myQuest->steps = cur->next;
    } else {
        prev->next = cur->next;
    }
    quest_destroy_step(cur);
    myStepCount--;

    endRemoveRows();

    emit questModified(myQuest);

    return true;
}

void CREQuestItemModel::moveUp(int step)
{
    if (step < 1 || myStepCount < 2)
        return;

    beginMoveRows(QModelIndex(), step, step, QModelIndex(), step - 1);

    quest_step_definition *cur = myQuest->steps, *prev = nullptr;
    while (step > 1) {
        prev = cur;
        cur = cur->next;
        step--;
    }

    if (!prev) {
        Q_ASSERT(cur);
        Q_ASSERT(cur->next);
        myQuest->steps = cur->next;
        cur->next = cur->next->next;
        myQuest->steps->next = cur;
    } else {
        prev->next = cur->next;
        if (cur->next)
            cur->next = cur->next->next;
        else
            cur->next = nullptr;
        prev->next->next = cur;
    }
    endMoveRows();

    emit questModified(myQuest);
}

void CREQuestItemModel::moveDown(int step)
{
    if (step >= myStepCount - 1)
        return;

    moveUp(step + 1);
}

quest_step_definition *CREQuestItemModel::getStep(int step) const {
    auto left = step;
    Q_ASSERT(left <= myStepCount);
    auto current = myQuest->steps;
    while (left > 0) {
        Q_ASSERT(current);
        current = current->next;
        left--;
    }
    return current;
}
