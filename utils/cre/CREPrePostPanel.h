#ifndef _CREPREPOSTPANEL_H
#define	_CREPREPOSTPANEL_H

#include <QWidget>

class QListWidget;
class QComboBox;
class QLineEdit;

class CREPrePostPanel : public QWidget
{
    Q_OBJECT

    public:
        CREPrePostPanel(const QStringList& choices, QWidget* parent);
        virtual ~CREPrePostPanel();

        QList<QStringList> getData();
        void setData(const QList<QStringList> data);

    signals:
        void dataModified();

    private:
        QList<QStringList> myData;
        QListWidget* myItems;
        QComboBox* myChoices;
        QListWidget* mySubItems;
        QLineEdit* myItemEdit;

    private slots:
        void onAddItem(bool);
        void onDeleteItem(bool);
        void onAddSubItem(bool);
        void onDeleteSubItem(bool);
        void currentItemChanged(int index);
        void currentSubItemChanged(int);
        void currentChoiceChanged(int index);
        void subItemChanged(const QString& text);
};

#endif	/* _CREPREPOSTPANEL_H */

