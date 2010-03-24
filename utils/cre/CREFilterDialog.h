#ifndef CRE_FILTER_DIALOG_H
#define CRE_FILTER_DIALOG_H

class CREFilter;
class QTextEdit;

class CREFilterDialog : public QDialog
{
    public:
        CREFilterDialog(CREFilter* filter);

    protected:
        CREFilter* myFilter;
        QTextEdit* myScript;

        virtual void accept();
};

#endif // CRE_FILTER_DIALOG_H
