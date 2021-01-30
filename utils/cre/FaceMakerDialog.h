#ifndef FACEMAKERDIALOG_H
#define FACEMAKERDIALOG_H

#include <QDialog>
#include <QObject>

#include "ResourcesManager.h"

class FaceMakerDialog : public QDialog {
    Q_OBJECT

    public:
        FaceMakerDialog(QWidget* parent, ResourcesManager* manager);

    protected slots:
        void makeFaces();

    private:
        ResourcesManager* myManager;
        QTextEdit* mySettings;

        QColor parse(const QString& color);

};

#endif /* FACEMAKERDIALOG_H */

