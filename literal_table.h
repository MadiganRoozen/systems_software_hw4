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
extern int literal_table_get_offset(const char *target, literal_value value);

//does the table contain the value
extern table_entry literal_table_contains(const char *target, literal_value value);

//add the value to the table
extern unsigned int literal_table_add(const char *val_string, literal_value value);
