#ifndef CRERESOURCESWINDOW_H
#define CRERESOURCESWINDOW_H

#include <QObject>
#include <QtGui>

#include "CREFilter.h"
#include "CREFilterDefinitionManager.h"
#include "CREReportDefinitionManager.h"

class CREMapInformationManager;

enum DisplayMode { DisplayAll = 255, DisplayArchetypes = 1, DisplayAnimations = 2, DisplayTreasures = 4, DisplayFormulae = 8, DisplayArtifacts = 16, DisplayFaces = 32, DisplayMaps = 64 };

class CREResourcesWindow : public QWidget
{
    Q_OBJECT

    public:
        CREResourcesWindow(CREMapInformationManager* store, DisplayMode mode = DisplayAll);
        virtual ~CREResourcesWindow();

    public slots:
        void updateFilters();
        void updateReports();

    signals:
        void filtersModified();
        void reportsModified();

    protected:
        QTreeWidget* myTree;
        QWidget* myCurrentPanel;
        QHash<QString, QPointer<QWidget> > myPanels;
        QSplitter* mySplitter;
        CREMapInformationManager* myStore;
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

        void fillData();
        void fillAnimations();
        void fillTreasures();
        void fillArchetypes();
        void fillFormulae();
        void fillArtifacts();
        void fillFaces();
        void fillMaps();
        void addPanel(QString name, QWidget* panel);

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
