#include <version.h>
#include <global.h>
#include <guile/gh.h>
#include <object.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#include <../random_maps/random_map.h>
#include <../random_maps/rproto.h>

#define WHO ((object *)(gh_scm2long(who)))

void guile_init_type_functions()
{
  gh_new_procedure("color-black", Script_colorBlack,0,0,0);
  gh_new_procedure("color-dark-blue", Script_colorDarkBlue,0,0,0);
  gh_new_procedure("color-red", Script_colorRed,0,0,0);
  gh_new_procedure("color-dark-yellow", Script_colorDarkYellow,0,0,0);
  gh_new_procedure("color-light-blue", Script_colorLightBlue,0,0,0);
  gh_new_procedure("color-orange", Script_colorOrange,0,0,0);
  gh_new_procedure("color-dark-green", Script_colorDarkGreen,0,0,0);
  gh_new_procedure("color-light-green", Script_colorLightGreen,0,0,0);
  gh_new_procedure("color-grey", Script_colorGrey,0,0,0);
  gh_new_procedure("color-brown", Script_colorBrown,0,0,0);
  gh_new_procedure("color-yellow", Script_colorYellow,0,0,0);
  gh_new_procedure("color-light-yellow", Script_colorLightYellow,0,0,0);

  gh_new_procedure("attack-type-physical", Script_attackTypePhysical,0,0,0);
  gh_new_procedure("attack-type-magic", Script_attackTypeMagic,0,0,0);
  gh_new_procedure("attack-type-fire", Script_attackTypeFire,0,0,0);
  gh_new_procedure("attack-type-electricity", Script_attackTypeElectricity,0,0,0);
  gh_new_procedure("attack-type-cold", Script_attackTypeCold,0,0,0);
  gh_new_procedure("attack-type-confusion", Script_attackTypeConfusion,0,0,0);
  gh_new_procedure("attack-type-acid", Script_attackTypeAcid,0,0,0);
  gh_new_procedure("attack-type-drain", Script_attackTypeDrain,0,0,0);
  gh_new_procedure("attack-type-weaponmagic", Script_attackTypeWeaponmagic,0,0,0);
  gh_new_procedure("attack-type-ghosthit", Script_attackTypeGhosthit,0,0,0);
  gh_new_procedure("attack-type-poison", Script_attackTypePoison,0,0,0);
  gh_new_procedure("attack-type-slow", Script_attackTypeSlow,0,0,0);
  gh_new_procedure("attack-type-paralyze", Script_attackTypeParalyze,0,0,0);
  gh_new_procedure("attack-type-turn-undead", Script_attackTypeTurnUndead,0,0,0);
  gh_new_procedure("attack-type-fear", Script_attackTypeFear,0,0,0);
  gh_new_procedure("attack-type-cancellation", Script_attackTypeCancellation,0,0,0);
  gh_new_procedure("attack-type-depletion", Script_attackTypeDepletion,0,0,0);
  gh_new_procedure("attack-type-death", Script_attackTypeDeath,0,0,0);
  gh_new_procedure("attack-type-chaos", Script_attackTypeChaos,0,0,0);
  gh_new_procedure("attack-type-counterspell", Script_attackTypeCounterspell,0,0,0);
  gh_new_procedure("attack-type-godpower", Script_attackTypeGodpower,0,0,0);
  gh_new_procedure("attack-type-holy-word", Script_attackTypeHolyWord,0,0,0);
  gh_new_procedure("attack-type-blind", Script_attackTypeBlind,0,0,0);
  gh_new_procedure("type-player",  Script_typePlayer,0,0,0);
  gh_new_procedure("type-bullet",  Script_typeBullet,0,0,0);
  gh_new_procedure("type-rod",  Script_typeRod,0,0,0);
  gh_new_procedure("type-treasure",  Script_typeTreasure,0,0,0);
  gh_new_procedure("type-potion",  Script_typePotion,0,0,0);
  gh_new_procedure("type-food",  Script_typeFood,0,0,0);
  gh_new_procedure("type-poison",  Script_typePoison,0,0,0);
  gh_new_procedure("type-book",  Script_typeBook,0,0,0);
  gh_new_procedure("type-clock",  Script_typeClock,0,0,0);
  gh_new_procedure("type-fire-bullet",  Script_typeFBullet,0,0,0);
  gh_new_procedure("type-fireball",  Script_typeFBall,0,0,0);
  gh_new_procedure("type-lightning",  Script_typeLightning,0,0,0);
  gh_new_procedure("type-arrow",  Script_typeArrow,0,0,0);
  gh_new_procedure("type-bow",  Script_typeBow,0,0,0);
  gh_new_procedure("type-weapon",  Script_typeWeapon,0,0,0);
  gh_new_procedure("type-armour",  Script_typeArmour,0,0,0);
  gh_new_procedure("type-pedestal",  Script_typePedestal,0,0,0);
  gh_new_procedure("type-altar",  Script_typeAltar,0,0,0);
  gh_new_procedure("type-confusion",  Script_typeConfusion,0,0,0);
  gh_new_procedure("type-locked-door",  Script_typeLockedDoor,0,0,0);
  gh_new_procedure("type-special-key",  Script_typeSpecialKey,0,0,0);
  gh_new_procedure("type-map",  Script_typeMap,0,0,0);
  gh_new_procedure("type-door",  Script_typeDoor,0,0,0);
  gh_new_procedure("type-key",  Script_typeKey,0,0,0);
  gh_new_procedure("type-magic-missile",  Script_typeMMissile,0,0,0);
  gh_new_procedure("type-timed-gate",  Script_typeTimedGate,0,0,0);
  gh_new_procedure("type-trigger",  Script_typeTrigger,0,0,0);
  gh_new_procedure("type-grim-reaper",  Script_typeGrimReaper,0,0,0);
  gh_new_procedure("type-magic-ear",  Script_typeMagicEar,0,0,0);
  gh_new_procedure("type-trigger-button",  Script_typeTriggerButton,0,0,0);
  gh_new_procedure("type-trigger-altar",  Script_typeTriggerAltar,0,0,0);
  gh_new_procedure("type-trigger-pedestal",  Script_typeTriggerPedestal,0,0,0);
  gh_new_procedure("type-shield",  Script_typeShield,0,0,0);
  gh_new_procedure("type-helmet",  Script_typeHelmet,0,0,0);
  gh_new_procedure("type-horn",  Script_typeHorn,0,0,0);
  gh_new_procedure("type-money",  Script_typeMoney,0,0,0);
  gh_new_procedure("type-class",  Script_typeClass,0,0,0);
  gh_new_procedure("type-gravestone",  Script_typeGravestone,0,0,0);
  gh_new_procedure("type-amulet",  Script_typeAmulet,0,0,0);
  gh_new_procedure("type-player-mover",  Script_typePlayerMover,0,0,0);
  gh_new_procedure("type-teleporter",  Script_typeTeleporter,0,0,0);
  gh_new_procedure("type-creator",  Script_typeCreator,0,0,0);
  gh_new_procedure("type-skill",  Script_typeSkill,0,0,0);
  gh_new_procedure("type-experience",  Script_typeExperience,0,0,0);
  gh_new_procedure("type-earthwall",  Script_typeEarthwall,0,0,0);
  gh_new_procedure("type-golem",  Script_typeGolem,0,0,0);
  gh_new_procedure("type-bomb",  Script_typeBomb,0,0,0);
  gh_new_procedure("type-thrown-object",  Script_typeThrownObject,0,0,0);
  gh_new_procedure("type-blindness",  Script_typeBlindness,0,0,0);
  gh_new_procedure("type-god",  Script_typeGod,0,0,0);
  gh_new_procedure("type-detector",  Script_typeDetector,0,0,0);
  gh_new_procedure("type-speedball",  Script_typeSpeedBall,0,0,0);
  gh_new_procedure("type-dead-object",  Script_typeDeadObject,0,0,0);
  gh_new_procedure("type-drink",  Script_typeDrink,0,0,0);
  gh_new_procedure("type-marker",  Script_typeMarker,0,0,0);
  gh_new_procedure("type-holy-altar",  Script_typeHolyAltar,0,0,0);
  gh_new_procedure("type-player-changer",  Script_typePlayerChanger,0,0,0);
  gh_new_procedure("type-battleground",  Script_typeBattleground,0,0,0);
  gh_new_procedure("type-peacemaker",  Script_typePeacemaker,0,0,0);
  gh_new_procedure("type-gem",  Script_typeGem,0,0,0);
  gh_new_procedure("type-firechest",  Script_typeFirechest,0,0,0);
  gh_new_procedure("type-firewall",  Script_typeFirewall,0,0,0);
  gh_new_procedure("type-inventory-checker",  Script_typeCheckInv,0,0,0);
  gh_new_procedure("type-mood-floor",  Script_typeMoodFloor,0,0,0);
  gh_new_procedure("type-exit",  Script_typeExit,0,0,0);
  gh_new_procedure("type-encounter",  Script_typeEncounter,0,0,0);
  gh_new_procedure("type-shop-floor",  Script_typeShopFloor,0,0,0);
  gh_new_procedure("type-shop-mat",  Script_typeShopMat,0,0,0);
  gh_new_procedure("type-ring",  Script_typeRing,0,0,0);
  gh_new_procedure("type-flesh",  Script_typeFlesh,0,0,0);
  gh_new_procedure("type-inorganic",  Script_typeInorganic,0,0,0);
  gh_new_procedure("type-lighter",  Script_typeLighter,0,0,0);
  gh_new_procedure("type-trap-part",  Script_typeTrapPart,0,0,0);
  gh_new_procedure("type-spellbook",  Script_typeSpellbook,0,0,0);
  gh_new_procedure("type-cloak",  Script_typeCloak,0,0,0);
  gh_new_procedure("type-cone",  Script_typeCone,0,0,0);
  gh_new_procedure("type-aura",  Script_typeAura,0,0,0);
  gh_new_procedure("type-spinner",  Script_typeSpinner,0,0,0);
  gh_new_procedure("type-gate",  Script_typeGate,0,0,0);
  gh_new_procedure("type-button",  Script_typeButton,0,0,0);
  gh_new_procedure("type-handle",  Script_typeHandle,0,0,0);
  gh_new_procedure("type-hole",  Script_typeHole,0,0,0);
  gh_new_procedure("type-trapdoor",  Script_typeTrapdoor,0,0,0);
  gh_new_procedure("type-word-of-recall",  Script_typeWordOfRecall,0,0,0);
  gh_new_procedure("type-paraimage",  Script_typeParaimage,0,0,0);
  gh_new_procedure("type-sign",  Script_typeSign,0,0,0);
  gh_new_procedure("type-boots",  Script_typeBoots,0,0,0);
  gh_new_procedure("type-gloves",  Script_typeGloves,0,0,0);
  gh_new_procedure("type-converter",  Script_typeConverter,0,0,0);
  gh_new_procedure("type-bracers",  Script_typeBracers,0,0,0);
  gh_new_procedure("type-poisoning",  Script_typePoisoning,0,0,0);
  gh_new_procedure("type-save-bed",  Script_typeSavebed,0,0,0);
  gh_new_procedure("type-poison-cloud",  Script_typePoisonCloud,0,0,0);
  gh_new_procedure("type-fire-holes",  Script_typeFireholes,0,0,0);
  gh_new_procedure("type-wand",  Script_typeWand,0,0,0);
  gh_new_procedure("type-ability",  Script_typeAbility,0,0,0);
  gh_new_procedure("type-scroll",  Script_typeScroll,0,0,0);
  gh_new_procedure("type-director",  Script_typeDirector,0,0,0);
  gh_new_procedure("type-girdle",  Script_typeGirdle,0,0,0);
  gh_new_procedure("type-force",  Script_typeForce,0,0,0);
  gh_new_procedure("type-potion-effect",  Script_typePotionEffect,0,0,0);
  gh_new_procedure("type-close-container",  Script_typeCloseContainer,0,0,0);
  gh_new_procedure("type-container",  Script_typeContainer,0,0,0);
  gh_new_procedure("type-armour-improver",  Script_typeArmourImprover,0,0,0);
  gh_new_procedure("type-weapon-improver",  Script_typeWeaponImprover,0,0,0);
  gh_new_procedure("type-skill-scroll",  Script_typeSkillScroll,0,0,0);
  gh_new_procedure("type-deep-swamp",  Script_typeDeepSwamp,0,0,0);
  gh_new_procedure("type-identify-altar",  Script_typeIdentifyAltar,0,0,0);
  gh_new_procedure("type-cancellation",  Script_typeCancellation,0,0,0);
  gh_new_procedure("type-menu",  Script_typeMenu,0,0,0);
  gh_new_procedure("type-ball-lightning",  Script_typeBallLightning,0,0,0);
  gh_new_procedure("type-swarm-spell",  Script_typeSwarmSpell,0,0,0);
  gh_new_procedure("type-rune",  Script_typeRune,0,0,0);
  gh_new_procedure("type-power-crystal",  Script_typePowerCrystal,0,0,0);
  gh_new_procedure("type-corpse",  Script_typeCorpse,0,0,0);
  gh_new_procedure("type-disease",  Script_typeDisease,0,0,0);
  gh_new_procedure("type-symptom",  Script_typeSymptom,0,0,0);
};

