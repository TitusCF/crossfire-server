#include <Qt>

#include <QMainWindow>

#include "CREResourcesWindow.h"

class QMdiArea;
class QAction;
class QMenu;
class QLabel;
class CREArtifactWindow;
class CREArchetypeWindow;
class CRETreasureWindow;
class CREAnimationWindow;
class CREFormulaeWindow;
class CREMapInformationManager;
class QuestManager;
class MessageManager;
class ResourcesManager;
class ScriptFileManager;

class CREMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        CREMainWindow();

    signals:
        void updateFilters();
        void updateReports();
        void commitData();

    private:
        QMdiArea* myArea;

        void createActions();
        void createMenus();

        QMenu* myOpenMenu;
        QMenu* mySaveMenu;

        QAction* myOpenExperience;
        QAction* mySaveFormulae;
        QAction* mySaveQuests;
        QAction* mySaveMessages;
        QAction* mySaveArchetypes;
        QAction* myReportDuplicate;
        QAction* myReportSpellDamage;
        QAction* myReportAlchemy;
        QAction* myReportAlchemyGraph;
        QAction* myReportSpells;
        QAction* myReportPlayer;
        QAction* myReportSummon;
        QAction* myReportShops;
        QAction *myReportQuests;
        QAction* myReportMaterials;
        QAction* myReportArchetypes;
        QAction* myReportLicenses;
        QMenu* myToolsMenu;
        QAction* myToolEditMonsters;
        QAction* myToolSmooth;
        QAction* myToolHPBar;
        QAction* myToolCombatSimulator;
        QAction* myToolFaceMaker;
        QActionGroup *myFacesetsGroup;
        QAction* myToolFacesetUseFallback;
        QAction* myClearMapCache;
        QAction* myToolReloadAssets;
        QLabel* myMapBrowseStatus;
        CREMapInformationManager* myMapManager;
        MessageManager* myMessageManager;
        ResourcesManager* myResourcesManager;
        ScriptFileManager* myScriptManager;

    protected:
        void closeEvent(QCloseEvent* event);
        void doResourceWindow(DisplayMode mode);
        void fillFacesets();

    private slots:
        void onOpenResources();
        void onOpenExperience();
        void onSaveFormulae();
        void onSaveQuests();
        void onSaveMessages();
        void onReportDuplicate();
        void onReportSpellDamage();
        void onReportAlchemy();
        void onReportAlchemyGraph();
        void onReportSpells();
        void onReportPlayer();
        void onReportSummon();
        void onReportShops();
        void onReportQuests();
        void onReportMaterials();
        void onReportArchetypes();
        void onReportLicenses();
        void onToolEditMonsters();
        void onToolSmooth();
        void onToolCombatSimulator();
        void onToolBarMaker();
        void onToolFaceMaker();
        void onToolFaceset(QAction* action);
        void onToolFacesetUseFallback();
        void onToolReloadAssets();
        void browsingMap(const QString& path);
        void browsingFinished();
        void onFiltersModified();
        void onReportsModified();
        void onClearCache();
};
