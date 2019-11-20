#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100
const char *selection = "0123456789ABCEDFGHIGKLMNOPQRSTUVWXYZ";

char *anybase(int n, int b)
{
	char *s = calloc(BUFFER_SIZE, sizeof(char));
	int len;

	if (n == 0)
		return strcpy(s, "");

	s = anybase(n / b, b);

	len = strlen(s);
	strncat(&s[len], &selection[n % b], sizeof(char));
	printf("%s, %d, %d\n", s, n, b);

	return s;
}
int main()
{
	char *s;

	s = anybase(900000, 18);
	printf("Ouput: %s, N: %d, B: %d\n", s, 90000, 18);
}
