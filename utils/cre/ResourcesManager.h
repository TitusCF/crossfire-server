#ifndef RESOURCESMANAGER_H
#define RESOURCESMANAGER_H

extern "C" {
#include "global.h"
#include "object.h"
}
#include "AssetsTracker.h"
#include <set>

class CREMapInformation;
class CREMapInformationManager;

enum ArchetypeUse {
  OTHER_ARCH,
  DEATH_ANIM,
  TREASURE_USE,
  MAP_USE,
  RANDOM_MAP_FINAL_EXIT,
  ALCHEMY_PRODUCT,
};

typedef std::function<bool(ArchetypeUse use, const archt*, const treasurelist*, const CREMapInformation*, recipe*)> AssetUseCallback;

class ResourcesManager : public QObject, AssetsTracker<archt>
{
    Q_OBJECT

    public:
        ResourcesManager();
        virtual ~ResourcesManager();

        void load();

        int recipeMaxIngredients() const;
        QStringList recipes(int count) const;
        const recipestruct* recipe(int ingredients, const QString& name) const;

        virtual void assetDefined(const archt *arch, const std::string &filename);

        const QHash<QString, QList<const archt*> >& origins() const { return myOrigins; }
        QString originOf(const archt * arch) const;

        static void archetypeUse(const archt* item, CREMapInformationManager* store, AssetUseCallback callback);

        bool hasPendingChanges() const { return !myDirty.empty(); }

    public slots:
      void archetypeModified(archetype *arch);
      void saveArchetypes();

    protected:
        QHash<QString, QList<const archt*> > myOrigins;
        QList<QHash<QString, recipestruct*> > myRecipes;
        std::set<archetype *> myDirty;
};

#endif /* RESOURCESMANAGER_H */
