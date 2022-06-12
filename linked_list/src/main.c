#include "ll.c"
int main(int argc, char* argv[])
{
    addItem(1);
    addItem(2);
    deleteItem(1);

    printf("First item's ID: %d", head.next->ID);

    return 0;
}
