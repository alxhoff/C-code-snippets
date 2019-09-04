#include <stdio.h>
#include <stdlib.h>

#include "yahtzee.h"

#define ROLL 1
#define PRINT 2
#define EXIT 3

void prompt_int(char *message, int *value)
{
retry:
	printf("%s: ", message);
	scanf("%d", value);

	if (!*value) {
		printf("You cannot enter zero\n");
		goto retry;
	}
}

unsigned char prompt_cont_print(void)
{
	char error_count = 0;
	char input;
retry:
	printf("Roll dice [r/R], print cup [p/P] or exit [e/E]:");
	input = getchar(); //Gets next char from stdin stream

	switch (input) {
	case 'r':
	case 'R':
		getchar();
		return ROLL;
	case 'p':
	case 'P':
		getchar();
		return PRINT;
	case 'e':
	case 'E':
		getchar();
		return EXIT;
	default:
		if (input != '\n')
			while ((input = getchar()) != '\n' && input != EOF)
				;

		error_count++;
		printf("Invalid input\n");
		if (error_count < 3)
			goto retry;
		else
			return 0; //Zero on error
		break;
	}
}

unsigned char check_cup(struct cup *my_cup)
{
	for (int i = 1; i < my_cup->dice_count; i++)
		if (my_cup->dice[i]->current_value !=
		    my_cup->dice[i - 1]->current_value)
			return 0;

	return 1;
}

int main(int argc, char *argv[])
{
	int max_value = 0, dice_count = 0;
	int ret;
	struct cup *my_cup = NULL;

	prompt_int("Please enter the max value of each die", &max_value);
	prompt_int("Please enter the number of die in the cup", &dice_count);

	my_cup = create_cup(dice_count);

	ret = fill_cup(my_cup, max_value);
	if (ret) //Filling failed
		exit(EXIT_FAILURE);

	getchar(); //Clear stdin
roll_again:
	ret = prompt_cont_print();
	if (ret == ROLL) {
		roll_cup(my_cup);
		goto roll_again;
	} else if (ret == PRINT) {
		print_cup(my_cup);
		goto roll_again;
	} else if (ret == EXIT)
		exit(EXIT_SUCCESS);
	else {
		printf("Too much invalid input, exiting\n");
		exit(EXIT_FAILURE);
	}
}
