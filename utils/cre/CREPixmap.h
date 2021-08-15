#ifndef CREPIXMAP_H
#define CREPIXMAP_H

#include <QtWidgets>

class CREPixmap
{
    public:
        static void init();

        static face_sets *faceset;
        static void setFaceset(const QString& prefix);
        static void setUseFacesetFallback(bool use);

        static QIcon getIcon(int faceNumber);

        static QIcon getTreasureIcon();
        static QIcon getTreasureOneIcon();
        static QIcon getTreasureYesIcon();
        static QIcon getTreasureNoIcon();

    protected:
        static QIcon getIcon(const face_sets* faceset, int faceNumber);

        static QHash<int, QIcon> allFaces;
        static QIcon* myTreasureIcon;
        static QIcon* myTreasureOneIcon;
        static QIcon* myTreasureYesIcon;
        static QIcon* myTreasureNoIcon;
        static bool myUseFaceFallback;
};

#endif // CREPIXMAP_H
