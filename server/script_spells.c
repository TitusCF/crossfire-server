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

void guile_init_spell_functions()
{
  gh_new_procedure("spell-magic-bullet", Script_spellBullet, 0,0,0);
  gh_new_procedure("spell-small-fireball", Script_spellSmallFireball, 0,0,0);
  gh_new_procedure("spell-medium-fireball", Script_spellMediumFireball, 0,0,0);
  gh_new_procedure("spell-large-fireball", Script_spellLargeFireball, 0,0,0);
  gh_new_procedure("spell-burning-hands", Script_spellBurningHands, 0,0,0);
  gh_new_procedure("spell-small-lightning", Script_spellSmallLightning, 0,0,0);
  gh_new_procedure("spell-large-lightning", Script_spellLargeLightning, 0,0,0);
  gh_new_procedure("spell-magic-missile", Script_spellMagicMissile, 0,0,0);
  gh_new_procedure("spell-create-bomb", Script_spellCreateBomb, 0,0,0);
  gh_new_procedure("spell-summon-golem", Script_spellSummonGolem, 0,0,0);
  gh_new_procedure("spell-summon-fire-elemental", Script_spellSummonFireElemental, 0,0,0);
  gh_new_procedure("spell-summon-earth-elemental", Script_spellSummonEarthElemental, 0,0,0);
  gh_new_procedure("spell-summon-water-elemental", Script_spellSummonWaterElemental, 0,0,0);
  gh_new_procedure("spell-summon-air-elemental", Script_spellSummonAirElemental, 0,0,0);
  gh_new_procedure("spell-dimension-door", Script_spellDimensionDoor, 0,0,0);
  gh_new_procedure("spell-create-earth-wall", Script_spellCreateEarthWall, 0,0,0);
  gh_new_procedure("spell-paralyze", Script_spellParalyze, 0,0,0);
  gh_new_procedure("spell-icestorm", Script_spellIcestorm, 0,0,0);
  gh_new_procedure("spell-magic-mapping", Script_spellMagicMapping, 0,0,0);
  gh_new_procedure("spell-turn-undead", Script_spellTurnUndead, 0,0,0);
  gh_new_procedure("spell-fear", Script_spellFear, 0,0,0);
  gh_new_procedure("spell-poison-cloud", Script_spellPoisonCloud, 0,0,0);
  gh_new_procedure("spell-wall-of-wonder", Script_spellWOW, 0,0,0);
  gh_new_procedure("spell-destruction", Script_spellDestruction, 0,0,0);
  gh_new_procedure("spell-perceive-self", Script_spellPerceiveSelf, 0,0,0);
  gh_new_procedure("spell-WOR", Script_spellWOR, 0,0,0);
  gh_new_procedure("spell-invisibility", Script_spellInvisibility, 0,0,0);
  gh_new_procedure("spell-invisible-to-undead", Script_spellInvisibleToUndead, 0,0,0);
  gh_new_procedure("spell-probe", Script_spellProbe, 0,0,0);
  gh_new_procedure("spell-large-bullet", Script_spellLargeBullet, 0,0,0);
  gh_new_procedure("spell-improved-invisibility", Script_spellImprovedInvisibility, 0,0,0);
  gh_new_procedure("spell-holy-word", Script_spellHolyWord, 0,0,0);
  gh_new_procedure("spell-minor-healing", Script_spellMinorHealing, 0,0,0);
  gh_new_procedure("spell-medium-healing", Script_spellMediumHealing, 0,0,0);
  gh_new_procedure("spell-major-healing", Script_spellMajorHealing, 0,0,0);
  gh_new_procedure("spell-heal", Script_spellHeal, 0,0,0);
  gh_new_procedure("spell-create-food", Script_spellCreateFood, 0,0,0);
  gh_new_procedure("spell-earth-to-dust", Script_spellEarthToDust, 0,0,0);
  gh_new_procedure("spell-armour", Script_spellArmour, 0,0,0);
  gh_new_procedure("spell-strength", Script_spellStrength, 0,0,0);
  gh_new_procedure("spell-dexterity", Script_spellDexterity, 0,0,0);
  gh_new_procedure("spell-constitution", Script_spellConstitution, 0,0,0);
  gh_new_procedure("spell-charisma", Script_spellCharisma, 0,0,0);
  gh_new_procedure("spell-firewall", Script_spellFireWall, 0,0,0);
  gh_new_procedure("spell-frostwall", Script_spellFrostWall, 0,0,0);
  gh_new_procedure("spell-protection-cold", Script_spellProtectionCold, 0,0,0);
  gh_new_procedure("spell-protection-electricity", Script_spellProtectionElectricity, 0,0,0);
  gh_new_procedure("spell-protection-fire", Script_spellProtectionFire, 0,0,0);
  gh_new_procedure("spell-protection-poison", Script_spellProtectionPoison, 0,0,0);
  gh_new_procedure("spell-protection-slow", Script_spellProtectionSlow, 0,0,0);
  gh_new_procedure("spell-protection-paralyze", Script_spellProtectionParalyze, 0,0,0);
  gh_new_procedure("spell-protection-drain", Script_spellProtectionDrain, 0,0,0);
  gh_new_procedure("spell-protection-magic", Script_spellProtectionMagic, 0,0,0);
  gh_new_procedure("spell-protection-attack", Script_spellProtectionAttack, 0,0,0);
  gh_new_procedure("spell-levitate", Script_spellLevitate, 0,0,0);
  gh_new_procedure("spell-small-speedball", Script_spellSmallSpeedball, 0,0,0);
  gh_new_procedure("spell-large-speedball", Script_spellLargeSpeedball, 0,0,0);
  gh_new_procedure("spell-hellfire", Script_spellHellfire, 0,0,0);
  gh_new_procedure("spell-firebreath", Script_spellFirebreath, 0,0,0);
  gh_new_procedure("spell-large-icestorm", Script_spellLargeIcestorm, 0,0,0);
  gh_new_procedure("spell-charging", Script_spellCharging, 0,0,0);
  gh_new_procedure("spell-polymorph", Script_spellPolymorph, 0,0,0);
  gh_new_procedure("spell-cancellation", Script_spellCancellation, 0,0,0);
  gh_new_procedure("spell-confusion", Script_spellConfusion, 0,0,0);
  gh_new_procedure("spell-mass-confusion", Script_spellMassConfusion, 0,0,0);
  gh_new_procedure("spell-summon-pet-monster", Script_spellSummonPetMonster, 0,0,0);
  gh_new_procedure("spell-slow", Script_spellSlow, 0,0,0);
  gh_new_procedure("spell-regenerate-spellpoints", Script_spellRegenerateSpellpoints, 0,0,0);
  gh_new_procedure("spell-cure-poison", Script_spellCurePoison, 0,0,0);
  gh_new_procedure("spell-cure-confusion", Script_spellCureConfusion, 0,0,0);
  gh_new_procedure("spell-protection-cancellation", Script_spellProtectionCancellation, 0,0,0);
  gh_new_procedure("spell-protection-depletion", Script_spellProtectionDepletion, 0,0,0);
  gh_new_procedure("spell-alchemy", Script_spellAlchemy, 0,0,0);
  gh_new_procedure("spell-remove-curse", Script_spellRemoveCurse, 0,0,0);
  gh_new_procedure("spell-remove-damnation", Script_spellRemoveDamnation, 0,0,0);
  gh_new_procedure("spell-identify", Script_spellIdentify, 0,0,0);
  gh_new_procedure("spell-detect-magic", Script_spellDetectMagic, 0,0,0);
  gh_new_procedure("spell-dectect-monster", Script_spellDetectMonster, 0,0,0);
  gh_new_procedure("spell-detect-evil", Script_spellDetectEvil, 0,0,0);
  gh_new_procedure("spell-detect-curse", Script_spellDetectCurse, 0,0,0);
  gh_new_procedure("spell-heroism", Script_spellHeroism, 0,0,0);
  gh_new_procedure("spell-aggravation", Script_spellAggravation, 0,0,0);
  gh_new_procedure("spell-firebolt", Script_spellFirebolt, 0,0,0);
  gh_new_procedure("spell-frostbolt", Script_spellFrostbolt, 0,0,0);
  gh_new_procedure("spell-shockwave", Script_spellShockwave, 0,0,0);
  gh_new_procedure("spell-color-spray", Script_spellColorSpray, 0,0,0);
  gh_new_procedure("spell-haste", Script_spellHaste, 0,0,0);
  gh_new_procedure("spell-face-of-death", Script_spellFaceOfDeath, 0,0,0);
  gh_new_procedure("spell-ball-lightning", Script_spellBallLightning, 0,0,0);
  gh_new_procedure("spell-meteor-swarm", Script_spellMeteorSwarm, 0,0,0);
  gh_new_procedure("spell-meteor", Script_spellMeteor, 0,0,0);
  gh_new_procedure("spell-mystic-fist", Script_spellMysticFist, 0,0,0);
  gh_new_procedure("spell-raise-dead", Script_spellRaiseDead, 0,0,0);
  gh_new_procedure("spell-resurrection", Script_spellResurrection, 0,0,0);
  gh_new_procedure("spell-reincarnation", Script_spellReincarnation, 0,0,0);
  gh_new_procedure("spell-immune-cold", Script_spellImmuneCold, 0,0,0);
  gh_new_procedure("spell-immune-electricity", Script_spellImmuneElectricity, 0,0,0);
  gh_new_procedure("spell-immune-fire", Script_spellImmuneFire, 0,0,0);
  gh_new_procedure("spell-immune-poison", Script_spellImmunePoison, 0,0,0);
  gh_new_procedure("spell-immune-slow", Script_spellImmuneSlow, 0,0,0);
  gh_new_procedure("spell-immune-paralyze", Script_spellImmuneParalyze, 0,0,0);
  gh_new_procedure("spell-immune-drain", Script_spellImmuneDrain, 0,0,0);
  gh_new_procedure("spell-immune-magic", Script_spellImmuneMagic, 0,0,0);
  gh_new_procedure("spell-immune-attack", Script_spellImmuneAttack, 0,0,0);
  gh_new_procedure("spell-invulnerability", Script_spellInvulnerability, 0,0,0);
  gh_new_procedure("spell-protection", Script_spellProtection, 0,0,0);
  gh_new_procedure("spell-rune-of-fire", Script_spellRuneFire, 0,0,0);
  gh_new_procedure("spell-rune-of-frost", Script_spellRuneFrost, 0,0,0);
  gh_new_procedure("spell-rune-of-shocking", Script_spellRuneShock, 0,0,0);
  gh_new_procedure("spell-rune-of-blasting", Script_spellRuneBlast, 0,0,0);
  gh_new_procedure("spell-rune-of-death", Script_spellRuneDeath, 0,0,0);
  gh_new_procedure("spell-rune-of-marking", Script_spellRuneMark, 0,0,0);
  gh_new_procedure("spell-build-director", Script_spellBuildDirector, 0,0,0);
  gh_new_procedure("spell-create-pool-of-chaos", Script_spellChaosPool, 0,0,0);
  gh_new_procedure("spell-build-bullet-wall", Script_spellBuildBulletWall, 0,0,0);
  gh_new_procedure("spell-build-lightning-wall", Script_spellBuildLightningWall, 0,0,0);
  gh_new_procedure("spell-build-fireball-wall", Script_spellBuildFireballWall, 0,0,0);
  gh_new_procedure("spell-rune-of-magic", Script_spellRuneMagic, 0,0,0);
  gh_new_procedure("spell-rune-of-magic-draining", Script_spellRuneDrainSP, 0,0,0);
  gh_new_procedure("spell-rune-of-antimagic", Script_spellRuneAntimagic, 0,0,0);
  gh_new_procedure("spell-rune-of-transferrence", Script_spellRuneTransferrence, 0,0,0);
  gh_new_procedure("spell-transfer", Script_spellTransfer, 0,0,0);
  gh_new_procedure("spell-magic-drain", Script_spellMagicDrain, 0,0,0);
  gh_new_procedure("spell-counterspell", Script_spellCounterspell, 0,0,0);
  gh_new_procedure("spell-rune-of-dispel", Script_spellDispelRune, 0,0,0);
  gh_new_procedure("spell-protection-confusion", Script_spellProtectionConfusion, 0,0,0);
  gh_new_procedure("spell-restoration", Script_spellRestoration, 0,0,0);
  gh_new_procedure("spell-sommon-evil-monster", Script_spellSummonEvilMonster, 0,0,0);
  gh_new_procedure("spell-counterwall", Script_spellCounterwall, 0,0,0);
  gh_new_procedure("spell-cause-light-wounds", Script_spellCauseLightWounds, 0,0,0);
  gh_new_procedure("spell-cause-medium-wounds", Script_spellCauseMediumWounds, 0,0,0);
  gh_new_procedure("spell-cause-heavy-wounds", Script_spellCauseHeavyWounds, 0,0,0);
  gh_new_procedure("spell-charm", Script_spellCharm, 0,0,0);
  gh_new_procedure("spell-banishment", Script_spellBanishment, 0,0,0);
  gh_new_procedure("spell-create-missile", Script_spellCreateMissile, 0,0,0);
  gh_new_procedure("spell-show-invisible", Script_spellShowInvisible, 0,0,0);
  gh_new_procedure("spell-x-ray", Script_spellXRay, 0,0,0);
  gh_new_procedure("spell-pacify", Script_spellPacify, 0,0,0);
  gh_new_procedure("spell-summon-fog", Script_spellSummonFog, 0,0,0);
  gh_new_procedure("spell-steambolt", Script_spellSteambolt, 0,0,0);
  gh_new_procedure("spell-command-undead", Script_spellCommandUndead, 0,0,0);
  gh_new_procedure("spell-holy-orb", Script_spellHolyOrb, 0,0,0);
  gh_new_procedure("spell-summon-avatar", Script_spellSummonAvatar, 0,0,0);
  gh_new_procedure("spell-holy-possession", Script_spellHolyPossession, 0,0,0);
  gh_new_procedure("spell-bless", Script_spellBless, 0,0,0);
  gh_new_procedure("spell-curse", Script_spellCurse, 0,0,0);
  gh_new_procedure("spell-regeneration", Script_spellRegeneration, 0,0,0);
  gh_new_procedure("spell-consecrate", Script_spellConsecrate, 0,0,0);
  gh_new_procedure("spell-summon-cult-monsters", Script_spellSummonCultMonsters, 0,0,0);
  gh_new_procedure("spell-cause-critical-wounds", Script_spellCauseCriticalWounds, 0,0,0);
  gh_new_procedure("spell-holy-wrath", Script_spellHolyWrath, 0,0,0);
  gh_new_procedure("spell-retribution", Script_spellRetribution, 0,0,0);
  gh_new_procedure("spell-finger-of-death", Script_spellFingerDeath, 0,0,0);
  gh_new_procedure("spell-insect-plague", Script_spellInsectPlague, 0,0,0);
  gh_new_procedure("spell-call-holy-servant", Script_spellHolyServant, 0,0,0);
  gh_new_procedure("spell-wall-of-thorns", Script_spellWallOfThorns, 0,0,0);
  gh_new_procedure("spell-staff-to-snake", Script_spellStaffToSnake, 0,0,0);
  gh_new_procedure("spell-light", Script_spellLight, 0,0,0);
  gh_new_procedure("spell-darkness", Script_spellDarkness, 0,0,0);
  gh_new_procedure("spell-nightfall", Script_spellNightfall, 0,0,0);
  gh_new_procedure("spell-daylight", Script_spellDaylight, 0,0,0);
  gh_new_procedure("spell-sunspear", Script_spellSunSpear, 0,0,0);
  gh_new_procedure("spell-faery-fire", Script_spellFaeryFire, 0,0,0);
  gh_new_procedure("spell-cure-blindness", Script_spellCureBlindness, 0,0,0);
  gh_new_procedure("spell-dark-vision", Script_spellDarkVision, 0,0,0);
  gh_new_procedure("spell-bullet-swarm", Script_spellBulletSwarm, 0,0,0);
  gh_new_procedure("spell-bullet-storm", Script_spellBulletStorm, 0,0,0);
  gh_new_procedure("spell-cause-many-wounds", Script_spellCauseManyWounds, 0,0,0);
  gh_new_procedure("spell-small-snowstorm", Script_spellSmallSnowstorm, 0,0,0);
  gh_new_procedure("spell-medium-snowstorm", Script_spellMediumSnowstorm, 0,0,0);
  gh_new_procedure("spell-large-snowstorm", Script_spellLargeSnowstorm, 0,0,0);
  gh_new_procedure("spell-cure-disease", Script_spellCureDisease, 0,0,0);
  gh_new_procedure("spell-cause-ebola", Script_spellCauseEbola, 0,0,0);
  gh_new_procedure("spell-cause-flu", Script_spellCauseFlu, 0,0,0);
  gh_new_procedure("spell-cause-plague", Script_spellCausePlague, 0,0,0);
  gh_new_procedure("spell-cause-leprosy", Script_spellCauseLeprosy, 0,0,0);
  gh_new_procedure("spell-cause-small-pox", Script_spellCauseSmallPox, 0,0,0);
  gh_new_procedure("spell-cause-pneumonic-plague", Script_spellCausePneumonicPlague, 0,0,0);
  gh_new_procedure("spell-cause-anthrax", Script_spellCauseAnthrax, 0,0,0);
  gh_new_procedure("spell-cause-typhoid", Script_spellCauseTyphoid, 0,0,0);
  gh_new_procedure("spell-cause-mana-blast", Script_spellManaBlast, 0,0,0);
  gh_new_procedure("spell-small-manaball", Script_spellSmallManaball, 0,0,0);
  gh_new_procedure("spell-medium-manaball", Script_spellMediumManaball, 0,0,0);
  gh_new_procedure("spell-large-manaball", Script_spellLargeManaball, 0,0,0);
  gh_new_procedure("spell-manabolt", Script_spellManabolt, 0,0,0);
  gh_new_procedure("spell-dancing-sword", Script_spellDancingSword, 0,0,0);
  gh_new_procedure("spell-animate-weapon", Script_spellAnimateWeapon, 0,0,0);
  gh_new_procedure("spell-cause-cold", Script_spellCauseCold, 0,0,0);
  gh_new_procedure("spell-divine-shock", Script_spellDivineShock, 0,0,0);
  gh_new_procedure("spell-wind-storm", Script_spellWindStorm, 0,0,0);
  gh_new_procedure("spell-sanctuary", Script_spellSanctuary, 0,0,0);
  gh_new_procedure("spell-peace", Script_spellPeace, 0,0,0);
  gh_new_procedure("spell-spider-web", Script_spellSpiderWeb, 0,0,0);
  gh_new_procedure("spell-conflict", Script_spellConflict, 0,0,0);
  gh_new_procedure("spell-rage", Script_spellRage, 0,0,0);
  gh_new_procedure("spell-forked-lightning", Script_spellForkedLightning, 0,0,0);
  gh_new_procedure("spell-poison-fog", Script_spellPoisonFog, 0,0,0);
  gh_new_procedure("spell-flame-aura", Script_spellFlameAura, 0,0,0);
  gh_new_procedure("spell-vitriol", Script_spellVitriol, 0,0,0);
  gh_new_procedure("spell-vitriol-splash", Script_spellVitriolSplash, 0,0,0);
  gh_new_procedure("spell-ironwood-skin", Script_spellIronwoodSkin, 0,0,0);
  gh_new_procedure("spell-wrathfull-eye", Script_spellWrathfullEye, 0,0,0);
};

