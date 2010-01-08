/*
 * static char *rcsid_acount_c =
 *   "$Id: alchemy.c 12219 2009-10-24 21:28:41Z akirschbaum $";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2010 Mark Wedel & Crossfire Development Team

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/


/**
 * @file
 * This file contains account management logic - creation, deletion,
 * log in verification, as well as associating player files with the
 * account.
 *
 * The code in this file mostly assumes that the number of account operations
 * will be low - for example, we load up all acounts in a linked list, and save
 * them all out.  If we have 10,000 accounts, this would be very slow.  But for
 * a few hundred accounts, should be plenty fast on most any hardware.
 *
 * Likewise, the account structure is not exposed outside this file - this means
 * that account access/updates is done through the account name.  This adds some
 * extra overhead in that code has to search the linked list, but for a relatively
 * low number of entries, and relatively infrequent updates, this should not be much
 * of an issue.
 * 
 * Since the accounts file is updated in several places in this file,
 * it is documented here.  The file is a list of accounts, one line
 * per account, with fields separted by colons, eg:
 *
 * Account name:Password:Account last used:Characters (semicolon separated):expansion
 * mwedel@sonic.net:mypassword:1260686494:Mark;MarkW;:
 *
 * none of the the fields listed could contain colons, and player names can not contain
 * semicolon, as that is used to separate those.  In addition, everything is limited to
 * printable characters.
 * The accounts file is designed to be human readable, even if it is not expected that
 * a human will read it.
 * The passwords are stored in whatever crypt_string() returns.  On some systems, this
 * may be plaintext.
 * expansion (last field) is there for possible expansion.  There may be desire to store
 * bits of information there.  An example might be dm=1.  But it is there for expansion.
 */

#include <global.h>
#include <object.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/**
 * The maximum characters per account is really driven by the size of
 * the buffer we use to read in the data.  Take 150 characters off for
 * the account name, password, overhead, and other wiggle room and
 * find a maximum.  From my quick calculations, this amounts to
 * 18 characters/account.  I think that is sufficient - moving to a
 * HUGE_BUF would allow 82.
 * The code could be more clever and look at the length of each
 * character name and total it up, but having the same limit for everyone
 * is better IMO.
 */
#define MAX_CHARACTERS_PER_ACCOUNT (VERY_BIG_BUF - 150) / (MAX_NAME+1)

/* Number of fields in the accounts file.  These are colon seperated */
#define NUM_ACCOUNT_FIELDS 5

/**
 * Structure that holds account data.  This is basically in game representation
 * of the data store in the file above.
 * Note that there is no field here for the expansion area - if that gets used,
 * then almost certainly the values will be extracted and stored into new fields
 * in this structure, and not just a character that contains the data in an
 * unprocessed form, eg, a int dm field could get added.
 * all char* data here is from strdup_local(), and thus the shared string
 * comparisons/functions should not be used on it.
 */

typedef struct account_struct {
    char  *name;                    /** Account name */
    char  *password;                /** Password for this account */
    time_t last_login;              /** Last time this account was logged in */
    int	  num_characters;           /** Number of characters on this account */
    char  *character_names[MAX_CHARACTERS_PER_ACCOUNT+1];
                                    /** character names associated with this account */
    struct account_struct *next;    /** Next in list */
} account_struct;

/**
 * list of all accounts.
 */
static account_struct *accounts=NULL;

/**
 * Name of the accounts file.  I can not ever see a reason why this
 * name would not work, but may as well still make it easy to change it.
 */
#define ACCOUNT_FILE "accounts"

/**
 * This is used purely by the test harness - by clearing the accounts,
 * it can then verify that the data is added is loaded back into memory
 * properly.  As such, we don't worry about memory cleanup, etc.
 */
void clear_accounts() {
    accounts = NULL;
}

/**
 * This loads all the account entries into memory.  It is presumed to only
 * be called once during the program startup.
 */
