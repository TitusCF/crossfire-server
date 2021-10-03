#include "ArchetypesModel.h"
#include "assets.h"
#include "AssetsManager.h"
#include "CREPixmap.h"
#include "CREWrapperObject.h"

#define PROPERTY_COUNT  8
const char *properties[PROPERTY_COUNT] = {
    "hp",
    "ac",
    "wc",
    "weight",
    "experience",
    "level",
    "damage",
    "speed"
};

ArchetypesModel::ArchetypesModel() {
    getManager()->archetypes()->each([this] (archetype *arch) {
        if (QUERY_FLAG(&arch->clone, FLAG_MONSTER) && (!arch->head)) {
            CREWrapperObject *wrapper = new CREWrapperObject();
            wrapper->setObject(&arch->clone);
            myMonsters.push_back(wrapper);
        }
    });
}

ArchetypesModel::~ArchetypesModel() {
    qDeleteAll(myMonsters);
}

int ArchetypesModel::rowCount(const QModelIndex &parent) const {
    (void)parent;
    return myMonsters.size();
}
  
int ArchetypesModel::columnCount(const QModelIndex &parent) const {
    (void)parent;
    return PROPERTY_COUNT;
}

QVariant ArchetypesModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QVariant();
    }
    auto monster = myMonsters[index.row()];
    return monster->property(properties[index.column()]);
}

QVariant ArchetypesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole) {
            return myMonsters[section]->name();
        } else if (role == Qt::DecorationRole) {
            return CREPixmap::getIcon(myMonsters[section]->getObject()->face->number);
        }
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    return properties[section];
}

Qt::ItemFlags ArchetypesModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.column() < 0 || index.column() >= PROPERTY_COUNT) {
        return QAbstractItemModel::flags(index);
    }

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    int pi = CREWrapperObject::staticMetaObject.indexOfProperty(properties[index.column()]);
    if (pi != -1 && CREWrapperObject::staticMetaObject.property(pi).isWritable()) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool ArchetypesModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != Qt::EditRole)
        return false;

    auto monster = myMonsters[index.row()];
    monster->setProperty(properties[index.column()], value);

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    emit archetypeModified(monster->arch()->arch());
    return true;
}
