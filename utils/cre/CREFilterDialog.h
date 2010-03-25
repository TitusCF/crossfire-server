#ifndef CRE_FILTER_DIALOG_H
#define CRE_FILTER_DIALOG_H

#include <QDialog>

class CREFilter;
class QTextEdit;

class CREFilterDialog : public QDialog
{
    Q_OBJECT

    public:
        CREFilterDialog(CREFilter* filter);

    protected:
        CREFilter* myFilter;
        QTextEdit* myScript;

        virtual void accept();

    protected slots:
        void onHelp();
};

#endif // CRE_FILTER_DIALOG_H
