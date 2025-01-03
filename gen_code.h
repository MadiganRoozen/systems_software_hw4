#include <stdio.h>
#include "ast.h"
#include "bof.h"
#include "instruction.h"
#include "code.h"
#include "code_seq.h"

//these are both called in compiler_main.c
extern void gen_code_initialize();

extern void gen_code_program(BOFFILE bf, block_t prog);

extern code_seq gen_code_var_decls(var_decls_t vars);

extern code_seq gen_code_var_decl(var_decl_t var);