SCM Script_colorBlack()
{
  return gh_int2scm(1);
};
SCM Script_colorDarkBlue()
{
  return gh_int2scm(2);
};
SCM Script_colorRed()
{
  return gh_int2scm(3);
};
SCM Script_colorDarkYellow()
{
  return gh_int2scm(4);
};
SCM Script_colorLightBlue()
{
  return gh_int2scm(5);
};
SCM Script_colorOrange()
{
  return gh_int2scm(6);
};
SCM Script_colorDarkGreen()
{
  return gh_int2scm(7);
};
SCM Script_colorLightGreen()
{
  return gh_int2scm(8);
};
SCM Script_colorGrey()
{
  return gh_int2scm(9);
};
SCM Script_colorBrown()
{
  return gh_int2scm(10);
};
SCM Script_colorYellow()
{
  return gh_int2scm(11);
};
SCM Script_colorLightYellow()
{
  return gh_int2scm(12);
};

SCM Script_attackTypePhysical()
{
  return gh_long2scm(1);
};
SCM Script_attackTypeMagic()
{
  return gh_long2scm(2);
};
SCM Script_attackTypeFire()
{
  return gh_long2scm(4);
};
SCM Script_attackTypeElectricity()
{
  return gh_long2scm(8);
};
SCM Script_attackTypeCold()
{
  return gh_long2scm(16);
};
SCM Script_attackTypeConfusion()
{
  return gh_long2scm(32);
};
SCM Script_attackTypeAcid()
{
  return gh_long2scm(64);
};
SCM Script_attackTypeDrain()
{
  return gh_long2scm(128);
};
SCM Script_attackTypeWeaponmagic()
{
  return gh_long2scm(256);
};
SCM Script_attackTypeGhosthit()
{
  return gh_long2scm(512);
};
SCM Script_attackTypePoison()
{
  return gh_long2scm(1024);
};
SCM Script_attackTypeSlow()
{
  return gh_long2scm(2048);
};
SCM Script_attackTypeParalyze()
{
  return gh_long2scm(4096);
};
SCM Script_attackTypeTurnUndead()
{
  return gh_long2scm(8192);
};
SCM Script_attackTypeFear()
{
  return gh_long2scm(16384);
};
SCM Script_attackTypeCancellation()
{
  return gh_long2scm(32768);
};
SCM Script_attackTypeDepletion()
{
  return gh_long2scm(65536);
};
SCM Script_attackTypeDeath()
{
  return gh_long2scm(131072);
};
SCM Script_attackTypeChaos()
{
  return gh_long2scm(262144);
};
SCM Script_attackTypeCounterspell()
{
  return gh_long2scm(524288);
};
SCM Script_attackTypeGodpower()
{
  return gh_long2scm(1048576);
};
SCM Script_attackTypeHolyWord()
{
  return gh_long2scm(2097152);
};
SCM Script_attackTypeBlind()
{
  return gh_long2scm(4194304);
};

