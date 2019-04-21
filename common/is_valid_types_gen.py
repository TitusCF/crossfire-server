'''
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2021 The Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
'''
import re
import sys

# Here we will open the file specified on the command line to search for valid object types.
# This replaces the previous script, is_valid_types_gen.pl, since it is the only required part
# of the server that requires perl. The server already needs Python for gameplay stuff, so
# we're better off using Python than perl for dynamically building the check of valid types.

# sys.argv[0] is the script name
if len(sys.argv) > 1:
    # Initialize variables we'll need later
    max_obj = -1
    type = [False] * 255 # 255 is arbitrary. If we get more object types than that, then we'll need to adjust.
    # sys.argv[1] is the file we want to open
    with open(sys.argv[1], 'r') as file:
        # Pre-compile the regexes we will be checking for.
        start_type_list = re.compile(r"enum object_type")
        type_listing_match = re.compile(r"^\W*(\w+)\s*\=\s*(\d+)\,?.*$")
        end_type_list = re.compile(r"^(([^}]*};).*)$")
        # While we haven't found the start, we want to only check for the start.
        start_handling = False
        line = file.readline()
        # While there are lines in the file to consume:
        while line is not None:
            # Skip lines until we find the ones we want to parse.
            if not start_handling:
                found = start_type_list.search(line)
                if found is not None:
                    # If we found it, set the flag.
                    start_handling = True
            else:
                found = end_type_list.search(line)
                if found is not None:
                    break # If we found the end, then break out of the read loop.
                found = type_listing_match.search(line)
                if found is not None:
                    # If the field name is OBJECT_TYPE_MAX, then we note that as a special case
                    if found.group(1) == "OBJECT_TYPE_MAX":
                        max_obj = int(found.group(2))
                    else:
                        # We found an object type. second capture group is the number.
                        type[int(found.group(2))] = True
            line = file.readline()

    # Okay. Now we produce the output.
    print("/*****************************************")
    print(" * This file is automatically generated! *")
    print(" * Its contents will be overwritten on   *")
    print(" * the next build.                       *")
    print(" *                                       *")
    print(" * is_valid_types_gen.py generates this. *")
    print(" *****************************************/")
    print("\n/**")
    print(" * Checks if the specified type is a valid one for a Crossfire object.")
    print(" *")
    print(" * @param type value to check.")
    print(" * @return 1 if the type is valid, 0 else.")
    print(" */")
    print("#include <global.h>")
    print("#include <libproto.h>")
    print("int is_type_valid(uint8_t type) {")
    print("    if (type >= OBJECT_TYPE_MAX)")
    print("        return 0;")
    print("    switch (type) { // Listed types are invalid ones")
    # Now we print the invalid types.
    for i in range(1, max_obj):
        if (type[i] != True):
            print("        case %d:" % i)
    print("            return 0;")
    print("    }")
    print("    return 1;")
    print("}")