#ifndef _CREQUESTPANEL_H
#define _CREQUESTPANEL_H

#include <QWidget>
#include <QtWidgets>

#include "CREFilterDialog.h"
#include "CREReportDialog.h"
#include "CREPanel.h"

extern "C" {
#include "global.h"
#include "quest.h"
}

class Quest;
class QuestStep;
class CREMapInformationManager;
class CREQuestItemModel;
class MessageManager;
class ResourcesManager;
class FaceComboBox;

class CREQuestPanel : public CRETPanel<quest_definition>
{
    Q_OBJECT

    public:
        CREQuestPanel(CREMapInformationManager* mapManager, MessageManager* messageManager, ResourcesManager *resources, QWidget* parent);
        virtual ~CREQuestPanel();

        virtual void commitData();

        virtual void setItem(quest_definition *quest);
    private:
        CREMapInformationManager *myMapManager;
        MessageManager* myMessageManager;
        ResourcesManager *myResources;
        quest_definition *myQuest;
        QuestStep* myCurrentStep;
        QLineEdit* myCode;
        QLineEdit* myTitle;
        FaceComboBox* myFace;
        QComboBox* myFile;
        QCheckBox* myCanRestart;
        QCheckBox* myIsSystem;
        QComboBox* myParent;
        QTextEdit* myDescription;
        QPlainTextEdit* myComment;
        CREQuestItemModel* myStepsModel;
        QTreeView* mySteps;
        QTreeWidget* myUse;

        void displaySteps();

    protected slots:
        void deleteStep(bool);
        void moveUp(bool);
        void moveDown(bool);
};

#endif /* _CREQUESTPANEL_H */
