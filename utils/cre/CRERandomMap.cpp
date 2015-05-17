#include "CRERandomMap.h"

CRERandomMap::CRERandomMap(CREMapInformation* source, int x, int y, const char* parameters)
{
    myMap = source;
    myX = x;
    myY = y;
    memset(&myParameters, 0, sizeof myParameters);
    set_random_map_variable(&myParameters, parameters);
}

const CREMapInformation* CRERandomMap::map() const
{
    return myMap;
}

int CRERandomMap::x() const
{
    return myX;
}

int CRERandomMap::y() const
{
    return myY;
}

const RMParms* CRERandomMap::parameters() const
{
    return &myParameters;
}
