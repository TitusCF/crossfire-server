#ifndef _CREMAPPANEL_H
#define _CREMAPPANEL_H

#include <QObject>
#include <QtGui>
#include "CREPanel.h"

class CREMapInformation;
class ScriptFileManager;

class CREMapPanel : public CRETPanel<CREMapInformation>
{
    public:
        CREMapPanel(ScriptFileManager* manager);
        virtual ~CREMapPanel();

        virtual void setItem(CREMapInformation* map);

    protected:
        ScriptFileManager* myManager;
        QLabel* myName;
        QTreeWidget* myExitsTo;
        QTreeWidget* myExitsFrom;
        QTreeWidget* myScripts;
};

#endif /* _CREMAPPANEL_H */
