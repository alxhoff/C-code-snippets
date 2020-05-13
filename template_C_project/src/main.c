#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

struct my_struct {
    int a;
    int b;
    int foo[5];
};

void my_func(struct my_struct *stru)
{
    (*stru).a++;
    
    stru->a++;
}

int main(int argc, char* argv[])
{
    struct my_struct bar = {
        .a = 4,
        .b = 7,
        .foo = {1,2,3,4,5},
    };

    bar.a++;

    struct my_struct *p_bar = &bar;

    my_func(p_bar);

    struct my_struct *c = calloc(1, sizeof(struct my_struct));

    my_func(c);
}
