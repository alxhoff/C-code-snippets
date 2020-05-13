
#include <stdio.h>
#include <stdlib.h>

#include "my_string.h"

const char *test_string = "This is the test string";

int main(int argc, char *argv[])
{
	myStringSetString(test_string);

	char *string_copy = myStringGetString();
	ssize_t string_size = myStringGetStringLen();

	printf("%d:'%s'\n", string_size, string_copy);

    myStringSetString("This is a much longer string than the first string");

	printf("%d:'%s'\n", myStringGetStringLen(), myStringGetString());

    return 0;
}
