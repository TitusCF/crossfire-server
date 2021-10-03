#include <QtWidgets>
#include <QSortFilterProxyModel>

#include "EditMonstersDialog.h"
#include "ArchetypesModel.h"
#include "ResourcesManager.h"


EditMonstersDialog::EditMonstersDialog(ResourcesManager *resources) {
    setWindowTitle(tr("Edit monsters"));

    QGridLayout* layout = new QGridLayout(this);

    QTableView *view = new QTableView();
    layout->addWidget(view, 0, 0, 1, 2);

    QPushButton* save = new QPushButton(tr("Save"), this);
    connect(save, SIGNAL(clicked()), resources, SLOT(saveArchetypes()));
    layout->addWidget(save, 1, 0, 1, 1);

    QPushButton *close = new QPushButton(tr("Close"), this);
    connect(close, SIGNAL(clicked()), this, SLOT(reject()));
    layout->addWidget(close, 1, 1, 1, 1);

    setLayout(layout);

    auto model = new ArchetypesModel();
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(model);
    view->setModel(proxyModel);
    view->setEditTriggers(QAbstractItemView::DoubleClicked);
    view->setSortingEnabled(true);
    connect(model, SIGNAL(archetypeModified(archetype *)), resources, SLOT(archetypeModified(archetype *)));
}
