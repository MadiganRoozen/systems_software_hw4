#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bof.h"
#include "ast.h"
#include "utilities.h"
#include "symtab.h"

//these are both called in compiler_main.c
extern void gen_code_initialize();

extern void gen_code_program(BOFFILE bf, block_t prog);