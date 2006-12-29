/**
 * static char *rcsid_shstr_c =
 *   "$Id$";
 *
 * @file shstr.c
 * This is a simple shared strings package with a simple interface.
 *
 * Author: Kjetil T. Homme, Oslo 1992.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>

#if defined (__sun__) && defined (StupidSunHeaders)
#include <sys/time.h>
#include "sunos.h"
#endif

#include "shstr.h"

#ifndef WIN32
#include <autoconf.h>
#endif
#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>
#endif

/** Hash table to store our string. */
static shared_string *hash_table[TABLESIZE];

/**
 * Initialises the hash-table used by the shared string library.
 */
void init_hash_table(void) {
    /* A static object should be zeroed out always */
#if !defined(__STDC__)
    (void) memset((void *)hash_table, 0, TABLESIZE * sizeof(shared_string *));
#endif
}

/**
 * Hashing-function used by the shared string library.
 *
 * @param str
 * string to hash.
 * @return
 * hash of string, suitable for use in ::hash_table.
 */
static int hashstr(const char *str) {
    unsigned long hash = 0;
    int i = 0;
    unsigned rot = 0;
    const char *p;

    GATHER(hash_stats.calls);

    for (p = str; i < MAXSTRING && *p; p++, i++) {
        hash ^= (unsigned long) *p << rot;
        rot += 2;
        if (rot >= (sizeof(long) - sizeof(char)) * CHAR_BIT)
            rot = 0;
    }
    return (hash % TABLESIZE);
}

/**
 * Allocates and initialises a new shared_string structure, containing
 * the string str.
 *
 * @param str
 * string to store.
 * @return
 * sharing structure.
 *
 * @todo
 * check return of malloc.
 */
static shared_string* new_shared_string(const char *str) {
    shared_string *ss;

    /* Allocate room for a struct which can hold str. Note
     * that some bytes for the string are already allocated in the
     * shared_string struct.
     */
    ss = (shared_string *) malloc(sizeof(shared_string) - PADDING +
        strlen(str) + 1);
    ss->u.previous = NULL;
    ss->next = NULL;
    ss->refcount = 1;
    strcpy(ss->string, str);

    return ss;
}

/**
 * This will add 'str' to the hash table. If there's no entry for this
 * string, a copy will be allocated, and a pointer to that is returned.
 *
 * @param str
 * string to share.
 * @return
 * pointer to string identical to str, but shared.
 */
const char* add_string(const char *str) {
    shared_string *ss;
    int ind;

    GATHER(add_stats.calls);

    /* Should really core dump here, since functions should not be calling
     * add_string with a null parameter.  But this will prevent a few
     * core dumps.
     */
    if (str==NULL) {
#ifdef MANY_CORES
        abort();
#else
        return NULL;
#endif
    }

    ind = hashstr(str);
    ss = hash_table[ind];

    /* Is there an entry for that hash?
     */
    if (ss) {
        /* Simple case first: See if the first pointer matches. */
        if (str != ss->string) {
            GATHER(add_stats.strcmps);
            if (strcmp(ss->string, str)) {
                /* Apparantly, a string with the same hash value has this 
                 * slot. We must see in the list if "str" has been 
                 * registered earlier.
                 */
                while (ss->next) {
                    GATHER(add_stats.search);
                    ss = ss->next;
                    if (ss->string != str) {
                        GATHER(add_stats.strcmps);
                        if (strcmp(ss->string, str)) {
                            /* This wasn't the right string... */
                            continue;
                        }
                    }
                    /* We found an entry for this string. Fix the
                    * refcount and exit.
                    */
                    GATHER(add_stats.linked);
                    ++(ss->refcount);

                    return ss->string;
                }
                /* There are no occurences of this string in the hash table. */
                {
                    shared_string *new_ss;

                    GATHER(add_stats.linked);
                    new_ss = new_shared_string(str);
                    ss->next = new_ss;
                    new_ss->u.previous = ss;
                    return new_ss->string;
                }
            }
            /* Fall through. */
        }
        GATHER(add_stats.hashed);
        ++(ss->refcount);
        return ss->string;
    } else {
        /* The string isn't registered, and the slot is empty. */
        GATHER(add_stats.hashed);
        hash_table[ind] = new_shared_string(str);

        /* One bit in refcount is used to keep track of the union. */
        hash_table[ind]->refcount |= TOPBIT;
        hash_table[ind]->u.array = &(hash_table[ind]);

        return hash_table[ind]->string;
    }
}

/**
 * This will increase the refcount of the string str.
 * @param str
 * string which *must* have been returned from a previous add_string().
 * @return
 * str
 */
const char* add_refcount(const char *str) {
    GATHER(add_ref_stats.calls);
    ++(SS(str)->refcount);
    return str;
}

/**
 * This will return the refcount of the string str.
 *
 * @param str
 * string which *must* have been returned from a previous add_string().
 * @return
 * refcount of the string.
 */
int query_refcount(const char *str) {
    return (SS(str)->refcount) & ~TOPBIT;
}

/**
 * Searches a string in the shared strings.
 *
 * @param str
 * string to search for.
 * @return
 * pointer to identical string or NULL
 */
