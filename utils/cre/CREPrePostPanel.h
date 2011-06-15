#ifndef _CREPREPOSTPANEL_H
#define	_CREPREPOSTPANEL_H

#include <QWidget>

#include "MessageManager.h"

class QListWidget;
class QComboBox;
class QLineEdit;
class QuestConditionScript;

/**
 * Base class for a pre- or post- panel displaying script arguments.
 */
class CRESubItemWidget : public QWidget
{
    Q_OBJECT

    public:
        CRESubItemWidget(QWidget* parent) : QWidget(parent) { };

        virtual void setData(const QStringList& data) = 0;

    signals:
        void dataModified(const QStringList& data);
};

/**
 * Pre- or post- panel displaying script arguments as a string list.
 */
class CRESubItemList : public CRESubItemWidget
{
    Q_OBJECT

    public:
        CRESubItemList(QWidget* parent);
        void setData(const QStringList& data);

    private:
        /** For one condition, arguments to the script. */
        QListWidget* mySubItems;
        /** Argument edit zone. */
        QLineEdit* myItemEdit;
        /** Current arguments. */
        QStringList myData;

    private slots:
        void currentSubItemChanged(int);
        void subItemChanged(const QString& text);
        void onAddSubItem(bool);
        void onDeleteSubItem(bool);
};

/**
 * This panel is the 'pre' or 'post' subpanel in the messages panel.
 */
class CREPrePostPanel : public QWidget
{
    Q_OBJECT

    public:
        /**
         * Standard constructor.
         * @param scripts available script types for the conditions.
         * @param parent ancestor of this panel.
         */
        CREPrePostPanel(const QList<QuestConditionScript*> scripts, QWidget* parent);
        virtual ~CREPrePostPanel();

        QList<QStringList> getData();
        void setData(const QList<QStringList> data);

    signals:
        /** Emitted when the data this panel manages was changed. */
        void dataModified();

    private:
        /** Pre- or post- conditions we're working on. */
        QList<QStringList> myData;
        /** The first item of each condition. */
        QListWidget* myItems;
        /** Available conditions types. */
        QComboBox* myChoices;
        /** Matching between index of myChoices and the variable subpanels. */
        QList<CRESubItemWidget*> mySubWidgets;
        QStackedWidget* mySubItemsStack;

    private slots:
        void onAddItem(bool);
        void onDeleteItem(bool);
        void currentItemChanged(int index);
        void currentChoiceChanged(int index);
        void subItemChanged(const QStringList& data);
};

#endif	/* _CREPREPOSTPANEL_H */

