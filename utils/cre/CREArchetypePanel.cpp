#include <Qt>

extern "C" {
#include "global.h"
}

#include "CREArchetypePanel.h"
#include "CREUtils.h"

CREArchetypePanel::CREArchetypePanel()
{
    QGridLayout* layout = new QGridLayout(this);

    myDisplay = new QTextEdit(this);
    layout->addWidget(myDisplay, 1, 1);
}

void CREArchetypePanel::setArchetype(const archt* archetype)
{
    myArchetype = archetype;
    StringBuffer* dump = stringbuffer_new();
    object_dump((object*)&myArchetype->clone, dump);
    char* final = stringbuffer_finish(dump);
    myDisplay->setText(final);
    free(final);
}
