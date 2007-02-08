/*
 * static char *rcsid_readable_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
    Copyright (C) 1992 Frank Tore Johansen

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
 * @file readable.c
 * This file contains code relevant to the BOOKS hack -- designed
 * to allow randomly occuring messages in non-magical texts.
 *
 * @todo
 * replace message type with defines/enums & such things.
 */

/* laid down initial file - dec 1995. -b.t. thomas@astro.psu.edu */

#include <global.h>
#include <book.h>
#include <living.h>
#include <spells.h>


/* Define this if you want to archive book titles by contents.  
 * This option should enforce UNIQUE combinations of titles,authors and 
 * msg contents during and *between* game sessions. 
 * Note: a slight degeneracy exists since books are archived based on an integer 
 * index value calculated from the message text (similar to alchemy formulae). 
 * Sometimes two widely different messages have the same index value (rare). In 
 * this case,  it is possible to occasionally generate 2 books with same title and 
 * different message content. Not really a bug, but rather a feature. This action 
 * should  keeps player on their toes ;). 
 * Also, note that there is *finite* space available for archiving message and titles. 
 * Once this space is used, books will stop being archived. Not a serious problem 
 * under the current regime, since there are generally fewer possible (random) 
 * messages than space available on the titlelists.
 * One exception (for sure) are the monster messages. But no worries, you should
 * see all of the monster info in some order (but not all possble combinations)
 * before the monster titlelist space is run out. You can increase titlelist
 * space by increasing the array sizes for the monster book_authours and book_names 
 * (see  max_titles[] array and include/read.h). Since the unique_book algorthm is 
 * kinda stupid, this program *may* slow down program execution if defined (but I don't 
 * think its a significant problem, at least, I have no problems running this option
 * on a Sparc 10! Also, once archive title lists are filled and/or all possible msg 
 * combinations have been generated, unique_book isnt called anymore. It takes 5-10 
 * sessions for this to happen). 
 * Final note: the game remembers book/title/msg combinations from reading the
 * file lib/bookarch. If you REMOVE this file, you will lose your archive. So
 * be sure to copy it over to the new lib directory when you change versions. 
 * -b.t.
 */

/* This flag is useful to see what kind of output messages are created */
/* #define BOOK_MSG_DEBUG */

/* This flag is useful for debuging archiving action */
/* #define ARCHIVE_DEBUG */

/* Moved these structures from struct.h to this file in 0.94.3 - they
 * are not needed anyplace else, so why have them globally declared?
 */

/** 'title' and 'titlelist' are used by the readable code */
typedef struct titlestruct {
    const char *name;     /**< the name of the book */
    const char *authour;  /**< the name of the book authour */
    const char *archname;  /**< the archetype name of the book */
    int level;       /**< level of difficulty of this message */ 
    int size;       /**< size of the book message */ 
    int msg_index;  /**< an index value derived from book message */
    struct titlestruct *next;
} title;

typedef struct titleliststruct {
    int number;       /**< number of items in the list */
    struct titlestruct *first_book;     /**< pointer to first book in this list */
    struct titleliststruct *next;  /**< pointer to next book list */
} titlelist;


/** special structure, used only by art_name_array[] */
typedef struct namebytype
{
    const char *name;  /**< generic name to call artifacts of this type */
    int type;          /**< matching type */
} arttypename;


/** booklist is the buffer of books read in from the bookarch file */
static titlelist *booklist = NULL;

/** Information on monsters */
static objectlink *first_mon_info = NULL;

/**
 * these are needed for creation of a linked list of
 * pointers to all (hostile) monster objects
 */
static int nrofmon = 0, need_to_write_bookarchive=0;


/**
 * this is needed to keep track of status of initialization
 * of the message file
 */
static int nrofmsg = 0;

/**
 * first_msg is the started of the linked list of messages as read from
 * the messages file
 */
static linked_char *first_msg = NULL;

/**
 * Spellpath information
 */
static uint32 spellpathdef[NRSPELLPATHS] =
{
    PATH_PROT,
    PATH_FIRE,
    PATH_FROST,
    PATH_ELEC,
    PATH_MISSILE,
    PATH_SELF,
    PATH_SUMMON,
    PATH_ABJURE,
    PATH_RESTORE,
    PATH_DETONATE,
    PATH_MIND,
    PATH_CREATE,
    PATH_TELE,
    PATH_INFO,
    PATH_TRANSMUTE,
    PATH_TRANSFER,
    PATH_TURNING,
    PATH_WOUNDING,
    PATH_DEATH,
    PATH_LIGHT
};

/** Book names for path information. */
static const char* const path_book_name[] =
{
    "codex",
    "compendium",
    "exposition",
    "tables",
    "treatise"
};

/** Used by spellpath texts */
static const char* const path_author[] =
{
    "aether",
    "astral byways",
    "connections",
    "the Grey Council",
    "deep pathways",
    "knowledge",
    "magic",
    "mystic ways",
    "pathways",
    "power",
    "spells",
    "transforms",
    "the mystic veil",
    "unknown spells"
};

/**
 * Artiface/item information.
 *
 * if it isnt listed here, then art_attr_msg() will never generate
 * a message for this type of artifact. -b.t.
 */
static arttypename art_name_array[] =
{
    {"Helmet", HELMET},
    {"Amulet", AMULET},
    {"Shield", SHIELD},
    {"Bracers", BRACERS},
    {"Boots", BOOTS},
    {"Cloak", CLOAK},
    {"Gloves", GLOVES},
    {"Gridle", GIRDLE},
    {"Ring", RING},
    {"Horn", HORN},
    {"Missile Weapon", BOW},
    {"Missile", ARROW},
    {"Hand Weapon", WEAPON},
    {"Artifact", SKILL},
    {"Food", FOOD},
    {"Body Armour", ARMOUR}
};

/** Book titles for artifact information. */
static const char* const art_book_name[] =
{
    "collection",
    "file",
    "files",
    "guide",
    "handbook",
    "index",
    "inventory",
    "list",
    "listing",
    "record",
    "record book"
};

/** Used by artifact texts */
static const char* const art_author[] =
{
    "ancient things",
    "artifacts",
    "Havlor",			/* ancient warrior scribe :) */
    "items",
    "lost artifacts",
    "the ancients",
    "useful things"
};

/**
 * Monster book information.
 */
static const char* const mon_book_name[] =
{
    "beastuary",
    "catalog",
    "compilation",
    "collection",
    "encyclopedia",
    "guide",
    "handbook",
    "list",
    "manual",
    "notes",
    "record",
    "register",
    "volume"
};


/** Used by monster beastuary texts */
static const char* const mon_author[] =
{
    "beasts",
    "creatures",
    "dezidens",
    "dwellers",
    "evil nature",
    "life",
    "monsters",
    "nature",
    "new life",
    "residents",
    "the spawn",
    "the living",
    "things"
};

/**
 * God book information
 */
static const char* const gods_book_name[] =
{
    "devotional",
    "devout notes",
    "divine text",
    "divine work",
    "holy book",
    "holy record",
    "moral text",
    "sacred guide",
    "testament",
    "transcript"
};

/** Used by gods texts */
static const char* const gods_author[] =
{
    "cults",
    "joy",
    "lasting curse",
    "madness",
    "religions",
    "the dead",
    "the gods",
    "the heirophant",
    "the poor priest",
    "the priestess",
    "pain",
    "white"
};


/**
 * Alchemy (formula) information
 */
static const char* const formula_book_name[] =
{
    "cookbook",
    "formulary",
    "lab book",
    "lab notes",
    "recipe book",
    "experiment record",
    "work plan",
    "design notes"
};

/** This isn't used except for empty books */
static const char* const formula_author[] =
{
    "Albertus Magnus",
    "alchemy",
    "balms",
    "creation",
    "dusts",
    "magical manufacture",
    "making",
    "philosophical items",
    "potions",
    "powders",
    "the cauldron",
    "the lamp black",
    "transmutation",
    "waters"
};

/**
 * Generic book information
 */

