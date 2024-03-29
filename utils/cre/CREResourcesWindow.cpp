#include <Qt>
#include <QtWidgets>
#include <QScriptValue>
#include <stdexcept>

#include "CREResourcesWindow.h"
#include "CREUtils.h"
#include "CREPixmap.h"

#include "CREFilter.h"
#include "CREFilterDialog.h"
#include "CREFilterDefinition.h"

#include "CRESettings.h"

#include "CREReportDialog.h"
#include "CREReportDisplay.h"
#include "CREReportDefinition.h"

#include "CRETreeItemEmpty.h"
#include "CRETreeItemQuest.h"

#include "CREAnimationPanel.h"
#include "CREArchetypePanel.h"
#include "CRETreasurePanel.h"
#include "CREArtifactPanel.h"
#include "CREFormulaePanel.h"
#include "CREFacePanel.h"
#include "CREMapPanel.h"
#include "CRERegionPanel.h"
#include "CREQuestPanel.h"
#include "CREMessagePanel.h"
#include "CREScriptPanel.h"
#include "CREGeneralMessagePanel.h"
#include "CREFacesetsPanel.h"

#include "CREWrapperObject.h"
#include "CREWrapperArtifact.h"
#include "CREWrapperFormulae.h"
#include "CREWrapperTreasure.h"

#include "CREMapInformationManager.h"
#include "MessageFile.h"
#include "ScriptFileManager.h"

#include "CREScriptEngine.h"

#include "CRERandomMap.h"
#include "CRERandomMapPanel.h"

extern "C" {
#include "global.h"
#include "recipe.h"
}
#include "assets.h"
#include "AssetsManager.h"

#include "MessageManager.h"
#include "ResourcesManager.h"
#include "ScriptFile.h"

CREResourcesWindow::CREResourcesWindow(CREMapInformationManager* store, MessageManager* messages, ResourcesManager* resources, ScriptFileManager* scripts, QWidget* parent, DisplayMode mode) : QWidget(parent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    myDisplay = mode;

    Q_ASSERT(store);
    myStore = store;
    Q_ASSERT(messages);
    myMessages = messages;
    Q_ASSERT(resources);
    myResources = resources;
    Q_ASSERT(scripts);
    myScripts = scripts;

    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* layout = new QVBoxLayout(this);

    myFiltersMenu = new QMenu(this);
    QHBoxLayout* buttons = new QHBoxLayout();
    myFilterButton = new QPushButton(tr("Filter..."), this);
    myFilterButton->setMenu(myFiltersMenu);
    buttons->addWidget(myFilterButton);

    myReportsMenu = new QMenu(this);
    QPushButton* report = new QPushButton(tr("Report"), this);
    report->setMenu(myReportsMenu);
    buttons->addWidget(report);

    layout->addLayout(buttons);

    mySplitter = new QSplitter(this);
    layout->addWidget(mySplitter);
    myTree = new QTreeWidget(this);
    mySplitter->addWidget(myTree);
    myTree->setIconSize(QSize(32, 32));
    myTree->setHeaderLabel(tr("All resources"));
//    myTree->sortByColumn(0, Qt::AscendingOrder);

//    myTree->setSortingEnabled(true);
    myTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(treeCustomMenu(const QPoint&)));
    connect(myTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(tree_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    /* dummy panel to display for empty items */
    CREPanel* dummy = new CREPanel(this);
    QVBoxLayout* dl = new QVBoxLayout(dummy);
    dl->addWidget(new QLabel(tr("No details available."), dummy));
    addPanel("(dummy)", dummy);
    dummy->setVisible(true);
    myCurrentPanel = dummy;

    fillData();

    connect(&myFiltersMapper, SIGNAL(mapped(QObject*)), this, SLOT(onFilterChange(QObject*)));
    updateFilters();
    connect(&myReportsMapper, SIGNAL(mapped(QObject*)), this, SLOT(onReportChange(QObject*)));
    updateReports();

    QApplication::restoreOverrideCursor();
}

CREResourcesWindow::~CREResourcesWindow()
{
    qDeleteAll(myTreeItems);
    myTreeItems.clear();
    qDeleteAll(myDisplayedItems);
    myDisplayedItems.clear();
    qDeleteAll(myPanels);
}

void CREResourcesWindow::fillData()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    myTree->clear();
    qDeleteAll(myTreeItems);
    myTreeItems.clear();
    qDeleteAll(myDisplayedItems);
    myDisplayedItems.clear();

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
    if (myDisplay & DisplayMaps)
    {
        title = tr("Maps");
        fillMaps();
    }
    if (myDisplay & DisplayQuests)
    {
        title = tr("Quests");
        fillQuests();
    }
    if (myDisplay & DisplayMessage)
    {
        title = tr("NPC dialogs");
        fillMessages();
    }
    if (myDisplay & DisplayScripts)
    {
        title = tr("Scripts");
        fillScripts();
    }
    if (myDisplay & DisplayRandomMaps)
    {
        title = tr("Random maps");
        fillRandomMaps();
    }
    if (myDisplay & DisplayGeneralMessages)
    {
        title = tr("General messages");
        fillGeneralMessages();
    }
    if (myDisplay & DisplayFacesets)
    {
        title = tr("Facesets");
        fillFacesets();
    }

    if (myDisplay == DisplayAll)
        title = tr("All resources");

    if (myTree->topLevelItemCount() == 1)
        myTree->topLevelItem(0)->setExpanded(true);

    setWindowTitle(title);

    myTree->resizeColumnToContents(0);

    QApplication::restoreOverrideCursor();
}