SCM Script_spellBullet()
{
  return gh_int2scm(SP_BULLET);
};

SCM Script_spellSmallFireball()
{
  return gh_int2scm(SP_S_FIREBALL);
};

SCM Script_spellMediumFireball()
{
  return gh_int2scm(SP_M_FIREBALL);
};

SCM Script_spellLargeFireball()
{                                		/*0*/
  return gh_int2scm(SP_L_FIREBALL);
};

SCM Script_spellBurningHands()
{
  return gh_int2scm(SP_BURNING_HANDS);
};

SCM Script_spellSmallLightning()
{
  return gh_int2scm(SP_S_LIGHTNING);
};

SCM Script_spellLargeLightning()
{
  return gh_int2scm(SP_L_LIGHTNING);
};

SCM Script_spellMagicMissile()
{
  return gh_int2scm(SP_M_MISSILE);		/*5*/
};

SCM Script_spellCreateBomb()
{
  return gh_int2scm(SP_BOMB);
};

SCM Script_spellSummonGolem()
{
  return gh_int2scm(SP_GOLEM);
};

SCM Script_spellSummonFireElemental()
{
  return gh_int2scm(SP_FIRE_ELEM);
};

SCM Script_spellSummonEarthElemental()
{
  return gh_int2scm(SP_EARTH_ELEM);
};

