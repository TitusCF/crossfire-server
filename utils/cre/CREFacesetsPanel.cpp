#include <QtWidgets>

#include "CREFacesetsPanel.h"

#include "assets.h"
#include "AssetsManager.h"

CREFacesetsPanel::CREFacesetsPanel(QWidget* parent) : CRETPanel(parent)
{
    QGridLayout* layout = new QGridLayout(this);

    int line = 0;

    layout->addWidget(new QLabel(tr("Prefix:"), this), 0, 0);
    myPrefix = new QLabel(this);
    layout->addWidget(myPrefix, line++, 1);

    layout->addWidget(new QLabel(tr("Full name:"), this), line, 0);
    myFullname = new QLabel(this);
    layout->addWidget(myFullname, line++, 1);

    layout->addWidget(new QLabel(tr("Fallback:"), this), line, 0);
    myFallback = new QLabel(this);
    layout->addWidget(myFallback, line++, 1);

    layout->addWidget(new QLabel(tr("Size:"), this), line, 0);
    mySize = new QLabel(this);
    layout->addWidget(mySize, line++, 1);

    layout->addWidget(new QLabel(tr("Extension:"), this), line, 0);
    myExtension = new QLabel(this);
    layout->addWidget(myExtension, line++, 1);

    layout->addWidget(new QLabel(tr("Defined faces:"), this), line, 0);
    myImages = new QLabel(this);
    layout->addWidget(myImages, line++, 1);

    QWidget *bottomFiller = new QWidget(this);
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(bottomFiller, line++, 0, 1, 2);
}

CREFacesetsPanel::~CREFacesetsPanel()
{
}

void CREFacesetsPanel::setItem(face_sets* fs)
{
    myPrefix->setText(fs->prefix);
    myFullname->setText(fs->fullname);
    myFallback->setText(fs->fallback ? QString("%1 (%2)").arg(fs->fallback->prefix, fs->fallback->fullname) : "(none)");
    mySize->setText(fs->size);
    myExtension->setText(fs->extension);

    size_t count = 0, total = getManager()->faces()->count();
    for (size_t img = 0; img < total; img++)
    {
        if (img < fs->allocated && fs->faces[img].datalen > 0)
        {
            count++;
        }
    }

    uint8_t percent = count * 100 / total;
    if (percent == 100 && count < total)
        percent = 99;

    myImages->setText(QString("%1 out of %2 (%3%)").arg(count).arg(total).arg(percent));
}
