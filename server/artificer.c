// artificer takes a object in a couldron 
// along with materials and gives the object a bonus
// stats +1 to +30 OR
// resist +1 to +115

/*
Ingredient List for Stat Modification:
Archs will be drawn from archs/inorganic/, archs/potions/ & archs/flesh.

(these will most likely be placeholders as archs are modified or added to /archs 
 
-stat modifiers should always use permanent stat potions- 

Stat potion requirement should be only 1/5th of the required number of other ingredients)

Special_ac:       bat_wing, phil_salt, vial_yellow
Special_wc:       hand, phil_salt, vial_yellow 
Special_dam:      insect_stinger, phil_salt, vial_yellow
Special_speed:    foot, phil_salt, vial_water 
Special_luck:     serp_skin, phil_salt, vial_green 
Special_hp:       heart, phil_salt, vial_red
Special_maxhp:    heart, phil_sulpher, vial_red 
Special_sp:       brain, phil_salt, vial_magenta
Special_maxsp:    brain, phil_sulpher, vial_magenta
Special_grace:    dragon_eye, phil_dust, vial_magenta 
Special_maxgrace: dragon_eye, phil_sulpher, vial_magenta 
Special_exp:      eyes, phil_salt, potion_improve
Special_food:     tongue, phil_salt, water 

0 Resist_Physical:      dragon_eye,        uraniumpile, potion_shielding

1 Resist_Magic:         tongue,            uraniumpile, potion_magic
2 Resist_Fire:          hide_black,        uraniumpile, potion_fire
3 Resist_Electricity:   hand,              uraniumpile, potion_heroism
4 Resist_Cold:          hide_white,        uraniumpile, potion_cold2
5 Resist_Confusion:     brain,             uraniumpile, minor_potion_restoration
6 Resist_Acid:          icor,              uraniumpile, vial_yellow
7 Resist_Drain:         heart,             uraniumpile, vial_red
9 Resist_Ghosthit:      ectoplasm,         uraniumpile, potion_aethereality
10 Resist_Poison:        liver,             uraniumpile, vial_green
11 Resist_Slow:          foot,              river_stone, vial_water
12 Resist_Paralyze:      insect_stinger,    uraniumpile, vial_green
13 Resist_Turn_Undead:   tooth,             uraniumpile, vial_red
14 Resist_Fear:          demon_head,        uraniumpile, potion_heroism
16 Resist_Deplete:       heart,             uraniumpile, vial_red 
17 Resist_Death:         head,              uraniumpile, vial_red
21 Resist_Holyword:      bat_wing,          uraniumpile, vial_water
22 Resist_blind:         eye,               uraniumpile, potion_empty
24 Resist_Life_Stealing: skin,              uraniumpile, vial_red
25 Resist_Disease:       residue,           uraniumpile, vial_green

* we're going to keep the same list for now, except for artificer which will use
STR: potionstr, demon_head, ruby
DEX: potiondes, demon_head, sapphire
POW: potionpow, demon_head, amethyst
INT: potionint, demon_head, mithril
WIS: potionwis, demon_head, diamond
CHA: potioncha, demon_head, smallnugget
CON: potioncon, demon_head, emerald
*/

// you can also merge items
//so you have a long sword
//you want +2 str and +2 dex
//you have to make two swords
//modify them both and then merge them

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <global.h>
#include <object.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <skills.h>
#include <spells.h>

static object* find_base(const object* cauldron) {
    enum object_type bases[] = {AMULET, RING, WEAPON, ARMOUR, SHIELD, BOW, HELMET, CLOAK};
    for (int i = 0; i < sizeof(bases)/sizeof(enum object_type); i++) {
        object* base_item = object_find_by_type(cauldron, bases[i]);
        if (base_item != NULL) {
            return base_item;
        }
    }
    return NULL;
}

static object* find_secondary(const object* cauldron, const object* base_item) {
    object* tmp;
    for (tmp = cauldron->inv; tmp; tmp = tmp->below) {
        if (base_item->type == tmp->type && tmp != base_item) {
            return tmp;
        }
    }
    return NULL;
}

static bool merge_should_succeed(const object* op, const object* base,
                                 const object* result, const object* secondary,
                                 const int level) {
    if (QUERY_FLAG(op, FLAG_WIZ)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
                      "Note: Merging items as DM always succeeds.");
        return true;
    }

    int result_item_power = calc_item_power(result);
    float chance = (1.0*level * result_item_power) / base->item_power;
    int chance_int = chance * 100;
    chance_int = MIN(MAX(chance_int, 5), 60); // min 5, max 60
    LOG(llevDebug, "merge: result %d, level %d, chance %f gives: %d%\n",
        result_item_power, level, chance, chance_int);
    if (rndm(0, 100) <= chance_int) {
        return true;
    } else {
        return false;
    }
}

