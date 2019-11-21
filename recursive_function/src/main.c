#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 200
const char *selection = "0123456789ABCEDFGHIGKLMNOPQRSTUVWXYZ";

void anybase(int n, int b, char *s)
{
	if (n) {
		anybase(n / b, b, s);

		strncat(&s[strlen(s)], &selection[n % b], sizeof(char));
		printf("%s, %d, %d\n", s, n, b);
	}
}

int main()
{
	char buf[BUFFER_SIZE];
	memset(buf, 0, sizeof(char) * BUFFER_SIZE);

	anybase(900000, 18, buf);
	printf("Ouput: %s, N: %d, B: %d\n", buf, 90000, 18);
}
