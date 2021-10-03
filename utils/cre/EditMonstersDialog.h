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
};

#endif /* EDITMONSTERSDIALOG_H */
