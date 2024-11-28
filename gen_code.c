#include "gen_code.h"
#include "utilities.h"
#include "code.h"
#include "code_seq.h"
#include "code_utils.h"
#include "literal_table.h"
#include "spl.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include "regname.h"
#include <string.h>

//Definitions
#define STACK_SPACE 4096

//Initializer Function For Literal Table
void gen_code_initialize() {
    literal_table_initialize();
//    literal_table_test();
}

//
code_seq push_reg_on_stack(reg_num_type reg, offset_type offset, bool second, reg_num_type sp) {
    return code_seq_singleton(code_cpw(sp, (second)?1 : 0, reg, offset));
}

//Final Output Function - Places Literal Table Into Data Section of BOF
void gen_code_output_literals(BOFFILE bf) {

    //Starting Table Iteration
    literal_table_start_iteration();

    //While Loop - For All Literals In Table, Write Word w to bf
    while (literal_table_iteration_has_next()) {
        word_type w = literal_table_iteration_next();
        bof_write_word(bf, w);
    }
}

//Utility Function - Counts Size of Code Sequence and Returns int res (size) (Probably Not Needed) ------------------------------------
int gen_code_output_seq_count(code_seq cs) {

    //Initialize
    int res = 0;

    //While Code Sequence cs is Not Empty, Increase res and Go To Next Code in cs
    while (!code_seq_is_empty(cs)) {
//        bin_instr_t inst = code_seq_first(cs)->instr;
        res++;
        cs = code_seq_rest(cs);
    }

    return res;
}

//Final Output Function - Creates BOF Header From main_cs Code Sequence
BOFHeader gen_code_program_header(code_seq main_cs) {

    //Initializations
    BOFHeader ret;
    bof_write_magic_to_header(&ret);

    //Setting BOF Header Info
    ret.text_start_address = 0;
    ret.text_length = code_seq_size(main_cs);

    int dsa = MAX(ret.text_length, 1024) + BYTES_PER_WORD;
    ret.data_start_address = dsa;

    ret.data_length = literal_table_size();

    int sba = dsa + ret.data_start_address + ret.data_length + STACK_SPACE;
    ret.stack_bottom_addr = sba;

    return ret;
}

//Final Output Function - Writes Instructions Present in Code Sequence cs to BOF bf
void gen_code_output_seq(BOFFILE bf, code_seq cs) {

    //While Code Sequence is Not Empty
    while (!code_seq_is_empty(cs)) {

        //Get Instruction & Write Instruction
        bin_instr_t inst = code_seq_first(cs)->instr;
        instruction_write_bin_instr(bf, inst);
        //printf(" %s\n", instruction_assembly_form(0, inst) );

        //Move to Next Code in Sequence
        cs = code_seq_rest(cs);
    }
}

//Final Output Function - Handles Output to BOF Processes and Calls 
void gen_code_output_program(BOFFILE bf, code_seq main_cs) {

    //BOF Header
    BOFHeader bf_header = gen_code_program_header(main_cs);
    bof_write_header(bf, bf_header);

    //BOF Code Information
    gen_code_output_seq(bf, main_cs);
    gen_code_output_literals(bf);

    //BOF Written
    bof_close(bf);
}

//Unused
// code_seq gen_code_arith_op(token_t rel_op) {
//     code_seq base = code_seq_empty();

//     return base;
// }

