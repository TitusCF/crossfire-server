#include <Qt>
#include <QtGui>

#include "CREResourcesWindow.h"
#include "CREUtils.h"
#include "CREPixmap.h"

#include "CREFilter.h"
#include "CREFilteredModel.h"
#include "CREFilterDialog.h"

#include "CRETreeItemAnimation.h"
#include "CRETreeItemArchetype.h"
#include "CRETreeItemTreasure.h"
#include "CRETreeItemArtifact.h"
#include "CRETreeItemFormulae.h"
#include "CRETreeItemFace.h"

#include "CREAnimationPanel.h"
#include "CREArchetypePanel.h"
#include "CRETreasurePanel.h"
#include "CREArtifactPanel.h"
#include "CREFormulaePanel.h"
#include "CREFacePanel.h"

#include "CREWrapperArchetype.h"
#include "CREWrapperArtifact.h"
#include "CREWrapperFormulae.h"

extern "C" {
#include "global.h"
#include "recipe.h"
}

CREResourcesWindow::CREResourcesWindow(CREMapInformationManager* store, DisplayMode mode)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    myDisplay = mode;

    Q_ASSERT(store);
    myStore = store;

    setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout* layout = new QHBoxLayout(this);

    QVBoxLayout* buttons = new QVBoxLayout();
    QPushButton* filter = new QPushButton(tr("Filter"), this);
    connect(filter, SIGNAL(pressed()), this, SLOT(onFilter()));
    buttons->addWidget(filter);
    layout->addLayout(buttons);

    mySplitter = new QSplitter(this);
    layout->addWidget(mySplitter);
    myTree = new QTreeWidget(this);
    mySplitter->addWidget(myTree);
    myTree->setIconSize(QSize(32, 32));
    myTree->setHeaderLabel(tr("All resources"));
    myTree->sortByColumn(0, Qt::AscendingOrder);

    myTree->setSortingEnabled(true);

    myCurrentPanel = NULL;

    connect(myTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(tree_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    fillData();

    QApplication::restoreOverrideCursor();
}

void CREResourcesWindow::fillData()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    myTree->clear();

    QString title;
    if (myDisplay & DisplayArchetypes)
    {
        title = tr("Archetypes");
        fillArchetypes();
    }
    if (myDisplay & DisplayAnimations)
    {
        title = tr("Animations");
        fillAnimations();
    }
    if (myDisplay & DisplayTreasures)
    {
        title = tr("Treasures");
        fillTreasures();
    }
    if (myDisplay & DisplayFormulae)
    {
        title = tr("Formulae");
        fillFormulae();
    }
    if (myDisplay & DisplayArtifacts)
    {
        title = tr("Artifacts");
        fillArtifacts();
    }
    if (myDisplay & DisplayFaces)
    {
        title = tr("Faces");
        fillFaces();
    }

    if (myDisplay == DisplayAll)
        title = tr("All resources");

    setWindowTitle(title);

    QApplication::restoreOverrideCursor();
}

void CREResourcesWindow::tree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*)
{
    if (!current || current->data(0, Qt::UserRole).value<void*>() == NULL)
        return;
    CRETreeItem* item = reinterpret_cast<CRETreeItem*>(current->data(0, Qt::UserRole).value<void*>());
    if (!item)
        return;


    QWidget* newPanel = myPanels[item->getPanelName()];
    if (!newPanel)
    {
//        printf("no panel for %s\n", qPrintable(item->getPanelName()));
        return;
    }

    item->fillPanel(newPanel);

    if (myCurrentPanel != newPanel)
    {
        if (myCurrentPanel)
            myCurrentPanel->setVisible(false);
        newPanel->setVisible(true);
        myCurrentPanel = newPanel;
    }
}

void CREResourcesWindow::fillAnimations()
{
    QTreeWidgetItem* animationsNode = CREUtils::animationNode(NULL);
    myTree->addTopLevelItem(animationsNode);

    QTreeWidgetItem* item;

    // There is the "bug" animation to consider
    for (int anim = 0; anim <= num_animations; anim++)
    {
        item = CREUtils::animationNode(&animations[anim], animationsNode);
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(new CRETreeItemAnimation(&animations[anim])));
    }

    addPanel("Animation", new CREAnimationPanel());
}