static int attempt_merge(const object* caster, object* cauldron,
                          object* base_item, const int level) {
    object* merge_item = find_secondary(cauldron, base_item);
    if (merge_item == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, caster, MSG_TYPE_SKILL,
                      MSG_TYPE_SKILL_ERROR,
                      "You need two base items of the same type to merge.");
        return 0;
    }

    // Create merged result. Merged result is needed to determine if the merge
    // actually succeeds or not.
    object* merge_result = object_new();
    object_copy(base_item, merge_result);

    merge_result->stats.Str += merge_item->stats.Str;
    merge_result->stats.Dex += merge_item->stats.Dex;
    merge_result->stats.Con += merge_item->stats.Con;
    merge_result->stats.Wis += merge_item->stats.Wis;
    merge_result->stats.Cha += merge_item->stats.Cha;
    merge_result->stats.Int += merge_item->stats.Int;
    merge_result->stats.Pow += merge_item->stats.Pow;
    merge_result->stats.ac += merge_item->stats.ac;
    merge_result->stats.luck += merge_item->stats.luck;
    merge_result->stats.hp += merge_item->stats.hp;
    merge_result->stats.maxhp += merge_item->stats.maxhp;
    merge_result->stats.grace += merge_item->stats.grace;
    merge_result->stats.maxgrace += merge_item->stats.maxgrace;
    int l = 0;

    for (l = 0; l < NROFATTACKS; l++) {
        // merge resists
        merge_result->resist[l] += merge_item->resist[l];
    }

    bool success = false;
    if (merge_should_succeed(caster, base_item, merge_result, merge_item, level)) {
        base_item->stats.Str += merge_item->stats.Str;
        base_item->stats.Dex += merge_item->stats.Dex;
        base_item->stats.Con += merge_item->stats.Con;
        base_item->stats.Wis += merge_item->stats.Wis;
        base_item->stats.Cha += merge_item->stats.Cha;
        base_item->stats.Int += merge_item->stats.Int;
        base_item->stats.Pow += merge_item->stats.Pow;
        base_item->stats.ac += merge_item->stats.ac;
        base_item->stats.luck += merge_item->stats.luck;
        base_item->stats.hp += merge_item->stats.hp;
        base_item->stats.maxhp += merge_item->stats.maxhp;
        base_item->stats.grace += merge_item->stats.grace;
        base_item->stats.maxgrace += merge_item->stats.maxgrace;
        int l = 0;

        for (l = 0; l < NROFATTACKS; l++) {
            // merge resists
            base_item->resist[l] += merge_item->resist[l];
        }

        // Make custom base item name merged object's new name.
        if (base_item->custom_name != NULL) {
            FREE_AND_COPY(base_item->name, base_item->custom_name);
            FREE_AND_CLEAR_STR(base_item->custom_name);
        }

        char buf[MAX_BUF];
        snprintf(buf, sizeof(buf), "This artifact %s was made by %s.", base_item->name, caster->name);
        object_set_msg(base_item, buf);

        draw_ext_info(NDI_UNIQUE, 0, caster, MSG_TYPE_SKILL,
                      MSG_TYPE_SKILL_SUCCESS,
                      "You successfully merged the items.");
        success = true;
    } else {
        draw_ext_info(NDI_UNIQUE, 0, caster, MSG_TYPE_SKILL,
                      MSG_TYPE_SKILL_FAILURE,
                      "Your items failed to merge and items were destroyed in the process.");
    }

    object_decrease_nrof(merge_result, 1);
    object_decrease_nrof(merge_item, 1); // decrease the stack size.
    SET_FLAG(cauldron, FLAG_APPLIED); // caller uses this to send client update

    if (success) {
        return 1;
    } else {
        return 0;
    }
}

