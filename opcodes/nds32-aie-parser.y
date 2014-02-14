/*
    Andes Coprocessor Extension Language Syntax for GNU Bison
*/
%{

/*
    This file contains the generated bison grammar rules for
    the Andes Coprocessor Extension Language.

    DO NOT edit the file directly!
*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nds32-aie.h"
#include "nds32-aie-utils.h"
#include "nds32-aie-typedef-insn.h"
#define YYERROR_VERBOSE

#ifdef yylex
  #undef yylex
#endif
#define yylex nds32_aie_scanner_lex
extern int nds32_aie_scanner_lex(void);

/* global */
group g = AIE_UNKNOWN;
imm_type align_type = aie_ub;
aie_reg_type rtype = aie_s;
unsigned int mapping_range[4] = {0,0,0,0};
void yyerror(const char *format, ...);
stackop_token* stackop_list;
pseudo_reg_mapping* pseudo_reg_tmp;

/* used to construct indivisual instruction */
aie_insn_unit* itype_tmp = NULL;
aie_insn_unit* insn_tmp = NULL;
aie_insn_unit* gtmp = NULL;
sym_val_map_unit* sym_val_map_tmp = NULL;
aie_register* aie_register_tmp = NULL;
int is_define_pseudo_register_access_macro = 0;
char additional_string[MAX_STR_CONST] = "\0";
unsigned int set_common_format = 0;
unsigned int valid_bits_width = 0;
unsigned int gi = 0;
unsigned int gj = 0;
unsigned int k = 0;
unsigned int l = 0;
int offset = 0;
register_mapping  is_pseudo = physical;
int is_trivial_register = 0;
int is_trivial_sym_val_map = 0;
int is_trivial_typedef = 0;
int is_trivial_insn_declare = 0;
int is_trivial_def_insn = 0;
int is_trivial_format = 0;
int is_trivial_decode = 0;
int is_trivial_syntax = 0;

/* prototyping */
extern void eval_restart(FILE* ptr);
inline void bits_width_determinate(aie_insn_unit* insn);

inline void bits_width_determinate(aie_insn_unit* insn){
    switch(insn->insn_group) {
        case AIE_CPE1:
        case AIE_CPE2:
        case AIE_CPE3:
        case AIE_CPE4:
            insn->defined_width[0] = 0; insn->defined_width[10] = 1; insn->defined_width[20] = 1; insn->defined_width[30] = 0;
            insn->defined_width[1] = 0; insn->defined_width[11] = 1; insn->defined_width[21] = 1; insn->defined_width[31] = 0;
            insn->defined_width[2] = 0; insn->defined_width[12] = 1; insn->defined_width[22] = 1;
            insn->defined_width[3] = 0; insn->defined_width[13] = 1; insn->defined_width[23] = 1;
            insn->defined_width[4] = 0; insn->defined_width[14] = 1; insn->defined_width[24] = 1;
            insn->defined_width[5] = 0; insn->defined_width[15] = 1; insn->defined_width[25] = 0;
            insn->defined_width[6] = 1; insn->defined_width[16] = 1; insn->defined_width[26] = 0;
            insn->defined_width[7] = 1; insn->defined_width[17] = 1; insn->defined_width[27] = 0;
            insn->defined_width[8] = 1; insn->defined_width[18] = 1; insn->defined_width[28] = 0;
            insn->defined_width[9] = 1; insn->defined_width[19] = 1; insn->defined_width[29] = 0;
            break;
        case AIE_MFCPD:
        case AIE_MTCPD:
        case AIE_MFCPW:
        case AIE_MTCPW:
        case AIE_MFCPPW:
        case AIE_MTCPPW:
            insn->defined_width[0] = 0; insn->defined_width[10] = 1; insn->defined_width[20] = 0; insn->defined_width[30] = 0;
            insn->defined_width[1] = 0; insn->defined_width[11] = 1; insn->defined_width[21] = 0; insn->defined_width[31] = 0;
            insn->defined_width[2] = 0; insn->defined_width[12] = 1; insn->defined_width[22] = 0;
            insn->defined_width[3] = 0; insn->defined_width[13] = 1; insn->defined_width[23] = 0;
            insn->defined_width[4] = 0; insn->defined_width[14] = 1; insn->defined_width[24] = 0;
            insn->defined_width[5] = 0; insn->defined_width[15] = 1; insn->defined_width[25] = 0;
            insn->defined_width[6] = 0; insn->defined_width[16] = 1; insn->defined_width[26] = 0;
            insn->defined_width[7] = 0; insn->defined_width[17] = 1; insn->defined_width[27] = 0;
            insn->defined_width[8] = 1; insn->defined_width[18] = 1; insn->defined_width[28] = 0;
            insn->defined_width[9] = 1; insn->defined_width[19] = 1; insn->defined_width[29] = 0;
            break;
        default:
            insn->defined_width[0] = 0; insn->defined_width[10] = 0; insn->defined_width[20] = 1; insn->defined_width[30] = 0;
            insn->defined_width[1] = 0; insn->defined_width[11] = 0; insn->defined_width[21] = 1; insn->defined_width[31] = 0;
            insn->defined_width[2] = 0; insn->defined_width[12] = 0; insn->defined_width[22] = 1;
            insn->defined_width[3] = 0; insn->defined_width[13] = 0; insn->defined_width[23] = 1;
            insn->defined_width[4] = 0; insn->defined_width[14] = 0; insn->defined_width[24] = 1;
            insn->defined_width[5] = 0; insn->defined_width[15] = 0; insn->defined_width[25] = 0;
            insn->defined_width[6] = 0; insn->defined_width[16] = 0; insn->defined_width[26] = 0;
            insn->defined_width[7] = 0; insn->defined_width[17] = 0; insn->defined_width[27] = 0;
            insn->defined_width[8] = 0; insn->defined_width[18] = 0; insn->defined_width[28] = 0;
            insn->defined_width[9] = 0; insn->defined_width[19] = 0; insn->defined_width[29] = 0;
            break;
    }
}

extern token_type syntax_scanner(void);
%}

%union {
 char* text;
 int value;
}

%left PLUS MINUS
%left DIV MUL

/* General terminals */
%token END 0 "end of file"
%token <text> PERCENT "%"
%token <text> COMMA ","
%token <text> SEMICOLON ";"
%token <text> COLON ":"
%token <text> MINUS "-"
%token <text> PLUS "+"
%token <text> MUL "*"
%token <text> DIV "/"
%token <text> DOT "."
%token <text> EQ "="
%token <text> LT "<"
%token <text> GT ">"
%token <text> LPAREN "("
%token <text> RPAREN ")"
%token <text> LBRACK "["
%token <text> RBRACK "]"
%token <text> LBRACE "{"
%token <text> RBRACE "}"
%token <text> DOLLAR "$"
%token <text> CROSS "#"
%token <text> NOT "!"
%token <text> LOGIC_AND "&"
%token <text> LOGIC_OR "|"
%token <text> AT "@"
%token <text> SQUARE "^"
%token <text> SLASH "~"
%token <text> SDOT "`"
%token <text> RDOT "'"
%token <text> SKIP "unknown"
%token <text> PP "..."
%token <value> INT "integer"
%token <text> STR "string"
%token <text> INVALID_PRESENTABLE_TOKEN "invalid token"
%token <text> INVALID_IMPRESENTABLE_TOKEN

