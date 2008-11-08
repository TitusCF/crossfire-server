#ifndef CLASS_CRE_ARCHETYPE_PANEL_H
#define CLASS_CRE_ARCHETYPE_PANEL_H

#include <QObject>
#include <QtGui>

extern "C" {
#include "global.h"
}

class CREArchetypePanel : public QWidget
{
    Q_OBJECT

    public:
        CREArchetypePanel();
        void setArchetype(const archt* archetype);

    protected:
        const archt* myArchetype;
};

#endif // CLASS_CRE_ARCHETYPE_PANEL_H
