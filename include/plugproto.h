#ifndef PLUGPROTO_H_
#define PLUGPROTO_H_

f_plugin HookList[NR_OF_HOOKS] =
{
    NULL,
    CFWLog,
    CFWNewInfoMap,
    CFWSpringTrap,
    CFWCastSpell,
    CFWCmdRSkill,
    CFWBecomeFollower,
    CFWPickup,
    CFWGetMapObject,
    CFWESRVSendItem,
    CFWFindPlayer,
    CFWManualApply,
    CFWCmdDrop,
    CFWCmdTake,
    CFWCmdTitle,
    CFWTransferObject,
    CFWKillObject,
    CFWDoLearnSpell,
    CFWDoForgetSpell,
    CFWCheckSpellKnown,
    CFWESRVSendInventory,
    CFWCreateArtifact,
    CFWGetArchetype,
    CFWUpdateSpeed,
    CFWUpdateObject,
    CFWFindAnimation,
    CFWGetArchetypeByObjectName,
    CFWInsertObjectInMap,
    CFWReadyMapName,
    CFWAddExp,
    CFWDetermineGod,
    CFWFindGod,
    RegisterGlobalEvent,
    UnregisterGlobalEvent,
    CFWDumpObject,
    CFWLoadObject,
    CFWRemoveObject,
    CFWAddString,
    CFWFreeString,
    CFWAddRefcount,
    CFWGetFirstMap,
    CFWGetFirstPlayer,
    CFWGetFirstArchetype,
    CFWQueryCost,
    CFWQueryMoney,
    CFWPayForItem,
    CFWPayForAmount,
    CFWNewDrawInfo,
    CFWSendCustomCommand,
    CFWCFTimerCreate,
    CFWCFTimerDestroy,
    CFWMovePlayer,
    CFWMoveObject,
    CFWSetAnimation,
    CFWCommunicate,
    CFWFindBestObjectMatch,
    CFWApplyBelow,
    CFWFreeObject,
    CFWObjectCreateClone,
    CFWTeleportObject

};

#endif /*PLUGPROTO_H_*/
