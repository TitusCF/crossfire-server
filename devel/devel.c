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
 * Little program aimed at giving information to plugin about config of the crossfire server.
 * Simply invoke with the config parameter to get. Only the most common parameters (those
 * that could be needed by an independent configure script) are available. The rest is available
 * in config.h andd should be included in any plugin needing it.
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

const int cf_parameter_list_size = sizeof(cf_parameter_list)/sizeof(cf_parameter);

int main(int argc, char **argv) {
    int i;

    if (argc == 2) {
      if (!strcmp(argv[1], "--parameter-list")) {
          printf("parameter maybe one of:\n");
          printf("\tPLUGININSTALLDIR\n");
          for (i = 0; i < cf_parameter_list_size; i++)
              printf("\t%s\n", cf_parameter_list[i].name);
          return 0;
      }
      /*Special case, handle plugin installation dir, which is most likeley why
        user wants to use crossfire-config in a configure script*/
      if (!strcmp(argv[1], "PLUGININSTALLDIR")) {
          printf("%s/plugins/\n", LIBDIR);
          return 0;
      }
      if (!strcmp(argv[1], "--Dflags")) {
          for (i = 0; i < cf_parameter_list_size; i++)
              printf("-D%s=\\\"%s\\\" ", cf_parameter_list[i].name, cf_parameter_list[i].value);
          /*printf ("-DDATADIR=\\\"%s\\\" -DLIBDIR=\\\"%s\\\" -DLOCALDIR=\\\"%s\\\"\n",
                  cf_parameter_list[0].value, cf_parameter_list[1].value, cf_parameter_list[2].value);*/
          printf("\n");
          return 0;
      }
      for (i = 0; i < cf_parameter_list_size; i++) {
        if (!strcmp(argv[1], cf_parameter_list[i].name)) {
          printf("%s\n", cf_parameter_list[i].value);
          return 0;
        }
      }
    }
    /* Bad arguments count or invalid ones */
    printf("usage: crossfire-config --Dflags");
    printf(" (gives complete Dflags line for compiler invocation)\n");
    printf("usage: crossfire-config --parameter-list");
    printf(" (show the list of available parameters)\n");
    printf("usage: crossfire-config <parameter name>");
    printf(" (extract a compilation parameter)\n");
    return -1;
}
