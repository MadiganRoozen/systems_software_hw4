#include <stdio.h>
#include "ast.h"
#include "bof.h"
#include "instruction.h"
#include "code.h"
#include "code_seq.h"

 void gen_code_initialize();

code_seq push_reg_on_stack(reg_num_type reg, offset_type offset, bool second, reg_num_type sp);

void gen_code_output_literals(BOFFILE bf);

int gen_code_output_seq_count(code_seq cs);

BOFHeader gen_code_program_header(code_seq main_cs);

void gen_code_output_seq(BOFFILE bf, code_seq cs);

void gen_code_output_program(BOFFILE bf, code_seq main_cs);

code_seq gen_code_expr_bin(binary_op_expr_t expr, reg_num_type reg);

code_seq gen_code_ident(ident_t ident, bool second, reg_num_type reg);

code_seq gen_code_expr(expr_t exp, bool second, reg_num_type reg);

code_seq gen_code_number( char* varName, number_t num, bool negate, bool second, reg_num_type sp);

code_seq gen_code_print_stmt(print_stmt_t s);

code_seq gen_code_if_ck_db(db_condition_t stmt, int thenSize);

code_seq gen_code_if_ck_rel(rel_op_condition_t stmt, int elseSize, int thenSize, bool norm);

code_seq gen_code_assign_stmt(assign_stmt_t stmt);

bool isNormalRev(condition_t c);

code_seq gen_code_if_stmt(if_stmt_t stmt);

code_seq gen_code_while_stmt(while_stmt_t stmt);

code_seq gen_code_call_stmt(call_stmt_t stmt);

code_seq gen_code_read_stmt(read_stmt_t stmt);

code_seq gen_code_block_stmt(block_stmt_t stmt);

code_seq gen_code_stmt(stmt_t *s);

code_seq gen_code_const(const_def_t* const_def);

code_seq gen_code_consts(const_decls_t const_decls);

code_seq gen_code_stmts(stmts_t* stmts);

void gen_code_program(BOFFILE bf, block_t b);

code_seq gen_code_var_decls(var_decls_t vars);

code_seq gen_code_idents(ident_list_t idents);