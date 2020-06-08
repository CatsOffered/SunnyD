#include "framework.h"

int strcasecmp(char const* a, char const* b, size_t MaxCount)
{
    int d = 0;
    for (size_t i = 0; i < MaxCount; a++, b++, i++)
    {
        d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
           break;
    }
    return d;
}