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

    myArchetypes = new QTreeWidget(this);
    layout->addWidget(myArchetypes, 4, 1, 1, 2);
    myArchetypes->setHeaderLabel("Allowed/forbidden archetypes");
    myArchetypes->setIconSize(QSize(32, 32));
    myArchetypes->setRootIsDecorated(false);

    layout->addWidget(new QLabel(tr("Values:"), this), 5, 1, 1, 2);
    myValues = new QTextEdit(this);
    layout->addWidget(myValues, 6, 1, 1, 2);
}

void CREArtifactPanel::setArtifact(const artifact* artifact)
{
    Q_ASSERT(artifact);
    myArtifact = artifact;

    myName->setText(artifact->item->name);
    myChance->setText(QString::number(artifact->chance));
    myType->setText(QString::number(artifact->item->type));

    const archt* arch;
    const char* name;
    QTreeWidgetItem* item;
    bool check;

    myArchetypes->clear();

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

        arch = try_find_archetype(name);
        if (!arch)
            arch = find_archetype_by_object_name(name);

        if (arch)
        {
            item = CREUtils::archetypeNode(arch, NULL);
            item->setCheckState(0, check ? Qt::Checked : Qt::Unchecked);
            myArchetypes->addTopLevelItem(item);
        }
    }

    StringBuffer* dump = stringbuffer_new();
    get_ob_diff(dump, myArtifact->item, &empty_archetype->clone);
    char* final = stringbuffer_finish(dump);
    myValues->setText(final);
    free(final);
}
