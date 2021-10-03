#include <QString>
#include <qlist.h>
#include <qhash.h>
#include <QStringList>
#include "ResourcesManager.h"
#include <locale.h>

extern "C" {
#include "global.h"
#include "libproto.h"
#include "recipe.h"
#include "image.h"
#include "sproto.h"
}
#include "assets.h"
#include "AssetsManager.h"
#include "CREMapInformationManager.h"
#include "CRERandomMap.h"
#include "LicenseManager.h"
#include "ArchetypeWriter.h"

ResourcesManager::ResourcesManager()
{
}

ResourcesManager::~ResourcesManager()
{
}

void ResourcesManager::load()
{
    setlocale(LC_NUMERIC, "C");

    settings.archetypes_tracker = (AssetsTracker<archetype> *)(this);
    add_server_collect_hooks();
    settings.hooks[settings.hooks_count] = LicenseManager::readLicense;
    settings.hooks_filename[settings.hooks_count] = ".LICENSE";
    settings.hooks_count++;
    init_globals();
    init_library();

    QString key;

    for (int ing = 1; ; ing++)
    {
        recipelist* list = get_formulalist(ing);
        if (!list)
            break;

        QHash<QString, recipestruct*> recipes;
        for (recipestruct* rec = list->items; rec; rec = rec->next)
        {
            key = QString("%1_%2").arg(rec->arch_name[0], rec->title);
            recipes[key] = rec;
        }
        myRecipes.append(recipes);
    }
}

int ResourcesManager::recipeMaxIngredients() const
{
    return myRecipes.size();
}

QStringList ResourcesManager::recipes(int count) const
{
    if (count < 1 || count > myRecipes.size())
        return QStringList();

    QStringList keys = myRecipes[count - 1].keys();
    qSort(keys);
    return keys;
}

const recipe* ResourcesManager::recipe(int ingredients, const QString& name) const
{
    if (ingredients < 1 || ingredients > myRecipes.size())
        return NULL;

    return myRecipes[ingredients - 1][name];
}


void ResourcesManager::assetDefined(const archt *arch, const std::string &filename) {
    for (auto it = myOrigins.begin(); it != myOrigins.end(); it++) {
        it.value().removeAll(arch);
    }
    myOrigins[filename.c_str()].append(arch);
}

void ResourcesManager::archetypeUse(const archt* item, CREMapInformationManager* store, AssetUseCallback callback)
{
    bool goOn = true;
    getManager()->archetypes()->each([&item, &callback, &goOn] (archt *arch) {
        if (!goOn)
            return;

        if (arch->clone.other_arch == item)
        {
            goOn = callback(OTHER_ARCH, arch, nullptr, nullptr, nullptr);
        }

        sstring death_anim = NULL;
        if (goOn && (death_anim = object_get_value(&arch->clone, "death_animation")) && strcmp(death_anim, item->name) == 0)
        {
            goOn = callback(DEATH_ANIM, arch, nullptr, nullptr, nullptr);
        }
    });

    getManager()->treasures()->each([&item, callback, &goOn] (treasurelist *list) {
        if (!goOn)
            return;
        for (auto t = list->items; t; t = t->next)
        {
            if (t->item == item)
            {
                goOn = callback(TREASURE_USE, nullptr, list, nullptr, nullptr);
            }
        }
    });

    QList<CREMapInformation*> mapuse = store->getArchetypeUse(item);
    foreach(CREMapInformation* information, mapuse)
    {
        if (!goOn)
            continue;
        goOn = callback(MAP_USE, nullptr, nullptr, information, nullptr);
    }
    auto allMaps = store->allMaps();
    foreach(CREMapInformation *information, allMaps)
    {
        if (!goOn)
            return;
        foreach(CRERandomMap* rm, information->randomMaps())
        {
            if (strcmp(item->name, rm->parameters()->final_exit_archetype) == 0)
            {
                goOn = callback(RANDOM_MAP_FINAL_EXIT, nullptr, nullptr, information, nullptr);
            }
            if (!goOn)
                return;
        }
    }

    int count = 1;
    recipelist* list;
    while ((list = get_formulalist(count++)))
    {
        if (!goOn)
            break;
        recipestruct* rec = list->items;
        while (goOn && rec)
        {
            for (size_t ing = 0; ing < rec->arch_names; ing++)
            {
                if (strcmp(rec->arch_name[ing], item->name) == 0)
                {
                    goOn = callback(ALCHEMY_PRODUCT, nullptr, nullptr, nullptr, rec);
                    break;
                }
            }
            rec = rec->next;
        }
    }
}

QString ResourcesManager::originOf(const archt *arch) const {
    for (auto file = myOrigins.begin(); file != myOrigins.end(); file++) {
        if (file.value().contains(arch)) {
            return file.key();
        }
    }
    return "";
}

void ResourcesManager::archetypeModified(archetype *arch) {
    myDirty.insert(arch);
}

void write(const QString &filename, QList<const archt*> archs) {
    auto buf = stringbuffer_new();
    ArchetypeWriter writer;
    qDebug() << "writing archetype file" << filename;

    for (auto arch = archs.begin(); arch != archs.end(); arch++) {
        writer.write(*arch, buf);
    }

    size_t length = stringbuffer_length(buf);
    char *data = stringbuffer_finish(buf);

    QFile out(filename);
    out.open(QIODevice::WriteOnly);
    out.write(data, length);
    free(data);
}

void ResourcesManager::saveArchetypes() {
    for (auto a = myDirty.begin(); a != myDirty.end(); a++) {
        for (auto file = myOrigins.begin(); file != myOrigins.end(); file++) {
            if (file.value().contains(*a)) {
                write(file.key(), file.value());
            }
        }
    }

    myDirty.clear();
}
