#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bof.h"
#include "ast.h"
#include "utilities.h"
#include "symtab.h"
#include "gen_code.h"

extern void gen_code_initialize(){}

extern void gen_code_program(BOFFILE bf, block_t prog){}