void CREResourcesWindow::fillTreasures()
{
    QTreeWidgetItem* item, *sub;
    const treasurelist* list;
    const treasure* treasure;

    QTreeWidgetItem* treasures = CREUtils::treasureNode(NULL);
    myTree->addTopLevelItem(treasures);

    for (list = first_treasurelist; list; list = list->next)
    {
        item = CREUtils::treasureNode(list, treasures);

        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(new CRETreeItemTreasure(list)));
        //item->setData(0, Qt::UserRole, allTreasures[t]);
        if (list->total_chance != 0)
            item->setText(1, QString::number(list->total_chance));

        for (treasure = list->items; treasure; treasure = treasure->next)
        {
            sub = CREUtils::treasureNode(treasure, item);
            if (treasure->chance)
                sub->setText(1, QString::number(treasure->chance));
            sub->setData(0, Qt::UserRole, QVariant::fromValue<void*>(new CRETreeItemTreasure(list)));
        }
    }

    addPanel("Treasure", new CRETreasurePanel());
}

void CREResourcesWindow::fillArchetypes()
{
    QTreeWidgetItem* item, *root, *sub;
    archt* arch;

    root = CREUtils::archetypeNode(NULL);
    myTree->addTopLevelItem(root);

    CREWrapperArchetype wrapper;

    for (arch = first_archetype; arch; arch = arch->next)
    {
        wrapper.setArchetype(arch);
        if (!myFilter.showItem(&wrapper))
            continue;

        item = CREUtils::archetypeNode(arch, root);
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(new CRETreeItemArchetype(arch)));

        for (archt* more = arch->more; more; more = more->more)
        {
            sub = CREUtils::archetypeNode(more, item);
            sub->setData(0, Qt::UserRole, QVariant::fromValue<void*>(new CRETreeItemArchetype(more)));
        }
    }

    addPanel("Archetype", new CREArchetypePanel(myStore));
}

void CREResourcesWindow::fillFormulae()
{
    recipelist* list;
    recipe* recipe;
    QTreeWidgetItem* root, *form, *sub;
    CREWrapperFormulae wrapper;

    form = new QTreeWidgetItem(myTree, QStringList(tr("Formulae")));
//    myTree->addTopLevelItem(form);

    for (int ing = 1; ; ing++)
    {
        list = get_formulalist(ing);
        if (!list)
            break;

        root = new QTreeWidgetItem(form, QStringList(tr("%1 ingredients").arg(ing)));

        for (recipe = list->items; recipe; recipe = recipe->next)
        {
            wrapper.setFormulae(recipe);
            if (!myFilter.showItem(&wrapper))
                continue;

            sub = CREUtils::formulaeNode(recipe, root);
            sub->setData(0, Qt::UserRole, QVariant::fromValue<void*>(new CRETreeItemFormulae(recipe)));
        }
    }

    addPanel("Formulae", new CREFormulaePanel());
}

void CREResourcesWindow::fillArtifacts()
{
    QTreeWidgetItem* item, *root, *sub;
    artifactlist* list;
    const typedata* data;

    root = new QTreeWidgetItem(myTree, QStringList(tr("Artifacts")));

    CREWrapperArtifact wrapper;

    for (list = first_artifactlist; list; list = list->next)
    {
        data = get_typedata(list->type);

        item = new QTreeWidgetItem(root, QStringList(data ? data->name : tr("type %1").arg(list->type)));

        for (artifact* art = list->items; art; art = art->next)
        {
            wrapper.setArtifact(art);
            if (!myFilter.showItem(&wrapper))
                continue;

            sub = CREUtils::artifactNode(art, item);
            sub->setData(0, Qt::UserRole, QVariant::fromValue<void*>(new CRETreeItemArtifact(art)));
        }
    }

    addPanel("Artifact", new CREArtifactPanel());
}

void CREResourcesWindow::fillFaces()
{
    QTreeWidgetItem* item, *root;

    root = CREUtils::faceNode(NULL);
    myTree->addTopLevelItem(root);

    extern int nrofpixmaps;

    for (int f = 0; f < nrofpixmaps; f++)
    {
        item = CREUtils::faceNode(&new_faces[f], root);
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(new CRETreeItemFace(&new_faces[f])));
    }

    addPanel("Face", new CREFacePanel());
}

void CREResourcesWindow::addPanel(QString name, QWidget* panel)
{
    panel->setVisible(false);
    myPanels[name] = panel;
    mySplitter->addWidget(panel);
}

void CREResourcesWindow::onFilter()
{
    CREFilterDialog dlg(&myFilter);
    if (dlg.exec() != QDialog::Accepted)
        return;

    fillData();
}
