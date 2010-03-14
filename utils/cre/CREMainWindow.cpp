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

    connect(CREMapInformationManager::instance(), SIGNAL(browsingMap(const QString&)), this, SLOT(browsingMap(const QString&)));
    connect(CREMapInformationManager::instance(), SIGNAL(finished()), this, SLOT(browsingFinished()));
    CREMapInformationManager::instance()->start();
}

void CREMainWindow::closeEvent(QCloseEvent* event)
{
    CREMapInformationManager::instance()->cancel();
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

    mySaveMenu = menuBar()->addMenu(tr("&Save"));
    mySaveMenu->addAction(mySaveFormulae);
}

void CREMainWindow::onOpenArtifacts()
{
    QWidget* artifacts = new CREResourcesWindow(DisplayArtifacts);
    myArea->addSubWindow(artifacts);
    artifacts->show();
}

void CREMainWindow::onOpenArchetypes()
{
    QWidget* archetypes = new CREResourcesWindow(DisplayArchetypes);
    myArea->addSubWindow(archetypes);
    archetypes->show();
}

void CREMainWindow::onOpenTreasures()
{
    QWidget* myTreasures = new CREResourcesWindow(DisplayTreasures);
    myArea->addSubWindow(myTreasures);
    myTreasures->show();
}

void CREMainWindow::onOpenAnimations()
{
    QWidget* myAnimations = new CREResourcesWindow(DisplayAnimations);
    myArea->addSubWindow(myAnimations);
    myAnimations->show();
}

void CREMainWindow::onOpenFormulae()
{
    QWidget* myFormulae = new CREResourcesWindow(DisplayFormulae);
    myArea->addSubWindow(myFormulae);
    myFormulae->show();
}

void CREMainWindow::onOpenFaces()
{
    QWidget* myResources = new CREResourcesWindow(DisplayFaces);
    myArea->addSubWindow(myResources);
    myResources->show();
}

void CREMainWindow::onOpenResources()
{
    QWidget* myResources = new CREResourcesWindow();
    myArea->addSubWindow(myResources);
    myResources->show();
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
