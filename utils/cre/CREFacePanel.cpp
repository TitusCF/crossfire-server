#include <Qt>

extern "C" {
#include "global.h"
#include "face.h"
#include "image.h"
#include "quest.h"
}

#include "CREFacePanel.h"
#include "CREUtils.h"
#include "CRESmoothFaceMaker.h"
#include "assets.h"
#include "AssetsManager.h"
#include "LicenseManager.h"
#include "CREMapInformationManager.h"
#include "Quests.h"

/** @todo duplication with common/image */
static const char *const colorname[] = {
    "black",                    /* 0  */
    "white",                    /* 1  */
    "blue",                     /* 2  */
    "red",                      /* 3  */
    "orange",                   /* 4  */
    "light_blue",               /* 5  */
    "dark_orange",              /* 6  */
    "green",                    /* 7  */
    "light_green",              /* 8  */
    "grey",                     /* 9  */
    "brown",                    /* 10 */
    "yellow",                   /* 11 */
    "khaki"                     /* 12 */
};


CREFacePanel::CREFacePanel(QWidget* parent, CREMapInformationManager* maps) : CRETPanel(parent)
{
    myMaps = maps;
    myFace = 0;

    QGridLayout* layout = new QGridLayout(this);

    myUsing = new QTreeWidget(this);
    myUsing->setColumnCount(1);
    myUsing->setHeaderLabel(tr("Used by"));
    myUsing->setIconSize(QSize(32, 32));
    layout->addWidget(myUsing, 1, 1, 3, 2);

    myLicenses = new QTreeWidget(this);
    myLicenses->setColumnCount(2);
    myLicenses->setHeaderLabels(QStringList(tr("License field")) << "Value");
    myLicenses->setIconSize(QSize(32, 32));
    myLicenses->setRootIsDecorated(false);
    layout->addWidget(myLicenses, 1, 3, 3, 2);

    myColor = new QComboBox(this);
    layout->addWidget(new QLabel("Magicmap color: "), 4, 1);
    layout->addWidget(myColor, 4, 2, 1, 3);

    for(uint color = 0; color < sizeof(colorname) / sizeof(*colorname); color++)
        myColor->addItem(colorname[color], color);

    myFile = new QLineEdit(this);
    myFile->setReadOnly(true);
    layout->addWidget(new QLabel("Original file: "), 5, 1);
    layout->addWidget(myFile, 5, 2, 1, 3);

    mySave = new QPushButton(tr("Save face"));
    layout->addWidget(mySave, 6, 1);
    connect(mySave, SIGNAL(clicked(bool)), this, SLOT(saveClicked(bool)));

    QPushButton* smooth = new QPushButton(tr("Make smooth base"), this);
    layout->addWidget(smooth, 6, 2, 1, 3);
    connect(smooth, SIGNAL(clicked(bool)), this, SLOT(makeSmooth(bool)));
}

static bool treasureContains(const treasure *t, const archetype *arch)
{
    while (t)
    {
        if (t->item == arch)
            return true;
        if (t->next_yes && treasureContains(t->next_yes, arch))
            return true;
        if (t->next_no && treasureContains(t->next_no, arch))
            return true;
        t = t->next;
    }
    return false;
}

static bool isValidArchFlesh(const archetype *arch, const Face *fleshFace)
{
    if (!arch || !arch->clone.randomitems)
        return false;

    std::vector<const archetype *> sources;
    getManager()->archetypes()->each([&sources, &fleshFace] (const archetype *candidate) {
        if (candidate->clone.face == fleshFace)
            sources.push_back(candidate);
    });

    for (auto source : sources)
    {
        if (treasureContains(arch->clone.randomitems->items, source))
            return true;
    }

    return false;
}