SCM Script_typePlayer()
{
        return gh_int2scm(PLAYER);
};

SCM Script_typeBullet()
{
        return gh_int2scm(BULLET);
};

SCM Script_typeRod()
{
        return gh_int2scm(ROD);
};

SCM Script_typeTreasure()
{
        return gh_int2scm(TREASURE);
};

SCM Script_typePotion()
{
        return gh_int2scm(POTION);
};

SCM Script_typeFood()
{
        return gh_int2scm(FOOD);
};

SCM Script_typePoison()
{
        return gh_int2scm(POISON);
};

SCM Script_typeBook()
{
        return gh_int2scm(BOOK);
};

SCM Script_typeClock()
{
        return gh_int2scm(CLOCK);
};

SCM Script_typeFBullet()
{
        return gh_int2scm(FBULLET);
};

SCM Script_typeFBall()
{
        return gh_int2scm(FBALL);
};

SCM Script_typeLightning()
{
        return gh_int2scm(LIGHTNING);
};

SCM Script_typeArrow()
{
        return gh_int2scm(ARROW);
};

SCM Script_typeBow()
{
        return gh_int2scm(BOW);
};

SCM Script_typeWeapon()
{
        return gh_int2scm(WEAPON);
};

SCM Script_typeArmour()
{
        return gh_int2scm(ARMOUR);
};

