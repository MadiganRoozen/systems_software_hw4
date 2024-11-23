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
  int stack_bottom_address = data_start_addr + ret.data_start_address + ret.data_length + STACK_SPACE;
  ret.stack_bottom_addr = stack_bottom_address;
  
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
  int vars_len_in_bytes = (code_seq_size(main_seq) / 2) * BYTES_PER_WORD;

  main_seq code_seq_concat(code_utils_tear_down_program());
}
