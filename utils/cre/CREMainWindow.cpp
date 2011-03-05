#include <Qt>
#include <QtGui>
#include <CREMainWindow.h>
#include <CREResourcesWindow.h>
#include "CREMapInformationManager.h"
#include "CREExperienceWindow.h"
#include "QuestManager.h"
#include "MessageManager.h"
#include "CREReportDisplay.h"
#include "CREPixmap.h"
#include "CRESmoothFaceMaker.h"
#include "ResourcesManager.h"
#include "CRECombatSimulator.h"

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

    myResourcesManager = new ResourcesManager();
    myResourcesManager->load();

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
    delete myResourcesManager;
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

    myReportSpellDamage = new QAction(tr("Spell damage"), this);
    myReportSpellDamage->setStatusTip(tr("Display spell damage by level (bullet spells only for now)"));
    connect(myReportSpellDamage, SIGNAL(triggered()), this, SLOT(onReportSpellDamage()));

    myReportAlchemy = new QAction(tr("Alchemy"), this);
    myReportAlchemy->setStatusTip(tr("Display alchemy formulae, in a table."));
    connect(myReportAlchemy, SIGNAL(triggered()), this, SLOT(onReportAlchemy()));

    myReportSpells = new QAction(tr("Spells"), this);
    myReportSpells->setStatusTip(tr("Display all spells, in a table."));
    connect(myReportSpells, SIGNAL(triggered()), this, SLOT(onReportSpells()));

    myToolSmooth = new QAction(tr("Generate smooth face base"), this);
    myToolSmooth->setStatusTip(tr("Generate the basic smoothed picture for a face."));
    connect(myToolSmooth, SIGNAL(triggered()), this, SLOT(onToolSmooth()));

    myToolCombatSimulator = new QAction(tr("Combat simulator"), this);
    myToolCombatSimulator->setStatusTip(tr("Simulate fighting between two objects."));
    connect(myToolCombatSimulator, SIGNAL(triggered()), this, SLOT(onToolCombatSimulator()));
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

    QMenu* reportMenu = menuBar()->addMenu("&Reports");
    reportMenu->addAction(myReportSpellDamage);
    reportMenu->addAction(myReportAlchemy);
    reportMenu->addAction(myReportSpells);

    QMenu* toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction(myToolSmooth);
    toolsMenu->addAction(myToolCombatSimulator);
}