/* ISA terminals */
%token <text> TYPEDEF  "typedef"
%token <text> REGISTER "register"
%token <text> PSEUDO "pseduo"
%token <text> PARTIAL_PSEUDO "partial_pseduo"
%token <text> WIDTH "width"
%token <text> REGNUM "regnum"
%token <text> RD_INSN "rd_insn"
%token <text> WR_INSN "wr_insn"
%token <text> SYM_MAP "sym_map"
%token <text> SYMBOL_VALUE_MAP "symbol_value_map"
%token <text> INSN "insn"
%token <text> GROUP "group"
%token <text> CPE1 "CPE1"
%token <text> CPE2 "CPE2"
%token <text> CPE3 "CPE3"
%token <text> CPE4 "CPE4"
%token <text> MFCPD "MFCPD"
%token <text> MTCPD "MTCPF"
%token <text> MFCPW "MFCPW"
%token <text> MTCPW "MTCPW"
%token <text> MFCPPW "MFCPPW"
%token <text> MTCPPW "MTCPPW"
%token <text> CPLD_BI "CPLD_BI"
%token <text> CPSD_BI "CPSD_BI"
%token <text> CPLW_BI "CPLW_BI"
%token <text> CPSW_BI "CPSW_BI"
%token <text> LWC_BI "LWC_BI"
%token <text> SWC_BI "SWC_BI"
%token <text> LDC_BI "LDC_BI"
%token <text> SDC_BI "SDC_BI"
%token <text> CPLD "CPLD"
%token <text> CPSD "CPSD"
%token <text> CPLW "CPLW"
%token <text> CPSW "CPSW"
%token <text> LWC "LWC"
%token <text> SWC "SWC"
%token <text> LDC "LDC"
%token <text> SDC "SDC"
%token <text> CPID "cpid"
%token <text> STAGE "stage"
%token <text> EACK "eack"
%token <text> LACK "lack"
%token <text> D2M "d2m"
%token <text> D2C "d2c"
%token <text> FORMAT "format"
%token <text> SYNTAX "syntax"
%token <text> DECODE "decode"
%token <text> ID "identifier"
%token <text> PSTAGE "pstage"
%token <text> CSTAGE "cstage"
%token <text> SSSTAGE "ssstage"
%token <text> SSCYCLE "sscycle"
%token <text> SD "sd"
%token <text> SW "sw"
%token <text> SH "sh"
%token <text> SB "sb"
%token <text> UD "ud"
%token <text> UW "uw"
%token <text> UH "uh"
%token <text> UB "ub"
%token <text> SOURCE_OPERAND "s"
%token <text> DESTINATION_OPERAND "d"

/* Non-terminal for general tokens */
%type <text> field_descriptor operand_descriptor
//%type <text> reserved_word reserved_word_but_insn reserved_word_but_insn_comma_semicolon_presentableInvalidToken reserved_word_but_comma_semicolon

%type <text> invalid_literal_id invalid_nonrepresentable_literal_id
%type <value> invalid_digit_id
%type <text> is_valid_id valid_id

/* Non-terminals for ISA */
%type <text> re_input re_input1 input

/* Non-terminal for symbol value mapping */
%type <text> sym_val_struct re_sym_val_exp sym_val_exp additional_str

/* Non-terminal for typedef instruction definition */
%type <text> typedef_new_insn insn_body insn_expression
%type <text> aie_insn_format aie_insn_syntax aie_insn_decode aie_insn_group aie_insn_pstage aie_insn_cstage aie_insn_ssstage aie_insn_sscycle
%type <text> re_insn_format insn_format format_statement
%type <text> insn_decode decode_statement re_decode_statement
%type <text> re_insn_syntax insn_syntax syntax_symbol

/* Non-terminal for define intruction */
%type <text> define_new_single_insn define_insn_attribute re_define_insn_attribute
%type <text> invalid_presentable_insn_attribute_literal
%type <value> invalid_presentable_insn_attribute_digit

/* Non-terminal for instruction declaration */
%type <text> declare_insn re_declare_instance declare_and_overwrite_attribute possibly_declared_instance expression
%type <text> invalid_impresentable_declared_instance is_valid_declared_instance
%type <value> invalid_presentable_declared_instance_digit
%type <text> invalid_presentable_declared_instance_literal

/* Non-terminal for instruction assignment */
%type <text> assign_declared_insn_attribute

/* Non-terminal for register declaration */
%type <text> hardware_register re_hardware_register_statement hardware_register_statement is_pseudo_register
%type <text> aie_register_attr register_width register_num register_symbol register_reg_map
%type <text> register_rd_insn register_wr_insn
%type <text> pseudo_register_index re_pseudo_register_index pseudo_register_index_list
%type <text> physical_register_map physical_register re_physical_register physical_register_index physical_register_bit_range

/* Non-terminal for pipeline resources */
%type <text> pipe stage_list stage_mnemonic

%start re_input

%%

/*************************************************/
/* AIE Grammars for main entey                   */
/*************************************************/
re_input: /* empty */ { printf("Warning! The aie file is empty!\n"); }
        | re_input1
        {
           if (!check_aie_register_rd_wr_insn()) {
               YYERROR;
           }
        };

re_input1: input
	 | re_input1 input;

input :
    sym_val_struct
    | pipe
    | typedef_new_insn
    | declare_insn
    | assign_declared_insn_attribute
    | define_new_single_insn
    | hardware_register
    | SEMICOLON ;

/**************************************/
/* AIE Grammar for pipeline resources */
/**************************************/
pipe:
    STAGE
    {
        if (!init_aie_cop(&cop)) {
            YYERROR;
        }
    } EQ LBRACE stage_list RBRACE SEMICOLON
    | CPID EQ INT SEMICOLON
    {
        if (!add_cop_cpid(cop, $3)) { YYERROR; }
    }
    | EACK EQ is_valid_id SEMICOLON
    {
        if (!add_cop_etrap(cop, $3)) { YYERROR; }
    }
    | LACK EQ is_valid_id SEMICOLON
    {
        if (!add_cop_ltrap(cop, $3)) { YYERROR; }
    }
    | D2C EQ is_valid_id SEMICOLON
    {
        if (!add_cop_d2c(cop, $3)) { YYERROR; }
    }
    | D2M EQ is_valid_id SEMICOLON
    {
        if (!add_cop_d2m(cop, $3)) { YYERROR; }
    };
stage_list:
    stage_list COMMA stage_mnemonic | stage_mnemonic;
stage_mnemonic:
    is_valid_id
    {
        if (!add_cop_stage(cop, $1)) {
            YYERROR;
        }
    }

/**************************************/
/* AIE Grammar for hardware resources */
/**************************************/
hardware_register: is_pseudo_register hardware_register_statement;

is_pseudo_register:
    PSEUDO {is_pseudo = pseudo;}
    | PARTIAL_PSEUDO {is_pseudo = partial_pseudo;}
    | {is_pseudo = physical;};