void CREResourcesWindow::commitData()
{
    if (myCurrentPanel != NULL)
        myCurrentPanel->commitData();
}

void CREResourcesWindow::tree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*)
{
    if (!current || current->data(0, Qt::UserRole).value<void*>() == NULL)
        return;
    CRETreeItem* item = reinterpret_cast<CRETreeItem*>(current->data(0, Qt::UserRole).value<void*>());
    if (!item)
        return;

    commitData();

    CREPanel* newPanel = myPanels[item->getPanelName()];
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
    myTreeItems.append(new CRETreeItemEmpty());
    animationsNode->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(animationsNode);

    getManager()->animations()->each([this, &animationsNode] (const auto anim)
    {
        auto item = CREUtils::animationNode(anim, animationsNode);
        myTreeItems.append(new CRETTreeItem<const animations_struct>(anim, "Animation"));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    });

    addPanel("Animation", new CREAnimationPanel(this, myStore));
}

void CREResourcesWindow::fillTreasures()
{
    QTreeWidgetItem* treasures = CREUtils::treasureNode(NULL);
    myTreeItems.append(new CRETreeItemEmpty());
    treasures->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(treasures);

    getManager()->treasures()->each([this, &treasures] (const auto list)
    {
        auto wrapper = new CREWrapperTreasureList(list);
        if (!myFilter.showItem(wrapper)) {
            delete wrapper;
            return;
        }
        auto item = CREUtils::treasureNode(list, treasures);

        myTreeItems.append(new CRETTreeItem<const treasurelist>(list, "Treasure"));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
        //item->setData(0, Qt::UserRole, allTreasures[t]);
        if (list->total_chance != 0)
            item->setText(1, QString::number(list->total_chance));

        for (auto treasure = list->items; treasure; treasure = treasure->next)
        {
            auto sub = CREUtils::treasureNode(treasure, list, item);
            if (treasure->chance)
                sub->setText(1, QString::number(treasure->chance));
            myTreeItems.append(new CRETTreeItem<const treasurelist>(list, "Treasure"));
            sub->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
        }
        myDisplayedItems.append(wrapper);
    });

    addPanel("Treasure", new CRETreasurePanel(this));
    treasures->setText(0, tr("%1 [%2 items]").arg(treasures->text(0)).arg((getManager()->treasures()->count())));
}