//Expression Handling Function - Recursively Handles Evaluation of Binary Op Expressions & Returns code_seq of Expression expr
code_seq gen_code_expr_bin(binary_op_expr_t expr, reg_num_type reg) {

    //New code_seq For expr
    code_seq bin_cs = code_seq_empty();

    //Switch On Arithmetic Opcode of Binary Op
    switch (expr.arith_op.code) {

        //Case of Addition
        case plussym:

            //Placing expr1 & expr2 Onto Binary Op Code Sequence
            code_seq_concat(&bin_cs, gen_code_expr(*expr.expr1,false,SP));
            code_seq_concat(&bin_cs, gen_code_expr(*expr.expr2,true,SP));

            //Placing ADD ASM Code Onto Sequence
            code_seq_add_to_end(&bin_cs, code_add(reg, 0,SP, 1));
            break;

        //Case of Subtraction (Difference)
        case minussym:

            //Placing expr1 & expr2 Onto Binary Op Code Sequence
            code_seq_concat(&bin_cs, gen_code_expr(*expr.expr1,false,SP));
            code_seq_concat(&bin_cs, gen_code_expr(*expr.expr2,true,SP));

            //Placing SUB ASM Code Onto Sequence
            code_seq_add_to_end(&bin_cs, code_sub( reg, 0,SP, 1));
            break;

        //Case of Mulitplication
        case multsym:

            //Placing expr1 & expr2 Onto Binary Op Code Sequence
            code_seq_concat(&bin_cs, gen_code_expr(*expr.expr1,false,SP));
            code_seq_concat(&bin_cs, gen_code_expr(*expr.expr2,true,SP));

            //Placing SUB ASM Code Onto Sequence
            code_seq_add_to_end(&bin_cs, code_mul( SP, 1));
            code_seq_add_to_end(&bin_cs, code_cflo( SP, 0));
            break;

        //Case of Division
        case divsym:

            //Placing expr1 & expr2 Onto Binary Op Code Sequence
            code_seq_concat(&bin_cs, gen_code_expr(*expr.expr1,false,SP));
            code_seq_concat(&bin_cs, gen_code_expr(*expr.expr2,true,SP));

            //Placing SUB ASM Code Onto Sequence
            code_seq_add_to_end(&bin_cs, code_div( SP, 1));
            code_seq_add_to_end(&bin_cs, code_cflo( SP, 0));
            break;

        //Default - Invalid Arithmetic Opcode - Returning Empty Sequence (Probably Change to Error) ----------------------------
        default:
            bail_with_error("Unexpected arith_op.code (%s) in gen_code_expr_bin", expr.arith_op.code);
            break;
    }

    //Final Return of Binary Op Code Sequence
    return bin_cs;
}

//Expression Handling Function - Takes a Given Ident & Returns a Code Sequence of It as a Code ---------------------------------------
code_seq gen_code_ident(ident_t ident, bool second, reg_num_type reg) {
//    printf("IDENT %s: %d\n", ident.name, ident.idu->attrs->offset_count);
    int offset = ident.idu->attrs->offset_count; //------------------------------------------- check this
    code_seq seq = push_reg_on_stack(GP, offset, second, reg);

    return seq;
}

//Expression Handling Function - Takes a Given Expression exp To Be Evaluated & Returns a Code Sequence
code_seq gen_code_expr(expr_t exp, bool second, reg_num_type reg) {
    switch (exp.expr_kind) {
        case expr_ident:
            return gen_code_ident(exp.data.ident, second, reg);
        case expr_bin:
            return gen_code_expr_bin(exp.data.binary, reg);
        case expr_negated:
            return gen_code_number(NULL, exp.data.negated.expr->data.number,true, second, reg);
        case expr_number:
            return gen_code_number(NULL, exp.data.number,false, second, reg);
        default:
            bail_with_error("Unexpected expr_kind_e (%d) in gen_code_expr", exp.expr_kind);
            break;
    }
    return code_seq_empty();
}

//Utility Function - Creates a Code Sequence Of a Single Number Value
code_seq gen_code_number( char* varName, number_t num, bool negate, bool second, reg_num_type sp) {
    
    //Initialize
    word_type val = num.value;

    //If Negated, Negate the Number
    if (negate) {
        val = -(num.value);
    } else {
        val = num.value;
    }

    //If No Name, Return LIT ASM Code
    if (varName==NULL){
        return code_seq_singleton(code_lit(SP, (second?1:0), val));
    }

    //If Name, Return CPW ASM Code With Offset
    unsigned int global_offset = literal_table_get_offset(varName, val);
    return push_reg_on_stack(GP, global_offset, second, sp);
}