SCM Script_spellSummonWaterElemental()
{
  return gh_int2scm(SP_WATER_ELEM);		/*10*/
};

SCM Script_spellSummonAirElemental()
{
  return gh_int2scm(SP_AIR_ELEM);
};

SCM Script_spellDimensionDoor()
{
  return gh_int2scm(SP_D_DOOR);
};

SCM Script_spellCreateEarthWall()
{
  return gh_int2scm(SP_EARTH_WALL);
};

SCM Script_spellParalyze()
{
  return gh_int2scm(SP_PARALYZE);
};

SCM Script_spellIcestorm()
{
  return gh_int2scm(SP_ICESTORM);
};

SCM Script_spellMagicMapping()
{                        		/*15*/
  return gh_int2scm(SP_MAGIC_MAPPING);
};

SCM Script_spellTurnUndead()
{
  return gh_int2scm(SP_TURN_UNDEAD);
};

SCM Script_spellFear()
{
  return gh_int2scm(SP_FEAR);
};

SCM Script_spellPoisonCloud()
{
  return gh_int2scm(SP_POISON_CLOUD);
};

SCM Script_spellWOW()
{
  return gh_int2scm(SP_WOW);			/*20*/
};

SCM Script_spellDestruction()
{
  return gh_int2scm(SP_DESTRUCTION);
};

