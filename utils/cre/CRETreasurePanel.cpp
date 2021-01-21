#include <Qt>

extern "C" {
#include "global.h"
#include "treasure.h"
}

#include "CRETreasurePanel.h"
#include "CREUtils.h"

#include "assets.h"
#include "AssetsManager.h"

CRETreasurePanel::CRETreasurePanel(QWidget* parent) : CRETPanel(parent)
{
    QGridLayout* layout = new QGridLayout(this);

    myUsing = new QTreeWidget(this);
    myUsing->setColumnCount(1);
    myUsing->setHeaderLabel(tr("Used by"));
    myUsing->setIconSize(QSize(32, 32));
    layout->addWidget(myUsing, 1, 1, 2, 1);

    layout->addWidget(new QLabel("Difficulty:"), 1, 2);
    layout->addWidget(myDifficulty = new QSpinBox(this), 1, 3);
    myDifficulty->setRange(0, 150);
    myDifficulty->setValue(150);
    QPushButton* generate = new QPushButton("generate", this);
    connect(generate, SIGNAL(clicked(bool)), this, SLOT(onGenerate(bool)));
    layout->addWidget(generate, 1, 4);
    layout->addWidget(myGenerated = new QTreeWidget(this), 2, 2, 1, 3);
    myGenerated->setHeaderLabel(tr("Generation result"));
    myGenerated->setIconSize(QSize(32, 32));
}

void CRETreasurePanel::setItem(const treasurelist* treas)
{
    myUsing->clear();
    myTreasure = treas;

    QTreeWidgetItem* root = NULL;

    QString name = myTreasure->name;

    getManager()->archetypes()->each([this, &root, &name] (const archetype *arch) {
        if (arch->clone.randomitems && name == arch->clone.randomitems->name)
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

    getManager()->treasures()->each([this, &root, &name] (treasurelist *list) {
        for (const treasure *t = list->items; t; t = t->next)
        {
            if (t->name == name)
            {
                if (root == NULL)
                {
                    root = CREUtils::treasureNode(NULL);
                    myUsing->addTopLevelItem(root);
                    root->setExpanded(true);
                }
                CREUtils::treasureNode(list, root);
            }
        }
    });

    myGenerated->clear();
}

void CRETreasurePanel::onGenerate(bool)
{
    const int difficulty = myDifficulty->value();
    myGenerated->clear();
    object* result = object_new(), *item;
    create_treasure((treasurelist*)myTreasure, result, 0, difficulty, 0);
    while ((item = result->inv))
    {
        identify(result->inv);
        myGenerated->addTopLevelItem(CREUtils::objectNode(item, NULL));

        object_remove(item);
        object_free(item, 0);
    }

    object_free(result, 0);
}
