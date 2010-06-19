#ifndef _CREQUESTPANEL_H
#define	_CREQUESTPANEL_H

#include <QWidget>
#include <QtGui>

#include "CREFilterDialog.h"
#include "CREReportDialog.h"
#include "CREPanel.h"

class Quest;
class QuestStep;
class QuestManager;
class CREQuestItemModel;

class CREQuestPanel : public CREPanel
{
    Q_OBJECT

    public:
        CREQuestPanel(QuestManager* manager);
        virtual ~CREQuestPanel();

        virtual void commitData();

        void setQuest(Quest* quest);
    private:
        QuestManager* myQuestManager;
        Quest* myQuest;
        QuestStep* myCurrentStep;
        QLineEdit* myCode;
        QLineEdit* myTitle;
        QComboBox* myFile;
        QCheckBox* myCanRestart;
        QComboBox* myParent;
        QTextEdit* myDescription;
        CREQuestItemModel* myStepsModel;
        QTreeView* mySteps;

        void displaySteps();

    protected slots:
        void deleteStep(bool);
        void moveUp(bool);
        void moveDown(bool);
};

#endif	/* _CREQUESTPANEL_H */

