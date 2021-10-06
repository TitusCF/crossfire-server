#ifndef _CREPREPOSTPANEL_H
#define _CREPREPOSTPANEL_H

#include <QWidget>
#include <QObject>
#include <QtWidgets>

#include "MessageManager.h"
#include "CREPrePostList.h"

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
 * Post-condition panel displaying a connection (number).
 */
class CRESubItemConnection : public CRESubItemWidget
{
    Q_OBJECT

    public:
        CRESubItemConnection(QWidget* parent);

        virtual void setData(const QStringList& data);

    private:
        QLineEdit* myEdit;
        QLabel* myWarning;

        void showWarning(const QString& warning);

    private slots:
        void editChanged(const QString& text);
};

/**
 * Pre- or post- conditions panel displaying a quest step
 */
class CRESubItemQuest : public CRESubItemWidget
{
    Q_OBJECT

    public:
        CRESubItemQuest(CREPrePostList::Mode mode, QWidget* parent);

        virtual void setData(const QStringList& data);

    private:
        CREPrePostList::Mode myMode;
        /** List of quests. */
        QComboBox* myQuestList;
        /** Steps of the current quest for new step (post-) or at/frop step (pre-). */
        QComboBox* myFirstStep;
        /** Steps of the current quest for up to step (pre-). */
        QComboBox* mySecondStep;
        QRadioButton* myAtStep;
        QRadioButton* myBelowStep;  /**< For quest condition. */
        QRadioButton* myFromStep;
        QRadioButton* myStepRange;
        bool myInit;

        void fillQuestSteps();
        void updateData();

    private slots:
        void selectedQuestChanged(int index);
        void checkToggled(bool checked);
        void selectedStepChanged(int index);
};

/**
 * Pre- or post- conditions panel displaying a token, either as read or write.
 */
class CRESubItemToken : public CRESubItemWidget
{
    Q_OBJECT

    public:
        CRESubItemToken(bool isPre, QWidget* parent);

        virtual void setData(const QStringList& data);

    private:
        QLineEdit* myToken;
        QLineEdit* myValue;
        QTextEdit* myValues;

        void updateData();

    private slots:
        void tokenChanged(const QString&);
        void valuesChanged();
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
        void addItem(const QString& item);
        QStringList data() const;

        /** For one condition, arguments to the script. */
        QListWidget* mySubItems;

    private slots:
        void endEdition(QWidget* editor, QAbstractItemDelegate::EndEditHint hint);
        void onAddSubItem(bool);
        void onDeleteSubItem(bool);
};

/**
 * Edition of a single pre- or post- condition for a message, or a quest step list for a quest step.
 */
class CREPrePostPanel : public QDialog
{
    Q_OBJECT

    public:
        /**
         * Standard constructor.
         * @param mode what mode to work on.
         * @param scripts available script types for the conditions.
         * @param parent ancestor of this panel.
         */
        CREPrePostPanel(CREPrePostList::Mode mode, const QList<QuestConditionScript*> scripts, QWidget* parent);
        virtual ~CREPrePostPanel();

        QStringList getData();
        void setData(const QStringList& data);

    private:
        /** Pre- or post- condition we're working on. */
        QStringList myData;
        /** Original data, to reset the state. */
        QStringList myOriginal;
        /** Available conditions types. */
        QComboBox* myChoices;
        /** Matching between index of myChoices and the variable subpanels. */
        QList<CRESubItemWidget*> mySubWidgets;
        /** Arguments panels, only one visible based on the choice. */
        QStackedWidget* mySubItemsStack;
        /** Mode we're working in. */
        CREPrePostList::Mode myMode;

        /**
         * Creates a CRESubItemWidget for the specified script.
         * @param script the script to create the display for.
         * @return specialised CRESubItemWidget if available, CRESubItemList else.
         */
        CRESubItemWidget* createSubItemWidget(const QuestConditionScript* script);

    private slots:
        void currentChoiceChanged(int index);
        void subItemChanged(const QStringList& data);
        void onReset(bool);
};

#endif /* _CREPREPOSTPANEL_H */