void CREFacePanel::setItem(const Face* face)
{
    Q_ASSERT(face);
    myFace = face;

    myUsing->clear();

    QTreeWidgetItem* root = NULL;

    getManager()->archetypes()->each([this, &root] (archetype *arch)
    {
      auto key = object_get_value(&arch->clone, "identified_face");
        if (arch->clone.face == myFace || (key && strcmp(myFace->name, key) == 0))
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

    size_t pos = 0;
    std::string name(face->name);
    while ((pos = name.find('_', pos)) != std::string::npos)
    {
        auto arch = getManager()->archetypes()->find(name.substr(0, pos));
        auto flesh = getManager()->faces()->find(name.substr(pos + 1));
        if (isValidArchFlesh(arch, flesh))
        {
            if (root == NULL)
            {
                root = CREUtils::archetypeNode(NULL);
                myUsing->addTopLevelItem(root);
                root->setExpanded(true);
            }
            auto node = CREUtils::archetypeNode(arch, root);
            node->setText(0, node->text(0) + " (flesh face)");
        }
        pos++;
    }

    root = NULL;

    getManager()->animations()->each([this, &root] (Animations *anim)
    {
        for (int face = 0; face < anim->num_animations; face++)
        {
            if (anim->faces[face] == myFace)
            {
                if (root == NULL)
                {
                    root = CREUtils::animationNode(NULL);
                    myUsing->addTopLevelItem(root);
                    root->setExpanded(true);
                }
                CREUtils::animationNode(anim, root);
                break;
            }
        }
    });

    root = NULL;

    const artifactlist* list;
    const artifact* arti;

    for (list = first_artifactlist; list; list = list->next)
    {
        for (arti = list->items; arti; arti = arti->next)
        {
            if (arti->item->face == myFace)
            {
                if (!root)
                {
                    root = CREUtils::artifactNode(NULL);
                    myUsing->addTopLevelItem(root);
                    root->setExpanded(true);
                }

                CREUtils::artifactNode(arti, root);
            }
        }
    }

    root = NULL;
    getManager()->faces()->each([this, &root] (const Face *face) {
        if (face->smoothface == myFace)
        {
            if (!root)
            {
                root = CREUtils::faceNode(NULL);
                root->setText(0, root->text(0) + " (as smoothed face)");
                myUsing->addTopLevelItem(root);
                root->setExpanded(true);
            }

            CREUtils::faceNode(face, root);
        }
    });

    root = NULL;
    getManager()->messages()->each([this, &root] (const GeneralMessage *message)
    {
       if (myFace == message->face)
       {
           if (!root)
           {
               root = CREUtils::generalMessageNode();
               myUsing->addTopLevelItem(root);
               root->setExpanded(true);
           }

           CREUtils::generalMessageNode(message, root);
       }
    });

    root = NULL;
    getManager()->quests()->each([&] (auto quest) {
        if (myFace == quest->face)
        {
            if (!root)
            {
                root = CREUtils::questsNode();
                myUsing->addTopLevelItem(root);
                root->setExpanded(true);
            }

            CREUtils::questNode(quest, root);
        }
    });

    root = NULL;
    auto maps = myMaps->getFaceUse(myFace);
    for (const auto map : maps)
    {
        if (!root)
        {
            root = CREUtils::mapNode(nullptr);
            myUsing->addTopLevelItem(root);
            root->setExpanded(true);
        }

        CREUtils::mapNode(map, root);
    }

    myColor->setCurrentIndex(myFace->magicmap);

    myLicenses->clear();

    auto licenses = LicenseManager::get()->getForFace(myFace->name);
    for (auto l : licenses) {
        QTreeWidgetItem *wi = new QTreeWidgetItem(QStringList(QString(l.first.c_str())));
        for (auto p : l.second) {
            auto twi = new QTreeWidgetItem(wi, QStringList(p.first.c_str()) << p.second.c_str());
            twi->setToolTip(1, p.second.c_str());
        }
        myLicenses->addTopLevelItem(wi);
        wi->setExpanded(true);
    }
}
void CREFacePanel::saveClicked(bool)
{
}

void CREFacePanel::makeSmooth(bool)
{
    CRESmoothFaceMaker maker;
    maker.setSelectedFace(myFace);
    maker.setAutoClose();
    maker.exec();
}