//Statements Function - Handles Creation of Code Sequence for Print Statement
code_seq gen_code_print_stmt(print_stmt_t s) {

    //Create Empty Code Sequence & Expression Code Sequence
    code_seq print_cs = code_seq_empty();
    code_seq expr_code = gen_code_expr(s.expr, false, SP);

    //Place Expression Code Infront of Print Code
    code_seq_concat(&print_cs, expr_code);

    //Adds Print Integer ASM Code to Sequence
    code_seq_add_to_end(&print_cs, code_pint(SP,0 ));

    return print_cs;
}

//Conditional Function - Handles Code Sequence Creation for db_condition stmt for If Statement
code_seq gen_code_if_ck_db(db_condition_t stmt, int thenSize) {

    //Create New Empty Code Sequence
    code_seq if_cs = code_seq_empty();


    code_seq_concat(&if_cs, gen_code_expr(stmt.dividend, false, SP));

    //Add LIT, CFHI, & BEQ ASM Codes to Sequence for If Statement
    code_seq_add_to_end(&if_cs, code_lit(SP, 1, stmt.divisor.data.number.value));
    code_seq_add_to_end(&if_cs, code_cfhi(SP, 1));
    code_seq_add_to_end(&if_cs, code_beq(SP, 0, thenSize + 2));

    return if_cs;
}

//Conditional Function - Handles Code Sequence Creation for rel_op_condition stmt for If Statement
code_seq gen_code_if_ck_rel(rel_op_condition_t stmt, int elseSize, int thenSize, bool norm) {

    //Create New Empty Code Sequence
    code_seq if_cs = code_seq_empty();

    //Adding Expressions to if_cs Code Sequence
    code_seq_concat(&if_cs, gen_code_expr(stmt.expr1,false, SP));
    code_seq_concat(&if_cs, gen_code_expr(stmt.expr2,true, SP));

    //Determining If Statement Condition Branch Needed
    if (norm){
        
        //Handling == Condition
        if (strcmp(stmt.rel_op.text, "==") == 0) {
             code_seq_add_to_end(&if_cs, code_bne(SP,1,thenSize+2));
        } 

        //Handling != Condition
        else if (strcmp(stmt.rel_op.text, "!=") == 0) {
             code_seq_add_to_end(&if_cs, code_beq(SP,1,thenSize+2));
        }
    } 

    //Expression Requires Conditonal Evaluation
    else {

        //Handing <= Condition
        if (strcmp(stmt.rel_op.text, "<=") == 0) {
            code_seq_add_to_end(&if_cs, code_sub( SP, 0,SP, 1));
            code_seq_add_to_end(&if_cs, code_blez(SP, 0, elseSize + 2));
        }

        //Handing < Condition
        else if (strcmp(stmt.rel_op.text, "<") == 0) {
            code_seq_add_to_end(&if_cs, code_sub( SP, 0,SP, 1));
            code_seq_add_to_end(&if_cs, code_bltz(SP, 0, elseSize + 2));
        }

        //Handing >= Condition
        else if (strcmp(stmt.rel_op.text, ">=") == 0) {
            code_seq_add_to_end(&if_cs, code_sub( SP, 0,SP, 1));
            code_seq_add_to_end(&if_cs, code_bgez(SP, 0, elseSize + 2));
        }

        //Handing > Condition
        else if(strcmp(stmt.rel_op.text, ">") == 0){
            code_seq_add_to_end(&if_cs, code_sub( SP, 0,SP, 1));
            code_seq_add_to_end(&if_cs, code_bgtz(SP, 0, elseSize + 2));
        }
    }

    return if_cs;
}