SCM Script_spellPerceiveSelf()
{
  return gh_int2scm(SP_PERCEIVE);
};

SCM Script_spellWOR()
{
  return gh_int2scm(SP_WOR);
};

SCM Script_spellInvisibility()
{
  return gh_int2scm(SP_INVIS);
};

SCM Script_spellInvisibleToUndead()
{
  return gh_int2scm(SP_INVIS_UNDEAD);	/*25*/
};

SCM Script_spellProbe()
{
  return gh_int2scm(SP_PROBE);
};

SCM Script_spellLargeBullet()
{
  return gh_int2scm(SP_LARGE_BULLET);
};

SCM Script_spellImprovedInvisibility()
{
  return gh_int2scm(SP_IMPROVED_INVIS);
};

SCM Script_spellHolyWord()
{
  return gh_int2scm(SP_HOLY_WORD);
};

SCM Script_spellMinorHealing()
{
  return gh_int2scm(SP_MINOR_HEAL);		/*30*/
};

SCM Script_spellMediumHealing()
{
  return gh_int2scm(SP_MED_HEAL);
};

SCM Script_spellMajorHealing()
{
  return gh_int2scm(SP_MAJOR_HEAL);
};

SCM Script_spellHeal()
{
  return gh_int2scm(SP_HEAL);
};

