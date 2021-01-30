#include "FaceMakerDialog.h"
#include <QtWidgets>
#include <QtCore/qiodevice.h>
#include <QtCore/qfile.h>
#include <QMap>

#include "ResourcesManager.h"
#include "CREPixmap.h"

extern "C" {
#include "global.h"
}

FaceMakerDialog::FaceMakerDialog(QWidget* parent, ResourcesManager* manager) : QDialog(parent), myManager(manager)
{    
    QGridLayout* layout = new QGridLayout(this);
    int line = 0;

    layout->addWidget(new QLabel(tr("Settings:"), this), line++, 0);

    mySettings = new QTextEdit(this);
    layout->addWidget(mySettings, line++, 0);
    mySettings->setAcceptRichText(false);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, Qt::Horizontal, this);
    layout->addWidget(box, line++, 0);
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));
    connect(box, SIGNAL(accepted()), this, SLOT(makeFaces()));
    box->button(QDialogButtonBox::Ok)->setText("Generate");

    resize(500, 600);

    setWindowTitle(tr("Face variant maker"));

    mySettings->setPlainText(tr(R"RaW(dest: %1/arch/wall/light_blue_cave/
name: light_blue_cave
faces: cave_15.111, cave1.111, cave2.111, cave3.111, cave4.111, cave5.111
faces: cave6.111, cave7.111, cave8.111, cave9.111, cave10.111, cave11.111
faces: cave12.111, cave13.111, cave14.111, cave15.111, cave16.111, cave17.111
faces: cave18.111, cave19.111, cave20.111, cave21.111, cave22.111, cave23.111
faces: cave24.111, cave25.111
variants: 2
68, 68, 68: 4, 5, 65
119, 119, 119:14, 229, 197; 67, 151, 229
arch
name wall
anim_speed 4
client_anim_random 1
blocksview 1
move_block all
no_pick 1
archend
face
magicmap blue
visibility 100
faceend
)RaW").arg(settings.datadir));
}

QColor FaceMakerDialog::parse(const QString& color)
{
    auto split = color.trimmed().split(',');
    if (split.size() != 3) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid color %1").arg(color));
        return QColor();
    }
    return QColor::fromRgb(split[0].trimmed().toInt(), split[1].trimmed().toInt(), split[2].trimmed().toInt());
}

void FaceMakerDialog::makeFaces()
{
    QMap<QRgb, QList<QColor> > colors;
    QString dest, name, archContent, faceContent;
    QStringList faces;
    int variants = 1;

    bool inArch = false, inFace = false;
    auto lines = mySettings->toPlainText().split('\n');

    foreach (QString line, lines) {
        if (inArch) {
            if (line == "archend") {
                inArch = false;
            } else {
                archContent += line + "\n";
            }
            continue;
        }
        if (inFace) {
            if (line == "faceend") {
                inFace = false;
            } else {
                faceContent += line + "\n";
            }
            continue;
        }

        if (line.isEmpty()) {
            continue;
        }

        if (line.startsWith("dest:")) {
            dest = line.mid(5).trimmed();
        } else if (line.startsWith("name:")) {
            name = line.mid(5).trimmed();
        } else if (line.startsWith("faces:")) {
            QStringList add(line.mid(6).split(','));
            foreach (QString face, add) {
                faces.append(face.trimmed());
            }
        } else if (line.startsWith("variants:")) {
            variants = line.mid(9).trimmed().toInt();
        } else if (line == "arch") {
            inArch = true;
        } else if (line == "face") {
            inFace = true;
        } else {
            auto split = line.split(":");
            if (split.size() != 2) {
                QMessageBox::critical(this, tr("Error"), tr("Invalid line %1").arg(line));
                return;
            }
            QColor source = parse(split[0]);
            auto dest = split[1].split(';');
            foreach (QString d, dest) {
                colors[source.rgba()].append(parse(d));
            }
        }
    }

    if (dest.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Missing 'dest'!"));
        return;
    }
    if (name.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Missing 'name'!"));
        return;
    }
    if (faces.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No face to process!"));
        return;
    }

    if (variants <= 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("At least one variant required!"));
        return;
    }

    QFile arc(dest + QDir::separator() + name + ".arc");
    if (!archContent.isEmpty() && !arc.open(QFile::Truncate | QFile::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error while opening the archetype file %1!").arg(arc.fileName()));
        return;
    }
    QFile fface(dest + QDir::separator() + name + ".face");
    if ((!faceContent.isEmpty() || variants > 1) && !fface.open(QFile::Truncate | QFile::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error while opening the face file %1!").arg(fface.fileName()));
        return;
    }

    int i = 0;
    foreach(QString face, faces)
    {
        QString archName(name + "_" + QString::number(i));
        if (!archContent.isEmpty()) {
            QString item(tr("Object %1\nface %1.111\n").arg(archName));
            if (variants > 1) {
                item += tr("animation %1\n").arg(archName);
            }
            item += archContent;
            item += "end\n";
            arc.write(item.toLocal8Bit());
        }

        QString anim(tr("animation %1\n").arg(archName));

        int faceNumber = myManager->face(face)->number;

        for (int var = 1; var <= variants; var++) {
            QImage image;

            if (faceNumber >= 0 && CREPixmap::faceset->allocated >= static_cast<size_t>(faceNumber) && CREPixmap::faceset->faces[faceNumber].datalen > 0)
            {
                QPixmap face;
                if (face.loadFromData((uchar*)CREPixmap::faceset->faces[faceNumber].data, CREPixmap::faceset->faces[faceNumber].datalen))
                {
                    image = face.toImage();
                }
            }

            for (int x = 0; x < image.width(); x++) {
                for (int y = 0; y < image.height(); y++) {
                    auto rgba = image.pixel(x, y);
                    auto subs = colors.find(rgba);
                    if (subs != colors.end()) {
                        image.setPixelColor(x, y, QColor(subs.value()[rndm(0, subs.value().length() - 1)]));
                    }
                }
            }

            QString base = dest + QDir::separator() + name + "_" + QString::number(i) + ".base.11" + QString::number(var) + ".png";
            image.save(base, "PNG");

            if (!faceContent.isEmpty()) {
                QString fc(tr("face %1\n%2end\n").arg(archName + ".11" + QString::number(var), faceContent));
                fface.write(fc.toLocal8Bit());
            }
            anim += tr("%1.11%2\n").arg(archName).arg(var);
        }
        anim += "mina\n";
        if (variants > 1) {
            fface.write(anim.toLocal8Bit());
        }
        i++;
    }

    QMessageBox::information(this, tr("Completed"), tr("Generation completed"));
}