//Statements Function - Handles Code Generation of Assign Statements
code_seq gen_code_assign_stmt(assign_stmt_t stmt) {

    //Create New Empty Code Sequence
    code_seq assign_cs = code_seq_empty();

    //Creating Expression Code
    int offset = stmt.idu->attrs->offset_count;
    code_seq_concat(&assign_cs, gen_code_expr(*stmt.expr, false, SP));

    //Adding CPW ASM Code to assign_cs
    code_seq_add_to_end(&assign_cs, code_cpw(GP, offset, SP,0));

    return assign_cs;
}

//Utility Function - Returns a Bool If Condition c Rel_Op.text is an == or != Condition
bool isNormalRev(condition_t c) {

    // == Condition
    if (strcmp(c.data.rel_op_cond.rel_op.text, "==") == 0)
        return true;

    // != Condition
    if (strcmp(c.data.rel_op_cond.rel_op.text, "!=") == 0)
        return true;

    //Neither of These Conditions
    return false;
}

//Statements Function - Handles Code Generation of If Statements
code_seq gen_code_if_stmt(if_stmt_t stmt) {

    //Create New Empty Code Sequence
    code_seq if_cs = code_seq_empty();

    condition_t c = stmt.condition;

    //Rel False By Default, if rel_op condition, set True
    bool rel = false;
    if (c.cond_kind == ck_rel) {
        rel = true;
    }

    //Creating Code Sequences For Then / Else Bodies
    code_seq thenSeq = gen_code_stmts(stmt.then_stmts);
    code_seq elseSeq = gen_code_stmts(stmt.else_stmts);

    //Storing Size of Then / Else Code Sequence Bodies
    int thenSeqLength = code_seq_size(thenSeq);
    int elseSeqLength = code_seq_size(elseSeq);

    //Norm is False by Default, when rel == true and c is a rel_op condition, norm == true
    bool norm = false;
    if (rel && isNormalRev(c)) {
        norm = true;
    }

    //Determining If Statement Condition Handling
        //Db Condition
        if (c.cond_kind == ck_db) {
            code_seq_concat(&if_cs, gen_code_if_ck_db(c.data.db_cond,elseSeqLength));
        }

        //Rel Condition
        if (rel) {
            code_seq_concat(&if_cs, gen_code_if_ck_rel(c.data.rel_op_cond,elseSeqLength, thenSeqLength,  norm));
        }

        //Norm If Statement
        if (rel && isNormalRev(c)) {

            //Handling If and Else Statements
            code_seq_concat(&if_cs, thenSeq);
            code_seq_add_to_end(&if_cs, code_jrel(elseSeqLength + 1));
            code_seq_concat(&if_cs, elseSeq);
        } 

        //Other If Statement
        else {
            code_seq_concat(&if_cs, elseSeq);
            code_seq_add_to_end(&if_cs, code_jrel(thenSeqLength + 1));
            code_seq_concat(&if_cs, thenSeq);
        }

    return if_cs;
}