SCM Script_spellCreateFood()
{
  return gh_int2scm(SP_CREATE_FOOD);
};

SCM Script_spellEarthToDust()
{
  return gh_int2scm(SP_EARTH_DUST);		/*35*/
};

SCM Script_spellArmour()
{
  return gh_int2scm(SP_ARMOUR);
};

SCM Script_spellStrength()
{
  return gh_int2scm(SP_STRENGTH);
};

SCM Script_spellDexterity()
{
  return gh_int2scm(SP_DEXTERITY);
};

SCM Script_spellConstitution()
{
  return gh_int2scm(SP_CONSTITUTION);
};

SCM Script_spellCharisma()
{
  return gh_int2scm(SP_CHARISMA);		/*40*/
};

SCM Script_spellFireWall()
{
  return gh_int2scm(SP_FIRE_WALL);
};

SCM Script_spellFrostWall()
{
  return gh_int2scm(SP_FROST_WALL);
};

SCM Script_spellProtectionCold()
{
  return gh_int2scm(SP_PROT_COLD);
};

SCM Script_spellProtectionElectricity()
{
  return gh_int2scm(SP_PROT_ELEC);
};

SCM Script_spellProtectionFire()
{
  return gh_int2scm(SP_PROT_FIRE);		/*45*/
};

SCM Script_spellProtectionPoison()
{
  return gh_int2scm(SP_PROT_POISON);
};

SCM Script_spellProtectionSlow()
{
  return gh_int2scm(SP_PROT_SLOW);
};

SCM Script_spellProtectionParalyze()
{
  return gh_int2scm(SP_PROT_PARALYZE);
};

SCM Script_spellProtectionDrain()
{
  return gh_int2scm(SP_PROT_DRAIN);
};

SCM Script_spellProtectionMagic()
{
  return gh_int2scm(SP_PROT_MAGIC);      		/*50*/
};

SCM Script_spellProtectionAttack()
{
  return gh_int2scm(SP_PROT_ATTACK);
};

SCM Script_spellLevitate()
{
  return gh_int2scm(SP_LEVITATE);
};

SCM Script_spellSmallSpeedball()
{
  return gh_int2scm(SP_SMALL_SPEEDBALL);
};

SCM Script_spellLargeSpeedball()
{
  return gh_int2scm(SP_LARGE_SPEEDBALL);
};

SCM Script_spellHellfire()
{
  return gh_int2scm(SP_HELLFIRE);		/*55*/
};

SCM Script_spellFirebreath()
{
  return gh_int2scm(SP_FIREBREATH);
};

SCM Script_spellLargeIcestorm()
{
  return gh_int2scm(SP_LARGE_ICESTORM);
};

SCM Script_spellCharging()
{
  return gh_int2scm(SP_CHARGING);
};

SCM Script_spellPolymorph()
{
  return gh_int2scm(SP_POLYMORPH);
};

