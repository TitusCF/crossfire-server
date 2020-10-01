#ifndef CLASS_CRE_FACE_PANEL_H
#define CLASS_CRE_FACE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
}

class CREFacePanel : public CRETPanel<const Face>
{
    Q_OBJECT

    public:
        CREFacePanel(QWidget* parent);
        virtual void setItem(const Face* face);

    protected:
        const Face* myFace;

        QTreeWidget* myUsing;
        QComboBox* myColor;
        QLineEdit* myFile;
        QPushButton* mySave;

    private slots:
        void saveClicked(bool);
        void makeSmooth(bool);
};

#endif // CLASS_CRE_FACE_PANEL_H
