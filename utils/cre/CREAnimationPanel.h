#ifndef CREANIMATIONPANEL_H
#define CREANIMATIONPANEL_H

#include <QObject>
#include <QtGui>

extern "C" {
#include "global.h"
#include "image.h"
}

class CREAnimationControl;

class CREAnimationPanel : public QWidget
{
    Q_OBJECT

    public:
        CREAnimationPanel();
        void setAnimation(const Animations* animation);

    protected:
        const Animations* myAnimation;

        QTreeWidget* myUsing;
        CREAnimationControl* myDisplay;
};

#endif // CREANIMATIONPANEL_H
