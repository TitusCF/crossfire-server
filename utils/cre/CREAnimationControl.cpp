#include <Qt>

#include "CREAnimationControl.h"
#include "CREAnimationWidget.h"
#include "face.h"

CREAnimationControl::CREAnimationControl(QWidget* parent) : QWidget(parent)
{
    myAnimation = NULL;
    myStep = 0;
    myLastStep = 0;

    CREAnimationWidget* widget;
    QGridLayout* layout = new QGridLayout(this);

    for (int dir = 1; dir <= 8; dir++)
    {
        widget = new CREAnimationWidget(this);
        widget->setVisible(false);

        layout->addWidget(widget, 2 + freearr_y[dir], 2 + freearr_x[dir]);
        myWidgets.append(widget);
    }

    myTimer = new QTimer(this);
    connect(myTimer, SIGNAL(timeout()), this, SLOT(step()));
}

void CREAnimationControl::setAnimation(const Animations* animation, int facings)
{
    myTimer->stop();
    myAnimation = animation;
    display(animation, facings);
    myTimer->start(250);
}

void CREAnimationControl::display(const Animations* animation, int facings)
{
    myFacings = facings == -1 ? animation->facings : facings;

    int widget, widgetStep, faceCount, face, faceCurrent;

    for (widget = 0; widget < 8; widget++)
        myWidgets[widget]->setVisible(false);

    widget = 0;

    if (myFacings == 1)
    {
        widgetStep = 8;
        faceCount = animation->num_animations;
    }
    else if (myFacings == 2)
    {
        widgetStep = 4;
        faceCount = animation->num_animations / 2;
    }
    else if (myFacings == 4)
    {
        widgetStep = 2;
        faceCount = animation->num_animations / 4;
    }
    else
    {
        widgetStep = 1;
        faceCount = animation->num_animations / 8;
    }

    face = facings == -1 ? 0 : 1;
    while (widget < 8)
    {
        myWidgets[widget]->setVisible(true);
        QList<int> faces;
        for (faceCurrent = 0; faceCurrent < faceCount; faceCurrent++)
            faces.append(animation->faces[face++]->number);
        myWidgets[widget]->setAnimation(faces);

        widget += widgetStep;
    }
}

void CREAnimationControl::step()
{
    if (myAnimation == NULL)
        return;

    for (int w = 0; w < myWidgets.size(); w++)
        myWidgets[w]->step();
}
