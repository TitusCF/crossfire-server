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

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close | QDialogButtonBox::Help, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttons, SIGNAL(helpRequested()), this, SLOT(onHelp()));
    layout->addWidget(buttons);
}

void CREFilterDialog::accept()
{
    myFilter->setFilter(myScript->toPlainText());
    QDialog::accept();
}

void CREFilterDialog::onHelp()
{
    QMessageBox::information(this, tr("Filter help"), tr("Enter the script expression with which to filter items in the view. Current item is <b>item</b>, and it has the following properties:<br /><ul><li>for an archetype: name, clone</li><li>for a formulae: title, chance, difficulty, archs</li><li>for an artifact: item, chance, difficulty, allowed</li><li>for an object (for clone and item): type</li></ul><br />An item is shown if the expression evaluates to <i>true</i>.If a property is not defined for the current item, it will not be shown.<br /><br />Examples:<ul><li>items of type 5: <i>item.clone.type == 5</i></li><li>artifact allowed for all items of the type: <i>item.allowed.length == 0</i></il></ul>"));
}