/** Used by msg file and 'generic' books */
static const char* const light_book_name[] =
{
    "calendar",
    "datebook",
    "diary",
    "guidebook",
    "handbook",
    "ledger",
    "notes",
    "notebook",
    "octavo",
    "pamphlet",
    "practicum",
    "script",
    "transcript"
};

/** Name for big books. */
static const char* const heavy_book_name[] =
{
    "catalog",
    "compendium",
    "guide",
    "manual",
    "opus",
    "tome",
    "treatise",
    "volume",
    "work"
};


/** Used by 'generic' books */
static const char* const book_author[] =
{
    "Abdulah",
    "Al'hezred",
    "Alywn",
    "Arundel",
    "Arvind",
    "Aerlingas",
    "Bacon",
    "Baliqendii",
    "Bosworth",
    "Beathis",
    "Bertil",
    "Cauchy",
    "Chakrabarti",
    "der Waalis",
    "Dirk",
    "Djwimii",
    "Eisenstaadt",
    "Fendris",
    "Frank",
    "Habbi",
    "Harlod",
    "Ichibod",
    "Janus",
    "June",
    "Magnuson",
    "Nandii",
    "Nitfeder",
    "Norris",
    "Parael",
    "Penhew",
    "Sophia",
    "Skilly",
    "Tahir",
    "Thockmorton",
    "Thomas",
    "van Helsing",
    "van Pelt",
    "Voormis",
    "Xavier",
    "Xeno",
    "Zardoz",
    "Zagy"
};

/** Book descriptions. */
static const char* const book_descrpt[] =
{
    "ancient",
    "cryptic",
    "cryptical",
    "dusty",
    "hiearchical",
    "grizzled",
    "gold-guilt",
    "great",
    "lost",
    "magnificent",
    "musty",
    "mythical",
    "mystical",
    "rustic",
    "stained",
    "silvered",
    "transcendental",
    "weathered"
};
/**
 * Each line of this array is a readable subtype
 * Be careful to keep the order. If you add readable subtype, add them
 * at the bottom of the list. Never delete a subtype because index is used as
 * subtype paramater in arch files!
 */
static readable_message_type readable_message_types[] = 
{
	/*subtype 0  */ {0,0},
                    /* book messages subtypes */
	/*subtype 1  */ {MSG_TYPE_BOOK, MSG_TYPE_BOOK_CLASP_1},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_CLASP_2},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_ELEGANT_1},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_ELEGANT_2},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_QUARTO_1},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_QUARTO_2},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_SPELL_EVOKER},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_SPELL_PRAYER},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_SPELL_PYRO},
	/*subtype 10 */ {MSG_TYPE_BOOK, MSG_TYPE_BOOK_SPELL_SORCERER},
                    {MSG_TYPE_BOOK, MSG_TYPE_BOOK_SPELL_SUMMONER},
                    /* card messages subtypes*/
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_SIMPLE_1},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_SIMPLE_2},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_SIMPLE_3},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_ELEGANT_1},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_ELEGANT_2},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_ELEGANT_3},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_STRANGE_1},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_STRANGE_2},
	/*subtype 20 */ {MSG_TYPE_CARD, MSG_TYPE_CARD_STRANGE_3},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_MONEY_1},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_MONEY_2},
                    {MSG_TYPE_CARD, MSG_TYPE_CARD_MONEY_3},
                    
                    /* Paper messages subtypes */
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_NOTE_1},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_NOTE_2},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_NOTE_3},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_LETTER_OLD_1},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_LETTER_OLD_2},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_LETTER_NEW_1},
	/*subtype 30 */ {MSG_TYPE_PAPER, MSG_TYPE_PAPER_LETTER_NEW_2},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_ENVELOPE_1},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_ENVELOPE_2},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_SCROLL_OLD_1},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_SCROLL_OLD_2},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_SCROLL_NEW_1},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_SCROLL_NEW_2},
                    {MSG_TYPE_PAPER, MSG_TYPE_PAPER_SCROLL_MAGIC},

                    /* road signs messages subtypes */
                    {MSG_TYPE_SIGN, MSG_TYPE_SIGN_BASIC},
                    {MSG_TYPE_SIGN, MSG_TYPE_SIGN_DIR_LEFT},
	/*subtype 40 */ {MSG_TYPE_SIGN, MSG_TYPE_SIGN_DIR_RIGHT},
                    {MSG_TYPE_SIGN, MSG_TYPE_SIGN_DIR_BOTH},

                    /* stones and monument messages */
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_STONE_1},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_STONE_2},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_STONE_3},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_STATUE_1},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_STATUE_2},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_STATUE_3},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_GRAVESTONE_1},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_GRAVESTONE_2},
	/*subtype 50 */ {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_GRAVESTONE_3},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_WALL_1},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_WALL_2},
                    {MSG_TYPE_MONUMENT, MSG_TYPE_MONUMENT_WALL_3}
};
/** Number of elements in ::readable_message_types */
int last_readable_subtype = sizeof(readable_message_types)/sizeof(readable_message_type);

/** Number of titles for different name lists. */
static int max_titles[6] =
{
    ((sizeof (light_book_name) / sizeof (char *)) + (sizeof (heavy_book_name) / sizeof (char *))) * (sizeof (book_author) / sizeof (char *)),
    (sizeof (mon_book_name) / sizeof (char *)) * (sizeof (mon_author) / sizeof (char *)),
    (sizeof (art_book_name) / sizeof (char *)) * (sizeof (art_author) / sizeof (char *)),
    (sizeof (path_book_name) / sizeof (char *)) * (sizeof (path_author) / sizeof (char *)),
    (sizeof (formula_book_name) / sizeof (char *)) * (sizeof (formula_author) / sizeof (char *)),
    (sizeof (gods_book_name) / sizeof (char *)) * (sizeof (gods_author) / sizeof (char *))
};

/******************************************************************************
 *
 * Start of misc. readable functions used by others functions in this file
 *
 *****************************************************************************/

/**
 * Creates a titlelist.
 *
 * @return
 * new titlelist.
 *
 * @note
 * if memory allocation failes, calls fatal().
 */
static titlelist* get_empty_booklist (void)
{
    titlelist *bl = (titlelist *) malloc (sizeof (titlelist));
    if (bl == NULL)
        fatal (OUT_OF_MEMORY);
    bl->number = 0;
    bl->first_book = NULL;
    bl->next = NULL;
    return bl;
}

/**
 * Creates a title.
 *
 * @return
 * new title.
 *
 * @note
 * if memory allocation failes, calls fatal().
 */
static title* get_empty_book (void)
{
    title  *t = (title *) malloc (sizeof (title));
    if (t == NULL)
        fatal (OUT_OF_MEMORY);
    t->name = NULL;
    t->archname = NULL;
    t->authour = NULL;
    t->level = 0;
    t->size = 0;
    t->msg_index = 0;
    t->next = NULL;
    return t;
}

/**
 * Gets the ith titlelist.
 *
 * Will create items if they don't exist.
 *
 * @param i
 * index to get.
 * @return
 * pointer to the title list referenced by i. Will never be NULL.
 */

static titlelist* get_titlelist (int i)
{
    titlelist *tl = booklist;
    int     number = i;

    if (number < 0)
        return tl;

    while (tl && number)
    {
        if (!tl->next)
            tl->next = get_empty_booklist ();
        tl = tl->next;
        number--;
    }

    return tl;
}

/* HANDMADE STRING FUNCTIONS.., perhaps these belong in another file 
 * (shstr.c ?), but the quantity BOOK_BUF will need to be defined. */

/**
 * Simple routine to return the number of list
 * items in buf1 as separated by the value of buf2
 * 
 * @param buf1
 * items we want to split.
 * @param buf2
 * what to split by. Must not have a lenght greater than 11.
 * @return
 * number of elements.
 *
 * @todo
 * check lengths and such.
 */
int nstrtok (const char *buf1, const char *buf2)
{
    char   *tbuf, sbuf[12], buf[MAX_BUF];
    int     number = 0;

    if (!buf1 || !buf2)
        return 0;
    sprintf (buf, "%s", buf1);
    sprintf (sbuf, "%s", buf2);
    tbuf = strtok (buf, sbuf);
    while (tbuf)
    {
        number++;
        tbuf = strtok (NULL, sbuf);
    }
    return number;
}

