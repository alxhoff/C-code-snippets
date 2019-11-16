#include <stdio.h>
#include <stdlib.h>

typedef struct LIST_ITEM {
	int ID;
	struct LIST_ITEM *next;
} list_item_t;

list_item_t head = { { 0 } };

void deleteItem(int ID)
{
	list_item_t *iterator;
	list_item_t *xDelete;
	for (iterator = &head;
	     (iterator->next != NULL) && (iterator->next->ID != ID);
	     iterator = iterator->next)
		;
	//Should now have either last entry in list or entry before item to delete
	if (iterator->next) {
		if (iterator->next->ID == ID) {
			xDelete = iterator->next;
			//If entry to delete is the last entry in the list
			if (!iterator->next->next)
				iterator->next = NULL;
			//Current entry must point to entry after item to delete
			else
				iterator->next = xDelete->next;
			free(xDelete);
		}
	}
}

void addItem(int ID)
{
	list_item_t *iterator;
	for (iterator = &head; iterator->next != NULL;
	     iterator = iterator->next)
		;
	//Iterator now points to last entry in list
	iterator->next = (list_item_t *)malloc(sizeof(list_item_t));
	iterator->next->ID = ID;
}

int main(int argc, char *argv[])
{
	addItem(1);
	addItem(2);
	deleteItem(1);

	printf("First item's ID: %d", head.next->ID);

	return 0;
}
