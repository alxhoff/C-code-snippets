#include <stdlib.h>
#include <string.h>

#include "my_string.h"

struct my_string {
	ssize_t str_len;
	char *string;
};

my_string_handle_t myStringInit(const char *the_string)
{
	struct my_string *ret = calloc(1, sizeof(struct my_string));

	if (ret == NULL)
		return NULL;

	ret->str_len = strlen(the_string);

	ret->string = calloc(ret->str_len + 1, sizeof(char));

	if (ret->string == NULL) {
		free(ret);
		return NULL;
	}

	strcpy(ret->string, the_string);

	return (my_string_handle_t)ret;
}

int myStringSetString(my_string_handle_t the_string_structure,
		      const char *the_string)
{
	if (the_string == NULL)
		return -1; // return error

    struct my_string *st = (struct my_string *) the_string_structure;

	st->str_len = strlen(the_string);

	st->string =
		realloc(st->string,
			sizeof(char) * (st->str_len + 1));

	if (st->string == NULL) {
		free(st);
		return -1;
	}

	strcpy(st->string, the_string);

	return 0;
}

char *myStringGetString(my_string_handle_t the_string_structure)
{
	return strdup(((struct my_string *)the_string_structure)->string);
}

ssize_t myStringGetStringLength(my_string_handle_t the_string_structure)
{
    return ((struct my_string *)the_string_structure)->str_len;
}