/**
 * Takes a string in buf1 and separates it into
 * a list of strings delimited by buf2. Then returns a comma
 * separated string w/ decent punctuation.
 *
 * @param buf1
 * buffer to split.
 * @param buf2
 * what to split buf1 by.
 * @return
 * static buffer containing a comma separated string of values.
 *
 * @todo
 * don't use static buffer. Use safe string functions.
 */
char* strtoktolin (const char *buf1, const char *buf2)
{
    int     maxi, i = nstrtok (buf1, buf2);
    char   *tbuf, buf[MAX_BUF], sbuf[12];
    static char rbuf[BOOK_BUF];

    maxi = i;
    strcpy(buf, buf1);
    strcpy(sbuf, buf2);
    strcpy(rbuf, " ");
    tbuf = strtok (buf, sbuf);
    while (tbuf && i > 0)
    {
        strcat (rbuf, tbuf);
        i--;
        if (i == 1 && maxi > 1)
            strcat (rbuf, " and ");
        else if (i > 0 && maxi > 1)
            strcat (rbuf, ", ");
        else
            strcat (rbuf, ".");
        tbuf = strtok (NULL, sbuf);
    }
    return (char *) rbuf;
}

/**
 * Checks if buf1 and buf2 can be combined.
 * @param buf1
 * @param buf2
 * buffer we plan on combining.
 * @param booksize
 * maximum book size.
 * @return
 * 0 if buffers can be combined, 1 else.
 */
int book_overflow (const char *buf1, const char *buf2, int booksize)
{

    if (buf_overflow (buf1, buf2, BOOK_BUF - 2)		/* 2 less so always room for trailing \n */
        || buf_overflow (buf1, buf2, booksize))
        return 1;
    return 0;


}

/*****************************************************************************
 *
 * Start of initialization related functions.
 *
 ****************************************************************************/

/**
 * If not called before, initialize the info list.
 *
 * Reads the messages file into the list pointed to by first_msg
 */
static void init_msgfile (void)
{
    FILE   *fp;
    char    buf[MAX_BUF], msgbuf[HUGE_BUF], fname[MAX_BUF], *cp;
    int     comp;
    static int did_init_msgfile;

    if (did_init_msgfile)
        return;
    did_init_msgfile = 1;

    sprintf (fname, "%s/messages", settings.datadir);
    LOG (llevDebug, "Reading messages from %s...", fname);

    if ((fp = open_and_uncompress (fname, 0, &comp)) != NULL)
    {
        linked_char *tmp = NULL;
        while (fgets (buf, MAX_BUF, fp) != NULL)
        {
            if (*buf == '#')
                continue;
            if ((cp = strchr (buf, '\n')) != NULL)
                *cp = '\0';
            cp = buf;
            while (*cp == ' ')	/* Skip blanks */
                cp++;
            if (!strncmp (cp, "ENDMSG", 6))
            {
                if (strlen (msgbuf) > BOOK_BUF)
                {
                    LOG (llevDebug, "Warning: this string exceeded max book buf size:");
                    LOG (llevDebug, "  %s", msgbuf);
                }
                tmp->name = add_string (msgbuf);
                tmp->next = first_msg;
                first_msg = tmp;
                nrofmsg++;
                continue;
            }
            else if (!strncmp (cp, "MSG", 3))
            {
                tmp = (linked_char *) malloc (sizeof (linked_char));
                strcpy (msgbuf, " ");	/* reset msgbuf for new message */
                continue;
            }
            else if (!buf_overflow (msgbuf, cp, HUGE_BUF - 1))
            {
                strcat (msgbuf, cp);
                strcat (msgbuf, "\n");
            }
        }
        close_and_delete (fp, comp);
    }

#ifdef BOOK_MSG_DEBUG
    LOG (llevDebug, "\ninit_info_listfile() got %d messages.\n", nrofmsg);
#endif
    LOG (llevDebug, "done.\n");
}


/**
 * If not called before, initialize the info list.
 *
 * This reads in the bookarch file into memory. bookarch is the file
 * created and updated across multiple runs of the program.
 */
static void init_book_archive (void)
{
    FILE   *fp;
    int     comp, nroftitle = 0;
    char    buf[MAX_BUF], fname[MAX_BUF], *cp;
    title  *book = NULL;
    titlelist *bl = get_empty_booklist ();
    static int did_init_barch;

    if (did_init_barch)
        return;
    did_init_barch = 1;

    if (!booklist)
        booklist = bl;

    sprintf (fname, "%s/bookarch", settings.localdir);
    LOG (llevDebug, " Reading bookarch from %s...\n", fname);

    if ((fp = open_and_uncompress (fname, 0, &comp)) != NULL)
    {
        int value, type = 0;
        size_t i;

        while (fgets (buf, MAX_BUF, fp) != NULL)
        {
            if (*buf == '#')
                continue;
            if ((cp = strchr (buf, '\n')) != NULL)
                *cp = '\0';
            cp = buf;
            while (*cp == ' ')	/* Skip blanks */
                cp++;
            if (!strncmp (cp, "title", 4))
            {
                book = get_empty_book ();		/* init new book entry */
                book->name = add_string (strchr (cp, ' ') + 1);
                type = -1;
                nroftitle++;
                continue;
            }
            if (!strncmp (cp, "authour", 4))
            {
                book->authour = add_string (strchr (cp, ' ') + 1);
            }
            if (!strncmp (cp, "arch", 4))
            {
                book->archname = add_string (strchr (cp, ' ') + 1);
            }
            else if (sscanf (cp, "level %d", &value))
            {
                book->level = (uint16) value;
            }
            else if (sscanf (cp, "type %d", &value))
            {
                type = (uint16) value;
            }
            else if (sscanf (cp, "size %d", &value))
            {
                book->size = (uint16) value;
            }
            else if (sscanf (cp, "index %d", &value))
            {
                book->msg_index = (uint16) value;
            }
            else if (!strncmp (cp, "end", 3))
            {		/* link it */
                bl = get_titlelist (type);
                book->next = bl->first_book;
                bl->first_book = book;
                bl->number++;
            }
        }
        LOG (llevDebug, " book archives(used/avail): ");
        for (bl = booklist, i = 0; bl != NULL && i < sizeof(max_titles)/sizeof(*max_titles); bl = bl->next, i++)
        {
            LOG (llevDebug, "(%d/%d)", bl->number, max_titles[i]);
        }
        LOG (llevDebug, "\n");
        close_and_delete (fp, comp);
    }

#ifdef BOOK_MSG_DEBUG
    LOG (llevDebug, "\n init_book_archive() got %d titles.\n", nroftitle);
#endif
    LOG (llevDebug, " done.\n");
}

/**
 * Creates the linked list of pointers to
 * monster archetype objects if not called previously.
 *
 * @todo
 * check malloc return.
 */
static void init_mon_info (void)
{
    archetype *at;
    static int did_init_mon_info = 0;

    if (did_init_mon_info)
        return;
    did_init_mon_info = 1;


    for (at = first_archetype; at != NULL; at = at->next)
    {
        if (QUERY_FLAG (&at->clone, FLAG_MONSTER) &&
          (!QUERY_FLAG (&at->clone, FLAG_CHANGING)
          || QUERY_FLAG (&at->clone, FLAG_UNAGGRESSIVE))
          )
        {
            objectlink *mon = (objectlink *) malloc (sizeof (objectlink));
            mon->ob = &at->clone;
            mon->id = nrofmon;
            mon->next = first_mon_info;
            first_mon_info = mon;
            nrofmon++;
        }
    }
    LOG (llevDebug, "init_mon_info() got %d monsters\n", nrofmon);
}


/**
 * Initialize linked lists utilized by message functions in tailor_readable_ob() 
 *
 * This is the function called by the main routine to initialize
 * all the readable information.
 */
void init_readable (void)
{
    static int did_this;

    if (did_this) return;
    did_this = 1;

    LOG (llevDebug, "Initializing reading data...");
    init_msgfile ();
    init_book_archive ();
    init_mon_info ();
    LOG (llevDebug, " Done\n");
}

