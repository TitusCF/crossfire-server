/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel & Crossfire Development Team
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

    The authors can be reached via e-mail to crossfire-devel@real-time.com
*/

/*
 * Small program that gives plugins information about the compile-time
 * configuration of the server. Only the most common parameters (those that
 * might be needed by an independent configure script) are available. The rest
 * can be found in 'config.h' and should be included in any plugin needing it.
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct {
    const char *name;
    const char *value;
} cf_parameter;

const cf_parameter cf_parameter_list[] = {
    { "CONFDIR", CONFDIR },
    { "DATADIR", DATADIR },
    { "LIBDIR", LIBDIR },
    { "LOCALDIR", LOCALDIR },
    { "PLUGIN_SUFFIX", PLUGIN_SUFFIX },
};

const int cf_parameter_list_size =
    sizeof(cf_parameter_list) / sizeof(cf_parameter);

static void print_usage() {
    fprintf(stderr,
        "usage: crossfire-config <parameter>\n"
        "       crossfire-config [options]\n"
        "\n"
        "Options:\n"
        "    --Dflags            show complete Dflags line from compiler invocation\n"
        "    --parameter-list    show the list of available parameters\n"
        "\n"
    );
}

int main(int argc, char *argv[]) {
    int i;

    // Print usage instructions when given invalid command-line arguments.
    if (argc != 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "--Dflags") == 0) {
        for (i = 0; i < cf_parameter_list_size; i++) {
            printf("-D%s=\\\"%s\\\" ", cf_parameter_list[i].name,
                    cf_parameter_list[i].value);
        }

        printf("\n");
        exit(EXIT_SUCCESS);
    }

    // Show a list of available parameters.
    if (strcmp(argv[1], "--parameter-list") == 0) {
        printf("Available parameters:\n");

        // PLUGININSTALLDIR is a special parameter handled separately.
        printf("    PLUGININSTALLDIR\n");

        for (i = 0; i < cf_parameter_list_size; i++) {
            printf("    %s\n", cf_parameter_list[i].name);
        }

        exit(EXIT_SUCCESS);
    }

    // Handle plugin installation directory as a special case.
    if (strcmp(argv[1], "PLUGININSTALLDIR") == 0) {
        printf("%s/plugins/\n", LIBDIR);
        exit(EXIT_SUCCESS);
    }

    for (i = 0; i < cf_parameter_list_size; i++) {
        if (!strcmp(argv[1], cf_parameter_list[i].name)) {
            printf("%s\n", cf_parameter_list[i].value);
            exit(EXIT_SUCCESS);
        }
    }

    exit(EXIT_FAILURE);
}
