#ifndef RESOURCESMANAGER_H
#define RESOURCESMANAGER_H

extern "C" {
#include "global.h"
#include "object.h"
#include "quest.h"
}
#include "AssetsTracker.h"
#include <set>
#include <QObject>
#include "ModifiedAssetsManager.h"

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

class ResourcesManager : public QObject, AssetsTracker
{
    Q_OBJECT

    public:
        ResourcesManager();
        virtual ~ResourcesManager();

        void load();

        int recipeMaxIngredients() const;
        QStringList recipes(int count) const;
        const recipestruct* recipe(int ingredients, const QString& name) const;

        virtual void assetDefined(const archt *arch, const std::string &filename) override { myArchetypes.assetDefined(arch, filename); }
        virtual void assetDefined(const quest_definition *asset, const std::string &filename) override { myQuests.assetDefined(asset, filename); }

        const std::map<std::string, std::set<const archt*> >& origins() const { return myArchetypes.origins(); }
        std::string originOf(const archt *arch) const { return myArchetypes.originOf(arch); }

        const std::map<std::string, std::set<const quest_definition*> >& questOrigins() const { return myQuests.origins(); }
        std::string originOfQuest(const quest_definition *quest) const { return myQuests.originOf(quest); }

        static void archetypeUse(const archt* item, CREMapInformationManager* store, AssetUseCallback callback);

        bool hasPendingChanges() const { return myArchetypes.hasPendingChanges() || myQuests.hasPendingChanges(); }

    public slots:
      void archetypeModified(archetype *arch);
      void saveArchetypes();
      void questModified(quest_definition *quest);
      void saveQuests();

    protected:
        QList<QHash<QString, recipestruct*> > myRecipes;
        ModifiedAssetsManager<archetype> myArchetypes;
        ModifiedAssetsManager<quest_definition> myQuests;
};

#endif /* RESOURCESMANAGER_H */