/*****************************************************************************
 *
 * This is the start of the administrative functions when creating
 * new books (ie, updating title and the like)
 *
 *****************************************************************************/


/**
 * Search the titlelist (based on msgtype) to see if
 * book matches something already there.  IF so, return that title.
 *
 * @param book
 * book we're searching.
 * @param msgtype
 * message type.
 * @return
 * title if found, NULL if no match.
 */
static title* find_title (const object *book, int msgtype)
{
    title  *t = NULL;
    titlelist *tl = get_titlelist (msgtype);
    int     length = strlen (book->msg);
    int     index = strtoint (book->msg);

    if (msgtype < 0)
        return (title *) NULL;

    if (tl)
        t = tl->first_book;
    while (t)
        if (t->size == length && t->msg_index == index)
            break;
        else
            t = t->next;

#ifdef ARCHIVE_DEBUG
    if (t)
        LOG (llevDebug, "Found title match (list %d): %s %s (%d)\n",
          msgtype, t->name, t->authour, t->msg_index);
#endif

    return t;
}

/**
 * Only for objects of type BOOK. SPELLBOOK stuff is
 * handled directly in change_book_name(). Names are based on text 
 * msgtype
 *
 * This sets book book->name based on msgtype given.  What name
 * is given is based on various criteria
 *
 * @param book
 * book we want to alter.
 * @param msgtype
 * what information we want in the book.
 */
static void new_text_name (object *book, int msgtype)
{
    int     nbr;
    char    name[MAX_BUF];

    if (book->type != BOOK)
        return;

    switch (msgtype)
    {
        case 1:			/*monster */
            nbr = sizeof (mon_book_name) / sizeof (char *);
            strcpy (name, mon_book_name[RANDOM () % nbr]);
            break;
        case 2:			/*artifact */
            nbr = sizeof (art_book_name) / sizeof (char *);
            strcpy (name, art_book_name[RANDOM () % nbr]);
            break;
        case 3:			/*spellpath */
            nbr = sizeof (path_book_name) / sizeof (char *);
            strcpy (name, path_book_name[RANDOM () % nbr]);
            break;
        case 4:			/*alchemy */
            nbr = sizeof (formula_book_name) / sizeof (char *);
            strcpy (name, formula_book_name[RANDOM () % nbr]);
            break;
        case 5:			/*gods */
            nbr = sizeof (gods_book_name) / sizeof (char *);
            strcpy (name, gods_book_name[RANDOM () % nbr]);
            break;
        case 6:			/*msg file */
        default:
            if (book->weight > 2000)
            {			/* based on weight */
                nbr = sizeof (heavy_book_name) / sizeof (char *);
                strcpy (name, heavy_book_name[RANDOM () % nbr]);
            }
            else if (book->weight < 2001)
            {
                nbr = sizeof (light_book_name) / sizeof (char *);
                strcpy (name, light_book_name[RANDOM () % nbr]);
            }
            break;
      }
    free_string (book->name);
    book->name = add_string (name);
}

/**
 * A lot like new_text_name() above, but instead chooses an author
 * and sets op->title to that value
 *
 * @param op
 * book to alter.
 * @param msgtype
 * information we want.
 */
static void add_author (object *op, int msgtype)
{
    char    title[MAX_BUF], name[MAX_BUF];
    int     nbr = sizeof (book_author) / sizeof (char *);

    if (msgtype < 0 || strlen (op->msg) < 5)
        return;

    switch (msgtype)
    {
        case 1:			/* monster */
            nbr = sizeof (mon_author) / sizeof (char *);
            strcpy (name, mon_author[RANDOM () % nbr]);
            break;
        case 2:			/* artifacts */
            nbr = sizeof (art_author) / sizeof (char *);
            strcpy (name, art_author[RANDOM () % nbr]);
            break;
        case 3:			/* spellpath */
            nbr = sizeof (path_author) / sizeof (char *);
            strcpy (name, path_author[RANDOM () % nbr]);
            break;
        case 4:			/* alchemy */
            nbr = sizeof (formula_author) / sizeof (char *);
            strcpy (name, formula_author[RANDOM () % nbr]);
            break;
        case 5:			/* gods */
            nbr = sizeof (gods_author) / sizeof (char *);
            strcpy (name, gods_author[RANDOM () % nbr]);
            break;
        case 6:			/* msg file */
        default:
            strcpy (name, book_author[RANDOM () % nbr]);
      }

    sprintf (title, "of %s", name);
    op->title = add_string (title);
}

/**
 * Check to see if the book title/msg is unique. We 
 * go through the entire list of possibilities each time. If we find
 * a match, then unique_book returns true (because inst unique).
 *
 * @param book
 * book we're searching.
 * @param msgtype
 * type of information contained.
 * @return 
 */
static int unique_book (const object *book, int msgtype)
{
    title  *test;

    if (!booklist)
        return 1;		/* No archival entries! Must be unique! */

    /* Go through the booklist.  If the author and name match, not unique so
     * return 0.
     */
    for (test = get_titlelist (msgtype)->first_book; test; test=test->next) {
        if (!strcmp(test->name, book->name) && !strcmp(book->title, test->authour)) 
            return 0;
    }
    return 1;
}

/**
 * Adds a book to the list of existing books.
 *
 * @param book
 * book to add.
 * @param msgtype
 * what information the book contains.
 */
static void add_book_to_list (const object *book, int msgtype)
{
    titlelist *tl = get_titlelist (msgtype);
    title  *t;

    if (!tl)
    {
        LOG (llevError, "add_book_to_list can't get booklist!\n");
        return;
    }

    t = get_empty_book ();
    t->name = add_string (book->name);
    t->authour = add_string (book->title);
    t->size = strlen (book->msg);
    t->msg_index = strtoint (book->msg);
    t->archname = add_string (book->arch->name);
    t->level = book->level;

    t->next = tl->first_book;
    tl->first_book = t;
    tl->number++;

    /* We have stuff we need to write now */
    need_to_write_bookarchive=1;

#ifdef ARCHIVE_DEBUG
    LOG (llevDebug, "Archiving new title: %s %s (%d)\n", book->name,
        book->title, msgtype);
#endif

}

/** How many times to try to generate a unique name for a book. */
#define MAX_TITLE_CHECK 20

/**
 * Give a new, fancier name to generated 
 * objects of type BOOK and SPELLBOOK.
 * Aug 96 I changed this so we will attempt to create consistent 
 * authour/title and message content for BOOKs. Also, we will
 * alter books  that match archive entries to the archival 
 * levels and architypes. -b.t. 
 *
 * @param book
 * book to alter. Should be of type BOOK.
 * @param msgtype
 * what information the book contains.
 */