SCM Script_spellCancellation()
{
  return gh_int2scm(SP_CANCELLATION);	/*60*/
};

SCM Script_spellConfusion()
{
  return gh_int2scm(SP_CONFUSION);
};

SCM Script_spellMassConfusion()
{
  return gh_int2scm(SP_MASS_CONFUSION);
};

SCM Script_spellSummonPetMonster()
{
  return gh_int2scm(SP_PET);
};

SCM Script_spellSlow()
{
  return gh_int2scm(SP_SLOW);
};

SCM Script_spellRegenerateSpellpoints()
{
  return gh_int2scm(SP_REGENERATE_SPELLPOINTS);/*65*/
};

SCM Script_spellCurePoison()
{
  return gh_int2scm(SP_CURE_POISON);
};

SCM Script_spellProtectionConfusion()
{
  return gh_int2scm(SP_PROT_CONFUSE);
};

SCM Script_spellProtectionCancellation()
{
  return gh_int2scm(SP_PROT_CANCEL);
};

SCM Script_spellProtectionDepletion()
{
  return gh_int2scm(SP_PROT_DEPLETE);
};

SCM Script_spellAlchemy()
{
  return gh_int2scm(SP_ALCHEMY);		/*70*/
};

SCM Script_spellRemoveCurse()
{
  return gh_int2scm(SP_REMOVE_CURSE);
};

SCM Script_spellRemoveDamnation()
{
  return gh_int2scm(SP_REMOVE_DAMNATION);
};

SCM Script_spellIdentify()
{
  return gh_int2scm(SP_IDENTIFY);
};

SCM Script_spellDetectMagic()
{
  return gh_int2scm(SP_DETECT_MAGIC);
};

SCM Script_spellDetectMonster()
{
  return gh_int2scm(SP_DETECT_MONSTER);	/*75*/
};

SCM Script_spellDetectEvil()
{
  return gh_int2scm(SP_DETECT_EVIL);
};

SCM Script_spellDetectCurse()
{
  return gh_int2scm(SP_DETECT_CURSE);
};

SCM Script_spellHeroism()
{
  return gh_int2scm(SP_HEROISM);
};

SCM Script_spellAggravation()
{
  return gh_int2scm(SP_AGGRAVATION);
};

SCM Script_spellFirebolt()
{
  return gh_int2scm(SP_FIREBOLT);		/*80*/
};

SCM Script_spellFrostbolt()
{
  return gh_int2scm(SP_FROSTBOLT);
};

SCM Script_spellShockwave()
{
  return gh_int2scm(SP_SHOCKWAVE);
};

SCM Script_spellColorSpray()
{
  return gh_int2scm(SP_COLOR_SPRAY);
};

SCM Script_spellHaste()
{
  return gh_int2scm(SP_HASTE);
};

SCM Script_spellFaceOfDeath()
{
  return gh_int2scm(SP_FACE_OF_DEATH);	/*85*/
};

SCM Script_spellBallLightning()
{
  return gh_int2scm(SP_BALL_LIGHTNING);
};

SCM Script_spellMeteorSwarm()
{
  return gh_int2scm(SP_METEOR_SWARM);
};

SCM Script_spellMeteor()
{
  return gh_int2scm(SP_METEOR);
};

SCM Script_spellMysticFist()
{
  return gh_int2scm(SP_MYSTIC_FIST);
};

SCM Script_spellRaiseDead()
{
  return gh_int2scm(SP_RAISE_DEAD);		/*90*/
};

SCM Script_spellResurrection()
{
  return gh_int2scm(SP_RESURRECTION);
};

SCM Script_spellReincarnation()
{
  return gh_int2scm(SP_REINCARNATION);
};

SCM Script_spellImmuneCold()
{
/* mlee's spells*/
  return gh_int2scm(SP_IMMUNE_COLD);
};

SCM Script_spellImmuneElectricity()
{
  return gh_int2scm(SP_IMMUNE_ELEC);
};

SCM Script_spellImmuneFire()
{
  return gh_int2scm(SP_IMMUNE_FIRE);		/*95*/
};

SCM Script_spellImmunePoison()
{
  return gh_int2scm(SP_IMMUNE_POISON);
};

SCM Script_spellImmuneSlow()
{
  return gh_int2scm(SP_IMMUNE_SLOW);
};

SCM Script_spellImmuneParalyze()
{
  return gh_int2scm(SP_IMMUNE_PARALYZE);
};

SCM Script_spellImmuneDrain()
{
  return gh_int2scm(SP_IMMUNE_DRAIN);
};

SCM Script_spellImmuneMagic()
{
  return gh_int2scm(SP_IMMUNE_MAGIC);	/*100*/
};

SCM Script_spellImmuneAttack()
{
  return gh_int2scm(SP_IMMUNE_ATTACK);
};

SCM Script_spellInvulnerability()
{
  return gh_int2scm(SP_INVULNERABILITY);
};

SCM Script_spellProtection()
{
  return gh_int2scm(SP_PROTECTION);							/*105*/
};

/*Some more new spells by peterm */
SCM Script_spellRuneFire()
{
  return gh_int2scm(SP_RUNE_FIRE);
};

SCM Script_spellRuneFrost()
{
  return gh_int2scm(SP_RUNE_FROST);
};

SCM Script_spellRuneShock()
{
  return gh_int2scm(SP_RUNE_SHOCK);
};

