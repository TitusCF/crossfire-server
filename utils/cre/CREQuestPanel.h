#ifndef _CREQUESTPANEL_H
#define	_CREQUESTPANEL_H

#include <QWidget>
#include <QtGui>

#include "CREFilterDialog.h"
#include "CREReportDialog.h"
#include "CREPanel.h"

class Quest;

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
        QLineEdit* myCode;
        QLineEdit* myTitle;
        QTextEdit* myDescription;
        QTreeWidget* mySteps;
};

#endif	/* _CREQUESTPANEL_H */

