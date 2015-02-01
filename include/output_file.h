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
 * @file
 * Functions for creating text output files. During (re-)writing a new file or
 * if a write error occurs the old file contents are still available.
 *
 * Example usage:
 * <code>
 * <pre>
 * OutputFile of;
 * FILE *f;
 * f = of_open(&of, "some_file.txt", llevError);
 * if (f == NULL)
 *     return;
 * fprintf(f, "...");
 * fprintf(f, "...");
 * if (!of_close(&of))
 *     return;
 * </pre>
 * </code>
 */

#ifndef OUTPUT_FILE_H
#define OUTPUT_FILE_H

#include <stdio.h>


typedef struct
{
    /**
     * The original output file that is written.
     */
    char *fname;

    /**
     * The temporary output file that is written to until #of_close() is
     * called.
     */
    char *fname_tmp;

    /**
     * The file pointer for fname_tmp.
     */
    FILE *file;
} OutputFile;


/**
 * Opens an output file. It is equivalent to <code>fopen(fname, "w");</code>
 * but uses a temporary file instead of <code>fname</code> until #of_close() is
 * called.
 *
 * @param of an #OutputFile instance allocated by the caller; it must not be
 * freed or reused until #of_close() has been called
 *
 * @param fname the filename to open; the string may be freed or reused
 * afterwards
 *
 * @return the file pointer or <code>NULL</code> if the file could not be
 * opened; in this case an error message has been printed and the original file
 * content are unchanged
 */
FILE *of_open(OutputFile *of, const char *fname);

/**
 * Closes an output file. This function (or #of_cancel()} must be called
 * exactly once for each successful call to #of_open().
 *
 * @param of the output file instance to operate on
 *
 * @return whether writing was successful; if unsuccessful an error message has
 * been printed and the original file content are unchanged
 */
int of_close(OutputFile *of);

/**
 * Cancels a save process. This closes and deletes the temporary file, leaving
 * the original file contents unmodified. This function (or on_close()} must
 * be called exactly once for each successful call to op_open().
 *
 * @param of the output file instance to operate on
 */
void of_cancel(OutputFile *of);

#endif
