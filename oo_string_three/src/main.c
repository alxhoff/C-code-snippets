
#include <stdio.h>
#include <stdlib.h>

#include "my_string.h"

const char *test_string = "This is the test string";

int main(int argc, char* argv[])
{

    /** struct my_string *our_string = myStringInit(test_string); */
    my_string_handle_t our_string = myStringInit(test_string);

    char *our_string_copy = myStringGetString(our_string);

    printf("%d: '%s'\n", myStringGetStringLength(our_string), our_string_copy);

    myStringSetString(our_string, "This is a second string and it is longer");
    
    free(our_string_copy);
    our_string_copy = myStringGetString(our_string);

    printf("%d: '%s'\n", myStringGetStringLength(our_string), our_string_copy);

    return 0;
}