void CREResourcesWindow::fillArchetypes()
{
    QTreeWidgetItem* root;
    int added = 0, count = 0;

    root = CREUtils::archetypeNode(NULL);
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    CREWrapperObject* wrapper = NULL;

    getManager()->archetypes()->each([this, &wrapper, &root, &added, &count] (const auto arch)
    {
        if (arch->head) {
            return;
        }
        count++;
        if (!wrapper)
            wrapper = new CREWrapperObject();
        wrapper->setObject(&arch->clone);
        if (!myFilter.showItem(wrapper))
            return;

        auto item = CREUtils::archetypeNode(arch, root);
        myTreeItems.append(new CRETTreeItem<archt>(arch, "Archetype"));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));

        if (arch->more)
        {
            int min_x = 0, max_x = 0, min_y = 0, max_y = 0;

            for (archt* more = arch->more; more; more = more->more)
            {
                min_x = MIN(min_x, more->clone.x);
                max_x = MAX(max_x, more->clone.x);
                min_y = MIN(min_y, more->clone.y);
                max_y = MAX(max_y, more->clone.y);
                auto sub = CREUtils::archetypeNode(more, item);
                myTreeItems.append(new CRETTreeItem<const archt>(more, "Archetype"));
                sub->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
            }

            item->setText(0, tr("%1 (%2x%3)").arg(item->text(0)).arg(max_x - min_x + 1).arg(max_y - min_y + 1));
        }

        myDisplayedItems.append(wrapper);
        wrapper = NULL;
        added++;
    });

    delete wrapper;
    addPanel("Archetype", new CREArchetypePanel(myStore, myResources, this));
    if (added == count)
        root->setText(0, tr("%1 [%2 items]").arg(root->text(0)).arg(count));
    else
        root->setText(0, tr("%1 [%2 items out of %3]").arg(root->text(0)).arg(added).arg(count));
}

void CREResourcesWindow::fillFormulae()
{
    const recipe* r;
    QTreeWidgetItem* root, *form, *sub;
    CREWrapperFormulae* wrapper = NULL;
    int count = 0, added = 0, subCount, subAdded;

    form = new QTreeWidgetItem(myTree, QStringList(tr("Formulae")));
    myTreeItems.append(new CRETreeItemEmpty());
    form->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
//    myTree->addTopLevelItem(form);

    for (int ing = 1; ing <= myResources->recipeMaxIngredients() ; ing++)
    {
        root = new QTreeWidgetItem(form, QStringList(tr("%1 ingredients").arg(ing)));
        subCount = 0;
        subAdded = 0;

        QStringList recipes = myResources->recipes(ing);

        foreach(QString name, recipes)
        {
            r = myResources->recipe(ing, name);
            subCount++;
            count++;
            if (!wrapper)
                wrapper = new CREWrapperFormulae();
            wrapper->setFormulae(r);
            if (!myFilter.showItem(wrapper))
                continue;

            sub = CREUtils::formulaeNode(r, root);
            myTreeItems.append(new CRETTreeItem<const recipe>(r, "Formulae"));
            sub->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
            myDisplayedItems.append(wrapper);
            wrapper = NULL;
            subAdded++;
            added++;
        }
        if (subCount == subAdded)
            root->setText(0, tr("%1 [%2 items]").arg(root->text(0)).arg(subCount));
        else
            root->setText(0, tr("%1 [%2 items out of %3]").arg(root->text(0)).arg(subAdded).arg(subCount));
    }

    delete wrapper;
    addPanel("Formulae", new CREFormulaePanel(this));
    if (added == count)
        form->setText(0, tr("%1 [%2 items]").arg(form->text(0)).arg(count));
    else
        form->setText(0, tr("%1 [%2 items out of %3]").arg(form->text(0)).arg(added).arg(count));
}

void CREResourcesWindow::fillArtifacts()
{
    QTreeWidgetItem* item, *root, *sub;
    artifactlist* list;
    const typedata* data;
    int count = 0, added = 0;

    root = new QTreeWidgetItem(myTree, QStringList(tr("Artifacts")));
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));

    CREWrapperArtifact wrapper;

    for (list = first_artifactlist; list; list = list->next)
    {
        int subCount = 0, subAdded = 0;
        data = get_typedata(list->type);

        item = new QTreeWidgetItem(root, QStringList(data ? data->name : tr("type %1").arg(list->type)));

        for (artifact* art = list->items; art; art = art->next)
        {
            count++;
            subCount++;
            wrapper.setArtifact(art);
            if (!myFilter.showItem(&wrapper))
                continue;

            sub = CREUtils::artifactNode(art, item);
            myTreeItems.append(new CRETTreeItem<artifact>(art, "Artifact"));
            sub->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
            added++;
            subAdded++;
        }

        if (subCount == subAdded)
            item->setText(0, tr("%1 [%2 items]").arg(item->text(0)).arg(subCount));
        else
            item->setText(0, tr("%1 [%2 items out of %3]").arg(item->text(0)).arg(subAdded).arg(subCount));
    }

    addPanel("Artifact", new CREArtifactPanel(this));
    if (added == count)
        root->setText(0, tr("%1 [%2 items]").arg(root->text(0)).arg(count));
    else
        root->setText(0, tr("%1 [%2 items out of %3]").arg(root->text(0)).arg(added).arg(count));
}

