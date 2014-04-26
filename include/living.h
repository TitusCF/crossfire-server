/**
 * @file
 * Structure containing object statistics.
 */

#ifndef LIVING_H
#define LIVING_H

/** Object statistics. */
enum {
    STRENGTH = 0,
    DEXTERITY = 1,
    CONSTITUTION = 2,
    WISDOM = 3,
    CHARISMA = 4,
    INTELLIGENCE = 5,
    POWER = 6,
    NUM_STATS = 7  /**< Number of statistics. */
};

/** Maximum level a player can reach. */
#define MAXLEVEL 115

extern const char *const attacks[NROFATTACKS];

extern const char *const restore_msg[NUM_STATS];
extern const char *const statname[NUM_STATS];
extern const char *const short_stat_name[NUM_STATS];
extern const char *const lose_msg[NUM_STATS];

/**
 * Various statistics of objects.
 */
typedef struct liv {
    int8_t         Str, Dex, Con, Wis, Cha, Int, Pow;
    int8_t         wc;         /**< Weapon Class, how skilled, the lower the better. */
    int8_t         ac;         /**< Armour Class, how hard to hit, the lower the better. */
    int8_t         luck;       /**< Affects thaco and ac from time to time */
    int16_t        hp;         /**< Hit Points. */
    int16_t        maxhp;      /**< Max hit points. */
    int16_t        sp;         /**< Spell points.  Used to cast mage spells. */
    int16_t        maxsp;      /**< Max spell points. */
    int16_t        grace;      /**< Grace.  Used to invoke clerical prayers. */
    int16_t        maxgrace;   /**< Maximum grace.  Used to invoke clerical prayers. */
    int16_t        dam;        /**< How much damage this object does when hitting */
    int64_t        exp;        /**< Experience.  Killers gain 1/10. */
    int32_t        food;       /**< How much food in stomach.  0 = starved. */
} living;

int get_cha_bonus(int stat);
int get_dex_bonus(int stat);
int get_thaco_bonus(int stat);
uint32_t get_weight_limit(int stat);
int get_learn_spell(int stat);
int get_cleric_chance(int stat);
int get_turn_bonus(int stat);
int get_dam_bonus(int stat);
float get_speed_bonus(int stat);
int get_fear_bonus(int stat);

#endif /* LIVING_H */