SCM Script_typePedestal()
{
        return gh_int2scm(PEDESTAL);
};

SCM Script_typeAltar()
{
        return gh_int2scm(ALTAR);
};

SCM Script_typeConfusion()
{
        return gh_int2scm(CONFUSION);
};

SCM Script_typeLockedDoor()
{
        return gh_int2scm(LOCKED_DOOR);
};

SCM Script_typeSpecialKey()
{
        return gh_int2scm(SPECIAL_KEY);
};

SCM Script_typeMap()
{
        return gh_int2scm(MAP);
};

SCM Script_typeDoor()
{
        return gh_int2scm(DOOR);
};

SCM Script_typeKey()
{
        return gh_int2scm(KEY);
};

SCM Script_typeMMissile()
{
        return gh_int2scm(MMISSILE);
};

SCM Script_typeTimedGate()
{
        return gh_int2scm(TIMED_GATE);
};

SCM Script_typeTrigger()
{
        return gh_int2scm(TRIGGER);
};

SCM Script_typeGrimReaper()
{
        return gh_int2scm(GRIMREAPER);
};

SCM Script_typeMagicEar()
{
        return gh_int2scm(MAGIC_EAR);
};

SCM Script_typeTriggerButton()
{
        return gh_int2scm(TRIGGER_BUTTON);
};