void CREMainWindow::doResourceWindow(DisplayMode mode)
{
    QWidget* resources = new CREResourcesWindow(myMapManager, myQuestManager, myMessageManager, myResourcesManager, mode);
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



/**
 * This function takes a caster and spell and presents the
 * effective level the caster needs to be to cast the spell.
 * Basically, it just adjusts the spell->level with attuned/repelled
 * spellpaths.
 *
 * @param caster
 * person casting the spell.
 * @param spell
 * spell object.
 * @return
 * adjusted level.
 */
int min_casting_level(const object *caster, const object *spell) {
    int new_level;

    if (caster->path_denied&spell->path_attuned) {
        /* This case is not a bug, just the fact that this function is
         * usually called BEFORE checking for path_deny. -AV
         */
        return 1;
    }
    new_level = spell->level
                +((caster->path_repelled&spell->path_attuned) ? +2 : 0)
                +((caster->path_attuned&spell->path_attuned) ? -2 : 0);
    return MAX(new_level, 1);
}


/**
 * This function returns the effective level the spell
 * is being cast at.
 * Note that I changed the repelled/attuned bonus to 2 from 5.
 * This is because the new code compares casting_level against
 * min_caster_level, so the difference is effectively 4
 *
 * @param caster
 * person casting the spell.
 * @param spell
 * spell object.
 * @return
 * adjusted level.
 */
int caster_level(const object *caster, const object *spell) {
    int level = caster->level;

    /* If this is a player, try to find the matching skill */
    if (caster->type == PLAYER && spell->skill) {
        int i;

        for (i = 0; i < NUM_SKILLS; i++)
            if (caster->contr->last_skill_ob[i]
            && caster->contr->last_skill_ob[i]->skill == spell->skill) {
                level = caster->contr->last_skill_ob[i]->level;
                break;
            }
    };
    /* Got valid caster level.  Now adjust for attunement */
    level += ((caster->path_repelled&spell->path_attuned) ? -2 : 0)
            +((caster->path_attuned&spell->path_attuned) ? 2 : 0);

    /* Always make this at least 1.  If this is zero, we get divide by zero
     * errors in various places.
     */
    if (level < 1)
        level = 1;
    return level;
}

/**
 * Scales the spellpoint cost of a spell by it's increased effectiveness.
 * Some of the lower level spells become incredibly vicious at high
 * levels.  Very cheap mass destruction.  This function is
 * intended to keep the sp cost related to the effectiveness.
 *
 * Note that it is now possible for a spell to cost both grace and
 * mana.  In that case, we return which ever value is higher.
 *
 * @param caster
 * what is casting the spell.
 * @param spell
 * spell object.
 * @param flags
 * one of @ref SPELL_xxx.
 * @return
 * sp/mana points cost.
 */
sint16 SP_level_spellpoint_cost(object *caster, object *spell, int flags) {
    int sp, grace, level = caster_level(caster, spell);

    if (settings.spellpoint_level_depend == TRUE) {
        if (spell->stats.sp && spell->stats.maxsp) {
            sp = (int)(spell->stats.sp*(1.0+MAX(0, (float)(level-spell->level)/(float)spell->stats.maxsp)));
        } else
            sp = spell->stats.sp;

        sp *= PATH_SP_MULT(caster, spell);
        if (!sp && spell->stats.sp)
            sp = 1;

        if (spell->stats.grace && spell->stats.maxgrace) {
            grace = (int)(spell->stats.grace*(1.0+MAX(0, (float)(level-spell->level)/(float)spell->stats.maxgrace)));
        } else
            grace = spell->stats.grace;

        grace *= PATH_SP_MULT(caster, spell);
        if (spell->stats.grace && !grace)
            grace = 1;
    } else {
        sp = spell->stats.sp*PATH_SP_MULT(caster, spell);
        if (spell->stats.sp && !sp)
            sp = 1;
        grace = spell->stats.grace*PATH_SP_MULT(caster, spell);
        if (spell->stats.grace && !grace)
            grace = 1;
    }
    if (flags == SPELL_HIGHEST)
        return MAX(sp, grace);
    else if (flags == SPELL_GRACE)
        return grace;
    else if (flags == SPELL_MANA)
        return sp;
    else {
        LOG(llevError, "SP_level_spellpoint_cost: Unknown flags passed: %d\n", flags);
        return 0;
    }
}

/**
 * Returns adjusted damage based on the caster.
 *
 * @param caster
 * who is casting.
 * @param spob
 * spell we are adjusting.
 * @return
 * adjusted damage.
 */
int SP_level_dam_adjust(const object *caster, const object *spob) {
    int level = caster_level(caster, spob);
    int adj = level-min_casting_level(caster, spob);

    if (adj < 0)
        adj = 0;
    if (spob->dam_modifier)
        adj /= spob->dam_modifier;
    else
        adj = 0;
    return adj;
}


void CREMainWindow::onReportSpellDamage()
{
    QStringList spell;
    QList<QStringList> damage;

    archetype* arch = first_archetype;
    object* caster = create_archetype("orc");
    int dm, cost;

    while (arch != NULL)
    {
        if (arch->clone.type == SPELL && arch->clone.subtype == SP_BULLET && arch->clone.skill && strcmp(arch->clone.skill, "praying") == 0)
        {
            spell.append(arch->clone.name);
            QStringList dam;
            for (int l = 0; l < settings.max_level; l++)
            {
                caster->level = l;
                dm = arch->clone.stats.dam + SP_level_dam_adjust(caster, &arch->clone);
                cost = SP_level_spellpoint_cost(caster, &arch->clone, SPELL_GRACE);
                dam.append(tr("%1 [%2]").arg(dm).arg(cost));
            }
            damage.append(dam);
        }

        arch = arch->next;
    }
    object_free_drop_inventory(caster);

    QString report("<table><thead><tr><th>level</th>");

    for (int i = 0; i < spell.size(); i++)
    {
        report += "<th>" + spell[i] + "</th>";
    }

    report += "</tr></thead><tbody>";

    for (int l = 0; l < settings.max_level; l++)
    {
        report += "<tr><td>" + QString::number(l) + "</td>";
        for (int s = 0; s < spell.size(); s++)
            report += "<td>" + damage[s][l] + "</td>";
        report += "</tr>";
    }

    report += "</tbody></table>";

    CREReportDisplay show(report);
    show.exec();
}

static QString alchemyTable(const QString& skill)
{
    bool one = false;

    QString report = QString("<h2>%1</h2><table><thead><tr><th>product</th><th>difficulty</th><th>ingredients count</th><th>experience</th>").arg(skill);
    report += "</tr></thead><tbody>";

    QHash<int, QStringList> recipes;

    const recipelist* list;
    const recipe* recipe;

    for (int ing = 1; ; ing++)
    {
        list = get_formulalist(ing);
        if (!list)
            break;

        for (recipe = list->items; recipe; recipe = recipe->next)
        {
            if (skill == recipe->skill)
            {
                if (recipe->arch_names == 0)
                    // hu?
                    continue;

                const archetype* arch = find_archetype(recipe->arch_name[0]);

                QString name;
                if (strcmp(recipe->title, "NONE") == 0)
                {
                    if (arch->clone.title == NULL)
                        name = arch->clone.name;
                    else
                        name = QString("%1 %2").arg(arch->clone.name, arch->clone.title);
                }
                else
                {
                    name = QString("%1 of %2").arg(arch->clone.name, recipe->title);
                }
                recipes[recipe->diff].append(QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>").arg(name).arg(recipe->diff).arg(recipe->ingred_count).arg(recipe->exp));
                one = true;
            }
        }
    }

    if (!one)
        return QString();

    QList<int> difficulties = recipes.keys();
    qSort(difficulties);
    foreach(int difficulty, difficulties)
    {
        report += recipes[difficulty].join("\n");
    }

    report += "</tbody></table>";

    return report;
}

void CREMainWindow::onReportAlchemy()
{
    QStringList skills;

    const archt* arch = first_archetype;
    for (; arch; arch = arch->next)
    {
        if (arch->clone.type == SKILL)
            skills.append(arch->clone.name);
    }

    skills.sort();

    QString report("<h1>Alchemy formulae</h1>");

    foreach(const QString skill, skills)
    {
        report += alchemyTable(skill);
    }

    CREReportDisplay show(report);
    show.exec();
}

static QString spellsTable(const QString& skill)
{
    bool one = false;

    QString report = QString("<h2>%1</h2><table><thead><tr><th>Spell</th><th>Level</th>").arg(skill);
    report += "</tr></thead><tbody>";

    QHash<int, QStringList> spells;

    const archetype* spell;

    for (spell = first_archetype; spell; spell = spell->next)
    {
        if (spell->clone.type == SPELL && spell->clone.skill == skill)
        {
            spells[spell->clone.level].append(QString("<tr><td>%1</td><td>%2</td></tr>").arg(spell->clone.name).arg(spell->clone.level));
            one = true;
        }
    }

    if (!one)
        return QString();

    QList<int> levels = spells.keys();
    qSort(levels);
    foreach(int level, levels)
    {
        report += spells[level].join("\n");
    }

    report += "</tbody></table>";

    return report;
}

void CREMainWindow::onReportSpells()
{
    QStringList skills;

    const archt* arch = first_archetype;
    for (; arch; arch = arch->next)
    {
        if (arch->clone.type == SKILL)
            skills.append(arch->clone.name);
    }

    skills.sort();

    QString report("<h1>Spell list</h1>");

    foreach(const QString skill, skills)
    {
        report += spellsTable(skill);
    }

    CREReportDisplay show(report);
    show.exec();
}

void CREMainWindow::onToolSmooth()
{
    CRESmoothFaceMaker smooth;
    smooth.exec();
}

void CREMainWindow::onToolCombatSimulator()
{
    CRECombatSimulator simulator;
    simulator.exec();
}
