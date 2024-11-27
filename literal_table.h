#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "utilities.h"

//initialize the literal table
extern void literal_table_initialize();

//is the table empty
extern bool literal_table_empty();

//get the offset for the target value
extern int literal_table_get_offset(const char *target, word_type value);

//does the table contain the value
extern bool literal_table_contains(const char *target, word_type value);

//add the value to the table
extern unsigned int literal_table_add(const char *val_string, word_type value);

//get the size of the literal table
extern unsigned int literal_table_size();

//start iteration
extern void literal_table_start_iteration();

//is there a next value in the table
extern bool literal_table_iteration_has_next();

//return the current value of the iteration node and move to the next one
extern word_type literal_table_iteration_next();