hardware_register_statement:
    REGISTER is_valid_id LBRACE RBRACE SEMICOLON
    {
        yyerror("specify trivial register: `%s'", $2);
        YYERROR;
    }
    | REGISTER is_valid_id LBRACE
    {
        is_trivial_register = 1;
        is_define_pseudo_register_access_macro = 0;
        if (!add_aie_register(&aie_register_tmp, is_pseudo, $2)) {
            YYERROR;
        }
    } re_hardware_register_statement RBRACE SEMICOLON {
        if (is_trivial_register) {
            yyerror("specify trivial register: `%s'", $2);
            YYERROR;
        }
        if ((pseudo == aie_register_tmp->is_pseudo || partial_pseudo == aie_register_tmp->is_pseudo) &&
             !is_define_pseudo_register_access_macro) {
            yyerror("undefined reference to pseudo register `%s' access macro", aie_register_tmp->symbol);
            YYERROR;
        }

        // Check whether width is defined.
        if (0 == aie_register_tmp->width) {
            yyerror("undefined reference to %s.width", aie_register_tmp->symbol);
            YYERROR;
        }

        // Check whether regnum is defined.
        // Check regnum before sym_map.
        // If regnum is 1 and sym_map is null, the map_unit should be NULL.
        if (0 == aie_register_tmp->regnum) {
            yyerror("undefined reference to %s.regnum", aie_register_tmp->symbol);
            YYERROR;
        }

        // If regnum is 1, there is no need to check sym_map and map_unit can be NULL.
        // Check whether sym_map is defined.
        if ((aie_register_tmp->regnum > 1) && !aie_register_tmp->map_unit) {
            yyerror("undefined reference to %s.sym_map", aie_register_tmp->symbol);
            YYERROR;
        }

        // If map_unit is not NULL,
        // check whether total register number is not equal to total unique mapped value symbol
        if (aie_register_tmp->map_unit &&
            (aie_register_tmp->regnum !=
             (unsigned int)aie_register_tmp->map_unit->total_unique_mapped_value_symbol)) {
            yyerror("only have %d registers for %d unique mapped value symbol of %s.sym_map.%s", aie_register_tmp->regnum, aie_register_tmp->map_unit->total_unique_mapped_value_symbol, aie_register_tmp->symbol, aie_register_tmp->map_unit->name);
            YYERROR;
        }

    };

re_hardware_register_statement: re_hardware_register_statement aie_register_attr | aie_register_attr;

aie_register_attr: register_width | register_num | register_symbol | register_reg_map | register_rd_insn
    | register_wr_insn
    | ID EQ
    {
        yyerror("only support register attribute: regnum, width, sym_map, reg_map macro");
        YYERROR;
    }
    | SEMICOLON;

register_width:
    WIDTH EQ INT
    {
        is_trivial_register = 0;
        if (!add_aie_register_width(aie_register_tmp, $3)) {
            YYERROR;
        }
    };

register_num:
    REGNUM EQ INT
    {
        is_trivial_register = 0;
        if (!add_aie_register_regnum(aie_register_tmp, $3)) {
            YYERROR;
        }
    };

register_rd_insn:
    RD_INSN EQ is_valid_id
    {
        is_trivial_register = 0;
        if (!add_aie_register_rd_insn(aie_register_tmp, $3)) {
            YYERROR;
        }
    };

register_wr_insn:
    WR_INSN EQ is_valid_id
    {
        is_trivial_register = 0;
        if (!add_aie_register_wr_insn(aie_register_tmp, $3)) {
            YYERROR;
        }
    };

register_symbol:
    SYM_MAP EQ is_valid_id
    {
        is_trivial_register = 0;
        if (!add_aie_register_sym_val_mapping(aie_register_tmp, $3)) {
            YYERROR;
        }
    };

register_reg_map:
    ID
    {
        is_trivial_register = 0;

        if (physical == aie_register_tmp->is_pseudo) {
            yyerror("try to define register access macro for non-pseudo register: `%s'", aie_register_tmp->symbol);
            YYERROR;
        }

        if (0 != strcmp (aie_register_tmp->symbol, $1)) {
            yyerror("invalid naming of pseudo register access macro `%s', %s is required", $1, aie_register_tmp->symbol);
            YYERROR;
        }
    } LPAREN pseudo_register_index RPAREN EQ physical_register_map
    {
        //is pseudo register?
        if (pseudo == aie_register_tmp->is_pseudo || partial_pseudo == aie_register_tmp->is_pseudo) {
            //caution!, start at j = 1
            int j = 1;
            for (j = 1; j <= aie_register_tmp->num_dependency; j++) {
                //get sub source operand register sequentially
                aie_register* ssreg = aie_get_sub_reg(aie_register_tmp, j);

                //use to check pseudo reg index
                pseudo_reg_idx* tmp = aie_register_tmp->index_list_head;
                if (tmp) {
                     while(tmp) {
                        unsigned int mapped_idx = aie_get_sub_reg_idx(aie_register_tmp, j, tmp->idx);
                        if (ssreg->regnum < mapped_idx) {
                            yyerror("%s(%d) is mapped to physical register %s(%d) which is out of range", aie_register_tmp->symbol, tmp->idx, ssreg->symbol, mapped_idx);
                            YYERROR;
                        }
                        tmp = tmp->n;
                    }
                }
                else {
                    unsigned int i = 0;
                    for (i =0; i<aie_register_tmp->regnum; i++) {
                        unsigned int mapped_idx = aie_get_sub_reg_idx(aie_register_tmp, j, i);
                        if (ssreg->regnum < mapped_idx) {
                            yyerror("%s(%d) is mapped to physical register %s(%d) which is out of range", aie_register_tmp->symbol, i, ssreg->symbol, mapped_idx);
                            YYERROR;
                        }
                    }
                }
            }
        }

        //check all mapped pseudo register bit field is fully mapped or not
        int pseudo_reg_width = aie_register_tmp->width;
        pseudo_reg_mapping* tmp = aie_register_tmp->phy_reg_map_list;
        while(tmp) {
            pseudo_reg_width -= tmp->shift;
            tmp = tmp->n;
        }
        if (0 < pseudo_reg_width) {
            yyerror("some bit fields of %s are unmapped", aie_register_tmp->symbol);
            YYERROR;
        }
        if (0 > pseudo_reg_width) {
            yyerror("Total mapped bits are larger than the width of %s", aie_register_tmp->symbol);
            YYERROR;
        }


        is_define_pseudo_register_access_macro = 1;
    };

