#include <Qt>
#include <QtGui>
#include <CREMainWindow.h>
#include <CREResourcesWindow.h>
#include "CREMapInformationManager.h"
#include "CREExperienceWindow.h"
#include "QuestManager.h"

extern "C" {
#include "global.h"
#include "MessageManager.h"
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

    myQuestManager = new QuestManager();
    myQuestManager->loadQuests();

    myMessageManager = new MessageManager();
    myMessageManager->loadMessages();
}

void CREMainWindow::closeEvent(QCloseEvent* event)
{
    myMapManager->cancel();
    delete myMapManager;
    delete myQuestManager;
    delete myMessageManager;
    QMainWindow::closeEvent(event);
}

void CREMainWindow::createActions()
{
    myOpenArtifacts = new QAction(tr("Artifacts"), this);
    myOpenArtifacts->setStatusTip(tr("List all defined artifacts."));
    connect(myOpenArtifacts, SIGNAL(triggered()), this, SLOT(onOpenArtifacts()));

    myOpenArchetypes = new QAction(tr("Archetypes"), this);
    myOpenArchetypes->setStatusTip(tr("List all defined archetypes."));
    connect(myOpenArchetypes, SIGNAL(triggered()), this, SLOT(onOpenArchetypes()));

    myOpenTreasures = new QAction(tr("Treasures"), this);
    myOpenTreasures->setStatusTip(tr("List all defined treasure lists."));
    connect(myOpenTreasures, SIGNAL(triggered()), this, SLOT(onOpenTreasures()));

    myOpenAnimations = new QAction(tr("Animations"), this);
    myOpenAnimations->setStatusTip(tr("List all defined animations."));
    connect(myOpenAnimations, SIGNAL(triggered()), this, SLOT(onOpenAnimations()));

    myOpenFormulae = new QAction(tr("Formulae"), this);
    myOpenFormulae->setStatusTip(tr("List all defined alchemy recipes."));
    connect(myOpenFormulae, SIGNAL(triggered()), this, SLOT(onOpenFormulae()));

    myOpenResources = new QAction(tr("Resources"), this);
    myOpenResources->setStatusTip(tr("List all defined elements, except experience table."));
    connect(myOpenResources, SIGNAL(triggered()), this, SLOT(onOpenResources()));

    myOpenFaces = new QAction(tr("Faces"), this);
    myOpenFaces->setStatusTip(tr("List all defined faces."));
    connect(myOpenFaces, SIGNAL(triggered()), this, SLOT(onOpenFaces()));

    myOpenMaps = new QAction(tr("Maps"), this);
    myOpenMaps->setStatusTip(tr("List all maps, with their region."));
    connect(myOpenMaps, SIGNAL(triggered()), this, SLOT(onOpenMaps()));

    myOpenQuests = new QAction(tr("Quests"), this);
    myOpenQuests->setStatusTip(tr("List all defined quests."));
    connect(myOpenQuests, SIGNAL(triggered()), this, SLOT(onOpenQuests()));

    myOpenMessages = new QAction(tr("Messages"), this);
    myOpenMessages->setStatusTip(tr("List all message files."));
    connect(myOpenMessages, SIGNAL(triggered()), this, SLOT(onOpenMessages()));

    myOpenExperience = new QAction(tr("Experience"), this);
    myOpenExperience->setStatusTip(tr("Display the experience table."));
    connect(myOpenExperience, SIGNAL(triggered()), this, SLOT(onOpenExperience()));

    mySaveFormulae = new QAction(tr("Formulae"), this);
    mySaveFormulae->setEnabled(false);
    connect(mySaveFormulae, SIGNAL(triggered()), this, SLOT(onSaveFormulae()));

    mySaveQuests = new QAction(tr("Quests"), this);
    mySaveQuests->setStatusTip(tr("Save all modified quests to disk."));
    connect(mySaveQuests, SIGNAL(triggered()), this, SLOT(onSaveQuests()));

    mySaveMessages = new QAction(tr("Messages"), this);
    mySaveMessages->setStatusTip(tr("Save all modified messages."));
    connect(mySaveMessages, SIGNAL(triggered()), this, SLOT(onSaveMessages()));
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
    myOpenMenu->addAction(myOpenQuests);
    myOpenMenu->addAction(myOpenMessages);
    myOpenMenu->addAction(myOpenExperience);
    myOpenMenu->addSeparator();
    QAction* exit = myOpenMenu->addAction(tr("&Exit"));
    exit->setStatusTip(tr("Close the application."));
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    mySaveMenu = menuBar()->addMenu(tr("&Save"));
    mySaveMenu->addAction(mySaveFormulae);
    mySaveMenu->addAction(mySaveQuests);
    mySaveMenu->addAction(mySaveMessages);
}

void CREMainWindow::doResourceWindow(DisplayMode mode)
{
    QWidget* resources = new CREResourcesWindow(myMapManager, myQuestManager, myMessageManager, mode);
    connect(this, SIGNAL(updateFilters()), resources, SLOT(updateFilters()));
    connect(resources, SIGNAL(filtersModified()), this, SLOT(onFiltersModified()));
    connect(this, SIGNAL(updateReports()), resources, SLOT(updateReports()));
    connect(resources, SIGNAL(reportsModified()), this, SLOT(onReportsModified()));
    connect(this, SIGNAL(commitData()), resources, SLOT(commitData()));
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

void CREMainWindow::onOpenQuests()
{
    doResourceWindow(DisplayQuests);
}

void CREMainWindow::onOpenMessages()
{
    doResourceWindow(DisplayMessage);
}

void CREMainWindow::onOpenResources()
{
    doResourceWindow(DisplayAll);
}

void CREMainWindow::onOpenExperience()
{
    QWidget* experience = new CREExperienceWindow();
    myArea->addSubWindow(experience);
    experience->show();
}

void CREMainWindow::onSaveFormulae()
{
}

void CREMainWindow::onSaveQuests()
{
    emit commitData();
    myQuestManager->saveQuests();
}

void CREMainWindow::onSaveMessages()
{
    emit commitData();
    myMessageManager->saveMessages();
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

void CREMainWindow::onReportsModified()
{
    emit updateReports();
}