const char* find_string(const char *str) {
    shared_string *ss;
    int ind;

    GATHER(find_stats.calls);

    ind = hashstr(str);
    ss = hash_table[ind];

    /* Is there an entry for that hash?
     */
    if (ss) {
        /* Simple case first: Is the first string the right one? */
        GATHER(find_stats.strcmps);
        if (!strcmp(ss->string, str)) {
            GATHER(find_stats.hashed);
            return ss->string;
        } else {
            /* Recurse through the linked list, if there's one. */
            while (ss->next) {
                GATHER(find_stats.search);
                GATHER(find_stats.strcmps);
                ss = ss->next;
                if (!strcmp(ss->string, str)) {
                    GATHER(find_stats.linked);
                    return ss->string;
                }
            }
            /* No match. Fall through. */
        }
    }
    return NULL;
}

/**
 * This will reduce the refcount, and if it has reached 0, str will
 *     be freed.
 *
 * @param str
 * string to release, which *must* have been returned from a previous add_string().
 *
 * @note
 * the memory pointed to by str can be freed after this call, so don't use str anymore.
 */
void free_string(const char *str) {
    shared_string *ss;

    GATHER(free_stats.calls);

    ss = SS(str);

    if ((--ss->refcount & ~TOPBIT) == 0) {
        /* Remove this entry. */
        if (ss->refcount & TOPBIT) {
            /* We must put a new value into the hash_table[].
            */
            if (ss->next) {
                *(ss->u.array) = ss->next;
                ss->next->u.array = ss->u.array;
                ss->next->refcount |= TOPBIT;
            } else {
                *(ss->u.array) = NULL;
            }
            free(ss);
        } else {
            /* Relink and free this struct. */
            if (ss->next)
                ss->next->u.previous = ss->u.previous;
            ss->u.previous->next = ss->next;
            free(ss);
        }
    }
}

#ifdef SS_STATISTICS

extern char errmsg[];

/**
 * A call to this function will cause the statistics to be dumped
 *  into an external string errmsg, which must be large enough to get the values.
 *
 * The routines will gather statistics if SS_STATISTICS is defined.
 *
 * @todo
 * don't use a global variable. Use safe string functions.
 */
void ss_dump_statistics(void) {
    static char line[80];

    sprintf(errmsg, "%-13s %6s %6s %6s %6s %6s\n", 
        "", "calls", "hashed", "strcmp", "search", "linked");
    sprintf(line, "%-13s %6d %6d %6d %6d %6d\n", 
        "add_string:", add_stats.calls, add_stats.hashed, 
        add_stats.strcmps, add_stats.search, add_stats.linked);
    strcat(errmsg, line);
    sprintf(line, "%-13s %6d\n",
        "add_refcount:", add_ref_stats.calls);
    strcat(errmsg, line);
    sprintf(line, "%-13s %6d\n",
        "free_string:", free_stats.calls);
    strcat(errmsg, line);
    sprintf(line, "%-13s %6d %6d %6d %6d %6d\n", 
        "find_string:", find_stats.calls, find_stats.hashed, 
        find_stats.strcmps, find_stats.search, find_stats.linked);
    strcat(errmsg, line);
    sprintf(line, "%-13s %6d\n",
        "hashstr:", hash_stats.calls);
    strcat(errmsg, line);
}
#endif /* SS_STATISTICS */

/**
 * Dump the contents of the share string tables.
 *
 * @param what
 * combination of flags:
 * @li ::SS_DUMP_TABLE: dump the contents of the hash table to stderr.
 * @li ::SS_DUMP_TOTALS: return a string which says how many entries etc. there are in the table.
 * @return
 * a string if (what & SS_DUMP_TOTALS) or NULL.
 *
 * @todo
 * don't use static variable. Remove those pesky if 1.
 */
const char* ss_dump_table(int what) {
    static char totals[80];
    int entries = 0, refs = 0, links = 0;
    int i;

    for (i = 0; i < TABLESIZE; i++) {
        shared_string *ss;

        if ((ss = hash_table[i])!=NULL) {
            ++entries;
            refs += (ss->refcount & ~TOPBIT);
#if 1       /* Can't use stderr any longer, need to include global.h and
             if (what & SS_DUMP_TABLE)
             * use logfile. */
            fprintf(stderr, "%4d -- %4d refs '%s' %c\n",
                i, (ss->refcount & ~TOPBIT), ss->string,
                (ss->refcount & TOPBIT ? ' ' : '#'));
#endif
            while (ss->next) {
                ss = ss->next;
                ++links;
                refs += (ss->refcount & ~TOPBIT);
#if 1
                if (what & SS_DUMP_TABLE)
                    fprintf(stderr, "     -- %4d refs '%s' %c\n",
                        (ss->refcount & ~TOPBIT), ss->string,
                        (ss->refcount & TOPBIT ? '*' : ' '));
#endif
            }
        }
    }

    if (what & SS_DUMP_TOTALS) {
        sprintf(totals, "\n%d entries, %d refs, %d links.",
            entries, refs, links);
        return totals;
    }
    return NULL;
}

/**
 * We don't want to exceed the buffer size of buf1 by adding on buf2!
 *
 * @param buf1
 * @param buf2
 * buffers we plan on concatening. Can be NULL.
 * @param bufsize
 * size of buf1. Can be NULL.
 * @return
 * true if overflow will occur.
 */
int buf_overflow (const char *buf1, const char *buf2, int bufsize)
{
    int     len1 = 0, len2 = 0;

    if (buf1)
        len1 = strlen (buf1);
    if (buf2)
        len2 = strlen (buf2);
    if ((len1 + len2) >= bufsize)
        return 1;
    return 0;
}
