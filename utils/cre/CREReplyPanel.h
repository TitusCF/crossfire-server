#ifndef _CREREPLYPANEL_H
#define	_CREREPLYPANEL_H

#include <QWidget>

class QTreeWidget;
class QLineEdit;
class QComboBox;
class QTreeWidgetItem;

class CREReplyPanel : public QWidget
{
    Q_OBJECT

    public:
        CREReplyPanel(QWidget* parent);
        virtual ~CREReplyPanel();

        void setData(const QList<QStringList>& data);
        QList<QStringList> getData();

    signals:
        void dataModified();

    private:
        QList<QStringList> myData;
        QTreeWidget* myReplies;
        QLineEdit* myText;
        QLineEdit* myMessage;
        QComboBox* myType;

        void setText(QTreeWidgetItem* item, QStringList data);
        void updateItem();

    private slots:
        void currentReplyChanged(QTreeWidgetItem*, QTreeWidgetItem*);
        void onAddItem(bool);
        void onDeleteItem(bool);
        void onTextChanged(const QString&);
        void onTypeChanged(int);
};

#endif	/* _CREREPLYPANEL_H */
