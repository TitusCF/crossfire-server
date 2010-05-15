#ifndef _CREQUESTPANEL_H
#define	_CREQUESTPANEL_H

#include <QWidget>
#include <QtGui>

#include "CREFilterDialog.h"
#include "CREReportDialog.h"

class Quest;

class CREQuestPanel : public QWidget
{
    Q_OBJECT

    public:
        CREQuestPanel();
        virtual ~CREQuestPanel();

        void setQuest(const Quest* quest);
    private:
        QLineEdit* myCode;
        QLineEdit* myTitle;
        QTextEdit* myDescription;
        QTreeWidget* mySteps;
};

#endif	/* _CREQUESTPANEL_H */

