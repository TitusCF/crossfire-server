#include "CREReportDisplay.h"
#include <QtGui>

CREReportDisplay::CREReportDisplay(const QString& report)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("Report:"), this));

    QTextEdit* view = new QTextEdit(this);
    view->setReadOnly(true);
    view->setText(report);
    layout->addWidget(view);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close | QDialogButtonBox::Help, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    /*connect(buttons, SIGNAL(helpRequested()), this, SLOT(onHelp()));*/
    layout->addWidget(buttons);

    setSizeGripEnabled(true);
}
