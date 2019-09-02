#include <stdio.h>

void d_increment(int val)
{
    val++;
    val++;
}

void double_increment(int* val)
{
    (*val)++;

    (*val)++;
}

int main(int argc, char* argv[])
{
    int a = 0;
    int b = 5;
    int c = 0;

    a++;

    c = a + b;

    c++;

    d_increment(a);
    double_increment(&a);

    printf("hello world\n");

    printf("My three variables are %d %d %d", a, b, c);
}
