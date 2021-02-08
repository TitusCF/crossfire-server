#ifndef RESOURCESMANAGER_H
#define RESOURCESMANAGER_H

extern "C" {
#include "global.h"
#include "object.h"
}
#include "AssetsTracker.h"

class ResourcesManager : public AssetsTracker<archt>
{
    public:
        ResourcesManager();
        virtual ~ResourcesManager();

        void load();

        int recipeMaxIngredients() const;
        QStringList recipes(int count) const;
        const recipestruct* recipe(int ingredients, const QString& name) const;

        virtual void assetDefined(const archt *arch, const std::string &filename);

        const QHash<QString, QList<const archt*> >& origins() const { return myOrigins; }

    protected:
        QHash<QString, QList<const archt*> > myOrigins;
        QList<QHash<QString, recipestruct*> > myRecipes;
};

#endif /* RESOURCESMANAGER_H */
