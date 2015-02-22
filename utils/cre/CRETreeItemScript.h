#ifndef CRETREEITEMSCRIPT_H
#define	CRETREEITEMSCRIPT_H

#include "CRETreeItem.h"

class ScriptFile;

class CRETreeItemScript : public CRETreeItem
{
    public:
        CRETreeItemScript(ScriptFile* script);

        virtual QString getPanelName() const { return "Script"; }
        virtual void fillPanel(QWidget* panel);

    private:
        ScriptFile* myScript;
};

#endif	/* CRETREEITEMSCRIPT_H */

