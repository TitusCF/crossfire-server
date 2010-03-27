#include "CREReportDialog.h"
#include <QtGui>

CREReportDialog::CREReportDialog()
{
    setWindowTitle(tr("Report generation"));

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Headers:"), this));

    myHeaders = new QTextEdit(this);
    layout->addWidget(myHeaders);

    layout->addWidget(new QLabel(tr("Fields to display:"), this));

    myFields = new QTextEdit(this);
    layout->addWidget(myFields);

    layout->addWidget(new QLabel(tr("Sort:"), this));
    mySort = new QLineEdit(this);
    layout->addWidget(mySort);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close | QDialogButtonBox::Help, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    /*connect(buttons, SIGNAL(helpRequested()), this, SLOT(onHelp()));*/
    layout->addWidget(buttons);
}

QString CREReportDialog::getHeaders()
{
    return myHeaders->toPlainText();
}

QString CREReportDialog::getFields()
{
    return myFields->toPlainText();
}

QString CREReportDialog::getSort()
{
    return mySort->text();
}
