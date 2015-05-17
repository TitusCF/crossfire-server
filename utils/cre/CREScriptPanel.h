#ifndef CRESCRIPTPANEL_H
#define	CRESCRIPTPANEL_H

#include <QWidget>
#include "CREPanel.h"

class ScriptFile;

class CREScriptPanel : public CRETPanel<ScriptFile>
{
    public:
        CREScriptPanel();

        virtual void setItem(ScriptFile* script);

    private:
      QTreeWidget* myMaps;
};

#endif	/* CRESCRIPTPANEL_H */

