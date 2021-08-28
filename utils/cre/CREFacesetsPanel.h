#ifndef _CREFACESETSPANEL_H
#define _CREFACESETSPANEL_H

#include <QWidget>
#include "CREPanel.h"
class QLabel;

extern "C"
{
#include "global.h"
#include "image.h"
}

class CREFacesetsPanel : public CRETPanel<face_sets>
{
    public:
        CREFacesetsPanel(QWidget* parent);
        virtual ~CREFacesetsPanel();

        virtual void setItem(face_sets* fs);

    protected:
        QLabel* myPrefix;
        QLabel* myFullname;
        QLabel* myFallback;
        QLabel* mySize;
        QLabel* myExtension;
        QLabel* myImages;
        QLabel* myLicenses;
};

#endif /* _CREFACESETSPANEL_H */
