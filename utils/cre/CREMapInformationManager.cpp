#include <QtConcurrent/QtConcurrent>

#include "CREMapInformationManager.h"
#include "CRESettings.h"
#include "CREArchetypePanel.h"
#include "MessageManager.h"
#include "MessageFile.h"
#include "ScriptFileManager.h"
#include "ScriptFile.h"
#include "CRERandomMap.h"

extern "C" {
#include "global.h"
}
#include "assets.h"
#include "AssetsManager.h"

const char* eventNames[NR_EVENTS] = {
  "EVENT_NONE",
  "EVENT_APPLY",
  "EVENT_ATTACKED",
  "EVENT_DEATH",
  "EVENT_DROP",
  "EVENT_PICKUP",
  "EVENT_SAY",
  "EVENT_STOP",
  "EVENT_TIME",
  "EVENT_THROW",
  "EVENT_TRIGGER",
  "EVENT_CLOSE",
  "EVENT_TIMER",
  "EVENT_DESTROY",
  "EVENT_BORN",
  "EVENT_CLOCK",
  "EVENT_CRASH",
  "EVENT_PLAYER_DEATH",
  "EVENT_GKILL",
  "EVENT_LOGIN",
  "EVENT_LOGOUT",
  "EVENT_MAPENTER",
  "EVENT_MAPLEAVE",
  "EVENT_MAPRESET",
  "EVENT_REMOVE",
  "EVENT_SHOUT",
  "EVENT_TELL",
  "EVENT_MUZZLE",
  "EVENT_KICK",
  "EVENT_MAPUNLOAD",
  "EVENT_MAPLOAD",
  "EVENT_USER",
  "EVENT_SELLING",
  "EVENT_ATTACKS",
};

CREMapInformationManager::CREMapInformationManager(QObject* parent, MessageManager* messageManager, ScriptFileManager* scriptManager) : QObject(parent)
{
    Q_ASSERT(messageManager != NULL);
    Q_ASSERT(scriptManager != NULL);
    myMessageManager = messageManager;
    myScriptManager = scriptManager;
}

CREMapInformationManager::~CREMapInformationManager()
{
    qDeleteAll(myInformation);
}

bool CREMapInformationManager::browseFinished() const
{
    return myWorker.isFinished();
}

void CREMapInformationManager::start()
{
    if (myWorker.isRunning())
        return;

    myWorker = QtConcurrent::run(this, &CREMapInformationManager::browseMaps);
}

