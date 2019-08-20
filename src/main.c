#include <time.h>
#include <stdlib.h>
#include <stdio.h>

struct die{
    int max_value;
    int current_value;
};

struct cup{
    struct die **dice;
    int dice_count;
};

struct cup *create_cup( int dice_count )
{
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

void prompt_int( char* message, int *value )
{
retry:
    printf("%s: ", message);
    scanf("%d", value);

    if(!*value)
    {
        printf("You cannot enter zero\n");
        goto retry;
    }
}

void clear_stdin()
{
    char input;
}

#define ROLL    1
#define PRINT   2
#define EXIT    3

unsigned char prompt_cont_print(void)
{
    char error_count = 0;
    char input;
retry:
    printf("Roll dice [r/R], print cup [p/P] or exit [e/E]:");
    input = getchar(); //Gets next char from stdin stream

    switch(input)
    {
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
        if(input != '\n')
            while ((input = getchar()) != '\n' && input != EOF)
                ;

        error_count++;
        printf("Invalid input\n");
        if(error_count < 3)
            goto retry;
        else
            return 0; //Zero on error
    }
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

int main(int argc, char* argv[])
{
    int max_value = 0, dice_count = 0;
    int ret;
    struct cup *my_cup = NULL;
    time_t t;

    prompt_int( "Please enter the max value of each die", &max_value );
    prompt_int( "Please enter the number of die in the cup", &dice_count );

    my_cup = create_cup( dice_count );

    ret = fill_cup( my_cup, max_value );
    if (ret) //Filling failed
        exit(EXIT_FAILURE);

    srand( (unsigned) time(&t)); //Cast time structure into random seed value

    getchar(); //Clear stdin
roll_again:
    ret = prompt_cont_print();
    if(ret == ROLL)
    {
        roll_cup(my_cup);
        goto roll_again;    
    }
    else if(ret == PRINT)
    {
        print_cup(my_cup);
        goto roll_again;
    }
    else if(ret == EXIT)
        exit(EXIT_SUCCESS);
    else
    {
        printf("Too much invalid input, exiting\n");
        exit(EXIT_FAILURE);
    }
}

