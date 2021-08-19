#ifndef EDITMONSTERSDIALOG_H
#define EDITMONSTERSDIALOG_H

#include <QDialog>

class ArchetypesModel;
class ResourcesManager;
struct archt;

class EditMonstersDialog : public QDialog {
    Q_OBJECT

public:
    EditMonstersDialog(ResourcesManager *resources);

protected slots:
    void onSaveChanges();

private:
    void write(const QString &filename, QList<const archt*> archs);

    ArchetypesModel *myModel;
    ResourcesManager *myResources;
};

#endif /* EDITMONSTERSDIALOG_H */
