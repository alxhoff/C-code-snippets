
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "yahtzee.h"

struct die{
    int max_value;
    int current_value;
};

struct cup *create_cup( int dice_count )
{
    time_t t;
    
    srand( (unsigned) time(&t)); //Cast time structure into random seed value
    
    //Allocates cup object
    struct cup *ret = malloc( sizeof(struct cup) );
    
    if( !ret ) //Check memory allocation didn't fail
        return NULL;

    ret->dice_count = dice_count;
    
    //Allocates list of empty pointers to die objects
    ret->dice = calloc( dice_count, sizeof(struct cup *) ); 

    printf("Cup created\n");
    
    return ret; //Return pointer to allocated cup object
}

signed char fill_cup( struct cup *cup_to_fill, int max_value )
{
    if (cup_to_fill)
    {
        //Loop for the number of dice that need to be in the cup
        for (int i = 0; i < cup_to_fill->dice_count; i++ )
        {
            //Create each die in the array
            cup_to_fill->dice[i] = malloc( sizeof(struct die) );

            if (!cup_to_fill->dice[i])
                return -1; //Negative value on error

            cup_to_fill->dice[i]->current_value = 0;
            cup_to_fill->dice[i]->max_value = max_value;

        }
        printf("Cup filled\n");
        return 0; //No errors
    } else
        return -1; 
}

void roll_die( struct die *cur_die )
{
    //Set current_value to random value between 1 and max_value
    cur_die->current_value = (rand() % cur_die->max_value) + 1;
}

void roll_cup( struct cup *current_cup )
{
    //Iterate through die and roll them
    for(int i = 0; i < current_cup->dice_count; i++)
        roll_die(current_cup->dice[i]);
}

void print_cup( struct cup *current_cup )
{
    for(int i = 0; i < current_cup->dice_count; i++)
        printf("Die %d has a value of %d\n", i, 
                current_cup->dice[i]->current_value);
}

