#ifndef UTILS_H
#define UTILS_H

#include <string>

extern "C" {
#include "global.h"
}

class Utils {
public:
    /**
     * Checks if a string ends with another one.
     * @param str string to check the end of.
     * @param with string to check at the end of str.
     * @return true if 'str' ends with 'with', false else.
     */
    static bool endsWith(const std::string& str, const std::string& with);

    /**
     * Write a list of strings as ' a,b,c' (leading space).
     * @param list items to write.
     * @param buf where to write.
     */
    static void writeLinkedChar(const linked_char *list, StringBuffer *buf);

    /**
     * Write a list of strings as ' a,b,c' (leading space).
     * @param items items to write.
     * @param count number of items.
     * @param buf where to write.
     */
    static void writeStringArray(char **items, size_t count, StringBuffer *buf);
};

#endif /* UTILS_H */

