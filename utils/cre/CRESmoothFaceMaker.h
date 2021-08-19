#ifndef CRESMOOTHFACEMAKER_H
#define CRESMOOTHFACEMAKER_H

#include <QDialog>
#include <QObject>

class QLineEdit;
class FaceComboBox;

class CRESmoothFaceMaker : public QDialog
{
    Q_OBJECT

    public:
        CRESmoothFaceMaker();
        virtual ~CRESmoothFaceMaker();

        void setSelectedFace(const Face* face);
        QString destination() const;

        void setAutoClose(bool autoClose = true);

    protected slots:
        void makeSmooth();
        void browse(bool);
        void destinationEdited(const QString&);

    private:
        bool myAutoClose;
        bool myOverwrite;
        QLineEdit* myDestination;
        FaceComboBox* myFace;
};

#endif /* CRESMOOTHFACEMAKER_H */
