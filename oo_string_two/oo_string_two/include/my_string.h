#ifndef __MY_STRING_H__
#define __MY_STRING_H__

#include "stdint.h"

struct my_string {
	ssize_t str_len;
	char *string;
};

struct my_string *myStringInit(const char *the_string);
int myStringSetString(struct my_string *the_string_structure,
		      const char *the_string);
char *myStringGetString(struct my_string *the_string_structure);

#endif // __MY_STRING_H__