int attempt_do_artificer(object* caster, object* cauldron, object* base_item, int k) {
    int stat_improve[] = {0,    3,    12,   27,   48,   75,   108,  147,
                          192,  243,  300,  363,  432,  507,  588,  675,
                          768,  867,  972,  1083, 1200, 1323, 1452, 1587,
                          1728, 1875, 2028, 2187, 2352, 2523, 2700};
    int success = FALSE;
    int atmpt_bonus = 0; // how much of a bonus we are attempting.
    object *potion; // the potion item we are using to craft
    object *inorganic; // the inorganic item we are using to craft
    object *flesh; // the flesh item we are using to craft
    // now that we have our base_item set we need to pick a stat to improve depending on the
    // type of inorganic in the cauldron (forge)
    potion = object_find_by_type(cauldron, POTION);
    inorganic = object_find_by_type(cauldron, INORGANIC);
    flesh = object_find_by_type(cauldron, FLESH);

    // do a string search to see what type of stat is being improved.
    if (potion == NULL || inorganic == NULL ||
        flesh == NULL) { /* failure--no type found */
        draw_ext_info(NDI_UNIQUE, 0, caster, MSG_TYPE_SKILL,
                      MSG_TYPE_SKILL_ERROR,
                      "No potion, inorganic, or flesh.");
        return 0;
    }

    // level zero is +0, start at +1 bonus
    for(size_t i = 1; i < 31; i++) {
        if(potion->nrof >= stat_improve[i] / 5) { // use our list of needed mats to improve stats
            atmpt_bonus = i; // potions use 1/5th the requirements.
        } 
        // use our list of needed mats to improve stats but dont go over the current atmpt_bonus
        if(inorganic->nrof >= stat_improve[i] && i < atmpt_bonus) { 
            atmpt_bonus = i; 
        }
        if(flesh->nrof >= stat_improve[i] && i < atmpt_bonus) { 
            atmpt_bonus = i; 
        }
        if(i > atmpt_bonus){
            break; // once we hit our max atmpt_bonus we can break out.
        }
    }
    int success_chance = k - (atmpt_bonus * 2);
    if(rndm(0, 100) <= success_chance) {
        // do nothing
    } 
    else {
        atmpt_bonus = atmpt_bonus * -1; // flip to a negative bonus, caster recieves items either way.
    }

    // have all the ingredients necessary. 
    if(strcmp("potionstr", potion->name) && strcmp("demon_head", flesh->name) && strcmp("ruby", inorganic->name)) {
        base_item->stats.Str = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potiondes", potion->name) && strcmp("demon_head", flesh->name) && strcmp("sapphire", inorganic->name)) {
        base_item->stats.Dex = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potionpow", potion->name) && strcmp("demon_head", flesh->name) && strcmp("amethyst", inorganic->name)) {
        base_item->stats.Pow = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potionint", potion->name) && strcmp("demon_head", flesh->name) && strcmp("mithril", inorganic->name)) {
        base_item->stats.Int = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potionwis", potion->name) && strcmp("demon_head", flesh->name) && strcmp("diamond", inorganic->name)) {
        base_item->stats.Wis = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potioncha", potion->name) && strcmp("demon_head", flesh->name) && strcmp("smallnugget", inorganic->name)) {
        base_item->stats.Cha = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potioncon", potion->name) && strcmp("demon_head", flesh->name) && strcmp("emerald", inorganic->name)) {
        base_item->stats.Con = atmpt_bonus;
        success = TRUE; 
        }
    // end base stats part.
    else if(strcmp("vial_yellow", potion->name) && strcmp("bat_wing", flesh->name) && strcmp("phil_salt", inorganic->name)) {
        base_item->stats.ac = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_yellow", potion->name) && strcmp("hand", flesh->name) && strcmp("phil_salt", inorganic->name)) {
         base_item->stats.wc = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_yellow", potion->name) && strcmp("insect_stinger", flesh->name) && strcmp("phil_salt", inorganic->name)) {
        base_item->stats.dam = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_green", potion->name) && strcmp("serp_skin", flesh->name) && strcmp("phil_salt", inorganic->name)) {
        base_item->stats.luck = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_red", potion->name) && strcmp("heart", flesh->name) && strcmp("phil_salt", inorganic->name)) {
        base_item->stats.hp = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_red", potion->name) && strcmp("heart", flesh->name) && strcmp("phil_sulpher", inorganic->name)) {
        base_item->stats.maxhp = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_magenta", potion->name) && strcmp("brain", flesh->name) && strcmp("phil_salt", inorganic->name)) {
        base_item->stats.sp = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_magenta", potion->name) && strcmp("brain", flesh->name) && strcmp("phil_sulpher", inorganic->name)) {
        base_item->stats.maxsp = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_magenta", potion->name) && strcmp("dragon_eye", flesh->name) && strcmp("phil_dust", inorganic->name)) {
        base_item->stats.grace = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_magenta", potion->name) && strcmp("dragon_eye", flesh->name) && strcmp("phil_sulpher", inorganic->name)) {
        base_item->stats.maxgrace = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_improve", potion->name) && strcmp("eyes", flesh->name) && strcmp("phil_salt", inorganic->name)) {
        base_item->stats.exp = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("water", potion->name) && strcmp("tongue", flesh->name) && strcmp("phil_salt", inorganic->name)) {
        base_item->stats.food = atmpt_bonus;
        success = TRUE; 
        }
    // Start resistances 
    else if(strcmp("potion_shielding", potion->name) && strcmp("dragon_eye", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->stats.ac = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_magic", potion->name) && strcmp("dragon_eye", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[1] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_fire", potion->name) && strcmp("hide_black", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[2] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_heroism", potion->name) && strcmp("hand", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[3] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_heroism", potion->name) && strcmp("hide", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[4] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_cold2", potion->name) && strcmp("hide_white", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[5] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("minor_potion_restoration", potion->name) && strcmp("brain", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[6] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_yellow", potion->name) && strcmp("icor", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[7] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_red", potion->name) && strcmp("heart", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[9] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_aethereality", potion->name) && strcmp("ectoplasm", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[10] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_green", potion->name) && strcmp("liver", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[11] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_water", potion->name) && strcmp("foot", flesh->name) && strcmp("river_stone", inorganic->name)) {
        base_item->resist[12] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_green", potion->name) && strcmp("insect_stinger", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[13] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_red", potion->name) && strcmp("tooth", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[14] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_heroism", potion->name) && strcmp("demon_head", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[14] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_empty", potion->name) && strcmp("heart", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[16] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_red", potion->name) && strcmp("head", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[17] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_water", potion->name) && strcmp("bat_wing", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[21] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("potion_empty", potion->name) && strcmp("eye", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[22] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_green", potion->name) && strcmp("skin", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[24] = atmpt_bonus;
        success = TRUE; 
        }
    else if(strcmp("vial_green", potion->name) && strcmp("residue", flesh->name) && strcmp("uraniumpile", inorganic->name)) {
        base_item->resist[25] = atmpt_bonus;
        success = TRUE; 
    }
    
    // if we craft ANY object reduce the stack sizes by an appropriate amount.
    if(success) {
        object_decrease_nrof(potion, MAX(1, stat_improve[abs(atmpt_bonus)] / 5)); // decreaase the stack size taking into account 1/5th requirements
        object_decrease_nrof(inorganic, stat_improve[abs(atmpt_bonus)]); // decrease the stack size.
        object_decrease_nrof(flesh, stat_improve[abs(atmpt_bonus)]); // decrease the stack size.
        SET_FLAG(cauldron, FLAG_APPLIED); // not sure we need this but i don't think it hurts.
        if(atmpt_bonus > 0) { 
            draw_ext_info(NDI_UNIQUE, 0, caster, MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
                            "You successfully crafted the item.");
            return 1;
        }
        //we created it but it failed.
        else 
        {
            draw_ext_info(NDI_UNIQUE, 0, caster, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
                            "You failed to craft the item.");
            return 0;
        }
    }
    return 0;
}

static void artificer_help(object* op) {
    draw_ext_info_format(
        NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
        "You can:\nuse_skill artificer craft\nuse_skill artificer merge");
}

static object* find_cauldron(object* op) {
    object *unpaid_cauldron = NULL;
    FOR_MAP_PREPARE(op->map, op->x, op->y, tmp) {
        if (QUERY_FLAG(tmp, FLAG_IS_CAULDRON)) {
            if (QUERY_FLAG(tmp, FLAG_UNPAID)) {
                unpaid_cauldron = tmp;
                continue;
            } else {
                return tmp;
            }
        }
    } FOR_MAP_FINISH();

    if (unpaid_cauldron) {
        char name[MAX_BUF];
        query_base_name(unpaid_cauldron, 0, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL,
                             MSG_TYPE_SKILL_ERROR,
                             "You must pay for your %s first!", name);
    }

    return NULL;
}

int use_artificer(object* op, const char* args) {
    enum mode { DO_ARTIFICER, DO_MERGE };
    enum mode m;
    if (args == NULL) {
        m = DO_ARTIFICER;
    } else if (strcmp(args, "merge") == 0) {
        m = DO_MERGE;
    } else {
        artificer_help(op);
        return 0;
    }

    object* cauldron = find_cauldron(op);
    if (cauldron == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
                      "You need a cauldron for that!");
        return 0;
    }

    object* base_item = find_base(cauldron);
    if (base_item == NULL) {
        draw_ext_info(
            NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
            "You need to put in a base item to use artificer on this forge.");
        return 0;
    }

    int level = find_skill_by_number(op, SK_ARTIFICER)->level;
    int k = MIN(100, level); // minimum between 100 and skill

    int ret = 0;
    if (m == DO_ARTIFICER) {
        ret = attempt_do_artificer(op, cauldron, base_item, k);
    } else if (m == DO_MERGE) {
        ret = attempt_merge(op, cauldron, base_item, level);
    }

    if (QUERY_FLAG(cauldron, FLAG_APPLIED)) {
        esrv_send_inventory(op, cauldron);
    }

    return ret;
}
