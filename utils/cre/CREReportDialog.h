#ifndef CRE_REPORT_DIALOG_H
#define CRE_REPORT_DIALOG_H

#include <QDialog>

class QTextEdit;
class QLineEdit;

class CREReportDialog : public QDialog
{
    Q_OBJECT

    public:
        CREReportDialog();

        QString getHeaders();
        QString getFields();
        QString getSort();

    protected:
        QTextEdit* myHeaders;
        QTextEdit* myFields;
        QLineEdit* mySort;
};

#endif // CRE_REPORT_DIALOG_H
