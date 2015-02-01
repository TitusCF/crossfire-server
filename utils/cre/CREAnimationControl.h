#ifndef CREANIMATIONCONTROL_H
#define CREANIMATIONCONTROL_H

#include <QObject>
#include <QtGui>

extern "C" {
#include "global.h"
#include "face.h"
}

class CREAnimationWidget;

class CREAnimationControl : public QWidget
{
    Q_OBJECT

    public:
        CREAnimationControl(QWidget* parent);

        /**
         * Define the animation to display.
         * @param animation what to display, must not be NULL.
         * @param facings if -1 then uses the facings of the animation, else number of facings to display.
         */
        void setAnimation(const Animations* animation, int facings = -1);

    protected:
        const Animations* myAnimation;
        int myStep;
        int myLastStep;
        int myFacings;

        void display(const Animations* animation, int facings = -1);
        QList<CREAnimationWidget*> myWidgets;

    private slots:
        void step();
};

#endif // CREANIMATIONCONTROL_H