void change_book (object *book, int msgtype)
{
    int     nbr = sizeof (book_descrpt) / sizeof (char *);

    switch (book->type)
    {
        case BOOK:
        {
            titlelist *tl = get_titlelist (msgtype);
            title  *t = NULL;
            int     tries = 0;
            /* look to see if our msg already been archived. If so, alter
            * the book to match the archival text. If we fail to match,
            * then we archive the new title/name/msg combo if there is 
            * room on the titlelist. 
            */

            if ((strlen (book->msg) > 5) && (t = find_title (book, msgtype)))
            {
                object *tmpbook;

                /* alter book properties */
                if ((tmpbook = create_archetype (t->archname)) != NULL)
                {
                    if (tmpbook->msg)
                        free_string (book->msg);
                    tmpbook->msg = add_string (book->msg);
                    copy_object (tmpbook, book);
                    free_object (tmpbook);
                }

                book->title = add_string (t->authour);
                free_string (book->name);
                book->name = add_string (t->name);
                book->level = t->level;
            }
            /* Don't have any default title, so lets make up a new one */
            else
            {
                int     numb, maxnames = max_titles[msgtype];
                const char *old_title;
                const char *old_name;

                old_title = book->title ? add_string(book->title) : NULL;
                old_name = add_string(book->name);

                /* some pre-generated books have title already set (from
                * maps), also don't bother looking for unique title if
                * we already used up all the available names! */

                if (!tl)
                {
                    LOG (llevError, "change_book_name(): can't find title list\n");
                    numb = 0;
                }
                else
                    numb = tl->number;

                if (numb == maxnames)
                {
    #ifdef ARCHIVE_DEBUG
                    LOG (llevDebug, "titles for list %d full (%d possible).\n",
                        msgtype, maxnames);
    #endif
                    if (old_title != NULL)
                        free_string(old_title);
                    free_string(old_name);
                    break;
                }
                /* shouldnt change map-maker books */
                else if (!book->title)
                    do
                    {
                        /* random book name */
                        new_text_name (book, msgtype);
                        add_author (book, msgtype);	/* random author */
                        tries++;
                    }
                    while (!unique_book (book, msgtype) && tries < MAX_TITLE_CHECK);

                /* Now deal with 2 cases.
                 * 1)If no space for a new title exists lets just restore
                 * the old book properties. Remember, if the book had
                 * matchd an older entry on the titlelist, we shouldnt
                 * have called this routine in the first place!  
                 * 2) If we got a unique title, we need to add it to
                 * the list.
                 */

                if (tries == MAX_TITLE_CHECK || numb == maxnames)
                {		/* got to check maxnames again */
    #ifdef ARCHIVE_DEBUG
                    LOG (llevDebug, "Failed to obtain unique title for %s %s (names:%d/%d)\n",
                        book->name, book->title, numb, maxnames);
    #endif
                    /* restore old book properties here */
                    free_string (book->name);
                    free_string (book->title);
                    if (old_title!=NULL)
                        book->title = add_string (old_title);

                    if (RANDOM () % 4) {
                        /* Lets give the book a description to individualize it some */
                        char new_name[MAX_BUF];
                        snprintf (new_name, MAX_BUF, "%s %s", book_descrpt[RANDOM () % nbr], old_name);
                        book->name = add_string (new_name);
                    } else {
                        book->name = add_string (old_name);
                    }
                }
                else if (book->title && strlen (book->msg) > 5) {	/* archive if long msg texts */
                    add_book_to_list (book, msgtype);
                }

                if (old_title != NULL)
                    free_string(old_title);
                free_string(old_name);

            }
            break;
        }

        default:
            LOG (llevError, "change_book_name() called w/ illegal obj type.\n");
            return;
      }
}

/*****************************************************************************
 *
 * This is the start of the area that generates the actual contents
 * of the book.
 *
 *****************************************************************************/

/*****************************************************************************
 * Monster msg generation code.
 ****************************************************************************/

/**
 * Returns a random monster selected from linked
 * list of all monsters in the current game.
 * Changed 971225 to be greater than equal to level passed.  Also
 * made choosing by level more random.
 *
 * @param level
 * if non-zero, then only monsters greater than that level will be returned.
 * @return
 * random monster, or NULL if failure.
 */
object* get_random_mon (int level)
{
    objectlink *mon = first_mon_info;
    int     i = 0, monnr;

    /* safety check.  Problem w/ init_mon_info list? */
    if (!nrofmon || !mon)
        return (object *) NULL;

    if (!level)
    {
        /* lets get a random monster from the mon_info linked list */
        monnr = RANDOM () % nrofmon;

        for (mon = first_mon_info, i = 0; mon; mon = mon->next)
            if (i++ == monnr)
                break;

        if (!mon)
            {
            LOG (llevError, "get_random_mon: Didn't find a monster when we should have\n");
            return NULL;
            }
        return mon->ob;
    }

    /* Case where we are searching by level.  Redone 971225 to be clearer
     * and more random.  Before, it looks like it took a random monster from
     * the list, and then returned the first monster after that which was
     * appropriate level.  This wasn't very random because if you had a 
     * bunch of low level monsters and then a high level one, if the random
     * determine took one of the low level ones, it would just forward to the
     * high level one and return that.  Thus, monsters that immediatly followed
     * a bunch of low level monsters would be more heavily returned.  It also
     * means some of the dragons would be poorly represented, since they
     * are a group of high level monsters all around each other.
     */

    /* First count number of monsters meeting level criteria */
    for (mon = first_mon_info, i = 0; mon; mon = mon->next)
        if (mon->ob->level >= level)
            i++;

    if (i == 0)
    {
        LOG (llevError, "get_random_mon() couldn't return monster for level %d\n",
            level);
        return NULL;
    }

    monnr = RANDOM () % i;
    for (mon = first_mon_info; mon; mon = mon->next)
        if (mon->ob->level >= level && monnr-- == 0)
            return mon->ob;

    if (!mon)
    {
        LOG (llevError, "get_random_mon(): didn't find a monster when we should have\n");
        return NULL;
    }
    return NULL;		/* Should be unreached, but keeps warnings down */
}

/**
 * Returns a description of the monster.  This really needs to be
 * redone, as describe_item really gives a pretty internal description.
 *
 * @param mon
 * monster to describe.
 * @return
 * static buffer containing the description.
 *
 * @todo
 * remove static buffer use.
 */
char* mon_desc (const object *mon)
{
    static char retbuf[HUGE_BUF];

    sprintf (retbuf, " *** %s ***\n", mon->name);
    strcat (retbuf, describe_item(mon, NULL));

    return retbuf;
}


/**
 * This function returns the next monster after 'tmp' in the monster list.
 *
 * @param tmp
 * monster.
 * @return
 * next monster, or if no match is found, it returns NULL.
 *
 * @note
 * list is considered circular, asking for the next of the last element will return the first one.
 */
object* get_next_mon (object *tmp)
{
    objectlink *mon;

    for (mon = first_mon_info; mon; mon = mon->next)
        if (mon->ob == tmp)
            break;

    /* didn't find a match */
    if (!mon)
        return NULL;
    if (mon->next)
        return mon->next->ob;
    else
        return first_mon_info->ob;
}



/**
 * Generate a message detailing the properties 
 * of a randomly selected monster.
 *
 * @param level
 * book level.
 * @param booksize
 * size (in characters) of the book we want.
 * @return
 * static buffer containing the description.
 *
 * @todo
 * remove static buffer. Use safe string functions. Is it all right to ignore level for next monsters?
 */
char* mon_info_msg (int level, int booksize)
{
    static char retbuf[BOOK_BUF];
    char    tmpbuf[HUGE_BUF];
    object *tmp;

    /*preamble */
    strcpy (retbuf, "This beastiary contains:");

    /* lets print info on as many monsters as will fit in our 
     * document. 
     * 8-96 Had to change this a bit, otherwise there would
     * have been an impossibly large number of combinations
     * of text! (and flood out the available number of titles
     * in the archive in a snap!) -b.t. 
     */
    tmp = get_random_mon (level * 3);
    while (tmp) {
        /* monster description */
        sprintf (tmpbuf, "\n---\n%s", mon_desc (tmp));

        if (!book_overflow (retbuf, tmpbuf, booksize))
            strcat (retbuf, tmpbuf);
        else
            break;

        /* Note that the value this returns is not based on level */
        tmp = get_next_mon (tmp);
    }

#ifdef BOOK_MSG_DEBUG
    LOG (llevDebug, "\n mon_info_msg() created strng: %d\n", strlen (retbuf));
    fprintf (logfile, " MADE THIS:\n%s\n", retbuf);
#endif

    return retbuf;
}


/*****************************************************************************
 * Artifact msg generation code.
 ****************************************************************************/

/**
 * Generate a message detailing the properties
 * of 1-6 artifacts drawn sequentially from the artifact list.
 *
 * @param level
 * level of the book.
 * @param booksize
 * length of the book.
 * @return
 * static buffer containing the description.
 *
 * @todo
 * remove static buffer. Use safe string functions.
 */
