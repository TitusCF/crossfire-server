#include <Qt>
#include <QtGui>
#include <CREMainWindow.h>
#include <CREResourcesWindow.h>
#include "CREMapInformationManager.h"

extern "C" {
#include "global.h"
}

CREMainWindow::CREMainWindow()
{
    myArea = new QMdiArea();
    setCentralWidget(myArea);

    createActions();
    createMenus();

    statusBar()->showMessage(tr("Ready"));
    myMapBrowseStatus = new QLabel(tr("Browsing maps..."));
    statusBar()->addPermanentWidget(myMapBrowseStatus);

    setWindowTitle(tr("Crossfire Resource Editor"));

    myMapManager = new CREMapInformationManager(this);
    connect(myMapManager, SIGNAL(browsingMap(const QString&)), this, SLOT(browsingMap(const QString&)));
    connect(myMapManager, SIGNAL(finished()), this, SLOT(browsingFinished()));
    myMapManager->start();
}

void CREMainWindow::closeEvent(QCloseEvent* event)
{
    myMapManager->cancel();
    QMainWindow::closeEvent(event);
}

void CREMainWindow::createActions()
{
    myOpenArtifacts = new QAction(tr("Artifacts"), this);
    connect(myOpenArtifacts, SIGNAL(triggered()), this, SLOT(onOpenArtifacts()));

    myOpenArchetypes = new QAction(tr("Archetypes"), this);
    connect(myOpenArchetypes, SIGNAL(triggered()), this, SLOT(onOpenArchetypes()));

    myOpenTreasures = new QAction(tr("Treasures"), this);
    connect(myOpenTreasures, SIGNAL(triggered()), this, SLOT(onOpenTreasures()));

    myOpenAnimations = new QAction(tr("Animations"), this);
    connect(myOpenAnimations, SIGNAL(triggered()), this, SLOT(onOpenAnimations()));

    myOpenFormulae = new QAction(tr("Formulae"), this);
    connect(myOpenFormulae, SIGNAL(triggered()), this, SLOT(onOpenFormulae()));

    myOpenResources = new QAction(tr("Resources"), this);
    connect(myOpenResources, SIGNAL(triggered()), this, SLOT(onOpenResources()));

    myOpenFaces = new QAction(tr("Faces"), this);
    connect(myOpenFaces, SIGNAL(triggered()), this, SLOT(onOpenFaces()));

    myOpenMaps = new QAction(tr("Maps"), this);
    connect(myOpenMaps, SIGNAL(triggered()), this, SLOT(onOpenMaps()));

    mySaveFormulae = new QAction(tr("Formulae"), this);
    connect(mySaveFormulae, SIGNAL(triggered()), this, SLOT(onSaveFormulae()));
}

void CREMainWindow::createMenus()
{
    myOpenMenu = menuBar()->addMenu(tr("&Open"));
    myOpenMenu->addAction(myOpenResources);
    myOpenMenu->addAction(myOpenArtifacts);
    myOpenMenu->addAction(myOpenArchetypes);
    myOpenMenu->addAction(myOpenTreasures);
    myOpenMenu->addAction(myOpenAnimations);
    myOpenMenu->addAction(myOpenFormulae);
    myOpenMenu->addAction(myOpenFaces);
    myOpenMenu->addAction(myOpenMaps);

    mySaveMenu = menuBar()->addMenu(tr("&Save"));
    mySaveMenu->addAction(mySaveFormulae);
}

void CREMainWindow::doResourceWindow(DisplayMode mode)
{
    QWidget* resources = new CREResourcesWindow(myMapManager, mode);
    connect(this, SIGNAL(updateFilters()), resources, SLOT(updateFilters()));
    connect(resources, SIGNAL(filtersModified()), this, SLOT(onFiltersModified()));
    myArea->addSubWindow(resources);
    resources->show();
}

void CREMainWindow::onOpenArtifacts()
{
    doResourceWindow(DisplayArtifacts);
}

void CREMainWindow::onOpenArchetypes()
{
    doResourceWindow(DisplayArchetypes);
}

void CREMainWindow::onOpenTreasures()
{
    doResourceWindow(DisplayTreasures);
}

void CREMainWindow::onOpenAnimations()
{
    doResourceWindow(DisplayAnimations);
}

void CREMainWindow::onOpenFormulae()
{
    doResourceWindow(DisplayFormulae);
}

void CREMainWindow::onOpenFaces()
{
    doResourceWindow(DisplayFaces);
}

void CREMainWindow::onOpenMaps()
{
    doResourceWindow(DisplayMaps);
}

void CREMainWindow::onOpenResources()
{
    doResourceWindow(DisplayAll);
}

void CREMainWindow::onSaveFormulae()
{
}

void CREMainWindow::browsingMap(const QString& path)
{
    myMapBrowseStatus->setText(tr("Browsing map %1").arg(path));
}

void CREMainWindow::browsingFinished()
{
    statusBar()->showMessage(tr("Finished browsing maps."), 5000);
    myMapBrowseStatus->setVisible(false);
}

void CREMainWindow::onFiltersModified()
{
    emit updateFilters();
}