void account_load_entries()
{
    char fname[MAX_BUF], buf[VERY_BIG_BUF];
    FILE *fp;
    account_struct *ac, *last=NULL;

    if (accounts != NULL) {
        LOG(llevError, "account_load_entries(): Called when accounts has been set.\n");
        return;
    }
    snprintf(fname, MAX_BUF,"%s/%s", settings.localdir, ACCOUNT_FILE);
    fp=fopen(fname,"r");
    if (!fp) {
        /* This may not in fact be a critical error - on a new server,
         * the accounts file may not yet exist.
         */
        LOG(llevInfo,"Warning: Unable to open %s\n");
        return;
    }
    while (fgets(buf, VERY_BIG_BUF, fp)) {
        char *tmp[NUM_ACCOUNT_FIELDS], *cp;
        int result, i;

        /* Ignore any comment lines */
        if (buf[0] == '#') continue;

        /* remove newline */
        cp = strchr(buf, '\n');
        if (cp) *cp='\0';

        if (split_string(buf, tmp, NUM_ACCOUNT_FIELDS, ':') != NUM_ACCOUNT_FIELDS) {
            LOG(llevError,"Corrupt entry in %s: %s\n", fname, buf);
            continue;
        }
        ac = malloc(sizeof(account_struct));
        ac->name = strdup_local(tmp[0]);
        ac->password = strdup_local(tmp[1]);
        ac->last_login = strtoul(tmp[2], (char**)NULL, 10);
        ac->next = NULL;

        /* count up how many semicolons - this is the character
         * seperator.  We start at one, because these are seperators,
         * so there will be one more name than seperators.
         */
        ac->num_characters=1;
        for (cp = tmp[3]; *cp != '\0'; cp++) {
            if (*cp == ';') ac->num_characters++;
        }

        result = split_string(tmp[3], ac->character_names, ac->num_characters, ';');
        /* This should never happen, but check for it.  Even if we do get it, not necessarily
         * a critical error - this is why we use calloc above.
         */
        if (result != ac->num_characters) {
            LOG(llevError, "account_load_entries: split_string found different number of characters: %d != %d\n",
                result, ac->num_characters);
        }

        if (ac->num_characters > MAX_CHARACTERS_PER_ACCOUNT) {
            LOG(llevError,"account_load_entries: Too many characters set for account %s - truncating to %d\n",
                ac->name, MAX_CHARACTERS_PER_ACCOUNT);
                ac->num_characters = MAX_CHARACTERS_PER_ACCOUNT;
        }

        /* The string data that the names are stored in is currently temporary data
         * that will go away, so we need to allocate some permanent data
         * now.  Also, if we have a NULL value, means we got the error above -
         * NULL values would only be at the end of the split, so just reduce
         * the character count.
         */
        for (i=0; i<ac->num_characters; i++) {
            if (ac->character_names[i] != NULL) {
                ac->character_names[i] = strdup_local(ac->character_names[i]);
            } else {
                ac->num_characters = i;
                break;
            }
        }
        /* NULL terminate - in that way, we can just return ac->character_names to
         * callers that want to know all characters associated with an account,
         * and it can just iterate until it gets the NULL terminator.
         */
        ac->character_names[ac->num_characters] = NULL;

        /* We tack on to the end of the list - in this way,
         * the order of the file remains the same.
         */
        if (last)
            last->next = ac;
        else
            accounts = ac;
        last = ac;
            
    }
    fclose(fp);
}

/**
 * This writes a single account entry to the given filepointer.
 * it is used both when saving all accounts, or if we just need
 * to append a new account to the end of the file.
 * @param fp
 * file pointer to write to.
 * @param ac
 * account structure to write out.
 */
static void account_write_entry(FILE *fp, account_struct *ac)
{
    int i;

    fprintf(fp,"%s:%s:%u:", ac->name, ac->password, (uint32)ac->last_login);
    for (i=0; i<ac->num_characters; i++) {
        if (i != 0)
            fprintf(fp,";%s", ac->character_names[i]);
        else
            fprintf(fp,"%s", ac->character_names[i]);
    }
    fprintf(fp,":\n");
}


/**
 * Save all the account information.  Since the data is stored in a flat
 * file, if an update is needed for an account, the only way to save updates
 * is to save all the data - there isn't an easy way to just add another player
 * name for an account.
 */
