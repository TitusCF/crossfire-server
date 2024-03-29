#include <Qt>

extern "C" {
#include "global.h"
#include "artifact.h"
#include "object.h"
#include "face.h"
}

#include "CREAnimationPanel.h"
#include "CREAnimationControl.h"
#include "CREUtils.h"

#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"
#include "CREMapInformationManager.h"

CREAnimationPanel::CREAnimationPanel(QWidget* parent, CREMapInformationManager* maps) : CRETPanel(parent)
{
    myMaps = maps;
    myAnimation = 0;

    QGridLayout* layout = new QGridLayout(this);

    myUsing = new QTreeWidget(this);
    myUsing->setColumnCount(1);
    myUsing->setHeaderLabel(tr("Used by"));
    myUsing->setIconSize(QSize(32, 32));
    myUsing->sortByColumn(0, Qt::AscendingOrder);
    layout->addWidget(myUsing, 1, 1);

    myFaces = new QTreeWidget(this);
    myFaces->setColumnCount(1);
    myFaces->setHeaderLabel(tr("Faces"));
    myFaces->setIconSize(QSize(32, 32));
    layout->addWidget(myFaces, 2, 1);

    myDisplay = new CREAnimationControl(this);
    layout->addWidget(myDisplay, 3, 1);
}

void CREAnimationPanel::setItem(const Animations* animation)
{
    Q_ASSERT(animation);
    myAnimation = animation;

    myUsing->clear();

    myDisplay->setAnimation(myAnimation);

    QTreeWidgetItem* root = NULL;

    getManager()->archetypes()->each([this, &root] (archetype *arch) {
      sstring key = object_get_value(&arch->clone, "identified_animation");
        if (arch->clone.animation == myAnimation || (key && strcmp(myAnimation->name, key) == 0))
        {
            if (root == NULL)
            {
                root = CREUtils::archetypeNode(NULL);
                myUsing->addTopLevelItem(root);
                root->setExpanded(true);
            }
            CREUtils::archetypeNode(arch, root);
        }
    });

    root = NULL;

    artifactlist* list;
    artifact* art;

    for (list = first_artifactlist; list; list = list->next)
    {
        for (art = list->items; art; art = art->next)
        {
            if (art->item->animation == myAnimation)
            {
                if (root == NULL)
                {
                    root = CREUtils::artifactNode(NULL);
                    myUsing->addTopLevelItem(root);
                    root->setExpanded(true);
                }
                CREUtils::artifactNode(art, root);
            }
        }
    }

    auto maps = myMaps->getAnimationUse(myAnimation);
    for (auto map : maps)
    {
        if (!root)
        {
            root = CREUtils::mapNode(nullptr);
            myUsing->addTopLevelItem(root);
            root->setExpanded(true);
        }

        CREUtils::mapNode(map, root);
    }

    myFaces->clear();
    root = CREUtils::faceNode(NULL);
    myFaces->addTopLevelItem(root);
    root->setExpanded(true);

    for (int face = 0; face < animation->num_animations; face++)
    {
      CREUtils::faceNode(animation->faces[face], root);
    }
}
