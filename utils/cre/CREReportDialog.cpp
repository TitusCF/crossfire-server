#include <QtWidgets>

#include "CREReportDialog.h"
#include "CRESettings.h"
#include "CREReportDefinition.h"

CREReportDialog::CREReportDialog()
{
    CRESettings settings;
    settings.loadReports(myReports);

    setWindowTitle(tr("Report parameters"));

    QGridLayout* layout = new QGridLayout(this);

    myList = new QListWidget(this);
    layout->addWidget(myList, 0, 0, 10, 2);

    QPushButton* add = new QPushButton(tr("Add"), this);
    connect(add, SIGNAL(clicked()), this, SLOT(onAdd()));
    layout->addWidget(add, 10, 0, 1, 1);

    QPushButton* del = new QPushButton(tr("Remove"), this);
    connect(del, SIGNAL(clicked()), this, SLOT(onDelete()));
    layout->addWidget(del, 10, 1, 1, 1);

    layout->addWidget(new QLabel(tr("Name:"), this), 0, 2, 1, 3);

    myName = new QLineEdit(this);
    layout->addWidget(myName, 1, 2, 1, 3);

    layout->addWidget(new QLabel(tr("Header:"), this), 2, 2, 1, 3);

    myHeader = new QTextEdit(this);
    layout->addWidget(myHeader, 3, 2, 1, 3);
    myHeader->setWhatsThis(tr("Text to display at the top of the report."));

    layout->addWidget(new QLabel(tr("Footer:"), this), 4, 2, 1, 3);

    myFooter = new QTextEdit(this);
    layout->addWidget(myFooter, 5, 2, 1, 3);
    myFooter->setWhatsThis(tr("Text to display at the bottom of the report."));

    layout->addWidget(new QLabel(tr("Item sort:"), this), 6, 2, 1, 3);

    mySort = new QTextEdit(this);
    layout->addWidget(mySort, 7, 2, 1, 3);
    mySort->setWhatsThis(tr("Expression used to sort items. The items to be compared are 'left' and 'right', and the expression should be true if left < right, false else."));

    layout->addWidget(new QLabel(tr("Item display:"), this), 8, 2, 1, 3);

    myDisplay = new QTextEdit(this);
    layout->addWidget(myDisplay, 9, 2, 1, 3);
    myDisplay->setWhatsThis(tr("Expression used to display one item. The current item is 'item', and the expression should be a string value."));

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close | QDialogButtonBox::Help, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttons, SIGNAL(helpRequested()), this, SLOT(onHelp()));

    layout->addWidget(buttons, 10, 2, 3, 1);

    setLayout(layout);
    connect(myList, SIGNAL(currentRowChanged(int)), this, SLOT(currentRowChanged(int)));
    refreshList();
}

void CREReportDialog::accept()
{
    saveCurrentReport();
    CRESettings settings;
    settings.saveReports(myReports);
    QDialog::accept();
}

void CREReportDialog::reject()
{
    if (QMessageBox::question(this, tr("Discard changes?"), tr("You are about to discard all changes!\nAre you sure?"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    QDialog::reject();
}

void CREReportDialog::onHelp()
{
    QMessageBox::information(this, tr("Report help"), tr(
R"(
This dialog allows to define a report.<br />
<br />
A report consists of an optional header, a line for each item of the view optionally sorted, an optional footer.
<br />
<br />
'Item sort' contains an optional script expression used to sort items. The items to be compared are 'left' and 'right', and the expression should be true if left &lt; right, false else.
<br />
<b>Example: </b><i>left.name.toLowerCase() &lt; right.name.toLowerCase()</i> will sort by ascending item's name (case unsensitive).
<br />
<br />
'Item display' is the expression used to display one item. The current item is 'item', and the expression should be a string value.
<br />
<b>Example: </b><i>item.name + " " + item.level</i> will display for each item its name and level.
)"));
}

void CREReportDialog::onAdd()
{
    saveCurrentReport();
    CREReportDefinition* report = new CREReportDefinition();
    report->setName(tr("<new Report>"));
    myReports.reports().append(report);
    refreshList();
    myList->setCurrentRow(myReports.reports().size() - 1);
}

void CREReportDialog::onDelete()
{
    if (myReportIndex == -1)
        return;

    CREReportDefinition* report = myReports.reports()[myReportIndex];
    if (QMessageBox::question(this, tr("Delete Report?"), tr("Really delete Report '%1'?").arg(report->name()), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    delete report;
    myReports.reports().removeAt(myReportIndex);
    myReportIndex = -1;
    refreshList();
}

void CREReportDialog::refreshList()
{
    myList->clear();

    foreach(const CREReportDefinition* report, myReports.reports())
    {
        myList->addItem(report->name());
    }
    myReportIndex = -1;
}

void CREReportDialog::saveCurrentReport()
{
    if (myReportIndex != -1)
    {
        CREReportDefinition* report = myReports.reports()[myReportIndex];
        report->setName(myName->text());
        report->setHeader(myHeader->toPlainText());
        report->setItemDisplay(myDisplay->toPlainText());
        report->setItemSort(mySort->toPlainText());
        report->setFooter(myFooter->toPlainText());
        myList->item(myReportIndex)->setText(report->name());
    }
}

void CREReportDialog::currentRowChanged(int currentRow)
{
    saveCurrentReport();

    myReportIndex = -1;
    if (currentRow >= 0 && currentRow < myReports.reports().size())
    {
        const CREReportDefinition* report = myReports.reports()[currentRow];
        myName->setText(report->name());
        myHeader->setText(report->header());
        myFooter->setText(report->footer());
        mySort->setText(report->itemSort());
        myDisplay->setText(report->itemDisplay());
        myReportIndex = currentRow;
    }
}
