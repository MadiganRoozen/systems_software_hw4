#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "utilities.h"

//table entries
typedef struct table_entry{
    struct table_entry *next; //next entry in the table as this is a linked list
    const char *text; //text representation of the constant's value ("42" or "Hello")
    word_type value; //actual constant value
    int offset; //offset number
} table_entry;

//initialize the literal table
void literal_table_initialize();

//is the table empty
bool literal_table_empty();

//get the offset for the target value
int literal_table_get_offset(const char *target, word_type value);

//does the table contain the value
table_entry* literal_table_contains(const char *target, word_type value);

//add the value to the table
void literal_table_add(const char *val_string, word_type newValue);

//get the size of the literal table
unsigned int literal_table_size();

//start iteration
void literal_table_start_iteration();

//is there a next value in the table
bool literal_table_iteration_has_next();

//return the current value of the iteration node and move to the next one
word_type literal_table_iteration_next();

