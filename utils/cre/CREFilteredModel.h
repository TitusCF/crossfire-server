#ifndef CRE_FILTERED_MODEL_H
#define CRE_FILTERED_MODEL_H

#include <QSortFilterProxyModel>

class CREFilter;

class CREFilteredModel : public QSortFilterProxyModel
{
    public:
        CREFilteredModel(CREFilter* filter, QObject* parent);

    protected:
        CREFilter* myFilter;
        virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
};

#endif // CRE_FILTERED_MODEL_H
