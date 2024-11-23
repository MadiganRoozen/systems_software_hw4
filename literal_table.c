#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "utilities.h"
#include "literal_table.h"

//union is used so the literal table can contain int or string values easily
//there may be a better solution ot this is the machine_types file
typedef union literal_value {
    int int_val;             
    const char *str_val;     
} literal_value;

//table entries
typedef struct table_entry{
    struct table_entry *next; //next entry in the table as this is a linked list
    const char *text; //text representation of the constant's value ("42" or "Hello")
    literal_value value; //actual constant value
    int offset; //offset number
} table_entry;

static table_entry *first;
static table_entry *last;
int global_data_offset = 0;

// initialize the literal_table
void literal_table_initialize()
{
    first = NULL;
    last = NULL;
}

//if the first node is empty, the table is empty
bool literal_table_empty(){
    if(first == NULL) return true;
    else return false;
}

unsigned int literal_table_size(){
    return global_data_offset;
}

table_entry literal_table_contains(const char *target, literal_value value){
    if(literal_table_empty()) return NULL;

    table_entry cur = first;
    while(cur != NULL){
        //if the target text and the constant value match, return table entry
        if(strcmp(target, cur->text) == 0 && cur->value == value){
            return cur;
        }
        //move to the next entry
        cur = cur->next;
    }
    //if the entry was not found, return null
    printf("entry not in table")
    return NULL;
}

int literal_table_get_offset(const char *target, literal_value value){
    //if there is nothing in the table or the value isnt in the table, return -1
    if(literal_table_empty()) return -1;

    //try to find the entry, if you don't find it return -1
    table_entry found_entry = literal_table_contains(target, value)
    if(found_entry == NULL) return -1;

    //if found entry is not null, return the offset
    return found_entry->offset;

}

void literal_table_add(const char *val_string, literal_value newValue){
    table_entry *newEntry = malloc(sizeof(table_entry));
    newEntry->next = null;
    newEntry->text = val_string;
    newEntry->value = newValue;
    newEntry->offset = global_data_offset;
    if(literal_table_empty()) {
        first = newEntry;
        last = newEntry;
    } else {
        last->next = newEntry;
        last = newEntry;
    }
    global_data_offset++;
}

