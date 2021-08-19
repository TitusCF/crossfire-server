#include <Qt>
#include "CREPixmap.h"

extern "C" {
#include "global.h"
#include "face.h"
#include "image.h"
}

#include "assets.h"
#include "AssetsManager.h"
#include "CRESettings.h"

QHash<uint16_t, QIcon> CREPixmap::allFaces;
QIcon* CREPixmap::myTreasureIcon;
QIcon* CREPixmap::myTreasureOneIcon;
QIcon* CREPixmap::myTreasureYesIcon;
QIcon* CREPixmap::myTreasureNoIcon;
face_sets *CREPixmap::faceset;
bool CREPixmap::myUseFaceFallback = true;

void CREPixmap::init()
{
    myTreasureIcon = new QIcon(":resources/treasure_list.png");
    myTreasureOneIcon = new QIcon(":resources/treasureone_list.png");
    myTreasureYesIcon = new QIcon(":resources/treasure_yes.png");
    myTreasureNoIcon = new QIcon(":resources/treasure_no.png");

    CRESettings settings;
    faceset = getManager()->facesets()->get(settings.facesetToDisplay().toStdString());
    myUseFaceFallback = settings.facesetUseFallback();
}

void CREPixmap::setFaceset(const QString& prefix)
{
    faceset = getManager()->facesets()->get(prefix.toStdString());
    allFaces.clear();
    CRESettings settings;
    settings.setFacesetToDisplay(prefix);
}

void CREPixmap::setUseFacesetFallback(bool use)
{
    myUseFaceFallback = use;
    allFaces.clear();
    CRESettings settings;
    settings.setFacesetUseFallback(use);
}

QIcon CREPixmap::getIcon(uint16_t faceNumber)
{
    return CREPixmap::getIcon(faceset, faceNumber);
}

QIcon CREPixmap::getIcon(const face_sets *faceset, uint16_t faceNumber)
{
    if (!allFaces.contains(faceNumber))
    {
        QPixmap face;

        if (faceset->allocated >= faceNumber && faceset->faces[faceNumber].datalen > 0)
        {
            if (face.loadFromData((uchar*)faceset->faces[faceNumber].data, faceset->faces[faceNumber].datalen))
            {
                QIcon icon(face.scaled(32, 32, Qt::KeepAspectRatio));
                allFaces[faceNumber] = icon;
            }
        }
        else if (myUseFaceFallback && faceset->fallback)
        {
            return getIcon(faceset->fallback, faceNumber);
        }
    }
    return allFaces[faceNumber];
}

QIcon CREPixmap::getTreasureIcon()
{
    Q_ASSERT(myTreasureIcon);
    return *myTreasureIcon;
}

QIcon CREPixmap::getTreasureOneIcon()
{
    Q_ASSERT(myTreasureOneIcon);
    return *myTreasureOneIcon;
}

QIcon CREPixmap::getTreasureYesIcon()
{
    Q_ASSERT(myTreasureYesIcon);
    return *myTreasureYesIcon;
}

QIcon CREPixmap::getTreasureNoIcon()
{
    Q_ASSERT(myTreasureNoIcon);
    return *myTreasureNoIcon;
}
