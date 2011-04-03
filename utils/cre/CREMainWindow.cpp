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
#include "sproto.h"
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
    cleanup();
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

    myReportPlayer = new QAction(tr("Player vs monsters"), this);
    myReportPlayer->setStatusTip(tr("Compute statistics related to player vs monster combat."));
    connect(myReportPlayer, SIGNAL(triggered()), this, SLOT(onReportPlayer()));

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
    reportMenu->addAction(myReportPlayer);

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

static int monsterFight(archetype* monster, archetype* skill, int level)
{
    int limit = 50, result = 1;
    player pl;
    memset(&pl, 0, sizeof(player));
    strncpy(pl.savebed_map, "HallOfSelection", MAX_BUF);
    pl.bed_x = 5;
    pl.bed_y = 5;
    extern int nrofpixmaps;
    pl.socket.faces_sent = (uint8*)calloc(sizeof(uint8), nrofpixmaps);

    object* obfirst = object_create_arch(find_archetype("dwarf_player"));
    obfirst->level = level;
    obfirst->contr = &pl;
    object* obskill = object_create_arch(skill);
    obskill->level = level;
    SET_FLAG(obskill, FLAG_APPLIED);
    object_insert_in_ob(obskill, obfirst);
    object* sword = object_create_arch(find_archetype((skill->clone.subtype == SK_TWO_HANDED_WEAPON) ? "sword_3" : "sword"));
    SET_FLAG(sword, FLAG_APPLIED);
    object_insert_in_ob(sword, obfirst);
    fix_object(obfirst);
    obfirst->stats.hp = obfirst->stats.maxhp;

    object* obsecond = object_create_arch(monster);
    tag_t tagfirst = obfirst->count;
    tag_t tagsecond = obsecond->count;

    // make a big map so large monsters are ok in map
    mapstruct* test_map = get_empty_map(50, 50);

    obfirst = object_insert_in_map_at(obfirst, test_map, NULL, 0, 0, 0);
    obsecond = object_insert_in_map_at(obsecond, test_map, NULL, 0, 1 + monster->tail_x, monster->tail_y);

    if (!obsecond || object_was_destroyed(obsecond, tagsecond))
    {
        qDebug() << "second removed??";
    }

    while (limit-- > 0 && obfirst->stats.hp >= 0 && obsecond->stats.hp >= 0)
    {
        if (obfirst->weapon_speed_left > 0) {
            --obfirst->weapon_speed_left;
            do_some_living(obfirst);

            move_player(obfirst, 3);
            if (object_was_destroyed(obsecond, tagsecond))
                break;

            /* the player may have been killed (acid for instance), so check here */
            if (object_was_destroyed(obfirst, tagfirst) || (obfirst->map != test_map))
            {
                result = 0;
                break;
            }
        }

        if (obsecond->speed_left > 0) {
            --obsecond->speed_left;
            monster_do_living(obsecond);

            attack_ob(obfirst, obsecond);
            /* when player is killed, she is teleported to bed of reality -> check map */
            if (object_was_destroyed(obfirst, tagfirst) || (obfirst->map != test_map))
            {
                result = 0;
                break;
            }
        }

        obfirst->weapon_speed_left += obfirst->weapon_speed;
        if (obfirst->weapon_speed_left > 1.0)
            obfirst->weapon_speed_left = 1.0;
        if (obsecond->speed_left <= 0)
            obsecond->speed_left += FABS(obsecond->speed);
    }

    free(pl.socket.faces_sent);
    if (!object_was_destroyed(obfirst, tagfirst))
    {
        object_remove(obfirst);
        object_free2(obfirst, FREE_OBJ_FREE_INVENTORY);
    }
    if (!object_was_destroyed(obsecond, tagsecond))
    {
        object_remove(obsecond);
        object_free2(obsecond, FREE_OBJ_FREE_INVENTORY);
    }
    delete_map(test_map);

    return result;
}

static int monsterFight(archetype* monster, archetype* skill, int level, int count)
{
    int victory = 0;
    while (count-- > 0)
        victory += monsterFight(monster, skill, level);

    return victory;
}