void accounts_save()
{
    char fname[MAX_BUF], fname1[MAX_BUF];;
    FILE *fp;
    account_struct *ac;

    snprintf(fname, MAX_BUF,"%s/%s.new", settings.localdir, ACCOUNT_FILE);

    fp = fopen(fname,"w");
    if (fp == NULL) {
        char err[MAX_BUF];

        LOG(llevError, "Cannot open accounts file %s: %s\n", fname,
            strerror_local(errno, err, sizeof(err)));
        return;
    }

    fprintf(fp, "# This file should not be edited while the server is running.\n");
    fprintf(fp, "# Otherwise, any changes made may be overwritten by the server\n");
    fprintf(fp, "# Format:\n");
    fprintf(fp, "# Account name:Password:Account last used:Characters (semicolon separated):expansion\n");
    for (ac=accounts; ac; ac=ac->next) {
                account_write_entry(fp, ac);
    }
    fclose(fp);

    /* We write to a new file name - in that way, if we crash while writing the file,
     * we are not left with a corrupted file.  So now we need to rename it to the
     * file to use.
     */
    snprintf(fname1, MAX_BUF,"%s/%s", settings.localdir, ACCOUNT_FILE);
    unlink(fname1);
    rename(fname, fname1);
}
	    
/**
 * Checks the existing accounts, and see if this account exists.
 * @param account_name
 * account name we are looking for.
 * @return
 * returns 1 on match, 0 on no match.
 */
int account_exists(char *account_name)
{
    account_struct *ac;

    for (ac=accounts; ac; ac=ac->next) {
        if (!strcasecmp(ac->name, account_name)) return 1;
    }
    return 0;
}

/**
 * Checks if the name and password are valid.  Note that on the return modes,
 * we do not differentiate on non existing account and wrong password -
 * one could use account_exists() if one is checking just for the name.
 * Note - if we do get a match, we update the last_login value - it is 
 * presumed that if someone knows the right accountname/password, that
 * the account is effectively getting logged in.
 * @param account_name
 * account name we are looking for.
 * @param account_password
 * password for the account.  This is the unencrypted password (password as entered
 * by user)
 * @return
 * 0 if no match/wrong password, 1 if a match is found and password matches.
 */
int account_check_name_password(char *account_name, char *account_password)
{
    account_struct *ac;

    for (ac=accounts; ac; ac=ac->next) {
        /* account names must be unique.  So if we get a matching name,
         * we want to check the password.  If the password is correct,
         * we have a good match.  If the password doesn't match, there is
         * no reason to do anymore searching on the list, because we won't
         * find another matching name.
         */
        if (!strcasecmp(ac->name, account_name)) {
            if (!strcmp(ac->password, crypt_string(account_password, ac->password))) {
                ac->last_login=time(NULL);
                return 1;
            }
            else return 0;
        }
    }
    return 0;
}

/**
 * Checks a string to make sure it does not have any invalid characters.
 * We are fairly permissive on character here - pretty much any
 * printable character that is not a : or ; is allowed.
 * @param str
 * string to check
 * @return
 * 0 if ok, 1 if we have an invalid character, 2 if string is too long.
 */
int account_check_string(const char *str)
{
    const char *cp = str;

    /* Do not allow the first character to be a space */
    if (isspace(*str)) return 1;
    for (; *str != '\0'; str++) {
        if (!isprint(*str)) return 1;
        if (*str == ':' || *str==';') return 1;
    }
    if ((str - cp) > MAX_NAME) return 2;
    return 0;
}

/**
 * Adds an account.  It does error checking, but the caller might
 * want to do checking before getting here.
 * @param account_name
 * account name we are adding
 * @param account_password
 * password for the account.  This is the unencrypted password (password as entered
 * by user)
 * @retval 0
 * account added successfully.
 * @retval 1
 * name or password has invalid character.
 * @retval 2
 * account already exists.
 */
