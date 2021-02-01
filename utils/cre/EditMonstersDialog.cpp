#include <QtWidgets>
#include <QSortFilterProxyModel>

#include "EditMonstersDialog.h"
#include "ArchetypesModel.h"
#include "ResourcesManager.h"

#include "ArchetypeWriter.h"


EditMonstersDialog::EditMonstersDialog(ResourcesManager *resources) : myResources(resources) {
    setWindowTitle(tr("Edit monsters"));

    QGridLayout* layout = new QGridLayout(this);

    QTableView *view = new QTableView();
    layout->addWidget(view, 0, 0, 1, 2);

    QPushButton* save = new QPushButton(tr("Save"), this);
    connect(save, SIGNAL(clicked()), this, SLOT(onSaveChanges()));
    layout->addWidget(save, 1, 0, 1, 1);

    QPushButton *close = new QPushButton(tr("Close"), this);
    connect(close, SIGNAL(clicked()), this, SLOT(reject()));
    layout->addWidget(close, 1, 1, 1, 1);

    setLayout(layout);

    myModel = new ArchetypesModel();
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(myModel);
    view->setModel(proxyModel);
    view->setEditTriggers(QAbstractItemView::DoubleClicked);
    view->setSortingEnabled(true);
}

void EditMonstersDialog::write(const QString &filename, QList<const archt*> archs) {
    auto buf = stringbuffer_new();
    ArchetypeWriter writer;

    for (auto arch = archs.begin(); arch != archs.end(); arch++) {
        writer.write(*arch, buf);
    }

    size_t length = stringbuffer_length(buf);
    char *data = stringbuffer_finish(buf);

    QFile out(filename);
    out.open(QIODevice::WriteOnly);
    out.write(data, length);
    free(data);
}

void EditMonstersDialog::onSaveChanges() {
    auto dirty = myModel->dirty();

    auto &origins = myResources->origins();

    for (auto a = dirty.begin(); a != dirty.end(); a++) {
        for (auto file = origins.begin(); file != origins.end(); file++) {
            if (file.value().contains(*a)) {
                write(file.key(), file.value());
            }
        }
    }

    myModel->markClean();
}
