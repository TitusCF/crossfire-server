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

ResourcesManager::ResourcesManager()
{
}

ResourcesManager::~ResourcesManager()
{
}

void ResourcesManager::load()
{
    setlocale(LC_NUMERIC, "C");

    settings.archetypes_tracker = this;
    add_server_collect_hooks();
    init_globals();
    init_library();

    for (archt* arch = get_next_archetype(NULL); arch; arch = get_next_archetype(arch))
    {
        myArchetypes[arch->name] = arch;
    }

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

    getManager()->animations()->each([this] (auto anim) {
        myAnimations[anim->name] = anim;
    });
}

QStringList ResourcesManager::archetypes() const
{
    QStringList keys = myArchetypes.keys();
    qSort(keys);
    return keys;
}

const archetype* ResourcesManager::archetype(const QString& name) const
{
    return myArchetypes[name];
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


QStringList ResourcesManager::allAnimations() const
{
    QStringList keys = myAnimations.keys();
    qSort(keys);
    return keys;
}

const animations_struct* ResourcesManager::animation(const QString& name) const
{
    return myAnimations[name];
}

void ResourcesManager::assetDefined(const archt *arch, const std::string &filename) {
    for (auto it = myOrigins.begin(); it != myOrigins.end(); it++) {
        it.value().removeAll(arch);
    }
    myOrigins[filename.c_str()].append(arch);
}
