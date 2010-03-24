#include "CREFilteredModel.h"
#include "CRETreeItem.h"
#include "CREFilter.h"

CREFilteredModel::CREFilteredModel(CREFilter* filter, QObject* parent) : QSortFilterProxyModel(parent)
{
    Q_ASSERT(filter);
    myFilter = filter;
}

bool CREFilteredModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (!source_parent.isValid() || source_parent.data(Qt::UserRole).value<void*>() == NULL)
        return false;
    const CRETreeItem* item = reinterpret_cast<const CRETreeItem*>(source_parent.data(Qt::UserRole).value<void*>());
    if (!item)
        return false;

    return false; //myFilter->showItem(item);
}
