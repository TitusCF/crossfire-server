#ifndef CRERESOURCESWINDOW_H
#define CRERESOURCESWINDOW_H

#include <QObject>
#include <QtGui>

enum DisplayMode { DisplayAll = 255, DisplayArchetypes = 1, DisplayAnimations = 2, DisplayTreasures = 4, DisplayFormulae = 8, DisplayArtifacts = 16, DisplayFaces = 32 };

class CREResourcesWindow : public QWidget
{
    Q_OBJECT

    public:
        CREResourcesWindow(DisplayMode mode = DisplayAll);

    protected:
        QTreeWidget* myTree;
        QWidget* myCurrentPanel;
        QHash<QString, QPointer<QWidget> > myPanels;
        QSplitter* mySplitter;

        void fillAnimations();
        void fillTreasures();
        void fillArchetypes();
        void fillFormulae();
        void fillArtifacts();
        void fillFaces();
        void addPanel(QString name, QWidget* panel);

    protected slots:
        void tree_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);
};

#endif // CRERESOURCESWINDOW_H
