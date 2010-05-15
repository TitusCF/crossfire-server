#ifndef _CREQUESTPANEL_H
#define	_CREQUESTPANEL_H

#include <QWidget>
#include <QtGui>

#include "CREFilterDialog.h"
#include "CREReportDialog.h"
#include "CREPanel.h"

class Quest;
class QuestStep;

class CREQuestPanel : public CREPanel
{
    Q_OBJECT

    public:
        CREQuestPanel();
        virtual ~CREQuestPanel();

        virtual void commitData();

        void setQuest(Quest* quest);
    private:
        Quest* myQuest;
        QuestStep* myCurrentStep;
        QLineEdit* myCode;
        QLineEdit* myTitle;
        QTextEdit* myDescription;
        QListWidget* mySteps;
        QLineEdit* myStep;
        QTextEdit* myStepDescription;
        QCheckBox* myStepEnd;

        void commitStep();

    private slots:
        void stepChanged(int newStep);
};

#endif	/* _CREQUESTPANEL_H */

