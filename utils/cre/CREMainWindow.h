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

        QAction* myOpenArtifacts;
        QAction* myOpenArchetypes;
        QAction* myOpenTreasures;
        QAction* myOpenAnimations;
        QAction* myOpenFormulae;
        QAction* myOpenFaces;
        QAction* myOpenMaps;
        QAction* myOpenResources;
        QAction* myOpenExperience;
        QAction* myOpenQuests;
        QAction* myOpenMessages;
        QAction* mySaveFormulae;
        QAction* mySaveQuests;
        QLabel* myMapBrowseStatus;
        CREMapInformationManager* myMapManager;
        QuestManager* myQuestManager;
        MessageManager* myMessageManager;

    protected:
        void closeEvent(QCloseEvent* event);
        void doResourceWindow(DisplayMode mode);

    private slots:
        void onOpenArtifacts();
        void onOpenArchetypes();
        void onOpenTreasures();
        void onOpenAnimations();
        void onOpenFormulae();
        void onOpenFaces();
        void onOpenMaps();
        void onOpenQuests();
        void onOpenResources();
        void onOpenMessages();
        void onOpenExperience();
        void onSaveFormulae();
        void onSaveQuests();
        void browsingMap(const QString& path);
        void browsingFinished();
        void onFiltersModified();
        void onReportsModified();
};