SCM Script_typeTriggerAltar()
{
        return gh_int2scm(TRIGGER_ALTAR);
};

SCM Script_typeTriggerPedestal()
{
        return gh_int2scm(TRIGGER_PEDESTAL);
};

SCM Script_typeShield()
{
        return gh_int2scm(SHIELD);
};

SCM Script_typeHelmet()
{
        return gh_int2scm(HELMET);
};

SCM Script_typeHorn()
{
        return gh_int2scm(HORN);
};

SCM Script_typeMoney()
{
        return gh_int2scm(MONEY);
};

SCM Script_typeClass()
{
        return gh_int2scm(CLASS);
};

SCM Script_typeGravestone()
{
        return gh_int2scm(GRAVESTONE);
};

SCM Script_typeAmulet()
{
        return gh_int2scm(AMULET);
};

SCM Script_typePlayerMover()
{
        return gh_int2scm(PLAYERMOVER);
};

SCM Script_typeTeleporter()
{
        return gh_int2scm(TELEPORTER);
};

SCM Script_typeCreator()
{
        return gh_int2scm(CREATOR);
};

SCM Script_typeSkill()
{
        return gh_int2scm(SKILL);
};

SCM Script_typeExperience()
{
        return gh_int2scm(EXPERIENCE);
};

SCM Script_typeEarthwall()
{
        return gh_int2scm(EARTHWALL);
};