pseudo_register_index:
    is_valid_id
    {
        if (!aie_strcpy(aie_register_tmp->index_symbol, $1)) {
            yyerror("try to use unsupported index `%s'", $1);
            YYERROR;
        }
    }
    | is_valid_id
    {
        if (!aie_strcpy(aie_register_tmp->index_symbol, $1)) {
            yyerror("try to use unsupported index `%s'", $1);
            YYERROR;
        }
    } re_pseudo_register_index
    {
        pseudo_reg_idx* tmp = aie_register_tmp->index_list_head;
        unsigned int defined_index_number = 0;
        while(tmp) {
            defined_index_number++;
            tmp = tmp->n;
        }

        if (pseudo == aie_register_tmp->is_pseudo) {
            if (defined_index_number != aie_register_tmp->regnum) {
                yyerror("possibly `%d' register index are defined but only have register: %s.regnum= %d", defined_index_number, aie_register_tmp->symbol, aie_register_tmp->regnum);
                YYERROR;
            }
        }
        else if (partial_pseudo == aie_register_tmp->is_pseudo) {
            if (0 == defined_index_number || defined_index_number >= aie_register_tmp->regnum) {
                yyerror("possibly `%d' register index are defined but only have register: %s.regnum= %d", defined_index_number, aie_register_tmp->symbol, aie_register_tmp->regnum);
                YYERROR;
            }
        }
    };
re_pseudo_register_index: re_pseudo_register_index COMMA pseudo_register_index_list | EQ pseudo_register_index_list;
pseudo_register_index_list:
    INT PP INT
    {
        int i = 0;
        for(i = $1; i<= $3; i++) {
            if (!add_pseudo_reg_idx(aie_register_tmp, i)) {
                YYERROR;
            }
        }
    }
    | INT
    {
        if (!add_pseudo_reg_idx(aie_register_tmp, $1)) {
            YYERROR;
        }
    };

physical_register_map: physical_register | LPAREN re_physical_register RPAREN;
re_physical_register: physical_register COMMA re_physical_register | physical_register;
physical_register:
    ID LPAREN
    {
        aie_register* tmp_reg = is_aie_register_exist($1);
        if (!tmp_reg || 0 == strcmp(aie_register_tmp->symbol, $1)) {
            yyerror("undefined or self reference to register: `%s'", $1);
            YYERROR;
        }
        if ((pseudo == tmp_reg->is_pseudo) || (partial_pseudo == tmp_reg->is_pseudo)) {
            yyerror("reference of pseudo register: `%s', non-pseudo register is required", tmp_reg->symbol);
            YYERROR;
        }

        pseudo_reg_tmp = (pseudo_reg_mapping*) aie_malloc(sizeof(pseudo_reg_mapping));
        init_pseudo_reg_mapping(pseudo_reg_tmp);

        if (!add_pseudo_reg(aie_register_tmp, pseudo_reg_tmp)) {
            yyerror("can't create link while parsing pseudo regsiter `%s'", aie_register_tmp->symbol);
            YYERROR;
        }

        //update physical register
        pseudo_reg_tmp->physical_reg = tmp_reg;

    } physical_register_index RPAREN physical_register_bit_range
    {
        //update dependent physical register
        aie_register_tmp->num_dependency++;
    };

physical_register_index:
    INT { add_stackop_token(pseudo_reg_tmp, STACKOP_DIGIT, $1); }

    | physical_register_index PLUS { add_stackop_token(pseudo_reg_tmp, STACKOP_POSTIVE, 0); }
      physical_register_index

    | physical_register_index MINUS { add_stackop_token(pseudo_reg_tmp, STACKOP_NEGATIVE, 0); }
      physical_register_index

    | physical_register_index MUL { add_stackop_token(pseudo_reg_tmp, STACKOP_MUL, 0); }
      physical_register_index

    | physical_register_index DIV { add_stackop_token(pseudo_reg_tmp, STACKOP_DIV, 0); }
      physical_register_index

    | LPAREN { add_stackop_token(pseudo_reg_tmp, STACKOP_LPAREN, 0); }
      physical_register_index RPAREN { add_stackop_token(pseudo_reg_tmp, STACKOP_RPAREN, 0); }

    | ID
    {
        if (0 != strcmp(aie_register_tmp->index_symbol, $1)) {
            yyerror("register access macro index: `%s' is not defined, %s is required ", $1, aie_register_tmp->index_symbol);
            YYERROR;
        }
        add_stackop_token(pseudo_reg_tmp, STACKOP_IDX, 0);
    };

physical_register_bit_range:
    LBRACK INT COLON INT RBRACK
    {
        //physical register width
        int physical_reg_width = pseudo_reg_tmp->physical_reg->width;

        //check whether the range is out of corresponding physical reigster width
        if ($2 < (signed int )0 || $2 > (physical_reg_width - 1)) {
            yyerror("Bit `%d' is out of range, %s[%d:%d]", $2, pseudo_reg_tmp->physical_reg->symbol, physical_reg_width - 1, 0);
            YYERROR;
        }

        //check whether the range is out of corresponding physical reigster width
        if ($4 < (signed int)0 || $4 > (physical_reg_width - 1)) {
            yyerror("Bit `%d' is out of range, %s[%d:%d]", $4, pseudo_reg_tmp->physical_reg->symbol, physical_reg_width - 1, 0);
            YYERROR;
        }
        if ($2 < $4) {
            yyerror("%s[%d:%d] is invalid, high<->low bit order is required", aie_register_tmp->symbol, $2, $4);
            YYERROR;
        }

        //update pyhsical reigster bit mask and bit shift
        pseudo_reg_tmp->mask = $2;
        pseudo_reg_tmp->shift = ($2 - $4 + 1);
    }
    |
    {
        //update pyhsical reigster bit mask and bit shift
        pseudo_reg_tmp->mask =
        (64 == pseudo_reg_tmp->physical_reg->width)? 0xffffffffffffffffLL :
        (32 == pseudo_reg_tmp->physical_reg->width) ? 0x00000000ffffffffLL :
        (16 == pseudo_reg_tmp->physical_reg->width) ? 0x000000000000ffffLL : 0x00000000000000ffLL;
        pseudo_reg_tmp->shift = pseudo_reg_tmp->physical_reg->width;
    };

/*************************************************/
/* AIE Grammar for assembly symbol value mapping */
/*************************************************/
sym_val_struct:
    SYMBOL_VALUE_MAP is_valid_id LBRACE RBRACE SEMICOLON
    {
        yyerror("specify trivial symbol_value_map: `%s'", $2);
        YYERROR;
    }
    | SYMBOL_VALUE_MAP is_valid_id LBRACE
    {
        //is this is a trivial_statement
        is_trivial_sym_val_map = 1;

        if (!add_sym_val_map(&sym_val_map_tmp, $2)) {
            YYERROR;
        }
    } re_sym_val_exp RBRACE SEMICOLON
    {
        if (is_trivial_sym_val_map) {
            yyerror("specify trivial symbol_value_map: `%s'", $2);
            YYERROR;
        }
    };