void CREMapInformationManager::checkItem(const object* item, CREMapInformation* information, const object* env)
{
    archetype *arch = find_archetype(item->arch->name);
    if (arch != NULL) {
        addArchetypeUse(arch->name, information);
        information->addArchetype(arch->name);
        if (item->face && item->face != arch->clone.face)
        {
            addFaceUse(item->face->name, information);
            information->addFace(item->face->name);
        }
        if (item->animation && item->animation != arch->clone.animation)
        {
            addAnimationUse(item->animation->name, information);
            information->addAnimation(item->animation->name);
        }
    }
    checkEvent(item, information, env);

    if (item->type == EXIT || item->type == TELEPORTER || item->type == PLAYER_CHANGER) {
        char ep[500];
        const char *start;

        if (!item->slaying) {
            ep[0] = '\0';
            /*if (warn_no_path)
                printf(" exit without any path at %d, %d on %s\n", item->x, item->y, info->path);*/
        } else {
            memset(ep, 0, 500);
            if (strcmp(item->slaying, "/!"))
                strcpy(ep, EXIT_PATH(item));
            else {
                if (!item->msg) {
                    //printf("  random map without message in %s at %d, %d\n", info->path, item->x, item->y);
                } else {
                    /* Some maps have a 'exit_on_final_map' flag, ignore it. */
                    start = strstr(item->msg, "\nfinal_map ");
                    if (!start && strncmp(item->msg, "final_map", strlen("final_map")) == 0)
                        /* Message start is final_map, nice */
                        start = item->msg;
                    if (start) {
                        const char *end = strchr(start+1, '\n');

                        start += strlen("final_map")+2;
                        strncpy(ep, start, end-start);
                    }

                    information->addRandomMap(new CRERandomMap(information, env->x, env->y, item->msg));
                }
            }

            if (strlen(ep)) {
                char exit_path[500], tmppath[MAX_BUF];
                path_combine_and_normalize(env->map->path, ep, exit_path, 500);
                create_pathname(exit_path, tmppath, MAX_BUF);
                if (!QFileInfo(tmppath).exists()) {
                    printf("  map %s doesn't exist in map %s, at %d, %d.\n", ep, env->map->path, env->x, env->y);
                } else {
                    QString exit = exit_path;
                    if (!myToProcess.contains(exit))
                        myToProcess.append(exit);

                    CREMapInformation* other = getOrCreateMapInformation(exit);
                    Q_ASSERT(other);
                    other->addAccessedFrom(exit);
                    information->addExitTo(exit_path);

#if 0
                    link = get_map_info(exit_path);
                    add_map(link, &info->exits_from);
                    add_map(info, &link->exits_to);

                    if (do_regions_link) {
                        mapstruct *link = ready_map_name(exit_path, 0);

                        if (link && link != m) {
                            /* no need to link a map with itself. Also, if the exit points to the same map, we don't
                            * want to reset it. */
                            add_region_link(m, link, item->arch->clone.name);
                            link->reset_time = 1;
                            link->in_memory = MAP_IN_MEMORY;
                            delete_map(link);
                        }
                    }
#endif
                }
            }
        }
    }
    else if (item->type == EVENT_CONNECTOR && item->subtype > 0 && item->subtype < NR_EVENTS)
    {
        ScriptFile* script = myScriptManager->getFile(item->slaying);
        script->addHook(new HookInformation(information, env->x, env->y, env->name, item->title, eventNames[item->subtype]));
    }

    if (QUERY_FLAG(item, FLAG_MONSTER))
        information->setExperience(information->experience() + item->stats.exp);

    FOR_INV_PREPARE(item, inv)
    {
        checkItem(inv, information, env);
    } FOR_INV_FINISH();
}

void CREMapInformationManager::process(const QString& path2)
{
    /*
     don't ask why, but the variable gets apparently destroyed on the myToProcess.append() when it reallocated values...
    so keep a copy to avoid messes
     */
    QString path(path2);

    if (myCancelled)
        return;

    emit browsingMap(path);
//    qDebug() << "processing" << path;
    CREMapInformation* information = getOrCreateMapInformation(path);

    char tmppath[MAX_BUF];
    create_pathname(path.toLatin1(), tmppath, MAX_BUF);
    QFileInfo info(tmppath);

    if (!info.exists())
    {
//        qDebug() << "non existant map" << tmppath;
        return;
    }

    if (!information->mapTime().isNull() && information->mapTime() >= info.lastModified())
    {
        foreach(QString exit, information->exitsTo())
        {
            if (!myToProcess.contains(exit))
                myToProcess.append(exit);
        }
//        qDebug() << "skipping " << tmppath;
        return;
    }

    /* remove scripts to avoid duplications */
    myScriptManager->removeMap(information);

    mapstruct *m = mapfile_load(path.toLatin1(), MAP_STYLE | MAP_NO_DIFFICULTY);
//    qDebug() << "processing" << path << information->mapTime() << info.lastModified();
    information->setName(m->name);
    information->setMapTime(info.lastModified());
    if (m->region != NULL)
        information->setRegion(m->region->name);
    else
        information->setRegion("wilderness"); /** @todo get from config */
    information->setDifficulty(m->difficulty);
    m->difficulty = 0;
    information->setComputedDifficulty(calculate_difficulty(m));
    m->difficulty = information->difficulty();
    if (m->background_music)
        information->setBackgroundMusic(m->background_music);

    information->setShopGreed(m->shopgreed);
    if (m->shopitems != NULL)
    {
        for (int i = 0; i < m->shopitems[0].index; i++)
        {
            information->shopItems().insert(QString(m->shopitems[i].name == NULL ? "*" : m->shopitems[i].name), m->shopitems[i].strength);
        }
    }
    if (m->shoprace != NULL)
      information->setShopRace(m->shoprace);
    information->setShopMin(m->shopmin);
    information->setShopMax(m->shopmax);

    char exit_path[500];

    for (int x = 0; x < 4; x++)
        if (m->tile_path[x] != NULL) {
            path_combine_and_normalize(m->path, m->tile_path[x], exit_path, sizeof(exit_path));
            create_pathname(exit_path, tmppath, MAX_BUF);
            if (!QFileInfo(tmppath).exists()) {
                printf("  map %s doesn't exist in map %s, for tile %d.\n", exit_path, m->path, x);
            }

            QString exit = exit_path;
            if (!myToProcess.contains(exit))
                myToProcess.append(exit);

            CREMapInformation* other = getOrCreateMapInformation(path);
            Q_ASSERT(other);
            other->addAccessedFrom(path);
            information->addExitTo(exit_path);
        }

    for (int x = MAP_WIDTH(m)-1; x >= 0; x--)
    {
        for (int y = MAP_HEIGHT(m)-1; y >= 0; y--)
        {
            FOR_MAP_PREPARE(m, x, y, item)
            {
                checkItem(item, information, item);
            } FOR_MAP_FINISH();
        }
    }

    QMutexLocker lock(&myLock);
    if (m->region == NULL)
        qDebug() << "map without region" << m->name << m->path;
    myExperience[m->region ? m->region->name : "(undefined)"] += information->experience();

    m->reset_time = 1;
    m->in_memory = MAP_IN_MEMORY;
    delete_map(m);
}

