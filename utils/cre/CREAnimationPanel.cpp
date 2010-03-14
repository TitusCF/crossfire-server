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

CREAnimationPanel::CREAnimationPanel()
{
    myAnimation = 0;

    QGridLayout* layout = new QGridLayout(this);

    myUsing = new QTreeWidget(this);
    myUsing->setColumnCount(1);
    myUsing->setHeaderLabel(tr("Used by"));
    myUsing->setIconSize(QSize(32, 32));
    myUsing->sortByColumn(0, Qt::AscendingOrder);
    layout->addWidget(myUsing, 1, 1);

    myDisplay = new CREAnimationControl(this);
    layout->addWidget(myDisplay, 2, 1);
}

void CREAnimationPanel::setAnimation(const Animations* animation)
{
    Q_ASSERT(animation);
    myAnimation = animation;

    myUsing->clear();

    myDisplay->setAnimation(myAnimation);

    QTreeWidgetItem* root = NULL;

    const archt* arch;

    for (arch = first_archetype; arch; arch = (arch->more ? arch->more : arch->next))
    {
        if (arch->clone.animation_id == myAnimation->num)
        {
            if (root == NULL)
            {
                root = CREUtils::archetypeNode(NULL);
                myUsing->addTopLevelItem(root);
                root->setExpanded(true);
            }
            CREUtils::archetypeNode(arch, root);
        }
    }

    root = NULL;

    artifactlist* list;
    artifact* art;

    for (list = first_artifactlist; list; list = list->next)
    {
        for (art = list->items; art; art = art->next)
        {
            if (art->item->animation_id == myAnimation->num)
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
}
