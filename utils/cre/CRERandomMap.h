#ifndef CRERANDOMMAP_H
#define	CRERANDOMMAP_H

#include <QString>

#include "random_maps/rproto.h"

class CREMapInformation;

class CRERandomMap
{
public:
    CRERandomMap(CREMapInformation* source, int x, int y, const char* parameters);

    const CREMapInformation* map() const;
    int x() const;
    int y() const;
    const RMParms* parameters() const;

private:
    CREMapInformation* myMap;
    int myX;
    int myY;
    RMParms myParameters;
};

#endif	/* CRERANDOMMAP_H */
