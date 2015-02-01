#include <QtGui>

extern "C" {
#include "global.h"
#include "artifact.h"
}

#include "CREArtifactPanel.h"
#include "CREUtils.h"

CREArtifactPanel::CREArtifactPanel()
{
    myArtifact = NULL;

    QGridLayout* layout = new QGridLayout(this);

    QLabel* label = new QLabel(this);
    label->setText("Name:");
    layout->addWidget(label, 1, 1);
    myName = new QLineEdit(this);
    layout->addWidget(myName, 1, 2);

    label = new QLabel(this);
    label->setText("Chance:");
    layout->addWidget(label, 2, 1);
    myChance = new QLineEdit(this);
    layout->addWidget(myChance, 2, 2);

    label = new QLabel(this);
    label->setText("Type:");
    layout->addWidget(label, 3, 1);
    myType = new QLineEdit(this);
    layout->addWidget(myType, 3, 2);

    myViaAlchemy = new QLabel(this);
    myViaAlchemy->setWordWrap(true);
    layout->addWidget(myViaAlchemy, 4, 1, 1, 2);

    layout->addWidget(new QLabel(tr("Values:"), this), 5, 1, 1, 2);
    myValues = new QTextEdit(this);
    layout->addWidget(myValues, 6, 1, 1, 2);

    myArchetypes = new QTreeWidget(this);
    layout->addWidget(myArchetypes, 7, 1, 2, 1);
    myArchetypes->setHeaderLabel("Allowed/forbidden archetypes");
    myArchetypes->setIconSize(QSize(32, 32));
    myArchetypes->setRootIsDecorated(false);
    connect(myArchetypes, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(artifactChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    layout->addWidget(new QLabel(tr("Result:"), this), 7, 2);
    myInstance = new QTextEdit(this);
    layout->addWidget(myInstance, 8, 2);
}

void CREArtifactPanel::computeMadeViaAlchemy(const artifact* artifact) const
{
    Q_ASSERT(artifact != NULL);

    const recipelist* list;
    const recipe* recipe;
    const archetype* arch;
    QStringList possible;

    for (int ing = 1; ; ing++)
    {
        list = get_formulalist(ing);
        if (!list)
            break;
        for (recipe = list->items; recipe; recipe = recipe->next)
        {
            if (recipe->title == NULL)
                continue;

            if (strcmp(recipe->title, artifact->item->name) != 0)
                continue;

            for (size_t a = 0; a < recipe->arch_names; a++)
            {
                arch = find_archetype(recipe->arch_name[a]);
                if (!arch)
                    continue;
                if ((arch->clone.type == artifact->item->type) && legal_artifact_combination(&arch->clone, artifact))
                {
                    if (!possible.contains(arch->name))
                        possible.append(arch->name);
                }
            }
        }
    }

    if (possible.isEmpty())
        myViaAlchemy->setText(tr("Can't be made via alchemy."));
    else
    {
        if (possible.size() == artifact->allowed_size)
            myViaAlchemy->setText(tr("Can be made via alchemy."));
        else
        {
            possible.sort();
            myViaAlchemy->setText(tr("The following archetypes can be used via alchemy: %1").arg(possible.join(tr(", "))));
        }
    }
}

/**
 * Add all possible archetypes for the specified artifact.
 * @param artifact artifact. Only the type is used.
 * @param name archetype or object name to allow. If NULL, all items of the correct type are added.
 * @param check if true then the archetype or object's name must match, else it must not match.
 * @param root tree to insert items into.
 */
static void addArchetypes(const artifact* artifact, const char* name, bool check, QTreeWidget* root)
{
    const archt* arch;
    QTreeWidgetItem* item = NULL;
    item = NULL;
    for (arch = first_archetype; arch != NULL; arch = arch->next)
    {
        if (arch->clone.type != artifact->item->type)
        {
          continue;
        }

        if (name == NULL || (check && (!strcmp(name, arch->clone.name) || (!strcmp(name, arch->name)))) || (!check && strcmp(name, arch->clone.name) && strcmp(name, arch->name)))
        {
            if (item == NULL)
            {
                item = new QTreeWidgetItem(root, QStringList(name == NULL ? "(all)" : name));
                item->setCheckState(0, check ? Qt::Checked : Qt::Unchecked);
                root->addTopLevelItem(item);
                item->setExpanded(true);
            }
            CREUtils::archetypeNode(arch, item)->setData(0, Qt::UserRole, arch->name);
        }
    }
}

void CREArtifactPanel::setArtifact(const artifact* artifact)
{
    Q_ASSERT(artifact);
    myArtifact = artifact;

    myName->setText(artifact->item->name);
    myChance->setText(QString::number(artifact->chance));
    myType->setText(QString::number(artifact->item->type));

    computeMadeViaAlchemy(artifact);

    const char* name;
    bool check;

    myArchetypes->clear();
    myInstance->clear();

    /* 'allowed' is either the archetype name or the item's name, so check all archetypes for each word */
    for (const linked_char* allowed = artifact->allowed; allowed; allowed = allowed->next)
    {
        name = allowed->name;
        if (name[0] == '!')
        {
            name = name + 1;
            check = false;
        }
        else
            check = true;

        addArchetypes(myArtifact, name, check, myArchetypes);
    }

    /* all items are allowed, so add them */
    if (artifact->allowed == NULL)
    {
        addArchetypes(myArtifact, NULL, true, myArchetypes);
    }

    StringBuffer* dump = stringbuffer_new();
    get_ob_diff(dump, myArtifact->item, &empty_archetype->clone);
    char* final = stringbuffer_finish(dump);
    myValues->setText(final);
    free(final);
}

void CREArtifactPanel::artifactChanged(QTreeWidgetItem* current, QTreeWidgetItem*)
{
    if (!current || current->data(0, Qt::UserRole).toString().isEmpty())
    {
        myInstance->clear();
        return;
    }
    archt* arch = try_find_archetype(current->data(0, Qt::UserRole).toString().toUtf8().constData());
    if (!arch)
    {
        myInstance->clear();
        return;
    }

    char* desc;
    object* obj = arch_to_object(arch);
    SET_FLAG(obj, FLAG_IDENTIFIED);
    give_artifact_abilities(obj, myArtifact->item);
    desc = stringbuffer_finish(describe_item(obj, NULL, NULL));
    myInstance->setText(desc);
    free(desc);

    object_free2(obj, FREE_OBJ_FREE_INVENTORY | FREE_OBJ_NO_DESTROY_CALLBACK);
}