re_sym_val_exp: re_sym_val_exp sym_val_exp | sym_val_exp ;
sym_val_exp:
    STR EQ INT SEMICOLON
    {
        if (!add_sym_val(sym_val_map_tmp, $1, $3)) {
            YYERROR;
        }
        is_trivial_sym_val_map = 0;
    }
    | STR LBRACK INT PP INT RBRACK additional_str EQ LBRACK INT PP INT RBRACK SEMICOLON
    {
        //[i...j] = [k...l]
        if (!aie_strcpy(eval_token, $1))
            YYERROR;

        gi = $3;
        gj = $5;
        k = $10;
        l = $12;

        offset = abs(gj - gi) + 1;
        if (offset != abs(l - k) + 1) {
            yyerror("mismatched range size between symbols and values");
            YYERROR;
        }

        //increment symbol
        if (gj > gi) {
            if (l > k) {
                while(0 != offset) {
                    char tmp[MAX_STR_CONST] = "\0";

                    if (strlen(additional_string)) {
                        sprintf(tmp, "%s%d%s", eval_token, gi++, additional_string);
                    }
                    else {
                        sprintf(tmp,"%s%d",eval_token,gi++);
                    }

                    if (!add_sym_val(sym_val_map_tmp, tmp, k++)) {
                        YYERROR;
                    }

                    offset--;
                }
            }
            else {
                while(0 != offset) {
                    char tmp[MAX_STR_CONST] = "\0";
                    if (strlen(additional_string)) {
                        sprintf(tmp, "%s%d%s", eval_token, gi++, additional_string);
                    }
                    else {
                        sprintf(tmp,"%s%d",eval_token,gi++);
                    }

                    if (!add_sym_val(sym_val_map_tmp, tmp, k--)) {
                        YYERROR;
                    }

                    offset--;
                }
            }
        }

        //decrement
        else {
            if (l > k) {
                while(0 != offset) {
                    char tmp[MAX_STR_CONST] = "\0";
                    if (strlen(additional_string)) {
                        sprintf(tmp, "%s%d%s", eval_token, gi--, additional_string);
                    }
                    else {
                        sprintf(tmp,"%s%d",eval_token,gi--);
                    }

                    if (!add_sym_val(sym_val_map_tmp, tmp, k++)) {
                        YYERROR;
                    }

                    offset--;
                }
            }
            else {
                while(0 != offset) {
                    char tmp[MAX_STR_CONST] = "\0";
                    if (strlen(additional_string)) {
                        sprintf(tmp, "%s%d%s", eval_token, gi--, additional_string);
                    }
                    else {
                        sprintf(tmp,"%s%d",eval_token,gi--);
                    }

                    if (!add_sym_val(sym_val_map_tmp, tmp, k--)) {
                        YYERROR;
                    }

                    offset--;
                }
            }
        }
        is_trivial_sym_val_map = 0;
    }
    | SEMICOLON ;

additional_str:
    STR
    {
        if(!aie_strcpy(additional_string, $1)) {
            YYERROR;
        }
    }
    |
    {
        additional_string[0] = '\0';
    };

/**************************************************/
/* AIE Grammar for instruction type define        */
/**************************************************/
typedef_new_insn:
    TYPEDEF INSN is_valid_id LBRACE RBRACE SEMICOLON
    {
        yyerror("define trivial typedef: `%s'", $3);
        YYERROR;
    }
    | TYPEDEF INSN is_valid_id LBRACE
    {
        is_trivial_typedef = 1;

        if (!add_aie_itype(&itype_tmp, $3)) {
            YYERROR;
        }

        //Set the following reference is itype_tmp not insn_tmp
        set_common_format = 1;

    } insn_body RBRACE SEMICOLON
    {
        set_common_format = 0;

        if (is_trivial_typedef) {
            yyerror("define trivial typedef: `%s'", $3);
            YYERROR;
        }
    } ;

insn_body: insn_body insn_expression | insn_expression;
insn_expression:
      aie_insn_group   { is_trivial_typedef = 0; }
    | aie_insn_format  { is_trivial_typedef = 0; }
    | aie_insn_syntax  { is_trivial_typedef = 0; }
    | aie_insn_decode  { is_trivial_typedef = 0; }
    | aie_insn_pstage  { is_trivial_typedef = 0; }
    | aie_insn_cstage  { is_trivial_typedef = 0; }
    | aie_insn_ssstage { is_trivial_typedef = 0; }
    | aie_insn_sscycle { is_trivial_typedef = 0; }
    | SEMICOLON
    | invalid_presentable_insn_attribute_literal
    {
        yyerror("invalid attribute: `%s', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", $1);
        YYERROR;
    }
    | invalid_presentable_insn_attribute_digit
    {
        yyerror("invalid attribute: `%d', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", $1);
        YYERROR;
    }
    | invalid_impresentable_insn_attribute
    {
        yyerror("invalid attribute, support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only");
        YYERROR;
    };


/*************************************************/
/* AIE Grammar for instruction declaration       */
/*************************************************/
declare_insn:
    ID is_valid_declared_instance
    {
        itype_tmp = is_aie_itype_exist($1);

        if (!itype_tmp) {
            yyerror("undefined reference to new instruction type: `%s'", $1);
            YYERROR;
        }

        if (is_multiple_aie_insn_create($2)) {
            yyerror("`%s' is already decalared", $2);
            YYERROR;
        }

        if (!add_aie_insn(&insn_tmp, itype_tmp, $2)) {
            YYERROR;
        }
    } re_declare_instance declare_and_overwrite_attribute;
re_declare_instance: re_declare_instance COMMA possibly_declared_instance | {};
possibly_declared_instance:
    is_valid_declared_instance
    {
        if (is_multiple_aie_insn_create($1)) {
            yyerror("`%s' is already decalared", $1);
            YYERROR;
        }

        if (!add_aie_insn(&insn_tmp, itype_tmp, $1)) {
            YYERROR;
        }
    };
declare_and_overwrite_attribute: DOT expression | SEMICOLON;

is_valid_declared_instance:
    invalid_impresentable_declared_instance
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier");
        YYERROR;
    }
    | invalid_presentable_declared_instance_digit
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier: `%d'", $1);
        YYERROR;
    }
    | invalid_presentable_declared_instance_literal
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier: `%s'", $1);
        YYERROR;
    }
    | ID;

invalid_impresentable_declared_instance: invalid_nonrepresentable_literal_id;
invalid_presentable_declared_instance_digit: invalid_digit_id;
invalid_presentable_declared_instance_literal:
    PERCENT | COLON | MINUS | PLUS | MUL | DIV | EQ | LT | GT | LPAREN | RPAREN
    | LBRACK | RBRACK | DOLLAR | CROSS | NOT | LOGIC_AND | LOGIC_OR | AT | SQUARE | SLASH
    | SDOT | RDOT | SKIP  | PP | STR | TYPEDEF  | REGISTER | PSEUDO | PARTIAL_PSEUDO | WIDTH | REGNUM | SYM_MAP
    | RD_INSN | WR_INSN
    | SYMBOL_VALUE_MAP | INSN | CPE1 | CPE2 | CPE3 | CPE4 | MFCPD | MTCPD | MFCPW | MTCPW | MFCPPW
    | MTCPPW | CPLD_BI | CPSD_BI | CPLW_BI | CPSW_BI | LWC_BI | SWC_BI | LDC_BI | SDC_BI | CPLD | CPSD | CPLW
    | CPSW | LWC | SWC | LDC | SDC | CPID | STAGE | EACK | LACK | D2M | D2C  | SD | SW | SH | SB | UD | UW
    | GROUP | SYNTAX | FORMAT | DECODE | PSTAGE | CSTAGE | SSSTAGE | SSCYCLE | LBRACE | RBRACE
    | UH | UB | SOURCE_OPERAND | DESTINATION_OPERAND | INVALID_PRESENTABLE_TOKEN;


