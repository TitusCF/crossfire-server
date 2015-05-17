#ifndef CRETREEITEM_H
#define CRETREEITEM_H

#include <QObject>
#include <QMenu>

#include "CREPanel.h"

class CRETreeItem : public QObject
{
    Q_OBJECT

    public:
        virtual QString getPanelName() const = 0;
        virtual void fillPanel(QWidget* panel) = 0;
        virtual void fillContextMenu(QMenu*) { };
};

template<typename T>
class CRETTreeItem : public CRETreeItem
{
public:
    CRETTreeItem(T* item, const QString& name)
    {
        myItem = item;
        myName = name;
    };
    virtual QString getPanelName() const
    {
        return myName;
    }
    virtual void fillPanel(QWidget* panel)
    {
        CRETPanel<T>* p = static_cast<CRETPanel<T>*>(panel);
        p->setItem(myItem);
    }
protected:
    T* myItem;
    QString myName;
};

#endif // CRETREEITEM_H