static const char *artifact_msg (int level, int booksize)
{
    artifactlist *al = NULL;
    artifact *art;
    int     chance, i, type, index;
    int     book_entries = level > 5 ? RANDOM () % 3 + RANDOM () % 3 + 2 : RANDOM () % level + 1;
    char   *ch, name[MAX_BUF], buf[BOOK_BUF], sbuf[MAX_BUF];
    static char retbuf[BOOK_BUF];
    object *tmp = NULL;

    /* values greater than 5 create msg buffers that are too big! */
    if (book_entries > 5)
        book_entries = 5;

    /* lets determine what kind of artifact type randomly.
     * Right now legal artifacts only come from those listed
     * in art_name_array. Also, we check to be sure an artifactlist
     * for that type exists!
     */
    i=0;
    do {
        index = RANDOM () % (sizeof (art_name_array) / sizeof (arttypename));
        type = art_name_array[index].type;
        al = find_artifactlist (type);
        i++;
    } while ((al == NULL) && (i<10));

    if (i==10) /* Unable to find a message */
        return("None");

    /* There is no reason to start on the artifact list at the begining. Lets
     * take our starting position randomly... */
    art = al->items;
    for (i = RANDOM () % level + RANDOM () % 2 + 1; i > 0; i--)
    {
        if (art == NULL)
            art = al->items;	/* hmm, out of stuff, loop back around */
        art = art->next;
    }

    /* the base 'generic' name for our artifact */
    strcpy(name, art_name_array[index].name);

    /* Ok, lets print out the contents */
    sprintf (retbuf, "Herein %s detailed %s...\n", book_entries > 1 ? "are" : "is",
        book_entries > 1 ? "some artifacts" : "an artifact");

    /* artifact msg attributes loop. Lets keep adding entries to the 'book'
     * as long as we have space up to the allowed max # (book_entires) 
     */
    while (book_entries > 0)
    {

        if (art == NULL)
            art = al->items;

        /* separator of items */
        strcpy (buf, "--- \n");

        /* Name */
        if (art->allowed != NULL && strcmp (art->allowed->name, "All"))
        {
            linked_char *temp, *next = art->allowed;
            do
            {
                temp = next;
                next = next->next;
            }
            while ((next != (linked_char *) NULL) && !RANDOM () % 2);
            sprintf (buf, "%s A %s of %s", buf, temp->name, art->item->name);
        }
        else			/* default name is used */
            sprintf (buf, "%s The %s of %s", buf, name, art->item->name);

        /* chance of finding */
        chance = 100 * ((float) art->chance / al->total_chance);
        if (chance >= 20)
            sprintf (sbuf, "an uncommon");
        else if (chance >= 10)
            sprintf (sbuf, "an unusual");
        else if (chance >= 5)
            sprintf (sbuf, "a rare");
        else
            sprintf (sbuf, "a very rare");
        sprintf (buf, "%s is %s\n", buf, sbuf);

        /* value of artifact */
        sprintf (buf, "%s item with a value that is %d times normal.\n",
            buf, art->item->value);

        /* include the message about the artifact, if exists, and book
        * level is kinda high */
        if (art->item->msg && (RANDOM () % 4 + 1) < level &&
            !((strlen (art->item->msg) + strlen (buf)) > BOOK_BUF))
            strcat(buf, art->item->msg);

        /* properties of the artifact */
        tmp = get_object ();
        add_abilities (tmp, art->item);
        tmp->type = type;
        SET_FLAG (tmp, FLAG_IDENTIFIED);
        if ((ch = describe_item (tmp, NULL)) != NULL && strlen (ch) > 1)
            sprintf (buf, "%s Properties of this artifact include: \n %s \n",
                buf, ch);
        free_object(tmp);
        /* add the buf if it will fit */
        if (!book_overflow (retbuf, buf, booksize))
            strcat (retbuf, buf);
        else
            break;

        art = art->next;
        book_entries--;
    }

#ifdef BOOK_MSG_DEBUG
    LOG (llevDebug, "artifact_msg() created strng: %d\n", strlen (retbuf));
    fprintf (logfile, " MADE THIS:\n%s", retbuf);
#endif
    return retbuf;
}

/*****************************************************************************
 * Spellpath message generation
 *****************************************************************************/

/**
 * Generate a message detailing the member incantations/prayers (and some of their
 * properties) belonging to a random spellpath. 
 *
 * @param level
 * level of the book.
 * @param booksize
 * length of the book.
 * @return
 * static buffer containing the description.
 *
 * @todo
 * remove static buffer. Use safe string functions.
 */
char* spellpath_msg (int level, int booksize)
{
    static char retbuf[BOOK_BUF];
    char    tmpbuf[BOOK_BUF];
    int     path = RANDOM () % NRSPELLPATHS, prayers = RANDOM () % 2;
    int     did_first_sp = 0;
    uint32  pnum = (path == -1) ? PATH_NULL : spellpathdef[path];
    archetype *at;

    /* Preamble */
    sprintf (retbuf, "Herein are detailed the names of %s\n",
        prayers ? "prayers": "incantations");

    if (path == -1)
        strcat (retbuf, "having no known spell path.\n");
    else
        sprintf (retbuf, "%sbelonging to the path of %s:\n", retbuf,
            spellpathnames[path]);

    for (at=first_archetype; at != NULL; at=at->next) {
        /* Determine if this is an appropriate spell.  Must
         * be of matching path, must be of appropriate type (prayer
         * or not), and must be within the valid level range.
         */
        if (at->clone.type == SPELL && at->clone.path_attuned & pnum &&
          ((at->clone.stats.grace && prayers) || (at->clone.stats.sp && !prayers)) &&
          (at->clone.level < (level * 8))) {
            strcpy (tmpbuf, at->clone.name);

            if (book_overflow (retbuf, tmpbuf, booksize))
                break;
            else {
                if (did_first_sp)
                    strcat (retbuf, ",\n");
                did_first_sp = 1;
                strcat (retbuf, tmpbuf);
            }
        }
    }
    /* Geez, no spells were generated. */
    if (!did_first_sp) {
        if (RANDOM () % 4)	/* usually, lets make a recursive call... */
            spellpath_msg (level, booksize);
        else	/* give up, cause knowing no spells exist for path is info too. */
            strcat (retbuf, "\n - no known spells exist -\n");
    }
    else {
        strcat (retbuf, "\n");
    }
    return retbuf;
}



/**
 * Generate a message detailing the properties of a randomly selected alchemical formula.
 *
 * @param book
 * book we write to.
 * @param level
 * level for formulaes and such.
 *
 * @todo
 * use safe string functions.
 */
void make_formula_book(object *book, int level) {
    char retbuf[BOOK_BUF], title[MAX_BUF];
    recipelist *fl;
    recipe *formula = NULL;
    int     chance;

    /* the higher the book level, the more complex (ie number of
     * ingredients) the formula can be. 
     */
    fl = get_formulalist (((RANDOM () % level) / 3) + 1);

    if (!fl)
        fl = get_formulalist (1);	/* safety */

    if (fl->total_chance == 0)
    {
        book->msg = add_string(" <indecipherable text>\n");
        new_text_name(book, 4);
        add_author(book,4);
        return;
      }

    /* get a random formula, weighted by its bookchance */
    chance = RANDOM () % fl->total_chance;
    for (formula = fl->items; formula != NULL; formula = formula->next) {
        chance -= formula->chance;
        if (chance <= 0)
            break;
    }

    if (!formula || formula->arch_names <= 0) {
        book->msg = add_string(" <indecipherable text>\n");
        new_text_name(book, 4);
        add_author(book,4);

    } else {
        /* looks like a formula was found. Base the amount
         * of information on the booklevel and the spellevel
         * of the formula. */

        const char   *op_name = formula->arch_name[RANDOM()%formula->arch_names];
        archetype *at;

        /* preamble */
        sprintf(retbuf, "Herein is described a project using %s: \n", 
            formula->skill?formula->skill:"an unknown skill");

        if ((at = find_archetype (op_name)) != (archetype *) NULL)
            op_name = at->clone.name;
        else
            LOG (llevError, "formula_msg() can't find arch %s for formula.\n",
            op_name);

        /* item name */
        if (strcmp (formula->title, "NONE")) {
            sprintf (retbuf, "%sThe %s of %s", retbuf, op_name, formula->title);
            /* This results in things like pile of philo. sulfur.
            * while philo. sulfur may look better, without this,
            * you get things like 'the wise' because its missing the
            * water of section.
            */
            sprintf(title,"%s: %s of %s", 
                formula_book_name[RANDOM() % (sizeof(formula_book_name) / sizeof(char*))],
                op_name, formula->title);
        }
        else
        {
            sprintf (retbuf, "%sThe %s", retbuf, op_name);
            sprintf(title,"%s: %s",
                formula_book_name[RANDOM() % (sizeof(formula_book_name) / sizeof(char*))],
                op_name);
            if (at->clone.title)
            {
                strcat (retbuf, " ");
                strcat (retbuf, at->clone.title);
                strcat(title, " ");
                strcat(title, at->clone.title);
            }
        }
        /* Lets name the book something meaningful ! */
        if (book->name) free_string(book->name);
        book->name = add_string(title);
        if (book->title) {
            free_string(book->title);
            book->title = NULL;
        }

        /* ingredients to make it */
        if (formula->ingred != NULL)
        {
            linked_char *next;
            archetype *at;

            at = find_archetype(formula->cauldron);

            sprintf(retbuf + strlen(retbuf),
                " may be made at %s using the following ingredients:\n",
                at?query_name(&at->clone):"an unknown place");

            for (next = formula->ingred; next != NULL; next = next->next)
            {
                strcat (retbuf, next->name);
                strcat (retbuf, "\n");
            }
        }
        else
            LOG (llevError, "formula_msg() no ingredient list for object %s of %s\n",
            op_name, formula->title);
        if (retbuf[strlen(retbuf)-1]!= '\n') strcat(retbuf, "\n");
        if (book->msg) free_string(book->msg);
        book->msg = add_string(retbuf);
    }
}