/*************************************************/
/* AIE Grammar for instruction assignement       */
/*************************************************/
assign_declared_insn_attribute:
    ID DOT
    {
        insn_tmp = is_aie_insn_valid($1);
        if (!insn_tmp) {
            yyerror("instruction type is not defined before declare new instruction: `%s'", $1);
            YYERROR;
        }
    } expression ;

expression:
    aie_insn_group      { insn_tmp->can_be_redef_group = 0; }
    | aie_insn_decode
    | aie_insn_format   { insn_tmp->can_be_redef_format = 0; }
    | aie_insn_syntax   { insn_tmp->can_be_redef_syntax = 0; }
    | aie_insn_pstage
    | aie_insn_cstage
    | aie_insn_ssstage
    | aie_insn_sscycle;
    | invalid_presentable_insn_attribute_literal
    {
        yyerror("invalid attribute: `%s', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", $1);
        YYERROR;
    }
    | invalid_presentable_insn_attribute_digit
    {
        yyerror("invalid attribute: `%d', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", $1);
        YYERROR;
    }
    | invalid_impresentable_insn_attribute
    {
        yyerror("invalid attribute, support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only");
        YYERROR;
    };

/***************************************************/
/* AIE Grammar for define new single instruction   */
/***************************************************/
define_new_single_insn:
    INSN is_valid_id LBRACE RBRACE SEMICOLON
    {
        yyerror("define trivial insn: `%s'", $2);
        YYERROR;
    }
    | INSN is_valid_id LBRACE
    {
        is_trivial_def_insn = 1;

        if (is_multiple_aie_insn_create($2)) {
            yyerror("insn `%s' is already decalared", $2);
            YYERROR;
        }

        if (!define_aie_insn(&insn_tmp, $2)) {
            YYERROR;
        }
    } re_define_insn_attribute RBRACE SEMICOLON
    {
        if (is_trivial_def_insn) {
            yyerror("define trivial insn: `%s'", $2);
            YYERROR;
        }
    };

re_define_insn_attribute: re_define_insn_attribute define_insn_attribute | define_insn_attribute;
define_insn_attribute:
    aie_insn_group      { is_trivial_def_insn = 0; }
    | aie_insn_format   { is_trivial_def_insn = 0; }
    | aie_insn_decode   { is_trivial_def_insn = 0; }
    | aie_insn_syntax   { is_trivial_def_insn = 0; }
    | aie_insn_pstage   { is_trivial_def_insn = 0; }
    | aie_insn_cstage   { is_trivial_def_insn = 0; }
    | aie_insn_ssstage  { is_trivial_def_insn = 0; }
    | aie_insn_sscycle  { is_trivial_def_insn = 0; }
    | SEMICOLON
    | invalid_presentable_insn_attribute_literal
    {
        yyerror("invalid insn attribute: `%s', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", $1);
        YYERROR;
    }
    | invalid_presentable_insn_attribute_digit
    {
        yyerror("invalid insn attribute: `%d', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", $1);
        YYERROR;
    }
    | invalid_impresentable_insn_attribute
    {
        yyerror("invalid insn attribute, support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only");
        YYERROR;
    };

invalid_impresentable_insn_attribute: invalid_nonrepresentable_literal_id;
invalid_presentable_insn_attribute_digit: invalid_digit_id;
invalid_presentable_insn_attribute_literal:
    PERCENT | COMMA | COLON | MINUS | PLUS | MUL | DIV | DOT | EQ | LT | GT | LPAREN | RPAREN
    | LBRACK | RBRACK | DOLLAR | CROSS | NOT | LOGIC_AND | LOGIC_OR | AT | SQUARE | SLASH
    | SDOT | RDOT | SKIP  | PP | STR | TYPEDEF  | REGISTER | PSEUDO | PARTIAL_PSEUDO | WIDTH | REGNUM | SYM_MAP
    | RD_INSN | WR_INSN
    | SYMBOL_VALUE_MAP | INSN | CPE1 | CPE2 | CPE3 | CPE4 | MFCPD | MTCPD | MFCPW | MTCPW | MFCPPW
    | MTCPPW | CPLD_BI | CPSD_BI | CPLW_BI | CPSW_BI | LWC_BI | SWC_BI | LDC_BI | SDC_BI | CPLD | CPSD | CPLW
    | CPSW | LWC | SWC | LDC | SDC | CPID | STAGE | EACK | LACK | D2M | D2C  | SD | SW | SH | SB | UD | UW
    | UH | UB | SOURCE_OPERAND | DESTINATION_OPERAND | INVALID_PRESENTABLE_TOKEN | ID;

