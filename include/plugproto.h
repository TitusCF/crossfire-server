#ifndef PLUGPROTO_H_
#define PLUGPROTO_H_

CFParm* CFWLog(CFParm* PParm);
CFParm* CFWNewInfoMap(CFParm* PParm);
CFParm* CFWSpringTrap(CFParm* PParm);
CFParm* CFWCastSpell(CFParm* PParm);
CFParm* CFWCmdRSkill(CFParm* PParm);
CFParm* CFWBecomeFollower(CFParm* PParm);
CFParm* CFWPickup(CFParm* PParm);
CFParm* CFWGetMapObject(CFParm* PParm);
CFParm* CFWESRVSendItem(CFParm* PParm);
CFParm* CFWFindPlayer(CFParm* PParm);
CFParm* CFWManualApply(CFParm* PParm);
CFParm* CFWCmdDrop(CFParm* PParm);
CFParm* CFWCmdTake(CFParm* PParm);
CFParm* CFWCmdTitle(CFParm* PParm);
CFParm* CFWTransferObject(CFParm* PParm);
CFParm* CFWKillObject(CFParm* PParm);
CFParm* CFWDoForgetSpell(CFParm* PParm);
CFParm* CFWDoLearnSpell(CFParm* PParm);
CFParm* CFWCheckSpellKnown(CFParm* PParm);
CFParm* CFWESRVSendInventory(CFParm* PParm);
CFParm* CFWCreateArtifact(CFParm* PParm);
CFParm* CFWGetArchetype(CFParm* PParm);
CFParm* CFWUpdateSpeed(CFParm* PParm);
CFParm* CFWUpdateObject(CFParm* PParm);
CFParm* CFWFindAnimation(CFParm* PParm);
CFParm* CFWGetArchetypeByObjectName(CFParm* PParm);
CFParm* CFWInsertObjectInMap(CFParm* PParm);
CFParm* CFWReadyMapName(CFParm* PParm);
CFParm* CFWAddExp(CFParm* PParm);
CFParm* CFWDetermineGod(CFParm* PParm);
CFParm* CFWFindGod(CFParm* PParm);
CFParm* CFWDumpObject(CFParm* PParm);
CFParm* CFWLoadObject(CFParm* PParm);
CFParm* CFWRemoveObject(CFParm* PParm);

CFParm* RegisterGlobalEvent(CFParm* PParm);
CFParm* UnregisterGlobalEvent(CFParm* PParm);

#endif /*PLUGPROTO_H_*/
