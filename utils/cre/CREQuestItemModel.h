#ifndef _CREQUESTITEMMODEL_H
#define _CREQUESTITEMMODEL_H

#include <QObject>
#include <QAbstractItemModel>

extern "C" {
#include "global.h"
#include "quest.h"
}

/**
 * Item model for the steps of a quest.
 */
class CREQuestItemModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        CREQuestItemModel(QObject* parent);
        virtual ~CREQuestItemModel();

        quest_definition *quest() const;
        void setQuest(quest_definition *quest);

        void moveUp(int step);
        void moveDown(int step);

        virtual int columnCount(const QModelIndex& parent) const;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;
        virtual QModelIndex parent(const QModelIndex& index) const;
        virtual int rowCount(const QModelIndex & parent) const;
        virtual QVariant data(const QModelIndex& index, int role) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
        virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

    signals:
        void questModified(quest_definition *quest);

    public slots:
        void addStep(bool);

    protected:
        quest_step_definition *getStep(int step) const;
        quest_definition *myQuest;
        size_t myStepCount;
};

#endif /* _CREQUESTITEMMODEL_H */
