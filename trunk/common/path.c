/* $Id$ */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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
 * @file path.c
 * Contains file path manipulation functions.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <global.h>

#include "define.h"
#include "path.h"


#if 0
/**
 * Define DEBUG_PATH to enable debug output.
 */
#define DEBUG_PATH
#endif

#if 0
/**
 * Define TEST_PATH to compile this file as a stand-alone self-test application
 * for the functions.
 */
#define TEST_PATH

#define LOG fprintf
#define llevDebug stderr
#define llevError stderr
#endif

/**
 * Combines 2 paths, which can be relative.
 *
 * @param src
 * path we're starting from.
 * @param dst
 * path we're doing to.
 * @param path
 * buffer containing the combined path.
 * @param size
 * size of path.
 * @return
 * path.
 *
 * @note
 * this doesn't handle the '..', check path_normalize().
 */
char *path_combine(const char *src, const char *dst, char* path, int size) {
    char *p;

    if (*dst == '/') {
        /* absolute destination path => ignore source path */
        snprintf(path, size, dst);
    } else {
        /* relative destination path => add after last '/' of source */
        snprintf(path, size, src);
        p = strrchr(path, '/');
        if (p != NULL) {
            p++;
        } else {
            p = path;
            if (*src == '/')
                *p++ = '/';
        }
        snprintf(p, size - (p - path), dst);
    }

#if defined(DEBUG_PATH)
    LOG(llevDebug, "path_combine(%s, %s) = %s\n", src, dst, path);
#endif
    return(path);
}

/**
 * Cleans specified path. Removes .. and things like that.
 *
 * @param path
 * path to clear. It will be modified in place.
 * @note
 * there shouldn't be any buffer overflow, as we just remove stuff.
 */
void path_normalize(char *path) {
    char *p; /* points to the beginning of the path not yet processed; this is
                either a path component or a path separator character */
    char *q; /* points to the end of the path component p points to */
    char *w; /* points to the end of the already normalized path; w <= p is
                maintained */
    size_t len; /* length of current component (which p points to) */

#if defined(DEBUG_PATH)
    LOG(llevDebug, "path_normalize: input '%s'\n", path);
#endif

    p = path;
    w = p;
    while (*p != '\0') {

        if (*p == '/') {
            if ((w == path && *path == '/') || (w > path && w[-1] != '/'))
                *w++ = '/';
            p++;
            continue;
        }

        q = strchr(p, '/');
        if (q == NULL)
            q = p+strlen(p);
        len = q-p;
        assert(len > 0);

#if defined(DEBUG_PATH)
        LOG(llevDebug, "path_normalize: checking '%.*s'\n", (int)len, p);
#endif

        if (len == 1 && *p == '.') {
            /* remove current component */
        } else if (len == 2 && memcmp(p, "..", 2) == 0) {
            if (w == path || (w == path+3 && memcmp(path, "../", 3) == 0)) {
                /* keep ".." at beginning of relative path ("../x" => "../x") */
                memmove(w, p, len);
                w += len;
            } else if (w == path+1 && *path == '/') {
                /* remove ".." at beginning of absolute path ("/../x" => "/x") */
            } else {
                /* remove both current component ".." and preceding one */
                if (w > path && w[-1] == '/')
                    w--;
                while (w > path && w[-1] != '/')
                    w--;
            }
        } else {
            /* normal component ==> add it */
            memmove(w, p, len);
            w += len;
        }

        p = q;

#if defined(DEBUG_PATH)
        LOG(llevDebug, "path_normalize: so far '%.*s'\n", (int)(w-path), path);
#endif
    }

    /* remove trailing slashes, but keep the one at the start of the path */
    while (w > path+1 && w[-1] == '/') {
        w--;
    }

    *w = '\0';

#if defined(DEBUG_PATH)
    LOG(llevDebug, "path_normalize: result '%s'\n", path);
#endif
}

/**
 * Combines the 2 paths.
 *
 * @param src
 * path we're starting from.
 * @param dst
 * path we're getting to.
 * @param path
 * buffer that will contain combined paths.
 * @param size
 * length of path.
 * @return
 * path
 */
char *path_combine_and_normalize(const char *src, const char *dst, char* path, int size) {

    path_combine(src, dst, path, size);
    path_normalize(path);
    return(path);
}

