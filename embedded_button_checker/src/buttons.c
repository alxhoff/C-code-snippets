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
	unsigned int button_count;
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

button_t *buttonFind(unsigned char identifier)
{
	button_t *iterator;

	xSemaphoreTake(bt.lock, portMAX_DELAY);

	for (iterator = &bt.head;
	     iterator->next && (iterator->next->identifier != identifier);
	     iterator = iterator->next)
		;

	xSemaphoreGive(bt.lock);

	return iterator->next;
}
void buttonAdd(char identifier, int (*poll)(int, int), int port_arg,
	       int pin_arg)
{
	button_t *iterator;

	if (bt.lock)
		xSemaphoreTake(bt.lock, portMAX_DELAY);

	for (iterator = &bt.head; iterator->next; iterator = iterator->next)
		;

	iterator->next = calloc(1, sizeof(button_t));

	if (!iterator->next) {
		xSemaphoreGive(bt.lock);
		fprintf(stderr, "Allocating button failed\n");
		exit(EXIT_FAILURE);
	}

	iterator->poll = poll;
	iterator->port_arg = port_arg;
	iterator->pin_arg = pin_arg;
	iterator->identifier = identifier;
	bt.button_count++;
	if (bt.lock) // In case called before task has been started and semaphore init'd
		xSemaphoreGive(bt.lock);
}

void buttonDelete(unsigned char identifier)
{
	button_t *iterator, *delete;

	if (bt.lock)
		xSemaphoreTake(bt.lock, portMAX_DELAY);

	for (iterator = &bt.head;
	     iterator->next && (iterator->next->identifier != identifier);
	     iterator = iterator->next)
		;

	if (iterator->next)
		if (iterator->next->identifier == identifier) {
			delete = iterator->next;
			if (!delete->next)
				iterator->next = NULL;
			else
				iterator->next = delete->next;
			free(delete);
			bt.button_count--;
		}

	if (bt.lock)
		xSemaphoreGive(bt.lock);
}

//TODO poll buttons
char *buttonGetPresses(void)
{
	char *ret;
	button_t *iterator;

	xSemaphoreTake(bt.lock, portMAX_DELAY);

	ret = calloc(bt.button_count, sizeof(char));
	if (!ret) {
		fprintf(stderr,
			"Could not allocate return buffer for poll buttons\n");
		exit(EXIT_FAILURE);
	}

	for (iterator = &bt.head; iterator->next; iterator = iterator->next)
		strncat(ret,
			checkButton(iterator->next) ? &iterator->identifier :
						      "",
			1);

	xSemaphoreGive(bt.lock);
	return ret;
}

void buttonTask(void *parameters)
{
	buttonInit();

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
