#include <stdlib.h>
#include <string.h>

#include "my_string.h"

struct my_string *myStringInit(const char *the_string)
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

	return ret;
}

int myStringSetString(struct my_string *the_string_structure,
		      const char *the_string)
{
	if (the_string == NULL)
		return -1; // return error

	the_string_structure->str_len = strlen(the_string);

	the_string_structure->string =
		realloc(the_string_structure->string,
			sizeof(char) * (the_string_structure->str_len + 1));

	if (the_string_structure->string == NULL) {
		free(the_string_structure);
		return -1;
	}

	strcpy(the_string_structure->string, the_string);

    return 0;
}

char *myStringGetString(struct my_string *the_string_structure)
{
    return strdup(the_string_structure->string); 
}
