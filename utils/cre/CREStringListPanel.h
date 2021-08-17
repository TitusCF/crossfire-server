#ifndef _CRESTRINGLISTPANEL_H
#define _CRESTRINGLISTPANEL_H

#include <QDialog>

class QListWidget;

/**
 * Display and allow edition of a list of multiline strings.
 */
class CREStringListPanel : public QDialog
{
    Q_OBJECT

    public:
        CREStringListPanel(QWidget* parent);
        virtual ~CREStringListPanel();

        void setData(const QStringList& list);
        QStringList data() const;

    private:
        QListWidgetItem* createItem(const QString& text);

        QListWidget* myItems;
        QStringList myOriginal;

    protected slots:
        void onAddItem(bool);
        void onDeleteItem(bool);
        void onReset(bool);
};

#endif /* _CRESTRINGLISTPANEL_H */
