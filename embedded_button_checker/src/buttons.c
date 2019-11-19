#include <stdlib.h>
#include <stdio.h>

#include "buttons.h"

#define DEBOUNCE_DELAY

typedef struct button {
	unsigned char identifier;
	unsigned char state : 1;
	unsigned int previous_press_time;

	int (*poll)(int, int);
	int port_arg;
	int pin_arg;

	struct button *next;
} button_t;

struct buttons {
	SemaphoreHandle_t lock;

	button_t head;
} bt = { 0 };

signed char checkButton(button_t *but)
{
	if ((xTaskGetTickCount() - but->previous_press_time) > DEBOUNCE_DELAY)
		if ((but->poll)(but->port_arg, but->pin_arg) != but->state) {
			if (but->state)
				return ((but->state = 0));
			else
				return ((but->state = 1));
		}
	return -1;
}

//TODO add/delete button
button_t *buttonFind(unsigned char identifier)
{
	return NULL;
}
void buttonAdd(int (*poll)(int, int), int port_arg, int pin_arg)
{
	return;
}

void buttonDelete(unsigned char indentifier)
{
	return;
}

//TODO poll buttons
unsigned char *buttonGetPresses(void)
{
	return NULL;
}

void buttonTask(void *parameters)
{
	while (1) {
	}
}

void buttonInit(void)
{
	bt.lock = xSemaphoreCreateMutex();

	if (pdPASS != xTaskCreate(buttonTask, "Button Task",
				  configMINIMAL_STACK_SIZE,
				  BUTTON_TASK_PRIORITY, NULL)) {
		fprintf(stderr, "Creating buttons task failed\n");
		exit(EXIT_FAILURE);
	}
}