void CREMapInformationManager::browseMaps()
{
    qDeleteAll(myInformation);
    myArchetypeUse.clear();

    loadCache();

    myCancelled = false;
    myCurrentMap = 0;
    myToProcess.clear();
    myToProcess.append(QString(first_map_path));

    /* try to find race-specific start maps */
    if (first_map_ext_path[0] != 0)
    {
        getManager()->archetypes()->each([this] (archetype *arch)
        {
            if (arch->clone.type == PLAYER)
            {
                char path[MAX_BUF], name[MAX_BUF];
                snprintf(name, sizeof(name), "%s/%s", first_map_ext_path, arch->name);
                create_pathname(name, path, sizeof(path));
                if (QFileInfo(path).exists()) {
                    myToProcess.append(name);
                }
            }
        });
    }

    /* Add style maps */
    recurseStyleDirectory("styles");

    while (myCurrentMap < myToProcess.size())
    {
        process(myToProcess[myCurrentMap]);
        myCurrentMap++;
        if (myCancelled)
            break;
    }

    storeCache();

    emit finished();

    /** @todo make nicer report */
    qDebug() << "experience repartition:";
    foreach(QString region, myExperience.keys())
    {
        qDebug() << region << myExperience[region];
    }

    qDebug() << myToProcess.size() << "maps processed";
}

void CREMapInformationManager::cancel()
{
    myCancelled = true;
    myWorker.waitForFinished();
}

QList<CREMapInformation*> CREMapInformationManager::allMaps()
{
    QMutexLocker lock(&myLock);
    return myInformation.values();
}

QList<CREMapInformation*> CREMapInformationManager::getArchetypeUse(const archetype* arch)
{
    QMutexLocker lock(&myLock);
    return myArchetypeUse.values(arch->name);
}

QList<CREMapInformation*> CREMapInformationManager::getFaceUse(const Face* face)
{
    QMutexLocker lock(&myLock);
    return myFaceUse.values(face->name);
}

QList<CREMapInformation*> CREMapInformationManager::getAnimationUse(const Animations* animation)
{
    QMutexLocker lock(&myLock);
    return myAnimationUse.values(animation->name);
}

QList<CREMapInformation*> CREMapInformationManager::getMapsForQuest(const quest_definition *quest)
{
    QMutexLocker lock(&myLock);
    return myQuestUse.values(quest->quest_code);
}

