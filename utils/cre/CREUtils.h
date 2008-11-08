#ifndef CREUTILS_H
#define CREUTILS_H

#include <QtGui>

extern "C" {
#include "global.h"
#include "artifact.h"
#include "treasure.h"
}

class CREUtils
{
    public:
        static QTreeWidgetItem* archetypeNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* archetypeNode(const archt* arch, QTreeWidgetItem* parent);

        static QTreeWidgetItem* artifactNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* artifactNode(const artifact* arti, QTreeWidgetItem* parent);

        static QTreeWidgetItem* treasureNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* treasureNode(const treasurelist* list, QTreeWidgetItem* parent);
        static QTreeWidgetItem* treasureNode(const treasure* treasure, QTreeWidgetItem* parent);

        static QTreeWidgetItem* formulaeNode(const recipe* recipe, QTreeWidgetItem* parent);

        static QTreeWidgetItem* faceNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* faceNode(const New_Face* face, QTreeWidgetItem* parent);

        static QTreeWidgetItem* animationNode(QTreeWidgetItem* parent);
        static QTreeWidgetItem* animationNode(const Animations* anim, QTreeWidgetItem* parent);
};

#endif // CREUTILS_H
