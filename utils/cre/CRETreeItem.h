#ifndef CRETREEITEM_H
#define CRETREEITEM_H

#include <QObject>
#include <Qt>
#include <QHash>
#include <QPointer>

class CRETreeItem : public QObject
{
    Q_OBJECT

    public:
        virtual QString getPanelName() const = 0;
        virtual void fillPanel(QWidget* panel) = 0;
};

#endif // CRETREEITEM_H