void CREMapInformationManager::loadCache()
{
    Q_ASSERT(myInformation.isEmpty());

    CRESettings settings;
    QFile file(settings.mapCacheDirectory() + QDir::separator() + "maps_cache.xml");
    file.open(QFile::ReadOnly);

    QXmlStreamReader reader(&file);
    bool hasMaps = false;
    CREMapInformation* map = NULL;

    while (!reader.atEnd())
    {
        reader.readNext();

        if (reader.isStartElement() && reader.name() == "maps")
        {
            int version = reader.attributes().value("version").toString().toInt();
            if (version < 1)
                return;
            hasMaps = true;
            continue;
        }

        if (!hasMaps)
            continue;

        if (reader.isStartElement() && reader.name() == "map")
        {
            map = new CREMapInformation();
            continue;
        }
        if (reader.isStartElement() && reader.name() == "path")
        {
            QString path = reader.readElementText();
            map->setPath(path);
            Q_ASSERT(!myInformation.contains(path));
            myInformation[path] = map;
            continue;
        }
        if (reader.isStartElement() && reader.name() == "name")
        {
            map->setName(reader.readElementText());
            continue;
        }
        if (reader.isStartElement() && reader.name() == "lastModified")
        {
            QString date = reader.readElementText();
            map->setMapTime(QDateTime::fromString(date, Qt::ISODate));
            continue;
        }
        if (reader.isStartElement() && reader.name() == "difficulty")
        {
            map->setDifficulty(reader.readElementText().toInt());
        }
        if (reader.isStartElement() && reader.name() == "computedDifficulty")
        {
            map->setComputedDifficulty(reader.readElementText().toInt());
        }
        if (reader.isStartElement() && reader.name() == "experience")
        {
            map->setExperience(reader.readElementText().toLongLong());
        }
        if (reader.isStartElement() && reader.name() == "region")
        {
            map->setRegion(reader.readElementText());
        }
        if (reader.isStartElement() && reader.name() == "arch")
        {
            QString arch = reader.readElementText();
            map->addArchetype(arch);
            addArchetypeUse(arch, map);
            continue;
        }
        if (reader.isStartElement() && reader.name() == "face")
        {
            QString face = reader.readElementText();
            map->addFace(face);
            addFaceUse(face, map);
            continue;
        }
        if (reader.isStartElement() && reader.name() == "animation")
        {
            QString anim = reader.readElementText();
            map->addAnimation(anim);
            addAnimationUse(anim, map);
            continue;
        }
        if (reader.isStartElement() && reader.name() == "exitTo")
        {
            QString path = reader.readElementText();
            map->addExitTo(path);
            continue;
        }
        if (reader.isStartElement() && reader.name() == "accessedFrom")
        {
            QString path = reader.readElementText();
            map->addAccessedFrom(path);
            continue;
        }
        if (reader.isStartElement() && reader.name() == "messageFile")
        {
            QString file = reader.readElementText();
            map->addMessage(file);
            MessageFile* message = myMessageManager->findMessage(file);
            if (message != NULL)
                message->maps().append(map);
            continue;
        }
        if (reader.isStartElement() && reader.name() == "quest")
        {
            QString code = reader.readElementText();
            map->addQuest(code);
            addQuestUse(code, map);
            continue;
        }
        if (reader.isStartElement() && reader.name() == "shopItem")
        {
            QString item = reader.attributes().value("name").toString();
            int strength = reader.readElementText().toInt();
            map->shopItems()[item] = strength;
        }
        if (reader.isStartElement() && reader.name() == "shopGreed")
        {
            double greed = reader.readElementText().toDouble();
            map->setShopGreed(greed);
        }
        if (reader.isStartElement() && reader.name() == "shopRace")
        {
            map->setShopRace(reader.readElementText());
        }
        if (reader.isStartElement() && reader.name() == "shopMin")
        {
            quint64 min = reader.readElementText().toULongLong();
            map->setShopMin(min);
        }
        if (reader.isStartElement() && reader.name() == "shopMax")
        {
            quint64 max = reader.readElementText().toULongLong();
            map->setShopMax(max);
        }
        if (reader.isStartElement() && reader.name() == "script")
        {
            int x = reader.attributes().value("x").toString().toInt();
            int y = reader.attributes().value("x").toString().toInt();
            QString item = reader.attributes().value("itemName").toString();
            QString plugin = reader.attributes().value("pluginName").toString();
            QString event = reader.attributes().value("eventName").toString();
            QString script = reader.readElementText();
            myScriptManager->getFile(script)->addHook(new HookInformation(map, x, y, item, plugin, event));
        }
        if (reader.isStartElement() && reader.name() == "random_map")
        {
            int x = reader.attributes().value("x").toString().toInt();
            int y = reader.attributes().value("y").toString().toInt();
            QString params = reader.attributes().value("params").toString();
            map->addRandomMap(new CRERandomMap(map, x, y, params.toLatin1().constData()));
        }
        if (reader.isStartElement() && reader.name() == "background_music")
        {
            map->setBackgroundMusic(reader.readElementText());
            continue;
        }

        if (reader.isEndElement() && reader.name() == "map")
        {
            map = NULL;
            continue;
        }
    }

//    qDebug() << "loaded maps from cache:" << myInformation.size();
}

