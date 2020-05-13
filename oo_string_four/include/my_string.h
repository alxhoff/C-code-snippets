#ifndef __MY_STRING_H__
#define __MY_STRING_H__

#include <stdint.h>

int myStringSetString(const char *the_string_to_set);
char *myStringGetString(void);
ssize_t myStringGetStringLen(void);

#endif // __MY_STRING_H__