static QString monsterFight(archetype* monster, archetype* skill)
{
    qDebug() << "monsterFight:" << monster->clone.name << skill->clone.name;
    int ret, min = settings.max_level + 1, half = settings.max_level + 1, count = 5, level;
    int first = 1, max = settings.max_level;

    while (first != max)
    {
        level = (max + first) / 2;
        if (level < first)
            level = first;
        if (first > max)
            first = max;

        ret = monsterFight(monster, skill, level, count);
        if (ret > 0)
        {
            if (level < min)
                min = level;
            if (ret > (count / 2) && (level < half))
                half = level;

            max = level;
        }
        else
        {
            if (first == level)
                break;
            first = level;
        }
    }

    //qDebug() << "   result:" << min << half;

    if (min == settings.max_level + 1)
        return "<td colspan=\"2\">-</td>";
    return "<td>" + QString::number(min) + "</td><td>" + ((half != 0) ? QString::number(half) : "") + "</td>";
}

static QString monsterTable(archetype* monster, QList<archetype*> skills)
{
    QString line = "<tr>";

    line += "<td>" + QString(monster->clone.name) + "</td>";
    line += "<td>" + QString::number(monster->clone.level) + "</td>";
    line += "<td>" + QString::number(monster->clone.speed) + "</td>";
    line += "<td>" + QString::number(monster->clone.stats.wc) + "</td>";
    line += "<td>" + QString::number(monster->clone.stats.dam) + "</td>";
    line += "<td>" + QString::number(monster->clone.stats.ac) + "</td>";
    line += "<td>" + QString::number(monster->clone.stats.hp) + "</td>";
    line += "<td>" + QString::number(monster->clone.stats.Con) + "</td>";

    foreach(archetype* skill, skills)
    {
        line += monsterFight(monster, skill);
    }
    line += "</tr>\n";

    return line;
}

void CREMainWindow::onReportPlayer()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QStringList names;
    QMap<QString, archetype*> monsters;
    QList<archetype*> skills;

    archt* arch = first_archetype;
    for (; arch; arch = arch->next)
    {
        if (QUERY_FLAG(&arch->clone, FLAG_MONSTER) && arch->clone.stats.hp > 0 && arch->head == NULL)
        {
            QString name(QString(arch->clone.name).toLower());
            if (monsters.contains(name))
            {
                int suffix = 1;
                do
                {
                    name = QString(arch->clone.name).toLower() + "_" + QString::number(suffix);
                    suffix++;
                } while (monsters.contains(name));
            }

            monsters[name] = arch;
        }
        if (arch->clone.type == SKILL && IS_COMBAT_SKILL(arch->clone.subtype))
        {
            if (strcmp(arch->name, "skill_missile_weapon") == 0 || strcmp(arch->name, "skill_throwing") == 0)
                continue;
            skills.append(arch);
        }
    }

    names = monsters.keys();
    names.sort();

    QString report(tr("<h1>Player vs monsters</h1><p><strong>fv</strong> is the level at which the first victory happened, <strong>hv</strong> is the level at which at least 50% of fights were victorious.</p>\n")), line;
    report += "<table border=\"1\"><tbody>\n";
    report += "<tr>";
    report += "<th rowspan=\"2\">Monster</th>";
    report += "<th rowspan=\"2\">level</th>";
    report += "<th rowspan=\"2\">speed</th>";
    report += "<th rowspan=\"2\">wc</th>";
    report += "<th rowspan=\"2\">dam</th>";
    report += "<th rowspan=\"2\">ac</th>";
    report += "<th rowspan=\"2\">hp</th>";
    report += "<th rowspan=\"2\">regen</th>";

    line = "<tr>";
    foreach(archetype* skill, skills)
    {
        report += "<th colspan=\"2\">" + QString(skill->clone.name) + "</th>";
        line += "<th>fv</th><th>hv</th>";
    }
    report += "</tr>\n" + line + "</tr>\n";

    int limit = 500;
    foreach(const QString name, names)
    {
        report += monsterTable(monsters[name], skills);
        if (limit-- <= 0)
            break;
    }

    report += "</tbody></table>\n";

    CREReportDisplay show(report);
    QApplication::restoreOverrideCursor();
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
