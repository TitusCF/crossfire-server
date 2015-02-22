#ifndef CRESCRIPTPANEL_H
#define	CRESCRIPTPANEL_H

#include <QWidget>
#include "CREPanel.h"

class ScriptFile;

class CREScriptPanel : public CREPanel
{
    public:
        CREScriptPanel();

        void setScript(ScriptFile* script);

    private:
      QTreeWidget* myMaps;
};

#endif	/* CRESCRIPTPANEL_H */

