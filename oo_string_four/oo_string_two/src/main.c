
#include <stdio.h>

#include "my_string.h"

const char *test_string = "This is the test string";

int main(int argc, char* argv[])
{

    struct my_string *our_string = myStringInit(test_string);

    printf("%s of len %d\n", our_string->string, our_string->str_len);

    myStringSetString(our_string, "This is the second string and it is longer");

    printf("%s of len %d\n", our_string->string, our_string->str_len);

    char *string_copy = myStringGetString(our_string);

    printf("The string copy '%s'\n", string_copy);

    return 0;
}
