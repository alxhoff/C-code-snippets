#include <stdlib.h>
#include <string.h>

#include "my_string.h"

#define STRING_BUFFER_LEN   1000

struct my_string {
	ssize_t str_len;
	char string[STRING_BUFFER_LEN + 1];
};

static struct my_string our_string = {0};

int myStringSetString(const char *the_string_to_set)
{
    if(the_string_to_set == NULL)
        return -1;

    size_t string_to_set_len = strlen(the_string_to_set);

    if(string_to_set_len > STRING_BUFFER_LEN)
        return -1;

    our_string.str_len = string_to_set_len;
    strcpy((char *)our_string.string, the_string_to_set);

    return 0;
}

char *myStringGetString(void)
{
    return strdup((char *)our_string.string);
}

ssize_t myStringGetStringLen(void)
{
    return our_string.str_len;
}