void CREMapInformationManager::storeCache()
{
    CRESettings settings;
    QFile file(settings.mapCacheDirectory() + QDir::separator() + "maps_cache.xml");
    file.open(QFile::WriteOnly | QFile::Truncate);

    QXmlStreamWriter writer(&file);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("maps");
    writer.writeAttribute("version", "1");

    QList<CREMapInformation*> maps = myInformation.values();
    foreach(CREMapInformation* map, maps)
    {
        writer.writeStartElement("map");
        writer.writeTextElement("path", map->path());
        writer.writeTextElement("name", map->name());
        writer.writeTextElement("lastModified", map->mapTime().toString(Qt::ISODate));
        writer.writeTextElement("difficulty", QString::number(map->difficulty()));
        writer.writeTextElement("computedDifficulty", QString::number(map->computedDifficulty()));
        writer.writeTextElement("experience", QString::number(map->experience()));
        writer.writeTextElement("region", map->region());
        foreach(QString arch, map->archetypes())
        {
            writer.writeTextElement("arch", arch);
        }
        foreach(QString face, map->faces())
        {
            writer.writeTextElement("face", face);
        }
        foreach(QString anim, map->animations())
        {
            writer.writeTextElement("animation", anim);
        }
        foreach(QString path, map->exitsTo())
        {
            writer.writeTextElement("exitTo", path);
        }
        foreach(QString path, map->accessedFrom())
        {
            writer.writeTextElement("accessedFrom", path);
        }
        foreach(QString file, map->messages())
        {
            writer.writeTextElement("messageFile", file);
        }
        foreach(QString code, map->quests())
        {
            writer.writeTextElement("quest", code);
        }
        foreach(QString item, map->shopItems().keys())
        {
            writer.writeStartElement("shopItem");
            writer.writeAttribute("name", item);
            writer.writeCharacters(QString::number(map->shopItems()[item]));
            writer.writeEndElement();
        }
        if (map->shopGreed() != 0)
        {
            writer.writeTextElement("shopGreed", QString::number(map->shopGreed()));
        }
        if (!map->shopRace().isEmpty())
        {
          writer.writeTextElement("shopRace", map->shopRace());
        }
        if (map->shopMin() != 0)
        {
          writer.writeTextElement("shopMin", QString::number(map->shopMin()));
        }
        if (map->shopMax() != 0)
        {
          writer.writeTextElement("shopMax", QString::number(map->shopMax()));
        }

        QList<ScriptFile*> scripts = myScriptManager->scriptsForMap(map);
        foreach(ScriptFile* script, scripts)
        {
            foreach(const HookInformation* hook, script->hooks())
            {
                if (hook->map() == map)
                {
                    writer.writeStartElement("script");
                    writer.writeAttribute("x", QString::number(hook->x()));
                    writer.writeAttribute("y", QString::number(hook->y()));
                    writer.writeAttribute("itemName", hook->itemName());
                    writer.writeAttribute("pluginName", hook->pluginName());
                    writer.writeAttribute("eventName", hook->eventName());
                    writer.writeCharacters(script->path());
                    writer.writeEndElement();
                }
            }
        }

        foreach(CRERandomMap* random, map->randomMaps())
        {
            writer.writeStartElement("random_map");
            writer.writeAttribute("x", QString::number(random->x()));
            writer.writeAttribute("y", QString::number(random->y()));
            StringBuffer* sb = write_map_parameters_to_string(random->parameters());
            char* params = stringbuffer_finish(sb);
            writer.writeAttribute("params", params);
            free(params);
            writer.writeEndElement();
        }

        if (!map->backgroundMusic().isEmpty())
        {
            writer.writeTextElement("background_music", map->backgroundMusic());
        }

        writer.writeEndElement();
    }

    writer.writeEndElement();

    writer.writeEndDocument();
}

