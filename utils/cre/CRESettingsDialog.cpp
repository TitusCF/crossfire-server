#include "CRESettingsDialog.h"
#include "CRESettings.h"
#include <QtWidgets>

CRESettingsDialog::CRESettingsDialog(CRESettings* settings)
{
    setWindowTitle(tr("CRE settings"));

    QGridLayout* layout = new QGridLayout();

    int line = 0;

    layout->addWidget(new QLabel(tr("Cache directory:"), this), line, 0);
    myMapCache = new QLineEdit(this);
    myMapCache->setText(settings->mapCacheDirectory());
    if (myMapCache->text().isEmpty())
        myMapCache->setText(QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0]);
    layout->addWidget(myMapCache, line, 1);

    line++;

    layout->addWidget(new QLabel(tr("This directory is used to store information between program runs.\nIt should be persistent (don't put in a temporary directory cleaned now and then),\nbut data can be recreated if needed."), this), line, 0, 1, 2);
    line++;

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttons, line, 0, 1, 2);

    setLayout(layout);
}

QString CRESettingsDialog::mapCache() const
{
    return myMapCache->text();
}
