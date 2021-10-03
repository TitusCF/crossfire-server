#include "ArchetypesModel.h"
#include "assets.h"
#include "AssetsManager.h"
#include "CREPixmap.h"

ArchetypesModel::ArchetypesModel() {
    getManager()->archetypes()->each([this] (archetype *arch) {
        if (QUERY_FLAG(&arch->clone, FLAG_MONSTER) && (!arch->head)) {
            myMonsters.push_back(arch);
        }
    });
}

int ArchetypesModel::rowCount(const QModelIndex &parent) const {
    (void)parent;
    return myMonsters.size();
}
  
int ArchetypesModel::columnCount(const QModelIndex &parent) const {
    (void)parent;
    return 8;
}

QVariant ArchetypesModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QVariant();
    }
    auto monster = &myMonsters[index.row()]->clone;
    switch(index.column()) {
        case 0:
            return monster->stats.hp;
        case 1:
            return monster->stats.ac;
        case 2:
            return monster->stats.wc;
        case 3:
            return monster->weight;
        case 4:
            return qlonglong(monster->stats.exp);
        case 5:
            return monster->level;
        case 6:
            return monster->stats.dam;
        case 7:
            return monster->speed;
    }
    return "";
}

QVariant ArchetypesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole) {
            return myMonsters[section]->name;
        } else if (role == Qt::DecorationRole) {
            return CREPixmap::getIcon(myMonsters[section]->clone.face->number);
        }
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch(section) {
        case 0:
            return "HP";
        case 1:
            return "AC";
        case 2:
            return "WC";
        case 3:
            return "Weight";
        case 4:
            return "XP";
        case 5:
            return "Level";
        case 6:
            return "Damage";
        case 7:
            return "Speed";
    }

    return "";
}

Qt::ItemFlags ArchetypesModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);;
}

bool ArchetypesModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != Qt::EditRole)
        return false;

    archetype *monster = myMonsters[index.row()];

    switch(index.column()) {
        case 0:
            monster->clone.stats.hp = value.toInt();
            break;
        case 1:
            monster->clone.stats.ac = value.toInt();
            break;
        case 2:
            monster->clone.stats.wc = value.toInt();
            break;
        case 3:
            monster->clone.weight = value.toInt();
            break;
        case 4:
            monster->clone.stats.exp = value.toLongLong();
            break;
        case 5:
            monster->clone.level = value.toInt();
            break;
        case 6:
            monster->clone.stats.dam = value.toInt();
            break;
        case 7:
            monster->clone.speed = value.toFloat();
            break;
    }

    myDirty.insert(monster);

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}