void CREResourcesWindow::fillFaces()
{
    QTreeWidgetItem* root;

    root = CREUtils::faceNode(NULL);
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    getManager()->faces()->each([this, &root] (const auto face)
    {
        auto item = CREUtils::faceNode(face, root);
        myTreeItems.append(new CRETTreeItem<const Face>(face, "Face"));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    });

    addPanel("Face", new CREFacePanel(this, myStore));
}

bool sortMapInformation(const CREMapInformation* left, const CREMapInformation* right)
{
    return left->displayName().compare(right->displayName(), Qt::CaseInsensitive) < 0;
}

void CREResourcesWindow::fillMaps()
{
    bool full = false;
    if (myDisplay == DisplayMaps)
    {
        QStringList headers;
        headers << tr("Maps") << tr("Experience") << tr("Difficulty") << tr("Computed difficulty");
        myTree->setHeaderLabels(headers);
        myTree->sortByColumn(0, Qt::AscendingOrder);
        full = true;
    }

    QTreeWidgetItem* regionNode, *root, *leaf;

    root = CREUtils::mapNode(NULL);
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    QHash<QString, region*> regions;
    for (region* reg = first_region; reg; reg = reg->next)
    {
      regions[reg->name] = reg;
    }

    QStringList names = regions.keys();
    names.sort();

    int totalMaps = 0, totalAdded = 0;

    foreach(QString name, names)
    {
        int added = 0;
        QList<CREMapInformation*> maps = myStore->getMapsForRegion(name);
        qSort(maps.begin(), maps.end(), sortMapInformation);
        regionNode = CREUtils::regionNode(name, maps.size(), root);
        myTreeItems.append(new CRETTreeItem<region>(regions[name], "Region"));
        regionNode->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
        foreach(CREMapInformation* map, maps)
        {
            if (!myFilter.showItem(map)) {
                continue;
            }
            added++;

            leaf = CREUtils::mapNode(map, regionNode);
            myTreeItems.append(new CRETTreeItem<CREMapInformation>(map, "Map"));
            leaf->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
            if (full)
            {
                leaf->setText(1, tr("%1").arg(QString::number(map->experience()), 20));
                leaf->setText(2, tr("%1").arg(QString::number(map->difficulty()), 20));
                leaf->setText(3, tr("%1").arg(QString::number(map->computedDifficulty()), 20));
            }

            /** @todo clean at some point - the issue is wrapper's ownership */
            myDisplayedItems.append(map->clone());

        }

        totalMaps += maps.size();
        totalAdded += added;
        if (added != maps.size())
        {
            regionNode->setText(0, tr("%1 - %2 maps out of %3").arg(name).arg(added).arg(maps.size()));
        }
    }

    if (full)
    {
        root->setExpanded(true);
        myTree->resizeColumnToContents(0);
        myTree->resizeColumnToContents(1);
    }
    if (totalMaps == totalAdded)
        root->setText(0, tr("Maps [%1 items]").arg(totalMaps));
    else
        root->setText(0, tr("Maps - [%1 out of %2]").arg(totalAdded).arg(totalMaps));

    addPanel("Region", new CRERegionPanel(this));
    addPanel("Map", new CREMapPanel(myScripts, this));
}

void CREResourcesWindow::fillQuests()
{
    QTreeWidgetItem* item, *root;

    root = CREUtils::questsNode();
    myTreeItems.append(new CRETreeItemQuest(NULL, root, this));
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    QStringList codes;

    getManager()->quests()->each([&] (auto quest) {
        item = CREUtils::questNode(quest, root);
        myTreeItems.append(new CRETreeItemQuest(quest, item, this));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    });

    addPanel("Quest", new CREQuestPanel(myStore, myMessages, myResources, this));
    root->setText(0, tr("%1 [%2 items]").arg(root->text(0)).arg(codes.size()));
}

