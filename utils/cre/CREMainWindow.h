#include <Qt>

#include <QMainWindow>

class QMdiArea;
class QAction;
class QMenu;
class CREArtifactWindow;
class CREArchetypeWindow;
class CRETreasureWindow;
class CREAnimationWindow;
class CREFormulaeWindow;
class CREResourcesWindow;

class CREMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        CREMainWindow();

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
        QAction* myOpenResources;
        QAction* mySaveFormulae;

    private slots:
        void onOpenArtifacts();
        void onOpenArchetypes();
        void onOpenTreasures();
        void onOpenAnimations();
        void onOpenFormulae();
        void onOpenFaces();
        void onOpenResources();
        void onSaveFormulae();
};
