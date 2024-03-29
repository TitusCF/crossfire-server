#ifndef CLASS_CRE_FACE_PANEL_H
#define CLASS_CRE_FACE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
}

class CREMapInformationManager;

class CREFacePanel : public CRETPanel<const Face>
{
    Q_OBJECT

    public:
        CREFacePanel(QWidget* parent, CREMapInformationManager* maps);
        virtual void setItem(const Face* face);

    protected:
        CREMapInformationManager* myMaps;
        const Face* myFace;

        QTreeWidget* myUsing;
        QComboBox* myColor;
        QLineEdit* myFile;
        QPushButton* mySave;
        QTreeWidget* myLicenses;

    private slots:
        void saveClicked(bool);
        void makeSmooth(bool);
};

#endif // CLASS_CRE_FACE_PANEL_H
