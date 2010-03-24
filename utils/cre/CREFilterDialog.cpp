#include <QtGui>

#include "CREFilterDialog.h"
#include "CREFilter.h"

CREFilterDialog::CREFilterDialog(CREFilter* filter)
{
    myFilter = filter;
    setWindowTitle(tr("Filter parameters"));

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Filter:"), this));

    myScript = new QTextEdit(this);
    myScript->setText(filter->filter());
    layout->addWidget(myScript);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttons);
}

void CREFilterDialog::accept()
{
    myFilter->setFilter(myScript->toPlainText());
    QDialog::accept();
}
