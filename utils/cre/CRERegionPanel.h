#ifndef _CREREGIONPANEL_H
#define	_CREREGIONPANEL_H

#include <QWidget>
class QLabel;

extern "C"
{
#include "global.h"
#include "map.h"
}

class CRERegionPanel : public QWidget
{
    public:
        CRERegionPanel();
        virtual ~CRERegionPanel();

        void setRegion(regiondef* region);

    protected:
        QLabel* myShortName;
        QLabel* myName;
        QLabel* myMessage;
        QLabel* myJail;
        QLabel* myJailX;
        QLabel* myJailY;
};

#endif	/* _CREREGIONPANEL_H */

