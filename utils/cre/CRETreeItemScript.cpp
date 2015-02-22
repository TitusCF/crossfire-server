#include "CRETreeItemScript.h"
#include "CREScriptPanel.h"

CRETreeItemScript::CRETreeItemScript(ScriptFile* script)
{
    myScript = script;
}

void CRETreeItemScript::fillPanel(QWidget* panel)
{
    Q_ASSERT(myScript);
    CREScriptPanel* p = static_cast<CREScriptPanel*>(panel);
    p->setScript(myScript);
}
