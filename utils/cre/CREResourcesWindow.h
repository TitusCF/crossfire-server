#ifndef CRERESOURCESWINDOW_H
#define CRERESOURCESWINDOW_H

#include <QObject>
#include <QtGui>

#include "CREFilter.h"
#include "CREFilterDefinitionManager.h"
#include "CREReportDefinitionManager.h"
#include "CREPanel.h"
#include "CRETreeItem.h"

class CREMapInformationManager;
class QuestManager;

enum DisplayMode {
    DisplayAll = 255,
    DisplayArchetypes = 1,
    DisplayAnimations = 2,
    DisplayTreasures = 4,
    DisplayFormulae = 8,
    DisplayArtifacts = 16,
    DisplayFaces = 32,
    DisplayMaps = 64,
    DisplayQuests = 128
};

class CREResourcesWindow : public QWidget
{
    Q_OBJECT

    public:
        CREResourcesWindow(CREMapInformationManager* store, QuestManager* quests, DisplayMode mode = DisplayAll);
        virtual ~CREResourcesWindow();

    public slots:
        void updateFilters();
        void updateReports();
        void commitData();

    signals:
        void filtersModified();
        void reportsModified();

    protected:
        QTreeWidget* myTree;
        CREPanel* myCurrentPanel;
        QHash<QString, QPointer<CREPanel> > myPanels;
        QSplitter* mySplitter;
        CREMapInformationManager* myStore;
        QuestManager* myQuests;
        DisplayMode myDisplay;
        CREFilter myFilter;
        QList<QObject*> myDisplayedItems;
        QPushButton* myFilterButton;
        QMenu* myFiltersMenu;
        QSignalMapper myFiltersMapper;
        CREFilterDefinitionManager myFilters;
        QMenu* myReportsMenu;
        QSignalMapper myReportsMapper;
        CREReportDefinitionManager myReports;
        QList<CRETreeItem*> myTreeItems;

        void fillData();
        void fillAnimations();
        void fillTreasures();
        void fillArchetypes();
        void fillFormulae();
        void fillArtifacts();
        void fillFaces();
        void fillMaps();
        void fillQuests();
        void addPanel(QString name, CREPanel* panel);

    protected slots:
        void tree_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);
        void onFilter();
        void onFilterChange(QObject* object);
        void onQuickFilter();
        void clearFilter();
        void onReport();
        void onReportChange(QObject* object);
};

#endif // CRERESOURCESWINDOW_H