#if defined(TEST_PATH)
static void check_combine(const char *src, const char *dst, const char *exp) {
    const char *res;

    fprintf(stderr, "path_combine(%s, %s) = ", src, dst);
    res = path_combine(src, dst);
    fprintf(stderr, "%s", res);
    if (strcmp(res, exp) != 0) {
        fprintf(stderr, ", should be %s\n", exp);
    } else {
        fprintf(stderr, " (OK)\n");
    }
}

static void check_normalize(const char *path, const char *exp0) {
    char tmp[HUGE_BUF];
    char exp[HUGE_BUF];

    strcpy(exp, exp0 == NULL ? path : exp0);

    strcpy(tmp, path);
    fprintf(stderr, "path_normalize(%s) = ", tmp);
    path_normalize(tmp);
    fprintf(stderr, "%s", tmp);
    if (strcmp(tmp, exp) != 0) {
        fprintf(stderr, ", should be %s\n", exp);
    } else {
        fprintf(stderr, " (OK)\n");
    }
}

static void check_combine_and_normalize(const char *src, const char *dst, const char *exp) {
    char res[HUE_BUF];

    fprintf(stderr, "path_combine_and_normalize(%s, %s) = ", src, dst);
    path_combine_and_normalize(src, dst, res, sizeof(res));
    fprintf(stderr, "%s", res);
    if (strcmp(res, exp) != 0) {
        fprintf(stderr, ", should be %s\n", exp);
    } else {
        fprintf(stderr, " (OK)\n");
    }
}

int main() {
    check_combine("/path1/file1", "/path2/file2", "/path2/file2");
    check_combine("path1/file1", "/path2/file2", "/path2/file2");
    check_combine("/path1/file1", "path2/file2", "/path1/path2/file2");
    check_combine("path1/file1", "path2/file2", "path1/path2/file2");
    check_combine("/path1", "/path2", "/path2");
    check_combine("path1", "/path2", "/path2");
    check_combine("/path1", "path2", "/path2");
    check_combine("path1", "path2", "path2");

    check_normalize("", NULL);
    check_normalize("/", NULL);
    check_normalize("path1/file1", NULL);
    check_normalize("/path1/file1", NULL);
    check_normalize("/path1//file1", "/path1/file1");
    check_normalize("//path1/file1", "/path1/file1");
    check_normalize("///////x////////y///////z////////", "/x/y/z");
    check_normalize("/a/b/../c/d/../e/../../f/g/../h", "/a/f/h");
    check_normalize("//a//b//..//c//d//..//e//..//..//f//g//..//h", "/a/f/h");
    check_normalize("../a", NULL);
    check_normalize("a/../../b", "../b");
    check_normalize("/../a", "/a");
    check_normalize("/a/../../b", "/b");
    check_normalize("./b/./c/.d/..e/./f", "b/c/.d/..e/f");
    check_normalize("/b/././././e", "/b/e");
    check_normalize(".", ""); /* maybe the result should be "."? */
    check_normalize("/.", "/");
    check_normalize("./", ""); /* maybe the result should be "."? */
    check_normalize("/a/b/..", "/a");
    check_normalize("/a/b/../..", "/");
    check_normalize("/a/b/../../..", "/");
    check_normalize("a/b/..", "a");
    check_normalize("a/b/../..", "");
    check_normalize("a/b/../../..", "..");

    check_combine_and_normalize("/path1/file1", "/path2/file2", "/path2/file2");
    check_combine_and_normalize("path1/file1", "/path2/file2", "/path2/file2");
    check_combine_and_normalize("/path1/file1", "path2/file2", "/path1/path2/file2");
    check_combine_and_normalize("/path1", "/path2", "/path2");
    check_combine_and_normalize("path1", "/path2", "/path2");
    check_combine_and_normalize("/path1", "path2", "/path2");
    check_combine_and_normalize("/path1/file1/../u", "path2/x/../y/z/../a/b/..", "/path1/path2/y/a");
    check_combine_and_normalize("/path1/file1", "/path2//file2", "/path2/file2");
    check_combine_and_normalize("/path1/file1", "/..", "/");
    check_combine_and_normalize("/path1/file1", "../x", "/x");
    check_combine_and_normalize("/path1/file1", "../../../x", "/x");
    check_combine_and_normalize("/path1/file1", "/.x/..x/...x/x", "/.x/..x/...x/x");

    return(0);
}
#endif
