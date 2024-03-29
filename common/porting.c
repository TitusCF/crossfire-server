/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file porting.c
 * This file contains various functions that are not really unique for
 * crossfire, but rather provides what should be standard functions
 * for systems that do not have them.  In this way, most of the
 * nasty system dependent stuff is contained here, with the program
 * calling these functions.
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32 /* ---win32 exclude/include headers */
#include "process.h"
#else
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/wait.h>
#endif

/* Has to be after above includes so we don't redefine some values */
#include "global.h"

/*****************************************************************************
 * File related functions
 ****************************************************************************/

/**
 * A replacement for the tempnam_local() function since that one is not very
 * secure. This one will open the file in an atomic way on platforms where it is
 * possible.
 *
 * @param dir
 * Directory where to create the file. Can be NULL, in which case NULL is returned.
 * @param pfx
 * Prefix to create unique name. Can be NULL.
 * @param filename
 * This should be a pointer to a char*, the function will overwrite the char*
 * with the name of the resulting file. Must be freed by caller. Value is
 * unchanged if the function returns NULL.
 * @return
 * A pointer to a FILE opened exclusively, or NULL if failure.
 * It is up to the caller to properly close it.
 * @note
 * The file will be opened read-write.
 *
 * @todo
 * Maybe adding some ifdef for non-UNIX? I don't have any such system around
 * to test with.
 */
FILE *tempnam_secure(const char *dir, const char *pfx, char **filename) {
    char *tempname = NULL;
    int fd;
    int i;
    FILE *file = NULL;
    const int maxretry = 10;

    /* Limit number of retries to MAXRETRY */
    for (i = 0; i < maxretry; i++) {
        tempname = tempnam(dir, pfx);
        if (!tempname)
            return NULL;

        fd = open(tempname, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd != -1)
            break;
        if (errno == EEXIST)
            LOG(llevError, "Created file detected in tempnam_secure. Someone hoping for a race condition?\n");
        free(tempname);
    }
    /* Check that we successfully got an fd. */
    if (fd == -1)
        return NULL;

    file = fdopen(fd, "w+");
    if (!file) {
        LOG(llevError, "fdopen() failed in tempnam_secure()!\n");
        free(tempname);
        return NULL;
    }

    *filename = tempname;
    return file;
}

/**
 * This function removes everything in the directory, and the directory itself.
 *
 * Errors are LOG() to error level.
 *
 * @param path
 * directory to remove.
 *
 * @note
 * will fail if any file has a name starting by .
 */
void remove_directory(const char *path) {
    DIR *dirp;
    char buf[MAX_BUF];
    struct stat statbuf;
    int status;

    if ((dirp = opendir(path)) != NULL) {
        struct dirent *de;

        for (de = readdir(dirp); de; de = readdir(dirp)) {
            /* Don't remove '.' or '..'  In  theory we should do a better
             * check for .., but the directories we are removing are fairly
             * limited and should not have dot files in them.
             */
            if (de->d_name[0] == '.')
                continue;

            /* Linux actually has a type field in the dirent structure,
             * but that is not portable - stat should be portable
             */
            status = stat(de->d_name, &statbuf);
            if ((status != -1) && (S_ISDIR(statbuf.st_mode))) {
                snprintf(buf, sizeof(buf), "%s/%s", path, de->d_name);
                remove_directory(buf);
                continue;
            }
            snprintf(buf, sizeof(buf), "%s/%s", path, de->d_name);
            if (unlink(buf)) {
                LOG(llevError, "Unable to remove %s\n", path);
            }
        }
        closedir(dirp);
    }
    if (rmdir(path)) {
        LOG(llevError, "Unable to remove directory %s\n", path);
    }
}

/**
 * Checks if any directories in the given path doesn't exist, and creates if necessary.
 *
 * @param filename
 * file path we'll want to access. Can be NULL.
 *
 * @note
 * will LOG() to debug and error.
 */
void make_path_to_file(const char *filename) {
    char buf[MAX_BUF], *cp = buf;
    struct stat statbuf;

    if (!filename || !*filename)
        return;

    safe_strncpy(buf, filename, sizeof(buf));
    while ((cp = strchr(cp+1, (int)'/'))) {
        *cp = '\0';
        if (stat(buf, &statbuf) || !S_ISDIR(statbuf.st_mode)) {
            LOG(llevDebug, "Was not dir: %s\n", buf);
#ifdef WIN32
            if (mkdir(buf)) {
#else
            if (mkdir(buf, SAVE_DIR_MODE)) {
#endif
                LOG(llevError, "Cannot mkdir %s: %s\n", buf, strerror(errno));
                return;
            }
        }
        *cp = '/';
    }
}

/**
 * Simple function we use below to keep adding to the same string
 * but also make sure we don't overwrite that string.
 *
 * @param dest
 * string to append to.
 * @param orig
 * string to append.
 * @param[out] curlen
 * current length of dest. Will be updated by this function.
 * @param maxlen
 * maximum length of dest buffer.
 */
void safe_strcat(char *dest, const char *orig, size_t *curlen, size_t maxlen) {
    assert(curlen != NULL);
    assert(*curlen < maxlen);
#ifdef HAVE_STRLCAT
    *curlen = strlcat(dest, orig, maxlen);
#else
    if (*curlen == (maxlen-1))
        return;
    strncpy(dest+*curlen, orig, maxlen-*curlen-1);
    dest[maxlen-1] = 0;
    *curlen += strlen(orig);
#endif
    if (*curlen > (maxlen-1))
        *curlen = maxlen-1;
}

#ifndef HAVE_STRLCPY
/**
 * Portable implementation of strlcpy(3).
 */
size_t strlcpy(char *dst, const char *src, size_t size) {
    strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
    return strlen(src);
}
#endif

#ifdef WIN32
#include <string.h>
#include <ctype.h>
const char *strcasestr(const char *s, const char *find)
{
    char c, sc;
    size_t len;

    if ((c = *find++) != 0) {
        c = tolower((unsigned char) c);
        len = strlen(find);
        do {
            do {
                if ((sc = *s++) == 0)
                    return (NULL);
            }
            while ((char) tolower((unsigned char) sc) != c);
        }
        while (strncasecmp(s, find, len) != 0);
        s--;
    }
    return s;
}
#endif
