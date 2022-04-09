#ifndef STR_UTIL_H
#define STR_UTIL_H

#include "global/global.h"

#include <string.h>


INLINE char* str_util_find_last_of(char* str, char* identifier)
{
    char* last = NULL;
    int n = strlen(identifier);
    while (*str)
    {
        // go through str and compare with identifier
        if (!memcmp(str++, identifier, n))
        {
            last = str - 1;
        }
    }
    return last;
}

INLINE char* str_util_trunc(char* str, int pos)
{
    int len = strlen(str);

    if (len > abs(pos)) 
    {
        if (pos > 0)
        {
            str = str + pos;
        }
        else
        {
            str[len + pos] = '\0';
        }
    }

    return str;
}

#endif