//Statements Function - Handles Code Generation For While Loop Statement 
code_seq gen_code_while_stmt(while_stmt_t stmt) {

    //Create New Empty Code Sequence
    code_seq while_cs = code_seq_empty();

    //While Body Sequence
    code_seq while_body = gen_code_stmts(stmt.body);
    int while_body_size = code_seq_size(while_body);

    //While Condition Sequence
    code_seq while_cond = code_seq_empty();
    int condition_size = 0;

    //Rel While Condition 
    if (stmt.condition.cond_kind == ck_rel) {
        rel_op_condition_t rel = stmt.condition.data.rel_op_cond;

        //Code Generation For Expr1 & Expr2
        code_seq_concat(&while_cond, gen_code_expr(rel.expr1, false, SP));
        code_seq_concat(&while_cond, gen_code_expr(rel.expr2, true, SP));

        //Condition Size Calculation
        condition_size = code_seq_size(while_cond);

        //Create Branch Code From Rel Expressions
        if (rel.expr1.expr_kind == expr_number &&   rel.expr2.expr_kind == expr_number) {

            //Stroing Number Values of Expressions
            int num1 = rel.expr1.data.number.value;
            int num2 = rel.expr2.data.number.value;

            //If Condition Are Equal
            if (num1 == num2) {

                //Rel op is <
                if (strcmp(rel.rel_op.text, "<") == 0) {
                    code_seq_add_to_end(&while_cond, code_jrel(while_body_size + 2));
                } 

                //Error From Invalid Rel Op
                else {
                    bail_with_error("Invalid Rel Op Found in gen_code_while_stmt if\n");
                }
            }
        }

        else {

            //Less Than Condition Rel Op
            if (strcmp(rel.rel_op.text, "<") == 0) {
                code_seq_add_to_end(&while_cond, code_sub(SP, 0, SP, 1));
                code_seq_add_to_end(&while_cond, code_bgtz(SP, 0, while_body_size + 2)); //Jump if false

            } 

            //Less Than or Equal Condition Rel Op
            else if (strcmp(rel.rel_op.text, "<=") == 0) {
                code_seq_add_to_end(&while_cond, code_sub(SP, 0, SP, 1));
                code_seq_add_to_end(&while_cond, code_bgez(SP, 0, while_body_size + 2)); //Jump if false
            } 

            //Greater Than Condition Rel Op
            else if (strcmp(rel.rel_op.text, ">") == 0) {
                code_seq_add_to_end(&while_cond, code_sub(SP, 0, SP, 1));
                code_seq_add_to_end(&while_cond, code_bltz(SP, 0, while_body_size + 2)); //Jump if false
            } 

            //Greater Than or Equal Rel Op
            else if (strcmp(rel.rel_op.text, ">=") == 0) {
                code_seq_add_to_end(&while_cond, code_sub(SP, 0, SP, 1));
                code_seq_add_to_end(&while_cond, code_blez(SP, 0, while_body_size + 2)); //Jump if false
            } 

            //Equal To Condition Rel Op
            else if (strcmp(rel.rel_op.text, "==") == 0) {
                code_seq_add_to_end(&while_cond, code_bne(SP, 1, while_body_size + 2)); //Jump if false
               // code_seq_add_to_end(&base, code_jrel(bodySeqSize));
            }   

            //Not Equal To Condition Rel Op
            else if (strcmp(rel.rel_op.text, "!=") == 0) {
                code_seq_add_to_end(&while_cond, code_beq(SP, 1, while_body_size + 2)); //Jump if false
            } 

            //Invalid Rel Op Condition Found
            else {
                bail_with_error("Invalid Rel Op Found in gen_code_while_stmt else\n");
            }
        }
    }
    
    //Invalid Rel Op
    else {
        bail_with_error("Invalid Rel Op Found in gen_code_while_stmt\n");
    }

    //Concat Conditional Code After Main While Sequence
    code_seq_concat(&while_cs, while_cond);

    //Concat Body Code After Main While Sequence
    code_seq_concat(&while_cs, while_body);

    //Add Jump Code to the End of While Sequence
    code_seq_add_to_end(&while_cs, code_jrel(-(condition_size + while_body_size + 2)));

    return while_cs;
}

//Statements Function - Handles Call Statement Code Generation, Returns Empty Code Sequence Until We Implement
code_seq gen_code_call_stmt(call_stmt_t stmt) {
    code_seq call_cs = code_seq_empty();
    return call_cs;
}

//Statements Function - Handles Code Generation for Read Statements
code_seq gen_code_read_stmt(read_stmt_t stmt) {

    //Create New Empty Code Sequence
    code_seq read_cs = code_seq_empty();

    //Lookup Statement Name For Offset
    int offset = stmt.idu->attrs->offset_count;
    code_seq_add_to_end(&read_cs, code_rch(GP, offset));

    return read_cs;
}

