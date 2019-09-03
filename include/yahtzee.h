#ifndef __YAHTZEE_H__
#define __YAHTZEE_H__

struct die{
    int max_value;
    int current_value;
};

struct cup{
    struct die **dice;
    int dice_count;
};

struct cup *create_cup( int dice_count );
signed char fill_cup( struct cup *cup_to_fill, int max_value );
void roll_cup( struct cup *current_cup );
void print_cup( struct cup *current_cup );

#endif
