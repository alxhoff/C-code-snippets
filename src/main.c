#include <stdio.h>
int main(int argc, char* argv[])
{
    char input;
prompt_again:
    printf("Enter number:");
    input = getchar() - '0';

    if ((input >= 1) && (input <= 6)) {
        // Do whatever it is you need to do
        getchar(); // clear stdin
        goto prompt_again;
    } else
        goto finish;

finish:
    printf("Wrong option\n");
}
