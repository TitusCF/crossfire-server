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

    init_globals();
    init_library();

    for (archt* arch = get_next_archetype(NULL); arch; arch = get_next_archetype(arch))
    {
        myArchetypes[arch->name] = arch;
    }

    getManager()->treasures()->each([this] (treasurelist *list) {
        myTreasures[list->name] = list;
    });

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

    getManager()->faces()->each([this] (auto face) {
        myFaces[face->name] = face;
    });

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

QStringList ResourcesManager::treasureLists() const
{
    QStringList keys = myTreasures.keys();
    qSort(keys);
    return keys;
}

const treasurelist* ResourcesManager::treasureList(const QString& name)
{
    return myTreasures[name];
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


QStringList ResourcesManager::faces() const
{
    QStringList keys = myFaces.keys();
    qSort(keys);
    return keys;
}

const Face* ResourcesManager::face(const QString& name) const
{
    return myFaces[name];
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
