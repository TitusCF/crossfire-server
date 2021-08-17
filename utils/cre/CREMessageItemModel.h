#ifndef CREMESSAGEITEMMODEL_H
#define CREMESSAGEITEMMODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include "MessageFile.h"

/**
 * Item model for a message file.
 */
class CREMessageItemModel : public QAbstractItemModel
{
public:
    CREMessageItemModel(QObject* parent);
    virtual ~CREMessageItemModel();

    void setMessage(MessageFile* message);

    void setSelectedRule(const QModelIndex& index);

    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    virtual QModelIndex parent(const QModelIndex& index) const override;
    virtual int rowCount(const QModelIndex & parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    void moveUpDown(int row, bool up);
    void duplicateRow(int row);

protected:
    MessageFile* myMessage;   /**< Message file currently displayed. */
    QSet<int> myBlue, myRed;  /**< Indexes of message rules that should be with a blue or red background. */
};

#endif /* CREMESSAGEITEMMODEL_H */