//Statements Function - Handles Block Statement Code Generation (Recursive for Inner Blocks)
code_seq gen_code_block_stmt(block_stmt_t stmt) {

    //Create New Empty Code Sequence
    code_seq block_cs = code_seq_empty();

    //Create Block Pointer
    struct block_s* block_stmt = stmt.block;

    //Create & Concat Code Sequences for Constants 
    code_seq_concat(&block_cs, gen_code_consts(block_stmt->const_decls));

    // code_seq_concat(&block_cs, gen_code_var_decls(block_stmt->var_decls)); 

    block_cs = code_utils_save_registers_for_AR();

    //Create & Concat Code Sequences for Body Statements 
    code_seq body_cs = gen_code_stmts(&block_stmt->stmts);
    code_seq_concat(&block_cs, body_cs);

    return block_cs;
}

//Statements Function - Handles Process Execution Determined By Statement 
code_seq gen_code_stmt(stmt_t *s) {

    //Create New Empty Code Sequence
    code_seq stmt_code = code_seq_empty();

    //Switch on Kind of Statement
    switch (s->stmt_kind) {

        //Assignment Statement
        case assign_stmt:
            stmt_code = gen_code_assign_stmt(s->data.assign_stmt);
            break;

        //Call Statement
        case call_stmt:
            stmt_code = gen_code_call_stmt(s->data.call_stmt);
            break;

        //If Statement
        case if_stmt:
            stmt_code = gen_code_if_stmt(s->data.if_stmt);
            break;

        //While Statement
        case while_stmt:
            stmt_code = gen_code_while_stmt(s->data.while_stmt);
            break;

        //Read Statement
        case read_stmt:
            stmt_code = gen_code_read_stmt(s->data.read_stmt);
            break;

        //Print Statement
        case print_stmt:
            stmt_code = gen_code_print_stmt(s->data.print_stmt);
            break;

        //Block Statement
        case block_stmt:
            stmt_code = gen_code_block_stmt(s->data.block_stmt);
            break;

        //Invalid Statement Kind Found
        default:
            bail_with_error("Invalid Statement Kind Found in gen_code_stmt\n");

    }

    return stmt_code;
}

//Code Sequence Function - Creates & Returns Code Sequence For Constant Definition List
code_seq gen_code_const(const_def_t* const_def) {

    //Create New Empty Code Sequence
    code_seq const_cs = code_seq_empty();

    //For Each Constant Definition
    while (const_def!=NULL) {
        bool negate = false;

        //Concat Code To Sequence
        literal_table_add(const_def->number.text, const_def->number.value);
        char* ident_name = malloc(sizeof(const_def->ident.name));
        strcpy(ident_name, const_def->ident.name);
        code_seq_concat(&const_cs, gen_code_number(ident_name, const_def->number, negate, false, SP));
        const_def = const_def->next;
    }

    return const_cs;
}

//Code Sequence Function - Handles Code Sequence Generation for All Constants in decls
code_seq gen_code_consts(const_decls_t const_decls) {

    //Create New Empty Code Sequence
    code_seq consts_cs = code_seq_empty();

    //For Each Constant Declaration List
    const_decl_t *start = const_decls.start;
    while (start != NULL) {
        const_def_t* def = start->const_def_list.start;
        while (def != NULL) {
            code_seq_concat(&consts_cs, gen_code_const(def));
            def = def->next;
        }

        start = start->next;
    }

    return consts_cs;
}

//Statements Function - Handles Code Sequence Generation For All Statements
code_seq gen_code_stmts(stmts_t* stmts) {

    //Create New Empty Code Sequence
    code_seq stmts_cs = code_seq_empty();

    //If No Statements, Return Empty Sequence
    if (stmts == NULL) {
        return stmts_cs;
    }

    stmts_t stmts_d = *stmts;

    //If Statements Are Empty, Return Empty Sequence
    if (stmts_d.stmts_kind == empty_stmts_e) {
        return stmts_cs;
    }

    //Statements Are Not Empty
    stmt_t* stmt = stmts_d.stmt_list.start;

    //For Each Statement, Create Code and Concat
    while (stmt != NULL) {
        code_seq stmt_code = gen_code_stmt(stmt);

        //Concat Code Sequences
        code_seq_concat(&stmts_cs, stmt_code);
        stmt = stmt->next;
    }

    return stmts_cs;
}