CREMapInformation* CREMapInformationManager::getOrCreateMapInformation(const QString& path)
{
    if (!myInformation.contains(path))
    {
        CREMapInformation* information = new CREMapInformation(path);
        myInformation[path] = information;
    }
    return myInformation[path];
}

void CREMapInformationManager::addArchetypeUse(const QString& name, CREMapInformation* map)
{
    QMutexLocker lock(&myLock);
    if (!myArchetypeUse.values(name).contains(map))
        myArchetypeUse.insert(name, map);
}

void CREMapInformationManager::addFaceUse(const QString& name, CREMapInformation* map)
{
    QMutexLocker lock(&myLock);
    if (!myFaceUse.values(name).contains(map))
        myFaceUse.insert(name, map);
}

void CREMapInformationManager::addAnimationUse(const QString& name, CREMapInformation* map)
{
    QMutexLocker lock(&myLock);
    if (!myAnimationUse.values(name).contains(map))
        myAnimationUse.insert(name, map);
}

void CREMapInformationManager::addQuestUse(const QString &name, CREMapInformation *map) {
    QMutexLocker lock(&myLock);
    if (!myQuestUse.values(name).contains(map))
        myQuestUse.insert(name, map);
}


void CREMapInformationManager::checkEvent(const object* item, CREMapInformation* map, const object* env)
{
    const QString slaying = "/python/dialog/npc_dialog.py";
    const QString python = "Python";

    if (item->type != EVENT_CONNECTOR)
        return;

    if (item->subtype > 0 && item->subtype < NR_EVENTS)
    {
        ScriptFile* script = myScriptManager->getFile(item->slaying);
        script->addHook(new HookInformation(map, env->x, env->y, env->name, item->title, eventNames[item->subtype]));
    }

    if (python != item->title)
        return;

    if (item->subtype == EVENT_SAY && slaying == item->slaying)
    {
        //qDebug() << "message event in" << map->path() << item->name;
        QString path = item->name;
        if (!path.startsWith('/'))
            path = '/' + path;

        MessageFile* message = myMessageManager->findMessage(path);
        if (message != NULL)
        {
            if (!message->maps().contains(map))
                message->maps().append(map);
            map->addMessage(path);
        } else
            qDebug() << "missing message file" << path << "in" << map->path();
    }

    if (QString(item->slaying).startsWith("/python/quests/"))
    {
        //qDebug() << "quest-related Python stuff";
        QStringList split = QString(item->name).split(' ', QString::SkipEmptyParts);
        if (split.length() > 1)
        {
            //qDebug() << "definitely quest" << split[0];
            map->addQuest(split[0]);
            addQuestUse(split[0], map);
        }
    }
}

QList<CREMapInformation*> CREMapInformationManager::getMapsForRegion(const QString& region)
{
    QList<CREMapInformation*> list;

    foreach(CREMapInformation* map, myInformation.values())
    {
        if (map->region() == region)
            list.append(map);
    }

    return list;
}

void CREMapInformationManager::clearCache()
{
    CRESettings settings;
    Q_ASSERT(myWorker.isFinished());
    QFile::remove(settings.mapCacheDirectory() + QDir::separator() + "maps_cache.xml");
}

QList<CRERandomMap*> CREMapInformationManager::randomMaps()
{
    QList<CRERandomMap*> maps;
    foreach(CREMapInformation* map, myInformation.values())
    {
        maps.append(map->randomMaps());
    }
    return maps;
}

void CREMapInformationManager::recurseStyleDirectory(const QString& from)
{
    char full[MAX_BUF];
    create_pathname(from.toLatin1(), full, sizeof(full));

    QDir dir(full);
    QFileInfoList items = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst);
    foreach(QFileInfo info, items)
    {
        QString relative(from + QDir::separator() + info.baseName());
        if (info.isDir())
        {
            recurseStyleDirectory(relative);
        }
        else
        {
            myToProcess.append(relative);
        }
    }
}