int account_add_account(char *account_name, char *account_password)
{
    account_struct *ac;

    /* We need to check the password because we don't know what crypt_string() will do -
     * it may just return the string we pass in.  We should probably check the results
     * returned from crypt_string(), but the problem there is that if we have a faulty
     * algorithm which in fact is putting in invalid characters, there isn not much
     * the players can do about that.
     */
    if (account_check_string(account_name) || account_check_string(account_password))
        return 1;

    if (account_exists(account_name)) return 2;
 
    ac = malloc(sizeof(account_struct));
    ac->name = strdup_local(account_name);
    ac->password = strdup_local(crypt_string(account_password, NULL));
    ac->last_login = time(NULL);
    ac->num_characters = 0;
    
    memset(ac->character_names, MAX_CHARACTERS_PER_ACCOUNT+1, sizeof(char*));

    /* We put this at the top of the list.  This means recent accounts will be at
     * the top of the file, which is likely a good thing.
     * We don't do a save right now.  When the player associates a character with
     * the account, we will save them - until that point, not too much reason
     * to save this out.  Note it is still possible for this to get saved out if
     * another player does something that forces writing out of the accounts file.
     */
    ac->next = accounts;
    accounts = ac;

    return 0;
}

/**
 * Adds a player name to an account.  Player corresponds to the names used
 * in the pl object, not the person sitting at the computer.  This function
 * presumes that the caller has done the work to verify that the player
 * does in fact exist, and does any related work to update the player.  What this
 * function does is simply update the account structure.
 * @param account_name
 * name of the account we are adding this player to.
 * @param player_name
 * name of this player.
 * @retval 0
 * player name added successfully.
 * @retval 1
 * could not find account of that name.
 * @retval 2
 * number of characters on this account has reached a maximum.
 */

int account_add_player_to_account(char *account_name, char *player_name)
{
    account_struct *ac;

    for (ac=accounts; ac; ac=ac->next) {
        if (!strcasecmp(ac->name, account_name)) break;
    }
    if (ac == NULL) return 1;

    if (ac->num_characters >= MAX_CHARACTERS_PER_ACCOUNT) return 2;

    ac->character_names[ac->num_characters] = strdup_local(player_name);
    ac->num_characters++;
    /* NULL terminate, as per notes above.  In theory not necessary since data
     * is all set to NULL */
    ac->character_names[ac->num_characters] = NULL;
    return 0;
}

/**
 * Adds a player name to an account.  Player corresponds to the names used
 * in the pl object, not the person sitting at the computer.  This function
 * presumes that the caller has done the work to verify that the player
 * does in fact exist, and does any related work to update the player.  What this
 * function does is simply update the account structure.
 * @param account_name
 * name of the account we are adding this player to.
 * @param player_name
 * name of this player.
 * @retval 0
 * player name removed successfully.
 * @retval 1
 * could not find account of that name.
 * @retval 2
 * player of this name not on account.
 */

int account_remove_player_from_account(char *account_name, char *player_name)
{
    account_struct *ac;
    int i, match=0;

    for (ac=accounts; ac; ac=ac->next) {
        if (!strcasecmp(ac->name, account_name)) break;
    }
    if (ac == NULL) return 1;

    /* Try to find the character name.  Once we find it, we set match, and
     * then move the remain character names down by one.  The array is
     * always null terminated, so this also makes sure we copy the null down.
     */
    for (i=0; i<ac->num_characters; i++) {
        if (!strcasecmp(ac->character_names[i], player_name)) {
            free(ac->character_names[i]);
            match=1;
        }
        if (match == 1) {
            ac->character_names[i] = ac->character_names[i+1];
        }
    }

    if (match) {
        ac->num_characters--;
        return 0;
    }
    /* Otherwise, did not find player name */
    return 2;
}    


/**
 * Returns an array of strings for the characters on this account - the array
 * is null terminated.  In fact, it just returns the ac->character_names.
 * @param account_name
 * name of the account to get the players for.
 * @return
 * array of character names for this account.  This will return NULL in the
 * case where we can not find the account.  It will return an array with the
 * first entry being NULL in the case of a valid account with no characters added.
 * This returned data should not be altered in any way.
 */
char **account_get_players_for_account(char *account_name)
{
    account_struct *ac;

    for (ac=accounts; ac; ac=ac->next) {
        if (!strcasecmp(ac->name, account_name)) return ac->character_names;
    }
    return NULL;
}
