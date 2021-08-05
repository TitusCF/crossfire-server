/**
 * @file
 * Characters associated with an account.n
 */

#ifndef ACCOUNT_CHAR_H
#define ACCOUNT_CHAR_H

/**
 * One character account.
 */
typedef struct account_char_struct {
    sstring name;             /** < Name of this character/player */
    sstring character_class;  /** < Class of this character */
    sstring race;             /** < Race of this character */
    uint8_t level;            /** < Level of this character */
    sstring face;             /** < Face of this character */
    sstring party;            /** < Character this party belonged to */
    sstring map;              /** < Last map this character was on */
    uint8_t isDead;           /** < Should stay at zero if alive, anything else if dead (hopefully 1, but doesn't have to be) */
    struct account_char_struct  *next;
} Account_Char;

Account_Char *account_char_remove(Account_Char *chars, const char *pl_name);
int make_perma_dead(object *op);
int unmake_perma_dead(char *account, char *player);

#endif