void CREResourcesWindow::fillMessages()
{
    QTreeWidgetItem* item, *root;

    root = CREUtils::messagesNode();
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    foreach(MessageFile* message, myMessages->messages())
    {
        item = CREUtils::messageNode(message, root);
        myTreeItems.append(new CRETTreeItem<MessageFile>(message, "Message"));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    }

    addPanel("Message", new CREMessagePanel(myMessages, this));
}

static bool scriptLessThan(const ScriptFile* left, const ScriptFile* right)
{
    return left->path().compare(right->path()) < 0;
}

void CREResourcesWindow::fillScripts()
{
    QTreeWidgetItem* item, *root;

    root = CREUtils::scriptsNode();
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    QList<ScriptFile*> scripts = myScripts->scripts();
    qSort(scripts.begin(), scripts.end(), scriptLessThan);

    foreach(ScriptFile* script, scripts)
    {
        item = CREUtils::scriptNode(script, root);
        myTreeItems.append(new CRETTreeItem<ScriptFile>(script, "Script"));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    }

    addPanel("Script", new CREScriptPanel(this));
}

void CREResourcesWindow::fillGeneralMessages()
{
    QTreeWidgetItem* root;

    root = CREUtils::generalMessageNode();
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    getManager()->messages()->each([this, &root] (const GeneralMessage* message)
    {
        auto item = CREUtils::generalMessageNode(message, root);
        myTreeItems.append(new CRETTreeItem<const GeneralMessage>(message, "GeneralMessage"));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    });

    addPanel("GeneralMessage", new CREGeneralMessagePanel(this));
}

void CREResourcesWindow::fillFacesets()
{
    QTreeWidgetItem* root;

    root = CREUtils::facesetsNode();
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    getManager()->facesets()->each([this, &root] (const face_sets* faceset)
    {
        auto item = CREUtils::facesetsNode(faceset, root);
        myTreeItems.append(new CRETTreeItem<const face_sets>(faceset, "Faceset"));
        item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    });

    addPanel("Faceset", new CREFacesetsPanel(this));
}

void CREResourcesWindow::addPanel(QString name, CREPanel* panel)
{
    panel->setVisible(false);
    myPanels[name] = panel;
    mySplitter->addWidget(panel);
}

void CREResourcesWindow::onFilter()
{
    CREFilterDialog dlg;
    if (dlg.exec() != QDialog::Accepted)
        return;

    /* sending this signal will ultimately call our own updateFilters() */
    emit filtersModified();
}

void CREResourcesWindow::onReport()
{
    CREReportDialog dlg;
    if (dlg.exec() != QDialog::Accepted)
        return;

    /* sending this signal will ultimately call our own updateReports() */
    emit reportsModified();
}

void CREResourcesWindow::updateFilters()
{
    CRESettings settings;
    settings.loadFilters(myFilters);

    myFiltersMenu->clear();

    if (myFilters.filters().size() > 0)
    {
        QAction* clear = new QAction(tr("(none)"), this);
        connect(clear, SIGNAL(triggered()), this, SLOT(clearFilter()));
        myFiltersMenu->addAction(clear);

        foreach(CREFilterDefinition* filter, myFilters.filters())
        {
            QAction* a = new QAction(filter->name(), this);
            myFiltersMenu->addAction(a);
            myFiltersMapper.setMapping(a, filter);
            connect(a, SIGNAL(triggered()), &myFiltersMapper, SLOT(map()));
        }

        myFiltersMenu->addSeparator();
    }

    QAction* quick = new QAction(tr("Quick filter..."), this);
    connect(quick, SIGNAL(triggered()), this, SLOT(onQuickFilter()));
    myFiltersMenu->addAction(quick);
    QAction* dialog = new QAction(tr("Filters definition..."), this);
    connect(dialog, SIGNAL(triggered()), this, SLOT(onFilter()));
    myFiltersMenu->addAction(dialog);

    clearFilter();
}

void CREResourcesWindow::onFilterChange(QObject* object)
{
    CREFilterDefinition* filter = qobject_cast<CREFilterDefinition*>(object);
    if (filter == NULL)
        return;
    myFilter.setFilter(filter->filter());
    fillData();
    myFilterButton->setText(tr("Filter: %1").arg(filter->name()));
}