SCM Script_spellRuneBlast()
{
  return gh_int2scm(SP_RUNE_BLAST);
};

SCM Script_spellRuneDeath()
{

  return gh_int2scm(SP_RUNE_DEATH);
};

SCM Script_spellRuneMark()
{
  return gh_int2scm(SP_RUNE_MARK);
};

SCM Script_spellBuildDirector()
{
  return gh_int2scm(SP_BUILD_DIRECTOR);	/*110*/
};

SCM Script_spellChaosPool()
{
  return gh_int2scm(SP_CHAOS_POOL);
};

SCM Script_spellBuildBulletWall()
{
  return gh_int2scm(SP_BUILD_BWALL);
};

SCM Script_spellBuildLightningWall()
{
  return gh_int2scm(SP_BUILD_LWALL);
};

SCM Script_spellBuildFireballWall()
{
  return gh_int2scm(SP_BUILD_FWALL);
};

SCM Script_spellRuneMagic()
{
  return gh_int2scm(SP_RUNE_MAGIC);		/*115*/
};

SCM Script_spellRuneDrainSP()
{
  return gh_int2scm(SP_RUNE_DRAINSP);
};

SCM Script_spellRuneAntimagic()
{
  return gh_int2scm(SP_RUNE_ANTIMAGIC);
};

SCM Script_spellRuneTransferrence()
{
  return gh_int2scm(SP_RUNE_TRANSFER);
};

SCM Script_spellTransfer()
{
  return gh_int2scm(SP_TRANSFER);
};

SCM Script_spellMagicDrain()
{
  return gh_int2scm(SP_MAGIC_DRAIN);		/*120*/
};

SCM Script_spellCounterspell()
{
  return gh_int2scm(SP_COUNTER_SPELL);
};

SCM Script_spellDispelRune()
{
  return gh_int2scm(SP_DISPEL_RUNE);
};

SCM Script_spellCureConfusion()
{
  return gh_int2scm(SP_CURE_CONFUSION);
};

SCM Script_spellRestoration()
{
  return gh_int2scm(SP_RESTORATION);
};

SCM Script_spellSummonEvilMonster()
{
  return gh_int2scm(SP_SUMMON_EVIL_MONST);	/*125*/
};

SCM Script_spellCounterwall()
{
  return gh_int2scm(SP_COUNTERWALL);
};

SCM Script_spellCauseLightWounds()
{
  return gh_int2scm(SP_CAUSE_LIGHT);
};

SCM Script_spellCauseMediumWounds()
{
  return gh_int2scm(SP_CAUSE_MEDIUM);
};

SCM Script_spellCauseHeavyWounds()
{
  return gh_int2scm(SP_CAUSE_HEAVY);
};

SCM Script_spellCharm()
{
  return gh_int2scm(SP_CHARM);		/*130*/
};

SCM Script_spellBanishment()
{
  return gh_int2scm(SP_BANISHMENT);
};

SCM Script_spellCreateMissile()
{
  return gh_int2scm(SP_CREATE_MISSILE);
};

SCM Script_spellShowInvisible()
{
  return gh_int2scm(SP_SHOW_INVIS);
};

SCM Script_spellXRay()
{
  return gh_int2scm(SP_XRAY);
};

SCM Script_spellPacify()
{
  return gh_int2scm(SP_PACIFY);		/*135*/
};

SCM Script_spellSummonFog()
{
  return gh_int2scm(SP_SUMMON_FOG);
};

SCM Script_spellSteambolt()
{
  return gh_int2scm(SP_STEAMBOLT);
};

SCM Script_spellCommandUndead()
{
  /* lots of new cleric spells);many need MULTIPLE_GODS defined to be
   * very usefull - b.t. */
  return gh_int2scm(SP_COMMAND_UNDEAD);
};

SCM Script_spellHolyOrb()
{
  return gh_int2scm(SP_HOLY_ORB);
};

SCM Script_spellSummonAvatar()
{
  return gh_int2scm(SP_SUMMON_AVATAR);	/*140*/
};

SCM Script_spellHolyPossession()
{
  return gh_int2scm(SP_HOLY_POSSESSION);
};

SCM Script_spellBless()
{
  return gh_int2scm(SP_BLESS);
};

SCM Script_spellCurse()
{
  return gh_int2scm(SP_CURSE);
};

SCM Script_spellRegeneration()
{
  return gh_int2scm(SP_REGENERATION);
};

SCM Script_spellConsecrate()
{
  return gh_int2scm(SP_CONSECRATE);		/*145*/
};

SCM Script_spellSummonCultMonsters()
{
  return gh_int2scm(SP_SUMMON_CULT);
};

SCM Script_spellCauseCriticalWounds()
{
  return gh_int2scm(SP_CAUSE_CRITICAL);
};

SCM Script_spellHolyWrath()
{
  return gh_int2scm(SP_HOLY_WRATH);
};

SCM Script_spellRetribution()
{
  return gh_int2scm(SP_RETRIBUTION);
};

SCM Script_spellFingerDeath()
{
  return gh_int2scm(SP_FINGER_DEATH);	/*150*/
};

SCM Script_spellInsectPlague()
{
  return gh_int2scm(SP_INSECT_PLAGUE);
};

SCM Script_spellHolyServant()
{
  return gh_int2scm(SP_HOLY_SERVANT);
};