/** 
 * Generate a message drawn randomly from a
 * file in lib/. Level currently has no effect on the message
 * which is returned.
 *
 * @param level
 * (ignored)
 * @param booksize
 * length of the book we want.
 */
char* msgfile_msg (int level, int booksize)
{
    static char retbuf[BOOK_BUF];
    int     i, msgnum;
    linked_char *msg = NULL;

    /* get a random message for the 'book' from linked list */
    if (nrofmsg > 1)
    {
        msg = first_msg;
        msgnum = RANDOM () % nrofmsg;
        for (i = 0; msg && i < nrofmsg && i != msgnum; i++)
            msg = msg->next;
    }

    if (msg && !book_overflow (retbuf, msg->name, booksize))
        strcpy (retbuf, msg->name);
    else
        sprintf (retbuf, "\n <undecipherable text>");

#ifdef BOOK_MSG_DEBUG
    LOG (llevDebug, "\n info_list_msg() created strng: %d\n", strlen (retbuf));
    LOG (llevDebug, " MADE THIS:\n%s\n", retbuf);
#endif

    return retbuf;
}


/**
 * Generate a message detailing the properties
 * of a random god. Used by the book hack. b.t.
 *
 * @param level
 * number of elements to give.
 * @param booksize
 * desired length of the book.
 * @return
 * static buffer containing the description, or NULL if no god was found.
 *
 * @todo
 * remove static buffer.
 */
const char   *
god_info_msg (int level, int booksize)
{
    static char retbuf[BOOK_BUF];
    const char   *name = NULL;
    char buf[BOOK_BUF];
    int     i, retlen, buflen;
    size_t  introlen;
    object *god = pntr_to_god_obj (get_rand_god ());

    if (booksize > BOOK_BUF) {
        LOG(llevError,"common/readable.c:god_info_msg() - passed in booksize (%d) is larger than book buffer (%d)\n",
            booksize, BOOK_BUF);
        booksize = BOOK_BUF;
    }

    if (!god)
        return (char *) NULL;	/* oops, problems... */
    name = god->name;

    /* preamble.. */
    snprintf (retbuf, BOOK_BUF, 
        "This document contains knowledge concerning the diety %s", 
        name);

    retlen = strlen(retbuf);

    /* Always have as default information the god's descriptive terms. */
    if (nstrtok (god->msg, ",") > 0) {
        safe_strcat(retbuf, ", known as", &retlen, BOOK_BUF);
        safe_strcat (retbuf, strtoktolin (god->msg, ","), &retlen, BOOK_BUF);
    }
    else
        safe_strcat (retbuf, "...", &retlen, BOOK_BUF);

    safe_strcat (retbuf, "\n ---\n", &retlen, BOOK_BUF);

    introlen = retlen;	/* so we will know if no new info is added later */

    /* Information about the god is random, and based on the level of the
     * 'book'. This goes through this loop 'level' times, reducing level by
     * 1 each time.  If the info provided is filled up, we exit the loop.
     * otherwise, buf is appended to the existing book buffer.
     */

    while (level > 0) {
        sprintf (buf, " ");
        if (level == 2 && RANDOM () % 2) {
            /* enemy god */

            if (god->title)
                snprintf (buf, BOOK_BUF,
                  "The gods %s and %s are enemies.\n ---\n",
                  name, god->title);
        }
        if (level == 3 && RANDOM () % 2) {
            /* enemy race, what the god's holy word effects */
            const char   *enemy = god->slaying;

            if (enemy && !(god->path_denied & PATH_TURNING) && 
              (i = nstrtok (enemy, ",")) > 0) {
                char    tmpbuf[MAX_BUF];

                snprintf (buf, BOOK_BUF,
                    "The holy words of %s have the power to "
                    "slay creatures belonging to the ",
                    name);

                if (i > 1)
                    snprintf (tmpbuf, MAX_BUF, "following races:%s\n ---\n",
                      strtoktolin (enemy, ","));
                else
                    snprintf (tmpbuf, MAX_BUF, "race of%s\n ---\n",
                      strtoktolin (enemy, ","));

                buflen = strlen(buf);
                safe_strcat(buf, tmpbuf, &buflen, BOOK_BUF);
            }
        }
        if (level == 4 && RANDOM () % 2) {
            /* Priest of god gets these protect,vulnerable... */

            char cp[BOOK_BUF];
            describe_resistance(god, 1, cp, BOOK_BUF);

            if (*cp) {  /* This god does have protections */
                snprintf (buf, BOOK_BUF,
                  "%s has a potent aura which is extended to"
                  "faithful priests. The effects of this aura include:\n"
                  "%s\n ---\n",
                  name, cp);
            }
        }
        if (level == 5 && RANDOM () % 2) {
            /* aligned race, summoning  */
            const char   *race = god->race;	/* aligned race */

            if (race && !(god->path_denied & PATH_SUMMON))
                if ((i = nstrtok (race, ",")) > 0) {
                    char    tmpbuf[MAX_BUF];

                    snprintf (buf, BOOK_BUF,
                        "Creatures sacred to %s include the \n",
                        name);

                    if (i > 1)
                        snprintf (tmpbuf, MAX_BUF,
                            "following races:%s\n ---\n",
                            strtoktolin (race, ","));
                    else
                        snprintf (tmpbuf, MAX_BUF,
                            "race of %s\n ---\n", 
                            strtoktolin (race, ","));

                    buflen = strlen(buf);
                    safe_strcat(buf, tmpbuf, &buflen, BOOK_BUF);
                }
        }
        if (level == 6 && RANDOM () % 2) {
            /* blessing,curse properties of the god */

            char cp[MAX_BUF];
            describe_resistance(god, 1, cp, MAX_BUF);

            if (*cp) {  /* This god does have protections */
                snprintf (buf, MAX_BUF,
                    "%s\nThe priests of %s are known to be able to "
                    "bestow a blessing which makes the recipient "
                    "%s\n ---\n",
                    buf, name, cp);
            }
        }
        if (level == 8 && RANDOM () % 2) {
            /* immunity, holy possession */
            int     has_effect = 0, tmpvar;
            char    tmpbuf[MAX_BUF];

            snprintf (buf, MAX_BUF,
                "\nThe priests of %s are known to make cast a mighty "
                "prayer of possession which gives the recipient"
                ,name);

            for (tmpvar=0; tmpvar<NROFATTACKS; tmpvar++) {
                if (god->resist[tmpvar]==100) {
                    has_effect = 1;
                    snprintf(tmpbuf, MAX_BUF,"Immunity to %s", 
                        attacktype_desc[tmpvar]);
                }
            }
            if (has_effect) {
                buflen = strlen(buf);
                safe_strcat(buf, tmpbuf, &buflen, BOOK_BUF);
                safe_strcat(buf, "\n ---\n", &buflen, BOOK_BUF);
            }
            else
                sprintf (buf, " ");
        }
        if (level == 12 && RANDOM () % 2) {
            /* spell paths */
            int     has_effect = 0;

            snprintf (buf, MAX_BUF,
                "It is rarely known fact that the priests of %s "
                "are mystically transformed. Effects of this include:\n",
                name);
            buflen = strlen(buf);

            if (god->path_attuned) {
                has_effect = 1;
                DESCRIBE_PATH_SAFE (buf, god->path_attuned, "Attuned", 
                    &buflen, BOOK_BUF);
            }
            if (god->path_repelled) {
                has_effect = 1;
                DESCRIBE_PATH_SAFE (buf, god->path_repelled, "Repelled",
                    &buflen, BOOK_BUF);
            }
            if (god->path_denied) {
                has_effect = 1;
                DESCRIBE_PATH_SAFE (buf, god->path_denied, "Denied",
                    &buflen, BOOK_BUF);
            }
            if (has_effect) {
                safe_strcat(buf, "\n ---\n", &buflen, BOOK_BUF);
            }
            else
                sprintf (buf, " ");
        }

        /* check to be sure new buffer size dont exceed either
        * the maximum buffer size, or the 'natural' size of the
        * book...
        */
        if (book_overflow (retbuf, buf, booksize))
            break;
        else if (strlen (buf) > 1)
            safe_strcat (retbuf, buf, &retlen, BOOK_BUF);

        level--;
    }
    if (retlen == introlen) {
        /* we got no information beyond the preamble! */
        safe_strcat (retbuf, 
            " Unfortunately the rest of the information is"
            "  hopelessly garbled!\n ---\n", &retlen, BOOK_BUF);
    }
#ifdef BOOK_MSG_DEBUG
    LOG (llevDebug, "\n god_info_msg() created strng: %d\n", strlen (retbuf));
    fprintf (logfile, " MADE THIS:\n%s", retbuf);
#endif
    return retbuf;
}

