#ifndef CRETREEITEMRANDOMMAP_H
#define	CRETREEITEMRANDOMMAP_H

#include "CRETreeItem.h"

class CRERandomMap;

class CRETreeItemRandomMap : public CRETreeItem
{
    Q_OBJECT

public:
    CRETreeItemRandomMap(const CRERandomMap* map);
    virtual ~CRETreeItemRandomMap();

    virtual QString getPanelName() const { return "Random map"; }
    virtual void fillPanel(QWidget* panel);

protected:
    const CRERandomMap* myMap;
};

#endif	/* CRETREEITEMRANDOMMAP_H */

