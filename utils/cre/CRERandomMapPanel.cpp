#include <QLabel>
#include <QTextEdit>

#include "CRERandomMapPanel.h"
#include "CRERandomMap.h"
#include "CREMapInformation.h"

CRERandomMapPanel::CRERandomMapPanel()
{
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Source map:"), this), 0, 0);
    layout->addWidget(mySource = new QLabel(this), 0, 1);
    layout->addWidget(new QLabel(tr("Parameters:"), this), 1, 0, 1, 2);
    layout->addWidget(myInformation = new QTextEdit(this), 2, 0, 1, 2);
    myInformation->setReadOnly(true);
}

void CRERandomMapPanel::setRandomMap(const CRERandomMap* map)
{
    StringBuffer* sb = write_map_parameters_to_string(map->parameters());
    char* text = stringbuffer_finish(sb);
    myInformation->setText(text);
    free(text);

    mySource->setText(tr("%1 [%2, %3]").arg(map->map()->displayName()).arg(map->x()).arg(map->y()));
}
