#ifndef _CREPREPOSTPANEL_H
#define	_CREPREPOSTPANEL_H

#include <QWidget>

#include "MessageManager.h"

class QListWidget;
class QComboBox;
class QLineEdit;
class QuestConditionScript;

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
        /** For one condition, arguments to the script. */
        QListWidget* mySubItems;
        /** Argument edit zone. */
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

