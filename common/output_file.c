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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "logger.h"
#include "output_file.h"


/**
 * The extension for temporary files that is appended to the original output
 * filename during write operations.
 */
#define TMP_EXT ".tmp"


FILE *of_open(OutputFile *of, const char *fname) {
    char *fname_tmp;
    FILE *f;
    char tmp[1024];

    fname_tmp = malloc(strlen(fname)+sizeof(TMP_EXT));
    if (fname_tmp == NULL) {
        LOG(llevError, "%s: out of memory\n", fname);
        return NULL;
    }

    sprintf(fname_tmp, "%s%s", fname, TMP_EXT);
    remove(fname_tmp);
    f = fopen(fname_tmp, "w");
    if (f == NULL) {
        LOG(llevError, "%s: %s\n", fname_tmp, strerror_local(errno, tmp, sizeof(tmp)));
        free(fname_tmp);
        return NULL;
    }

    of->fname = strdup_local(fname);
    if (of->fname == NULL) {
        LOG(llevError, "%s: out of memory\n", fname);
        free(fname_tmp);
        fclose(f);
        return NULL;
    }
    of->fname_tmp = fname_tmp;
    of->file = f;
    return f;
}

int of_close(OutputFile *of)
{
    char tmp[1024];
    int success = 1;

    if (ferror(of->file)) {
        LOG(llevError, "%s: write error\n", of->fname);
        fclose(of->file);
        remove(of->fname_tmp);
        success = 0;
    }
    if (fclose(of->file) != 0) {
        LOG(llevError, "%s: %s\n", of->fname, strerror_local(errno, tmp, sizeof(tmp)));
        remove(of->fname_tmp);
        success = 0;
    }
    if (rename(of->fname_tmp, of->fname) != 0) {
        LOG(llevError, "%s: cannot rename from %s: %s\n", of->fname, of->fname_tmp, strerror_local(errno, tmp, sizeof(tmp)));
        remove(of->fname_tmp);
        success = 0;
    }
    free(of->fname_tmp);
    free(of->fname);
    return success;
}

void of_cancel(OutputFile *of)
{
    fclose(of->file);
    remove(of->fname_tmp);
    free(of->fname_tmp);
    free(of->fname);
}