SCM Script_typeGolem()
{
        return gh_int2scm(GOLEM);
};

SCM Script_typeBomb()
{
        return gh_int2scm(BOMB);
};

SCM Script_typeThrownObject()
{
        return gh_int2scm(THROWN_OBJ);
};

SCM Script_typeBlindness()
{
        return gh_int2scm(BLINDNESS);
};

SCM Script_typeGod()
{
        return gh_int2scm(GOD);
};

SCM Script_typeDetector()
{
        return gh_int2scm(DETECTOR);
};

SCM Script_typeSpeedBall()
{
        return gh_int2scm(SPEEDBALL);
};

SCM Script_typeDeadObject()
{
        return gh_int2scm(DEAD_OBJECT);
};

SCM Script_typeDrink()
{
        return gh_int2scm(DRINK);
};

SCM Script_typeMarker()
{
        return gh_int2scm(MARKER);
};

SCM Script_typeHolyAltar()
{
        return gh_int2scm(HOLY_ALTAR);
};

SCM Script_typePlayerChanger()
{
        return gh_int2scm(PLAYER_CHANGER);
};

SCM Script_typeBattleground()
{
        return gh_int2scm(BATTLEGROUND);
};

SCM Script_typePeacemaker()
{
        return gh_int2scm(PEACEMAKER);
};

SCM Script_typeGem()
{
        return gh_int2scm(GEM);
};

SCM Script_typeFirechest()
{
        return gh_int2scm(FIRECHEST);
};

SCM Script_typeFirewall()
{
        return gh_int2scm(FIREWALL);
};

SCM Script_typeCheckInv()
{
        return gh_int2scm(CHECK_INV);
};

SCM Script_typeMoodFloor()
{
        return gh_int2scm(MOOD_FLOOR);
};

SCM Script_typeExit()
{
        return gh_int2scm(EXIT);
};

SCM Script_typeEncounter()
{
        return gh_int2scm(ENCOUNTER);
};

SCM Script_typeShopFloor()
{
        return gh_int2scm(SHOP_FLOOR);
};

SCM Script_typeShopMat()
{
        return gh_int2scm(SHOP_MAT);
};

SCM Script_typeRing()
{
        return gh_int2scm(RING);
};

SCM Script_typeFlesh()
{
        return gh_int2scm(FLESH);
};

SCM Script_typeInorganic()
{
        return gh_int2scm(INORGANIC);
};

SCM Script_typeLighter()
{
        return gh_int2scm(LIGHTER);
};

SCM Script_typeTrapPart()
{
        return gh_int2scm(TRAP_PART);
};

SCM Script_typeSpellbook()
{
        return gh_int2scm(SPELLBOOK);
};

SCM Script_typeCloak()
{
        return gh_int2scm(CLOAK);
};