/**
 * The main routine. This chooses a random
 * message to put in given readable object (type==BOOK) which will 
 * be referred hereafter as a 'book'. We use the book level to de- 
 * termine the value of the information we will insert. Higher 
 * values mean the book will (generally) have better/more info. 
 * See individual cases as to how this will be utilized.
 * "Book" name/content length are based on the weight of the 
 * document. If the value of msg_type is negative, we will randomly
 * choose the kind of message to generate. 
 * -b.t. thomas@astro.psu.edu
 *
 * @param book
 * the object we are creating into. Must be a book, can have a level.
 * @param msg_type
 * if it is a positive value, we use that to determine the
 * message type - otherwise a random value is used.
 */
void tailor_readable_ob (object *book, int msg_type)
{
    char    msgbuf[BOOK_BUF];
    int     level = book->level ? (RANDOM () % book->level) + 1 : 1;
    int     book_buf_size;

    /* safety */
    if (book->type != BOOK)
        return;

    if (level <= 0)
        return;			/* if no level no point in doing any more... */

    /* Max text length this book can have. */
    book_buf_size = BOOKSIZE (book);

    /* &&& The message switch &&& */
    /* Below all of the possible types of messages in the "book"s.
     */
    /*
     * IF you add a new type of book msg, you will have to do several things.
     * 1) make sure there is an entry in the msg switch below!
     * 2) make sure there is an entry in max_titles[] array.
     * 3) make sure there are entries for your case in new_text_title()
     *    and add_authour().
     * 4) you may want separate authour/book name arrays in read.h
     */

    msg_type = msg_type > 0 ? msg_type : (RANDOM () % 6);
    switch (msg_type)
    {
        case 1:			/* monster attrib */
            strcpy (msgbuf, mon_info_msg (level, book_buf_size));
            break;
        case 2:			/* artifact attrib */
        strcpy (msgbuf, artifact_msg (level, book_buf_size));
        break;
        case 3:			/* grouping incantations/prayers by path */
            strcpy (msgbuf, spellpath_msg (level, book_buf_size));
            break;
        case 4:			/* describe an alchemy formula */
            make_formula_book(book, level);
            /* make_formula_book already gives title */
            return;
            break;
        case 5:			/* bits of information about a god */
            strcpy (msgbuf, god_info_msg (level, book_buf_size));
            break;
        case 0:			/* use info list in lib/ */
        default:
            strcpy (msgbuf, msgfile_msg (level, book_buf_size));
        break;
    }

    strcat (msgbuf, "\n");	/* safety -- we get ugly map saves/crashes w/o this */
    if (strlen (msgbuf) > 1)
    {
        if (book->msg)
            free_string (book->msg);
        book->msg = add_string (msgbuf);
        /* lets give the "book" a new name, which may be a compound word */
        change_book (book, msg_type);
      }

}


/*****************************************************************************
 *
 * Cleanup routine for readble stuff.
 *
 *****************************************************************************/

void free_all_readable (void)
{
    titlelist *tlist, *tnext;
    title  *title1, *titlenext;
    linked_char *lmsg, *nextmsg;
    objectlink *monlink, *nextmon;

    LOG (llevDebug, "freeing all book information\n");

    for (tlist = booklist; tlist != NULL; tlist = tnext)
    {
        tnext = tlist->next;
        for (title1 = tlist->first_book; title1; title1 = titlenext)
        {
            titlenext = title1->next;
            if (title1->name)
                free_string (title1->name);
            if (title1->authour)
                free_string (title1->authour);
            if (title1->archname)
                free_string (title1->archname);
            free (title1);
        }
        free (tlist);
    }
    for (lmsg = first_msg; lmsg; lmsg = nextmsg)
    {
        nextmsg = lmsg->next;
        if (lmsg->name)
            free_string (lmsg->name);
        free (lmsg);
    }
    for (monlink = first_mon_info; monlink; monlink = nextmon)
    {
        nextmon = monlink->next;
        free (monlink);
    }
}


/*****************************************************************************
 *
 * Writeback routine for updating the bookarchive.
 *
 ****************************************************************************/

/**
 * Write out the updated book archive to bookarch file.
 *
 * @todo
 * reset the need_to_write_bookarchive flag only if success. Check write succeeds.
 */
void write_book_archive (void)
{
    FILE   *fp;
    int     index = 0;
    char    fname[MAX_BUF];
    title  *book = NULL;
    titlelist *bl = get_titlelist (0);

    /* If nothing changed, don't write anything */
    if (!need_to_write_bookarchive) return;
    need_to_write_bookarchive=0;

    sprintf (fname, "%s/bookarch", settings.localdir);
    LOG (llevDebug, "Updating book archive: %s...\n", fname);

    if ((fp = fopen (fname, "w")) == NULL)
    {
        LOG (llevDebug, "Can't open book archive file %s\n", fname);
    }
    else
    {
        while (bl)
        {
            for (book = bl->first_book; book; book = book->next)
                if (book && book->authour)
                {
                    fprintf (fp, "title %s\n", book->name);
                    fprintf (fp, "authour %s\n", book->authour);
                    fprintf (fp, "arch %s\n", book->archname);
                    fprintf (fp, "level %d\n", book->level);
                    fprintf (fp, "type %d\n", index);
                    fprintf (fp, "size %d\n", book->size);
                    fprintf (fp, "index %d\n", book->msg_index);
                    fprintf (fp, "end\n");
                }
            bl = bl->next;
            index++;
        }
        fclose (fp);
        chmod (fname, SAVE_MODE);
    }
}

/**
 * Get the readable type for an object (hopefully book).
 * @param readable
 * object for which we want the readable type.
 * @return
 * type of the book. Will never be NULL.
 */
readable_message_type* get_readable_message_type(object* readable) {
    uint8 subtype = readable->subtype;
    if (subtype>last_readable_subtype)
        return &(readable_message_types[0]);
    return &(readable_message_types[subtype]);
}