void CREResourcesWindow::onQuickFilter()
{
    bool ok;
    QString filter = QInputDialog::getText(this, tr("Quick filter"), tr("Filter:"), QLineEdit::Normal, myFilter.filter(), &ok);
    if (!ok)
        return;
    if (filter.isEmpty())
    {
        clearFilter();
        return;
    }
    myFilter.setFilter(filter);
    fillData();
    myFilterButton->setText(tr("Filter: %1").arg(filter));
}

void CREResourcesWindow::clearFilter()
{
    myFilter.setFilter(QString());
    fillData();
    myFilterButton->setText(tr("Filter..."));
}

void CREResourcesWindow::updateReports()
{
    CRESettings settings;
    settings.loadReports(myReports);

    myReportsMenu->clear();

    if (myReports.reports().size() > 0)
    {
        foreach(CREReportDefinition* report, myReports.reports())
        {
            QAction* a = new QAction(report->name(), this);
            myReportsMenu->addAction(a);
            myReportsMapper.setMapping(a, report);
            connect(a, SIGNAL(triggered()), &myReportsMapper, SLOT(map()));
        }

        myReportsMenu->addSeparator();
    }

    QAction* dialog = new QAction(tr("Reports definition..."), this);
    connect(dialog, SIGNAL(triggered()), this, SLOT(onReport()));
    myReportsMenu->addAction(dialog);
}

void CREResourcesWindow::onReportChange(QObject* object)
{
    CREReportDefinition* report = qobject_cast<CREReportDefinition*>(object);
    if (report == NULL)
        return;

    QProgressDialog progress(tr("Generating report..."), tr("Abort report"), 0, myDisplayedItems.size() - 1, this);
    progress.setWindowTitle(tr("Report: '%1'").arg(report->name()));
    progress.setWindowModality(Qt::WindowModal);

    QStringList headers = report->header().split("\n");
    QStringList fields = report->itemDisplay().split("\n");
    QString sort = report->itemSort();

    QString text("<table><thead><tr>");

    foreach(QString header, headers)
    {
        text += "<th>" + header + "</th>";
    }
    text += "</tr></thead><tbody>";

    CREScriptEngine engine;
    std::vector<QScriptValue> items;
    std::for_each(myDisplayedItems.begin(), myDisplayedItems.end(),
            [&items, &engine] (QObject* item) { items.push_back(engine.newQObject(item)); });

    if (!sort.isEmpty())
    {
        try
        {
            progress.setLabelText(tr("Sorting items..."));

            engine.pushContext();

            sort = "(function(left, right) { return " + sort + "; })";
            QScriptValue sortFun = engine.evaluate(sort);
            if (!sortFun.isValid() || engine.hasUncaughtException())
                throw std::runtime_error("A script error happened while compiling the sort criteria:\n" + engine.uncaughtException().toString().toStdString());

            std::sort(items.begin(), items.end(), [&sortFun, &engine](QScriptValue left, QScriptValue right) {
                QScriptValueList args;
                args.push_back(left);
                args.push_back(right);
                auto ret = sortFun.call(QScriptValue(), args);
                if (!ret.isValid() || engine.hasUncaughtException())
                {
                    throw std::runtime_error("A script error happened while sorting items:\n" + engine.uncaughtException().toString().toStdString());
                    return false;
                }
                return ret.isValid() ? ret.toBoolean() : true;
            });
            printf("complete");
            engine.popContext();
        }
        catch (std::runtime_error& ex)
        {
            QMessageBox::critical(this, "Script error", ex.what(), QMessageBox::Ok);
            return;
        }
    }

    progress.setLabelText(tr("Generating items text..."));
    foreach(QScriptValue item, items)
    {
        if (progress.wasCanceled())
            return;

        text += "<tr>";

        engine.pushContext();
        engine.globalObject().setProperty("item", item);

        foreach(QString field, fields)
        {
            text += "<td>";
            QString data = engine.evaluate(field).toString();
            if (engine.hasUncaughtException())
            {
                QMessageBox::critical(this, "Script error", "A script error happened while display items:\n" + engine.uncaughtException().toString(), QMessageBox::Ok);
                return;
            }
            text += data;
            text += "</td>\n";
        }
        engine.popContext();
        text += "</tr>\n";

        progress.setValue(progress.value() + 1);
    }
    text += "</tbody>";

    QStringList footers = report->footer().split("\n");
    text += "<tfoot>";

    foreach(QString footer, footers)
    {
        text += "<th>" + footer + "</th>";
    }
    text += "</tfoot>";

    text += "</table>";
    qDebug() << "report finished";

    CREReportDisplay display(text, tr("Report: '%1'").arg(report->name()));
    display.exec();
    progress.hide();
}

