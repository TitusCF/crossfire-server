#include "CREMapInformationManager.h"
extern "C" {
#include "global.h"
}

CREMapInformationManager* CREMapInformationManager::theInstance = NULL;
static QMutex lock;

CREMapInformationManager::CREMapInformationManager()
{
}

CREMapInformationManager* CREMapInformationManager::instance()
{
    if (theInstance == NULL)
    {
        QMutexLocker locker(&lock);
        if (theInstance == NULL)
            theInstance = new CREMapInformationManager();
    }
    return theInstance;
}

bool CREMapInformationManager::browseFinished() const
{
    return myWorker.isFinished();
}

void CREMapInformationManager::start()
{
    if (myWorker.isRunning())
        return;

    /** @todo clear memory */
    myInformation.clear();
    myArchetypeUse.clear();

    myCancelled = false;
    myCurrentMap = 0;
    myToProcess.clear();
    myToProcess.append(first_map_path);

    myWorker = QtConcurrent::run(this, &CREMapInformationManager::browseMaps);
}

void CREMapInformationManager::process(const QString& path)
{
    if (myCancelled)
        return;

    emit browsingMap(path);
//    qDebug() << "processing" << path;
    mapstruct *m = ready_map_name(path.toAscii(), 0);

    Q_ASSERT(!myInformation.contains(path));
    CREMapInformation* information = new CREMapInformation(path);
    myInformation[path] = information;
    information->setName(m->name);

    char exit_path[500];
    char tmppath[MAX_BUF];

    for (int x = MAP_WIDTH(m)-1; x >= 0; x--)
    {
        for (int y = MAP_HEIGHT(m)-1; y >= 0; y--)
        {
            FOR_MAP_PREPARE(m, x, y, item)
            {
                {
                    archetype *arch = find_archetype(item->arch->name);
                    QMutexLocker lock(&myLock);
                    if (!myArchetypeUse.values(arch->name).contains(information))
                        myArchetypeUse.insert(arch->name, information);
                }

                FOR_INV_PREPARE(item, inv)
                {
                    archetype *arch = find_archetype(inv->arch->name);
                    QMutexLocker lock(&myLock);
                    if (!myArchetypeUse.values(arch->name).contains(information))
                        myArchetypeUse.insert(arch->name, information);
                } FOR_INV_FINISH();

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
                                    char *end = strchr(start+1, '\n');

                                    start += strlen("final_map")+2;
                                    strncpy(ep, start, end-start);
                                }
                            }
                        }

                        if (strlen(ep)) {
                            path_combine_and_normalize(m->path, ep, exit_path, 500);
                            create_pathname(exit_path, tmppath, MAX_BUF);
                            struct stat stats;
                            if (stat(tmppath, &stats)) {
                                //printf("  map %s doesn't exist in map %s, at %d, %d.\n", ep, info->path, item->x, item->y);
                            } else {
                                if (!myToProcess.contains(exit_path))
                                    myToProcess.append(exit_path);
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
            } FOR_MAP_FINISH();
        }
    }

    m->reset_time = 1;
    m->in_memory = MAP_IN_MEMORY;
    delete_map(m);
}

void CREMapInformationManager::browseMaps()
{
    while (myCurrentMap < myToProcess.size())
    {
        process(myToProcess[myCurrentMap]);
        myCurrentMap++;
        if (myCancelled)
            break;
    }
    emit finished();
}

void CREMapInformationManager::cancel()
{
    myCancelled = true;
}

QList<CREMapInformation*> CREMapInformationManager::getArchetypeUse(const archetype* arch)
{
    QMutexLocker lock(&myLock);
    return myArchetypeUse.values(arch->name);
}