/*************************************************/
/* AIE Grammar for instruction format            */
/*************************************************/
aie_insn_format:
    FORMAT EQ LPAREN RPAREN SEMICOLON
    {
        yyerror("specify trivial `format' in %s", gtmp->name);
        YYERROR;
    }
    | FORMAT EQ LPAREN
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (gtmp->can_be_redef_format) {
            //Free memory
            free_ifield(insn_tmp->aie_ifield_list_head);

            gtmp->aie_ifield_list_head = NULL;
            gtmp->aie_ifield_list_tail = NULL;
            gtmp->aie_ifield_list_length = 0;
        }

        if (AIE_UNKNOWN == gtmp->insn_group) {
            yyerror("undefined reference to %s.group before specify %s.format",gtmp->name, gtmp->name);
            YYERROR;
        }

        //According to instruction group, determinate valid user defined bit widths
        bits_width_determinate(gtmp);

        if (gtmp->is_def_format && !gtmp->can_be_redef_format) {
            yyerror("redefinition of %s.format",gtmp->name);
            YYERROR;
        }
        is_trivial_format = 1;
    } re_insn_format {

        if (is_trivial_format) {
            yyerror("specify trivial `format' in %s", gtmp->name);
            YYERROR;
        }

        for (gi=0 ; gi<=31; gi++) {
            if (gtmp->defined_width[gi]) {
                yyerror("+-----------------------------------------------------------------------+");
                yyerror("|31    28|27    24|23    20|19    16|15    12|11    08| 7     4| 3     0|");
                yyerror("+-----------------------------------------------------------------------+");

                switch(gtmp->insn_group) {
                    case AIE_CPE1:
                    case AIE_CPE2:
                    case AIE_CPE3:
                    case AIE_CPE4:
                        yyerror("                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
                        yyerror("                24                                        6");
                        break;
                    case AIE_MFCPD:
                    case AIE_MTCPD:
                    case AIE_MFCPW:
                    case AIE_MTCPW:
                    case AIE_MFCPPW:
                    case AIE_MTCPPW:
                        yyerror("                            ^^^^^^^^^^^^^^^^^^^^^^^^^^");
                        yyerror("                            19                      08");
                        break;
                    default:
                        yyerror("                ^^^^^^^^^^^");
                        yyerror("                24       20");
                        break;
                }
                yyerror("^: allowed customized coprocessor instruction bit field");
                yyerror("");
                yyerror("bit[%d] is not defined", gi);
                YYERROR;
            }
        }
        gtmp->is_def_format = 1;
    } RPAREN SEMICOLON;

re_insn_format: insn_format format_statement ;
insn_format: insn_format format_statement COMMA | {};
format_statement:
    ID COLON INT COLON INT field_descriptor   //2 format support : 1). reg:5:pos
    {                                         //                   2). reg:5:pos:imm_align_type
        unsigned int shf_mnt = $5;
        unsigned int bit_width = $3;
        unsigned int valid_value = 0;
        unsigned int valid = 0;

        if (!add_ifield(gtmp, $1, align_type, aie_s, shf_mnt, bit_width, valid_value, valid)) {
            YYERROR;
        }
        is_trivial_format = 0;
    }
    | INT COLON INT COLON INT // 0x5:5:pos
    {
        unsigned int shf_mnt = $5;
        unsigned int bit_width = $3;
        unsigned int valid_value = $1;
        unsigned int valid = 1;

        if (!add_ifield(gtmp, NULL, aie_ub, aie_s, shf_mnt, bit_width, valid_value, valid)) {
            YYERROR;
        }
        is_trivial_format = 0;
    };

/*************************************************/
/* AIE Grammar for instruction decode            */
/*************************************************/
aie_insn_decode:
    DECODE
    {
        if (1 == set_common_format) {
            yyerror("only group, format are support within typedef");
            YYERROR;
        }
        is_trivial_decode = 1;
    }
    EQ LPAREN insn_decode;

insn_decode :
    RPAREN SEMICOLON
    {
        yyerror("specify trivial `decode' in %s", insn_tmp->name);
        YYERROR;
    }
    | re_decode_statement RPAREN SEMICOLON
    {
        if (is_trivial_decode) {
            yyerror("specify trivial `decode' in %s", gtmp->name);
            YYERROR;
        }
    };

re_decode_statement: re_decode_statement COMMA decode_statement | decode_statement;
decode_statement:
    is_valid_id EQ INT // op=4
    {
        if (!add_insn_decode(insn_tmp, $1, $3)) {
            YYERROR;
        }
        is_trivial_decode = 0;
    };

/*************************************************/
/* AIE Grammar for instruction syntax            */
/*************************************************/
aie_insn_syntax:
    SYNTAX
    {
        if (1 == set_common_format) {
            yyerror("only group, format are support within typedef");
            YYERROR;
        }
    }
    EQ LPAREN re_insn_syntax RPAREN SEMICOLON
    {
        insn_tmp->is_def_syntax = 1;

        aie_ifield* it = insn_tmp->aie_ifield_list_head;
        while(it) {
            if (!it->valid && !it->map_unit) {
                yyerror("can't resolve %s.format.%s in syntax field list", insn_tmp->name, it->symbol);
                yyerror("possibly forget to define operand list and its corresponding field list");
                yyerror("like syntax(\"mnemonic %s,..\", %s, ...)", "\%customized_operand", it->symbol);
                yyerror("or decode it first");
                YYERROR;
            }
            it = it->n;
        }

        //check whether maximum symbol value is out of encoded range defined in format
        it = insn_tmp->aie_ifield_list_head;
        while(it) {
            if (it->map_unit && it->register_unit) {

                //find maximum register mapped value
                unsigned int max_value = 0;
                sym_val_unit* max_sym_value_unit = NULL;
                sym_val_unit* sym_val = it->map_unit->sym_val_list_head;
                while(NULL != sym_val) {
                    if (sym_val->value >= max_value) {
                        max_value = sym_val->value;
                        max_sym_value_unit = sym_val;
                    }

                    sym_val = sym_val->n;
                }

                //maximum value > (2^(bit_width)-1)
                if (max_value > (unsigned int)((1 << it->bit_width) - 1)) {
                    yyerror("\"%s\" = `%d' which is out of range since only have %d bits for %s.format.%s",max_sym_value_unit->symbol, max_sym_value_unit->value, it->bit_width, insn_tmp->name, it->symbol);
                    YYERROR;
                }
            }
            it = it->n;
        }

        ////find unmapped register
        //unsigned int num_pairs = 0;
        //sym_val = map_unit->sym_val_list_head;
        //while(NULL != sym_val) {
        //    num_pairs++;
        //    sym_val = sym_val->n;
        //}

        //if (num_pairs < it->regnum) {
        //    AIE_PPRINT(aie_error, "possibly have %d un-symbol-value-mapped register in `%s'", it->regnum - num_pairs, it->symbol);
        //    return 0;
        //}
    };
re_insn_syntax:
    STR
    {
        if (!check_syntax_string(insn_tmp, $1)) {
            YYERROR;
        }
    } insn_syntax
    {
        if (0 != insn_tmp->syntax_symbol_count) {
            yyerror("syntax error! in field list, there is not corresponding field to customized operand");
            YYERROR;
        }
    } ;

insn_syntax : insn_syntax syntax_symbol | {}
syntax_symbol:
    COMMA is_valid_id
    {
        //actually syntax symbol is 0
        if (0 == insn_tmp->syntax_symbol_count) {
            yyerror("syntax error due to unmapped operand from %s", $2);
            YYERROR;
        }

        //support overwriting alignment/register type attribute, need to preset from old value
        aie_ifield* it = is_ifield_exist(insn_tmp, $2);
        if (NULL == it) {
            yyerror("try to resolve `%s' before define `%s.format.%s'",$2, insn_tmp->name, $2);
            YYERROR;
        }

        align_type = it->imm_align_type;
        rtype = it->register_type;
    }
    operand_descriptor
    {
        if (!find_ifield_for_symbol_stok($2, insn_tmp, align_type, rtype)) {
            YYERROR;
        }
    };

/*************************************************/
/* AIE Grammar for instruction coprocessor group */
/*************************************************/
aie_insn_group:
    GROUP EQ insn_group SEMICOLON
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        //if ((tmp->is_def_format)) {
        //    yyerror("%s.format should not defined before %s.group",tmp->name, tmp->name);
        //    YYERROR;
        //}

        if (!add_insn_group(gtmp, g)) {
            YYERROR;
        }
    } ;

/***********************************************/
/* AIE Grammar for instruction produce stage   */
/***********************************************/
aie_insn_pstage:
    PSTAGE EQ is_valid_id SEMICOLON
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (!add_insn_pstage(gtmp, $3)) {
            YYERROR;
        }
    } ;

/***********************************************/
/* AIE Grammar for instruction consume stage   */
/***********************************************/
aie_insn_cstage:
    CSTAGE EQ is_valid_id SEMICOLON
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (!add_insn_cstage(gtmp, $3)) {
            YYERROR;
        }
    } ;

/**************************************************/
/* AIE Grammar for instruction self-stall cycle   */
/**************************************************/
aie_insn_sscycle:
    SSCYCLE EQ INT SEMICOLON
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (!add_insn_sscycle(gtmp, $3)) {
            YYERROR;
        }
    } ;


/**************************************************/
/* AIE Grammar for instruction self-stall stage   */
/**************************************************/
aie_insn_ssstage:
    SSSTAGE EQ is_valid_id SEMICOLON
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (!add_insn_ssstage(gtmp, $3)) {
            YYERROR;
        }
    } ;

