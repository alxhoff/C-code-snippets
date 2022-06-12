#include "ll.h"

struct my_integers {
    int a;
    int b;
    int c;
    struct list_item list;
};

int main(int argc, char* argv[])
{
    struct list_item my_list;

    ll_init_list(&my_list);

    struct my_integers ints_one = { .a = 1, .b = 2, .c = 3 };
    struct my_integers ints_two = { .a = 4, .b = 5, .c = 6 };

    ll_add(&ints_one.list, &my_list);
    ll_add_tail(&ints_two.list, &ints_one.list);

    struct my_integers* first_struct
        = get_container(my_list.next, struct my_integers, list);

    struct my_integers* last_struct
        = get_container(my_list.prev, struct my_integers, list);

    return 0;
}