// Function to handle variable declarations for a single var-decl
code_seq gen_code_var_decl(var_decl_t *var_decl) {
    code_seq var_decl_cs = code_seq_empty();

    // Iterate through the identifiers in the declaration
    ident_t *ident = var_decl->ident_list.start;
    while (ident != NULL) {
        id_use *id = ident->idu; // Access identifier use information
        int lev_out = id->levelsOutward; // Levels outward from current frame
        int offset = id->attrs->offset_count; // Offset for the variable

        // Compute the frame pointer position
        code_seq fp_cs = code_utils_compute_fp(3, lev_out);
        code_seq_concat(&var_decl_cs, fp_cs);

        // Allocate space for the variable by decrementing SP and updating FP
        code_seq_add_to_end(&var_decl_cs, code_sri(SP, 1)); // Adjust SP down
        code_seq_add_to_end(&var_decl_cs, code_cpw(SP, 0, 3, offset)); // Assign offset
        code_seq_add_to_end(&var_decl_cs, code_cpr(FP, SP)); // Update FP
        
        ident = ident->next; // Move to next identifier
    }

    return var_decl_cs;
}

code_seq gen_code_var_decls(var_decls_t vars) {
    code_seq ret = code_seq_empty();

    var_decl_t *var_inst = vars.var_decls;
    while (var_inst != NULL) {
        ident_list_t idents = var_inst->ident_list;
        if (idents.start != NULL) {
            code_seq idents_cs = gen_code_idents(idents);
            code_seq_concat(&ret, idents_cs);
        }
        var_inst = var_inst->next;

        // Adjust SP for each variable and update FP
        code_seq_add_to_end(&ret, code_sri(SP, 1));
        code_seq_add_to_end(&ret, code_cpr(FP, SP));
    }
    return ret;
}

code_seq gen_code_idents(ident_list_t idents) {
    ident_t *idptr = idents.start;
    code_seq ret = code_seq_empty();

    while (idptr != NULL) {
        if (idptr->idu != NULL) {
            id_use *id = idptr->idu;
            if(id == NULL){
                printf("idu was null")
                assert(id != NULL);
            }
            int levout = id->levelsOutward;
            if(id->attrs == NULL){
                printf("id_attrs was null")
                assert(id->attrs != NULL);
            }
            int offset = id->attrs->offset_count;
            code_seq fp_cs = code_utils_compute_fp(3, levout);
            code_seq_concat(&ret, fp_cs);
            code_seq_add_to_end(&ret, code_cpw(SP, 0, 3, offset));
        }
        idptr = idptr->next;
    }
    return ret;
}


//Main Function - Handles General Overview of gen_code Startup, Process, and Output
void gen_code_program(BOFFILE bf, block_t b) {

    //Set Up Main Code Sequence
    code_seq main_cs = code_utils_set_up_program();

    

    //Concat Consts Code Sequences
    code_seq_concat(&main_cs, gen_code_consts(b.const_decls));

    // code_seq_concat(&main_cs, gen_code_var_decls(b.var_decls)); 

    main_cs = code_utils_save_registers_for_AR();

    //Concat Body Statement Code Sequences
    code_seq body_cs = gen_code_stmts(&b.stmts);

    //Concat To Main
    code_seq_concat(&main_cs, body_cs);

    //End / Teardown Program
    code_seq tear_down_cs = code_utils_tear_down_program(); //BROKEN
    code_seq_concat(&main_cs, tear_down_cs);

    //Output to BOF File
    gen_code_output_program(bf, main_cs);
//    literal_table_debug_print();
//    code_seq_debug_print(stdout, main_cs);
}