insn_group:
    CPE1 {g = AIE_CPE1;}
    | CPE2 {g = AIE_CPE2;}
    | CPE3 {g = AIE_CPE3;}
    | CPE4 {g = AIE_CPE4;}
    | MFCPD {g = AIE_MFCPD;}
    | MTCPD {g = AIE_MTCPD;}
    | MFCPW {g = AIE_MFCPW;}
    | MTCPW {g = AIE_MTCPW;}
    | MFCPPW {g = AIE_MFCPPW;}
    | MTCPPW {g = AIE_MTCPPW;}
    | CPLD {g = AIE_CPLD;}
    | CPSD {g = AIE_CPSD;}
    | CPLW {g = AIE_CPLW;}
    | CPSW {g = AIE_CPSW;}
    | LDC {g = AIE_LDC;}
    | LWC {g = AIE_LWC;}
    | SDC {g = AIE_SDC;}
    | SWC {g = AIE_SWC;}
    | CPLD_BI {g = AIE_CPLD_BI;}
    | CPSD_BI {g = AIE_CPSD_BI;}
    | CPLW_BI {g = AIE_CPLW_BI;}
    | CPSW_BI {g = AIE_CPSW_BI;}
    | LDC_BI {g = AIE_LDC_BI;}
    | LWC_BI {g = AIE_LWC_BI;}
    | SDC_BI {g = AIE_SDC_BI;}
    | SWC_BI {g = AIE_SWC_BI;}
    | ID
    {
        yyerror("invalid group: `%s', support:", $1);
        yyerror("CPE1, CPE2, CPE3, CPE4, MFCPD, MTCPD, MFCPW, MTCPW, MFCPPW, MTCPPW,");
        yyerror("CPLD, CPSD, CPLW, CPSW, LDC, LWC, SDC, SWC, CPLD_BI, CPSD_BI,");
        yyerror("CPLW_BI, CPSW_BI, LDC_BI, LWC_BI, SDC_BI and SWC_BI");
        YYERROR;
    };

field_descriptor:
    COLON SD   {align_type = aie_sd;}
    | COLON SW {align_type = aie_sw;}
    | COLON SH {align_type = aie_sh;}
    | COLON SB {align_type = aie_sb;}
    | COLON UD {align_type = aie_ud;}
    | COLON UW {align_type = aie_uw;}
    | COLON UH {align_type = aie_uh;}
    | COLON UB {align_type = aie_ub;}
    | COLON ID
    {
        yyerror("invalid field descriptor: `%s', support sd, sw, sh, sb, ud, uw, uh and ub only", $2);
        YYERROR;
    }
    | {align_type = aie_ub;};

operand_descriptor:
    COLON SOURCE_OPERAND { rtype = aie_s; }
    | COLON DESTINATION_OPERAND { rtype = aie_d; }
    | COLON ID
    {
        yyerror("invalid operand descriptor `%s', support s and d", $2);
        YYERROR;
    }
    |
    {
        rtype = aie_s;
    };

//reserved_word_but_insn_comma_semicolon_impresentableInvalidToken:
//    COLON | MINUS | PLUS | MUL | DIV | DOT | EQ | LT | GT | LPAREN | RPAREN
//  | LBRACK | RBRACK | LBRACE | RBRACE | DOLLAR | CROSS | NOT | LOGIC_AND | LOGIC_OR | AT | SQUARE | SLASH
//  | SDOT | RDOT | SKIP | PP | STR | TYPEDEF | REGISTER | PSEUDO | PARTIAL_PSEUDO | WIDTH | REGNUM | SYM_MAP
//  | SYMBOL_VALUE_MAP | GROUP | CPE1 | CPE2 | CPE3 | CPE4 | MFCPD | MTCPD | MFCPW | MTCPW | MFCPPW
//  | MTCPPW | CPLD_BI | CPSD_BI | CPLW_BI | CPSW_BI | LWC_BI | SWC_BI | LDC_BI | SDC_BI | CPLD | CPSD | CPLW
//  | CPSW | LWC | SWC | LDC | SDC | CPID | STAGE | EACK | LACK | D2M | D2C | FORMAT | SYNTAX | DECODE | PSTAGE
//  | CSTAGE | SSSTAGE | SSCYCLE |PERCENT | SD | SW | SH | SB | UD | UW | UH | UB | SOURCE_OPERAND
//  | DESTINATION_OPERAND | INVALID_PRESENTABLE_TOKEN;
//
//reserved_word_but_insn:
//    reserved_word_but_insn_comma_semicolon_impresentableInvalidToken | COMMA | SEMICOLON;
//
//reserved_word_but_comma_semicolon_impresentableInvalidToken:
//    reserved_word_but_insn_comma_semicolon_impresentableInvalidToken | INSN;
//
//reserved_word:
//    reserved_word_but_insn_comma_semicolon_impresentableInvalidToken | INSN | COMMA | SEMICOLON;

invalid_literal_id:
    PERCENT | COMMA | SEMICOLON | COLON | MINUS | PLUS | MUL | DIV | DOT | EQ | LT | GT | LPAREN | RPAREN
    | LBRACK | RBRACK | LBRACE | RBRACE | DOLLAR | CROSS | NOT | LOGIC_AND | LOGIC_OR | AT | SQUARE | SLASH
    | SDOT | RDOT | SKIP  | PP | STR | TYPEDEF  | REGISTER | PSEUDO | PARTIAL_PSEUDO | WIDTH | REGNUM | SYM_MAP
    | RD_INSN | WR_INSN
    | SYMBOL_VALUE_MAP | INSN | GROUP | CPE1 | CPE2 | CPE3 | CPE4 | MFCPD | MTCPD | MFCPW | MTCPW | MFCPPW
    | MTCPPW | CPLD_BI | CPSD_BI | CPLW_BI | CPSW_BI | LWC_BI | SWC_BI | LDC_BI | SDC_BI | CPLD | CPSD | CPLW
    | CPSW | LWC | SWC | LDC | SDC | CPID | STAGE | EACK | LACK | D2M | D2C | FORMAT | SYNTAX | DECODE
    | PSTAGE | CSTAGE | SSSTAGE | SSCYCLE | SD | SW | SH | SB | UD | UW | UH | UB | SOURCE_OPERAND
    | DESTINATION_OPERAND | INVALID_PRESENTABLE_TOKEN;
invalid_nonrepresentable_literal_id: INVALID_IMPRESENTABLE_TOKEN;
invalid_digit_id: INT;
valid_id: ID;

is_valid_id:
    invalid_literal_id
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier: `%s'", $1);
        YYERROR;
    }
    | invalid_nonrepresentable_literal_id
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier");
        YYERROR;
    }
    | invalid_digit_id
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier: `%d'", $1);
        YYERROR;
    }
    | valid_id;

%%

void yyerror(const char *format, ...) {
    va_list args;
    va_start(args, format);

    SET_PSTAT(aie_error);
    aie_print(aie_parser, GET_PSTAT(), args, format);
}

#if AIE_PARSER_MAIN
int main(void) {
    yyparse();
    return 0;
}
#endif
