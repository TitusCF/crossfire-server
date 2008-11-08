#include <Qt>

extern "C" {
#include "global.h"
}

#include "CREArchetypePanel.h"
#include "CREUtils.h"

CREArchetypePanel::CREArchetypePanel()
{
    QGridLayout* layout = new QGridLayout(this);

    QLabel* label = new QLabel(this);
    label->setText("Name:");
    layout->addWidget(label, 1, 1);
}

void CREArchetypePanel::setArchetype(const archt* archetype)
{
    myArchetype = archetype;
}
