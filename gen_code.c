 #include <limits.h>
#include <string.h>
#include "ast.h"
#include "code.h"
#include "id_use.h"
#include "literal_table.h"
#include "gen_code.h"
#include "utilities.h"
#include "regname.h"
#include "code_seq.h"
#include "code_utils.h"
#include "regname.h"

#define STACK_SPACE 4096

extern void gen_code_initialize(){
  literal_table_initialize();
}

//bf must be open for writing in binary, parser must have completed scope
//and type checking. Write all instructions in sequence to bf in order
static void gen_code_output_seq(BOFFILE bf, code_seq sequence){
  while(!code_seq_is_empty(sequence)){
    bin_instr_t inst = code_seq_first(sequence)->instr;
    instruction_write_bin_instr(bf, inst);
    sequence = code_seq_rest(sequence);
  }
}//end of gen_code_output_seq

static BOFHeader gen_code_program_header(code_seq main_sequence){
  BOFHeader ret;
  strncpy(ret.magic, "FBF", 4);//I copied this line straight from the float VM code, might not be right
  ret.text_start_address = 0;
  ret.text_length = code_seq_size(main_sequence) * BYTES_PER_WORD;
  int data_start_addr = MAX(ret.text_length, 1024) * BYTES_PER_WORD;
  ret.data_start_address = data_start_addr;
  ret.data_length = literal_table_size() * BYTES_PER_WORD;
  //int stack_bottom_address = data_start_addr + ret.data_start_address + ret.data_length + STACK_SPACE;
  ret.stack_bottom_addr = 32768 * BYTES_PER_WORD; //FP and SP start point
  
  return ret;
}//end of gen_code_program_header

static void gen_code_output_literals(BOFFILE bf){
  literal_table_start_iteration();
  while(literal_table_iteration_has_next()) {
    word_type lit = literal_table_iteration_next();
    bof_write_word(bf, lit);
  }
}//end of gen_code_output_literals

static void gen_code_output_program(BOFFILE bf, code_seq main_sequence){
  BOFHeader bfHeader = gen_code_program_header(main_sequence);
  bof_write_header(bf, bfHeader);
  gen_code_output_seq(bf, main_sequence);
  gen_code_output_literals(bf);
  bof_close(bf);
}//end of gen_code_output_program

void gen_code_program(BOFFILE bf, block_t prog){
  code_seq main_seq = code_utils_set_up_program();
  //place local variables on the runtime stack
  main_seq = gen_code_var_decls(prog.var_decls); 
  //generate code for the block
  code_seq_concat(&main_seq, gen_code_stmt(prog.stmts)); //for some reason gen_code_stmt is returning an integer
  code_seq_concat(&main_seq, code_utils_tear_down_program());
  gen_code_output_program(bf, main_seq);
}//end of gen_code_program

code_seq gen_code_var_decls(var_decls_t vars){
	code_seq ret = code_seq_empty();
	var_decl_t *var_inst = vars.var_decls;
	while(var_inst != NULL) { //changed this form var to var_inst bc it was giving an error -madigan 11/25
   		code_seq_concat(&ret, gen_code_var_decl(*var_inst));//not sure why these are in reverse order //changed it-madigan 11/25
   		var_inst = var_inst->next;
		//adjust FP down
		code_seq_add_to_end(&ret, code_sri(SP, 1));
		code_seq_add_to_end(&ret, code_cpr(3, FP));
	}
 return ret;
}//end of gen_code_var_decls

code_seq gen_code_var_decl(var_decl_t var) {
  	return gen_code_idents(var.ident_list);
}//end of gen_code_var_decl

//figure this out later
code_seq gen_code_idents(ident_list_t idents) {
	ident_t* idptr = idents.start;
	id_use* id = idptr->idu;
 	int levout = id->levelsOutward;//someone please double check my pointer shenanigans -caitlin
	int offset = id->attrs->offset_count;
	code_seq ret = code_utils_compute_fp(3, levout);
	ret = code_seq_add_to_end(&ret, code_cpw(SP, 0, 3, offset);
	return ret;
}//end of gen_code_idents

code_seq gen_code_stmt(stmt_t stmt) {
   switch(stmt.stmt_kind){
      case assign_stmt:
         return gen_code_assign_stmt(stmt.data.assign_stmt);
         break;
      case call_stmt:
         return gen_code_call_stmt(stmt.data.call_stmt);
         break;
      case if_stmt:
         return gen_code_if_stmt(stmt.data.if_stmt);
         break;
      case while_stmt:
         return gen_code_while_stmt(stmt.data.while_stmt);
         break;
    case read_stmt:
         return gen_code_read_stmt(stmt.data.read_stmt);
         break;
    case print_stmt:
         return gen_code_print_stmt(stmt.data.print_stmt);
         break;
    case block_stmt:
         return gen_code_block_stmt(stmt.data.block_stmt);
         break;
    default:
        bail_with_error("Call to gen_code_stmt with an AST that is not a statement!");
        break;
   }
 return code_seq_empty();
}//end of gen_code_stmt

code_seq gen_code_assign_stmt(assign_stmt_t stmt){
   code_seq ret;
   ret = gen_code_expr(*(stmt.expr));
   assert(stmt.idu != NULL);
   assert(id_use_get_attrs(stmt.idu) != NULL);
   id_kind kind = id_use_get_attrs(stmt.idu)->kind;
   code_seq_concat(&ret, /*SOMETHING GOES HERE*/);
 
   return ret;
}//end of gen_code_assign_stmt

code_seq gen_code_call_stmt(call_stmt_t stmt){
   
}//end of gen_code_call_stmt

code_seq gen_code_condition(condition_t con)
{
  switch (con.cond_kind) {
  case ck_db:
	  return; //return something idk what
	  break;
  case ck_rel:
	  return;  //return something idk what
  default:
	  break;
  }
  return code_seq_empty();
}

// Generate code for the if-statment given by stmt
code_seq gen_code_if_stmt(if_stmt_t stmt)
{
    // put truth value of stmt.expr in $v0
    code_seq ret = gen_code_condition(stmt.condition);
    //somehow load the condition into the stack at v0, this is just taken from the professors website
    code_seq_concat(&ret, code_pop_stack_into_reg(V0, bool_te));
    code_seq body = gen_code_stmt(*stmt.then_stmts);
    int body_len = code_seq_size(body);
    // skip over body if $v0 contains false
    code_seq_add_to_end(&ret,code_beq(V0, 0, body_len));
    code_seq_concat(&ret, body);
    return ret;
}//end of gen_code_if_stmt

code_seq gen_code_while_stmt(while_stmt_t stmt){
 
}//end of gen_code_while_stmt

code_seq gen_code_read_stmt(read_stmt_t stmt){
 
}//end of gen_code_read_stmt

code_seq gen_code_print_stmt(print_stmt_t stmt){
 
}//end of gen_code_print_stmt

code_seq gen_code_block_stmt(block_stmt_t stmt){
 
}//end of gen_code_block_stmt

code_seq gen_code_expr(expr_t expr){
   switch(
}//end of gen_code_expr

code_seq gen_code_binary_op_expr(binary_op_expr_t exp){
 
}//end of gen_code_binary_op_expr
