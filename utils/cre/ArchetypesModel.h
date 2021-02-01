#ifndef ARCHETYPESMODEL_H
#define ARCHETYPESMODEL_H

#include <set>

class ArchetypesModel : public QAbstractTableModel {
public:
  ArchetypesModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

  const std::set<archetype *>& dirty() const { return myDirty; }
  void markClean() { myDirty.clear(); }

private:
  std::vector<archetype *> myMonsters;
  std::set<archetype *> myDirty;
};

#endif /* ARCHETYPESMODEL_H */

