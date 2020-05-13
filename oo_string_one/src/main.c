
#include <stdio.h>

#include "my_string.h"

int main(int argc, char* argv[])
{
    printf("%s\n", my_string);

    my_dynamic_string = calloc(100, sizeof(char));

    strcpy(my_dynamic_string, "this is my dynamic string");

    printf("%s\n", my_dynamic_string);

    return 0;
}