SCM Script_typeCone()
{
        return gh_int2scm(CONE);
};

SCM Script_typeAura()
{
        return gh_int2scm(AURA);
};

SCM Script_typeSpinner()
{
        return gh_int2scm(SPINNER);
};

SCM Script_typeGate()
{
        return gh_int2scm(GATE);
};

SCM Script_typeButton()
{
        return gh_int2scm(BUTTON);
};

SCM Script_typeHandle()
{
        return gh_int2scm(HANDLE);
};

SCM Script_typeHole()
{
        return gh_int2scm(HOLE);
};

SCM Script_typeTrapdoor()
{
        return gh_int2scm(TRAPDOOR);
};

SCM Script_typeWordOfRecall()
{
        return gh_int2scm(WORD_OF_RECALL);
};

SCM Script_typeParaimage()
{
        return gh_int2scm(PARAIMAGE);
};

SCM Script_typeSign()
{
        return gh_int2scm(SIGN);
};

SCM Script_typeBoots()
{
        return gh_int2scm(BOOTS);
};

SCM Script_typeGloves()
{
        return gh_int2scm(GLOVES);
};

SCM Script_typeConverter()
{
        return gh_int2scm(CONVERTER);
};

SCM Script_typeBracers()
{
        return gh_int2scm(BRACERS);
};

SCM Script_typePoisoning()
{
        return gh_int2scm(POISONING);
};

SCM Script_typeSavebed()
{
        return gh_int2scm(SAVEBED);
};

SCM Script_typePoisonCloud()
{
        return gh_int2scm(POISONCLOUD);
};

SCM Script_typeFireholes()
{
        return gh_int2scm(FIREHOLES);
};

SCM Script_typeWand()
{
        return gh_int2scm(WAND);
};

SCM Script_typeAbility()
{
        return gh_int2scm(ABILITY);
};

SCM Script_typeScroll()
{
        return gh_int2scm(SCROLL);
};

SCM Script_typeDirector()
{
        return gh_int2scm(DIRECTOR);
};

SCM Script_typeGirdle()
{
        return gh_int2scm(GIRDLE);
};

SCM Script_typeForce()
{
        return gh_int2scm(FORCE);
};

SCM Script_typePotionEffect()
{
        return gh_int2scm(POTION_EFFECT);
};

SCM Script_typeCloseContainer()
{
        return gh_int2scm(CLOSE_CON);
};

SCM Script_typeContainer()
{
        return gh_int2scm(CONTAINER);
};

SCM Script_typeArmourImprover()
{
        return gh_int2scm(ARMOUR_IMPROVER);
};

SCM Script_typeWeaponImprover()
{
        return gh_int2scm(WEAPON_IMPROVER);
};

SCM Script_typeSkillScroll()
{
        return gh_int2scm(SKILLSCROLL);
};

SCM Script_typeDeepSwamp()
{
        return gh_int2scm(DEEP_SWAMP);
};

SCM Script_typeIdentifyAltar()
{
        return gh_int2scm(IDENTIFY_ALTAR);
};

SCM Script_typeCancellation()
{
        return gh_int2scm(CANCELLATION);
};

SCM Script_typeMenu()
{
        return gh_int2scm(MENU);
};

SCM Script_typeBallLightning()
{
        return gh_int2scm(BALL_LIGHTNING);
};

SCM Script_typeSwarmSpell()
{
        return gh_int2scm(SWARM_SPELL);
};

SCM Script_typeRune()
{
        return gh_int2scm(RUNE);
};

SCM Script_typePowerCrystal()
{
        return gh_int2scm(POWER_CRYSTAL);
};

SCM Script_typeCorpse()
{
        return gh_int2scm(CORPSE);
};

SCM Script_typeDisease()
{
        return gh_int2scm(DISEASE);
};

SCM Script_typeSymptom()
{
        return gh_int2scm(SYMPTOM);
};