SCM Script_spellWallOfThorns()
{
  return gh_int2scm(SP_WALL_OF_THORNS);
};

SCM Script_spellStaffToSnake()
{
  return gh_int2scm(SP_STAFF_TO_SNAKE);
};

SCM Script_spellLight()
{
  return gh_int2scm(SP_LIGHT);		/*155*/
};

SCM Script_spellDarkness()
{
  return gh_int2scm(SP_DARKNESS);
};

SCM Script_spellNightfall()
{
  return gh_int2scm(SP_NIGHTFALL);
};

SCM Script_spellDaylight()
{

  return gh_int2scm(SP_DAYLIGHT);
};

SCM Script_spellSunSpear()
{
  return gh_int2scm(SP_SUNSPEAR);
};

SCM Script_spellFaeryFire()
{
  return gh_int2scm(SP_FAERY_FIRE);		/*160*/
};

SCM Script_spellCureBlindness()
{
  return gh_int2scm(SP_CURE_BLINDNESS);
};

SCM Script_spellDarkVision()
{
  return gh_int2scm(SP_DARK_VISION);
};

SCM Script_spellBulletSwarm()
{

  return gh_int2scm(SP_BULLET_SWARM);
};

SCM Script_spellBulletStorm()
{
  return gh_int2scm(SP_BULLET_STORM);
};

SCM Script_spellCauseManyWounds()
{
  return gh_int2scm(SP_CAUSE_MANY);		/*165*/
};

SCM Script_spellSmallSnowstorm()
{
  return gh_int2scm(SP_S_SNOWSTORM);
};

SCM Script_spellMediumSnowstorm()
{
  return gh_int2scm(SP_M_SNOWSTORM);
};

SCM Script_spellLargeSnowstorm()
{
  return gh_int2scm(SP_L_SNOWSTORM);
};

SCM Script_spellCureDisease()
{
  return gh_int2scm(SP_CURE_DISEASE);
};

SCM Script_spellCauseEbola()
{
  return gh_int2scm(SP_CAUSE_EBOLA);		/*170*/
};

SCM Script_spellCauseFlu()
{
  return gh_int2scm(SP_CAUSE_FLU);
};

SCM Script_spellCausePlague()
{
  return gh_int2scm(SP_CAUSE_PLAGUE);
};

SCM Script_spellCauseLeprosy()
{
  return gh_int2scm(SP_CAUSE_LEPROSY);
};

SCM Script_spellCauseSmallPox()
{
  return gh_int2scm(SP_CAUSE_SMALLPOX);
};

SCM Script_spellCausePneumonicPlague()
{
  return gh_int2scm(SP_CAUSE_PNEUMONIC_PLAGUE);/*175*/
};

SCM Script_spellCauseAnthrax()
{
  return gh_int2scm(SP_CAUSE_ANTHRAX);
};

SCM Script_spellCauseTyphoid()
{
  return gh_int2scm(SP_CAUSE_TYPHOID);
};

SCM Script_spellManaBlast()
{

  return gh_int2scm(SP_MANA_BLAST);
};

SCM Script_spellSmallManaball()
{
  return gh_int2scm(SP_S_MANABALL);
};

SCM Script_spellMediumManaball()
{
  return gh_int2scm(SP_M_MANABALL);		/*180*/
};

SCM Script_spellLargeManaball()
{
  return gh_int2scm(SP_L_MANABALL);
};

SCM Script_spellManabolt()
{
  return gh_int2scm(SP_MANA_BOLT);
};

SCM Script_spellDancingSword()
{
  return gh_int2scm(SP_DANCING_SWORD);
};

SCM Script_spellAnimateWeapon()
{
  return gh_int2scm(SP_ANIMATE_WEAPON);
};

SCM Script_spellCauseCold()
{
  return gh_int2scm(SP_CAUSE_COLD);              /* 185 */
};

SCM Script_spellDivineShock()
{
  return gh_int2scm(SP_DIVINE_SHOCK);
};

SCM Script_spellWindStorm()
{
  return gh_int2scm(SP_WINDSTORM);
};

SCM Script_spellSanctuary()
{
  /* the below NIY */
  return gh_int2scm(SP_SANCTUARY);
};

SCM Script_spellPeace()
{
  return gh_int2scm(SP_PEACE);
};

SCM Script_spellSpiderWeb()
{
  return gh_int2scm(SP_SPIDERWEB);             /* 190 */
};

SCM Script_spellConflict()
{
  return gh_int2scm(SP_CONFLICT);
};

SCM Script_spellRage()
{
  return gh_int2scm(SP_RAGE);
};

SCM Script_spellForkedLightning()
{
  return gh_int2scm(SP_FORKED_LIGHTNING);
};

SCM Script_spellPoisonFog()
{
  return gh_int2scm(SP_POISON_FOG);
};

SCM Script_spellFlameAura()
{
  return gh_int2scm(SP_FLAME_AURA);            /* 195 */
};

SCM Script_spellVitriol()
{
  return gh_int2scm(SP_VITRIOL);
};

SCM Script_spellVitriolSplash()
{
  return gh_int2scm(SP_VITRIOL_SPLASH);
};

SCM Script_spellIronwoodSkin()
{
  return gh_int2scm(SP_IRONWOOD_SKIN);
};

SCM Script_spellWrathfullEye()
{
  return gh_int2scm(SP_WRATHFUL_EYE);                             /* 200 */
};