void CREResourcesWindow::fillItem(const QPoint& pos, QMenu* menu)
{
    QTreeWidgetItem* node = myTree->itemAt(pos);
    if (!node || node->data(0, Qt::UserRole).value<void*>() == NULL)
        return;
    CRETreeItem* item = reinterpret_cast<CRETreeItem*>(node->data(0, Qt::UserRole).value<void*>());
    if (!item)
        return;

    item->fillContextMenu(menu);
}

void CREResourcesWindow::treeCustomMenu(const QPoint & pos)
{
    QMenu menu;

    if (myDisplay & DisplayMessage)
    {
        QAction* addMessage = new QAction("add message", &menu);
        connect(addMessage, SIGNAL(triggered(bool)), this, SLOT(addMessage(bool)));
        menu.addAction(addMessage);
    }

    if (myDisplay & DisplayQuests)
    {
        QAction* addQuest = new QAction("add quest", &menu);
        connect(addQuest, SIGNAL(triggered(bool)), this, SLOT(addQuest(bool)));
        menu.addAction(addQuest);
    }

    fillItem(pos, &menu);

    if (menu.actions().size() == 0)
        return;
    menu.exec(myTree->mapToGlobal(pos));
}

void CREResourcesWindow::addQuest(bool)
{
#if 0
    Quest* quest = new Quest();
    quest->setCode("(new quest)");
    myQuests->quests().append(quest);
#endif
    fillData();
}

void CREResourcesWindow::addMessage(bool)
{
    MessageFile* file = new MessageFile("<new file>");
    file->setModified();
    myMessages->messages().append(file);
    fillData();
}

const ResourcesManager* CREResourcesWindow::resourcesManager() const
{
  return myResources;
}

static bool sortRandomMap(const CRERandomMap* left, const CRERandomMap* right)
{
    int name = left->map()->displayName().compare(right->map()->displayName(), Qt::CaseInsensitive);
    if (name == 0)
    {
        if (left->x() < right->x())
            return true;
        if (left->x() == right->x() && left->y() < right->y())
            return true;
        return false;
    }
    return name < 0;
}

void CREResourcesWindow::fillRandomMaps()
{
    bool full = false;
    if (myDisplay == DisplayRandomMaps)
    {
        QStringList headers;
        headers << tr("Random maps") << tr("Final map") << tr("Depth");
        myTree->setHeaderLabels(headers);
        myTree->sortByColumn(0, Qt::AscendingOrder);
        full = true;
    }

    QTreeWidgetItem* root = CREUtils::mapNode(NULL);
    myTreeItems.append(new CRETreeItemEmpty());
    root->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    myTree->addTopLevelItem(root);

    QList<CRERandomMap*> maps = myStore->randomMaps();
    qSort(maps.begin(), maps.end(), sortRandomMap);
    foreach(CRERandomMap* map, maps)
    {
        QString source(tr("from %1 [%2, %3]").arg(map->map()->name()).arg(map->x()).arg(map->y()));
        QStringList data(source);
        if (full)
        {
            data << map->parameters()->final_map << QString::number(map->parameters()->dungeon_depth);
        }
        QTreeWidgetItem* leaf = new QTreeWidgetItem(root, QStringList(data));
        myTreeItems.append(new CRETTreeItem<CRERandomMap>(map, "Random map"));
        leaf->setData(0, Qt::UserRole, QVariant::fromValue<void*>(myTreeItems.last()));
    }

    root->setText(0, tr("Random maps [%1]").arg(maps.size()));

    if (full)
    {
        root->setExpanded(true);
        myTree->resizeColumnToContents(0);
    }

    addPanel("Random map", new CRERandomMapPanel(this));
}
