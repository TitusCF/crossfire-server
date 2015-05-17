#ifndef CREANIMATIONPANEL_H
#define CREANIMATIONPANEL_H

#include <QObject>
#include <QtGui>
#include "CREPanel.h"

extern "C" {
#include "global.h"
#include "image.h"
}

class CREAnimationControl;

class CREAnimationPanel : public CRETPanel<const Animations>
{
    Q_OBJECT

    public:
        CREAnimationPanel();
        virtual void setItem(const Animations* animation);

    protected:
        const Animations* myAnimation;

        QTreeWidget* myUsing;
        QTreeWidget* myFaces;
        CREAnimationControl* myDisplay;
};

#endif // CREANIMATIONPANEL_H
