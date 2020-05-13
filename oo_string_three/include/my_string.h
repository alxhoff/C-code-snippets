#ifndef __MY_STRING_H__
#define __MY_STRING_H__

#include "stdint.h"

typedef void *my_string_handle_t;

my_string_handle_t myStringInit(const char *the_string);
int myStringSetString(my_string_handle_t the_string_structure,
		      const char *the_string);
char *myStringGetString(my_string_handle_t the_string_structure);
ssize_t myStringGetStringLength(my_string_handle_t the_string_structure);

#endif // __MY_STRING_H__
