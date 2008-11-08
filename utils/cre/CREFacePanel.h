#ifndef CLASS_CRE_FACE_PANEL_H
#define CLASS_CRE_FACE_PANEL_H

#include <QObject>
#include <QtGui>

extern "C" {
#include "global.h"
}

class CREFacePanel : public QWidget
{
    Q_OBJECT

    public:
        CREFacePanel();
        void setFace(const New_Face* face);

    protected:
        const New_Face* myFace;

        QTreeWidget* myUsing;
        QComboBox* myColor;
        QLineEdit* myFile;
        QPushButton* mySave;

    private slots:
        void saveClicked(bool);
};

#endif // CLASS_CRE_FACE_PANEL_H
