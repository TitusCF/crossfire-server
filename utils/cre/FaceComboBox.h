#ifndef FACECOMBOBOX_H
#define FACECOMBOBOX_H

#include <QtWidgets>

class Face;

/**
 * Combobox displaying a list of faces, allowing selection and search.
 */
class FaceComboBox : public QComboBox
{
    Q_OBJECT

 public:
    /**
     * Standard constructor.
     * @param parent ancestor of this widget.
     * @param allowNone whether to allow "(none)" face.
     */
    FaceComboBox(QWidget* parent, bool allowNone);
    virtual ~FaceComboBox();

    /**
     * Set the selected face.
     * @param face face to select.
     */
    void setFace(const Face* face);
    /**
     * Get the selected face.
     * @return selected face, nullptr if no face is selected.
     */
    const Face* face() const;
};

#endif /* FACECOMBOBOX_H */
