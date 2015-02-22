#ifndef _CREMAPPANEL_H
#define _CREMAPPANEL_H

#include <QObject>
#include <QtGui>
#include "CREPanel.h"

class CREMapInformation;
class ScriptFileManager;

class CREMapPanel : public CREPanel
{
    public:
        CREMapPanel(ScriptFileManager* manager);
        virtual ~CREMapPanel();

        void setMap(CREMapInformation* map);

    protected:
        ScriptFileManager* myManager;
        QLabel* myName;
        QTreeWidget* myExitsTo;
        QTreeWidget* myExitsFrom;
        QTreeWidget* myScripts;
};

#endif /* _CREMAPPANEL_H */
