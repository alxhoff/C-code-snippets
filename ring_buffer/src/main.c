#include "rbuf.h"

struct my_struct {
	char *str;
};

char *str_one = "this is string one";
char *str_two = "and this is number two";

int main(int argc, char *argv[])
{
	struct my_struct struct_one = { .str = str_one };
	struct my_struct struct_two = { .str = str_two };

	struct my_struct tmp_struct;

	rbuf_handle_t rbuf = rbuf_init(sizeof(struct my_struct), 5);

	rbuf_put(rbuf, &struct_one);
	rbuf_fput(rbuf, &struct_two);
	rbuf_put(rbuf, &struct_one);
	rbuf_fput(rbuf, &struct_two);
	rbuf_put(rbuf, &struct_one);
	rbuf_fput(rbuf, &struct_two);
	rbuf_get(rbuf, &tmp_struct);
	rbuf_put(rbuf, &struct_one);

	for (int i = 0; i < 5; i++) {
		rbuf_get(rbuf, &tmp_struct);
		printf("pop: %s\n", tmp_struct.str);
	}

	return 0;
}
