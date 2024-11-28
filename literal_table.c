#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "utilities.h"
#include "literal_table.h"

static table_entry *first;
static table_entry *last;
int global_data_offset = 0;

static table_entry *iteration_next;

// initialize the literal_table
void literal_table_initialize()
{
    first = NULL;
    last = NULL;
    iteration_next = NULL;
}

//if the first node is empty, the table is empty
bool literal_table_empty(){
    if(first == NULL) return true;
    else return false;
}

unsigned int literal_table_size() {
    return global_data_offset;
}

table_entry* literal_table_contains(const char *target, word_type value){
    if(literal_table_empty()) 
        return NULL;

    table_entry* cur = first;
    while(cur != NULL){
        //if the target text and the constant value match, return table entry
        if(cur->value == value){
            return cur;
        }
        //move to the next entry
        cur = cur->next;
    }
    //if the entry was not found, return false
    return NULL;
}

int literal_table_get_offset(const char *target, word_type value){
    //if there is nothing in the table or the value isnt in the table, return -1
    if(literal_table_empty()) 
        return -1;

    //try to find the entry, if you don't find it return -1
    table_entry* found_entry = literal_table_contains(target, value);
    if (found_entry == NULL) 
        return -10;

    //if found entry is not null, return the offset
    return found_entry->offset;

}

void literal_table_add(const char *val_string, word_type newValue){
    table_entry *newEntry = malloc(sizeof(table_entry));
    newEntry->next = NULL;
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

void literal_table_start_iteration()
{
    iteration_next = first;
}

bool literal_table_iteration_has_next()
{
    if(iteration_next == NULL){
        return false;
    }/*
    if(iteration_next->next == NULL){
        return false;
    }*/
    return true;
}

word_type literal_table_iteration_next()
{
    //return 0 if the iterator is on a NULL node
    if(iteration_next==NULL) return 0;
    //returns the value from the current iteration node and moves to the next one
    word_type val = iteration_next->value;
    iteration_next = iteration_next->next;
    return val;
}

