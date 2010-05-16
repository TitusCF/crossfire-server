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
        QTextEdit* myDescription;
        QListWidget* mySteps;
        QLineEdit* myStep;
        QTextEdit* myStepDescription;
        QCheckBox* myStepEnd;

        void commitStep();
        void displaySteps();

    private slots:
        void stepChanged(int newStep);
        void onAddStep(bool);
        void onDeleteStep(bool);
};

#endif	/* _CREQUESTPANEL_H */

