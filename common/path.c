/* $Id$ */

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


char *path_combine(const char *src, const char *dst) {
    char *p;
    static char path[HUGE_BUF];

    if (*dst == '/') {
        /* absolute destination path => ignore source path */
        strcpy(path, dst);
    } else {
        /* relative destination path => add after last '/' of source */
        strcpy(path, src);
        p = strrchr(path, '/');
        if (p != NULL) {
            p++;
        } else {
            p = path;
            if (*src == '/')
                *p++ = '/';
        }
        strcpy(p, dst);
    }

#if defined(DEBUG_PATH)
    LOG(llevDebug, "path_combine(%s, %s) = %s\n", src, dst, path);
#endif
    return(path);
}

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

char *path_combine_and_normalize(const char *src, const char *dst) {
    char *path;

    path = path_combine(src, dst);
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
    const char *res;

    fprintf(stderr, "path_combine_and_normalize(%s, %s) = ", src, dst);
    res = path_combine_and_normalize(src, dst);
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
