#include "CRETreeItemRandomMap.h"
#include "CRERandomMapPanel.h"
#include "CRERandomMap.h"

CRETreeItemRandomMap::CRETreeItemRandomMap(const CRERandomMap* map)
{
    myMap = map;
}

CRETreeItemRandomMap::~CRETreeItemRandomMap()
{
}

void CRETreeItemRandomMap::fillPanel(QWidget* panel)
{
    CRERandomMapPanel* p = static_cast<CRERandomMapPanel*>(panel);
    p->setRandomMap(myMap);
}
