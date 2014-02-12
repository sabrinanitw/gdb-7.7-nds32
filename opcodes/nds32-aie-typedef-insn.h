#ifndef _NDS32_AIE_TYPEDEF_INSN_H_
#define _NDS32_AIE_TYPEDEF_INSN_H_

#include "nds32-aie-utils.h"
#include "nds32-aie-pipeline.h"

/* Coprocessor register mapping */
typedef enum _register_mapping {
    physical,
    pseudo,
    partial_pseudo
} register_mapping;

/* Coprocessor instruction group */
/* order is fixed, do not change */
typedef enum _group_ {
    AIE_CPE1,       AIE_CPE2,       AIE_CPE3,       AIE_CPE4,                            //exe
    AIE_MFCPD,      AIE_MTCPD,      AIE_MFCPW,      AIE_MTCPW,   AIE_MFCPPW, AIE_MTCPPW, //mft
    AIE_CPLD,       AIE_CPSD,       AIE_CPLW,       AIE_CPSW,       //rr
    AIE_CPLD_BI,    AIE_CPSD_BI,    AIE_CPLW_BI,    AIE_CPSW_BI,    //rr
    AIE_LWC,        AIE_SWC,        AIE_LDC,        AIE_SDC,        //ri
    AIE_LWC_BI,     AIE_SWC_BI,     AIE_LDC_BI,     AIE_SDC_BI,     //ri
    AIE_UNKNOWN
} group;

/* immediate alignment type of "imm" */
typedef enum _imm_type_ {
    aie_sd, /* signed double word align */
    aie_ud, /* unsigned double word align */
    aie_sw, /* signed word align */
    aie_uw, /* unsigned word align */
    aie_sh, /* signed half word align */
    aie_uh, /* unsigned hald word align */
    aie_sb, /* signed byte align */
    aie_ub, /* unsigned byte align */
} imm_type;

/* register type for register */
typedef enum _aie_reg_type_ {
    aie_d, /* the register is treat as source register */
    aie_s, /* the register is treat as destination register */
} aie_reg_type;

/* Forward declaration */
typedef struct _aie_list_ aie_list;
typedef struct _sym_val_unit_ sym_val_unit;
typedef struct _sym_val_map_unit_ sym_val_map_unit;
typedef struct _aie_ifield_ aie_ifield;
typedef struct _aie_register_ aie_register;
typedef struct _aie_insn_unit_ aie_insn_unit;
typedef struct _aie_insn_unit_ aie_itype_unit;
typedef struct _aie_stoken_ aie_stoken;
typedef struct _stackop_token_ stackop_token;
typedef struct _pseudo_reg_mapping_ pseudo_reg_mapping;
typedef struct _pseudo_reg_idx_ pseudo_reg_idx;

/********************************************************************************************/
/* Global list info of instruction unit which is used by objdump and it's method            */
/* Use different list to chain different co processor group instruction for quick reference */
/********************************************************************************************/
struct _aie_list_ {
    aie_insn_unit* head;
    aie_insn_unit* tail;
    int length;
};
extern aie_list* objd_list;
extern int install_objd_list(void);

/******************************************************************************/
/* Declaration of symbol value unit, "symbol" map to "value" and it's methods */
/******************************************************************************/
struct _sym_val_unit_ {
    char symbol[MAX_STR_CONST];
    unsigned int value;
    sym_val_unit* n;
};
extern sym_val_unit* is_sym_val_exist (sym_val_map_unit* unit, char* symbol);
extern int add_sym_val(sym_val_map_unit* unit, char* symbol, unsigned int value);
extern int add_simple_sym_val_struct(char* id, char* symbol, unsigned int value);

struct _sym_val_map_unit_ {
    char name[MAX_STR_CONST];
    sym_val_unit* sym_val_list_head;
    sym_val_unit* sym_val_list_tail;
    int sym_val_list_length;
    int total_unique_mapped_value_symbol;
    sym_val_map_unit* n;
};
extern sym_val_map_unit* is_sym_val_map_exist (char* symbol);
extern int add_sym_val_map(sym_val_map_unit** it, char* symbol);

/* Global list info of symbol value mapping unit */
extern sym_val_map_unit* sym_val_map_list_head;
extern sym_val_map_unit* sym_val_map_list_tail;
extern int sym_val_map_list_length;

/************************************************************/
/* Some used stuffs for instruction fields and it's methods */
/************************************************************/
struct _aie_ifield_ {
    char symbol[MAX_STR_CONST];     //symbol name
    unsigned int shift;             //shift length
    unsigned int mask;              //field mask
    unsigned int bit_width;         //field width
    unsigned int value;             //encoded/decoded value
    aie_reg_type register_type;     //register source/destination type
    imm_type imm_align_type;        //immediate alignment type
    unsigned int valid;             //indicate the symbol is solved or not when define format
    unsigned int is_mapped;         //indicate the symbol is mapped at translation time (run tim bit)
    char map_symbol[MAX_STR_CONST]; //indicate the mapped symbol (run time string)
    sym_val_map_unit* map_unit;     //map to what kind of symbol value mapping unit if it is "SYMBOL" token
    aie_register* register_unit;    //map to what kind of aie_register unit if it is "SYMBOL" token
    aie_ifield* n;                  //next unit
};
extern aie_ifield* is_ifield_exist(aie_insn_unit* insn, char* symbol);
extern int add_ifield(aie_insn_unit* insn, char* symbol, imm_type sign, aie_reg_type reg_type, int shfmnt, int bit_width, int value, int valid);
extern void free_ifield(aie_ifield* it);

/*************************************************************/
/* Syntax token from evaluation scanner and it's methods     */
/*************************************************************/
struct _aie_stoken_ {
    token_type tok;                 //token type which is return from evaluation scanner
    char c;                         //char for "CHAR" type
    unsigned int value;             //value for "DIGIT" type
    char syntax[MAX_STR_CONST];     //nds32 syntax for SKIP_SYMBOL, PPP
    sym_val_map_unit* map_unit;     //map unit for "SYMBOL" type
    aie_register* register_unit;    //map unit for "SYMBOL" type
    aie_ifield* format_unit;        //ifield for "SYMBOL" type
    aie_register* reg;              //reg for normal register type
    aie_stoken* n;                  //next syntax token
};
extern int find_ifield_for_symbol_stok(char* symbol, aie_insn_unit* insn_tmp, imm_type align_type, aie_reg_type register_type);
extern int add_stoken(aie_insn_unit* it, token_type s, char c, sym_val_map_unit* map_unit, aie_register* reg);
extern void free_stoken(aie_stoken* it);
extern void init_aie_stoken(aie_stoken* it);


/******************************************/
/* Stack operation token and it's methods */
/******************************************/
struct _stackop_token_ {
    stackop_token_type type;
    stackop_token* n;
    int digit;
};
extern void init_stackop_token(stackop_token* it);
extern int add_stackop_token(pseudo_reg_mapping* reg, stackop_token_type type, int stackop_digit);
extern void debug_stackop_token(stackop_token* it, char* buf);

/*****************************************/
/* Pseudo register info and it's methods */
/*****************************************/
struct _pseudo_reg_mapping_ {
    aie_register* physical_reg;
    stackop_token* stok_list;
    unsigned long long mask;
    int shift;
    pseudo_reg_mapping* n;
};
extern void init_pseudo_reg_mapping(pseudo_reg_mapping* pseudo_map);
extern int add_pseudo_reg(aie_register* insn, pseudo_reg_mapping* reg);

/******************************************/
/* Used to display partial pseudo reg idx */
/******************************************/
struct _pseudo_reg_idx_ {
    int idx;
    int mapped_physical_reg_idx;
    pseudo_reg_idx* n;
};
extern int add_pseudo_reg_idx(aie_register* reg, int value);

/****************************************/
/* Hardware resource and it's methods   */
/****************************************/
struct _aie_register_ {
    char symbol[MAX_STR_CONST];                 //name
    char index_symbol[MAX_STR_CONST];           //index name
    unsigned int width;                         //bit width
    unsigned int regnum;                        //register num
	char rd_insn[MAX_STR_CONST];                //insn name to read the register
	char wr_insn[MAX_STR_CONST];                //insn name to write the register
	aie_insn_unit* rd_insn_unit;                //unit pointer to the register read insn.
	aie_insn_unit* wr_insn_unit;                //unit pointer to the register write insn.
    unsigned long long int* tstamp;             //for pipeline modeling
    sym_val_map_unit* map_unit;                 //map unit for "SYMBOL" type
    register_mapping is_pseudo;                 //is pseudo register
    pseudo_reg_mapping* phy_reg_map_list;       //detail mapping for pseudo register
    int num_dependency;                         //physical register number that map to
    int can_be_redef_mapping;
    int can_be_redef_regnum;
    int can_be_redef_width;
    int is_def_mapping;
    int is_def_regnum;
    int is_def_width;
	int is_def_rd_insn;
	int is_def_wr_insn;
    pseudo_reg_idx* index_list_head;            //used to check which register is not mapped;
    pseudo_reg_idx* index_list_tail;            //used to check which register is not mapped;
    aie_register* n;                            //next syntax token
};
extern int add_aie_register(aie_register** it, register_mapping is_pseudo, char* symbol);
extern int add_aie_register_width(aie_register* it, unsigned int value);
extern int add_aie_register_regnum(aie_register* it, int value);
extern int add_aie_register_sym_val_mapping(aie_register* it, char* symbol);
extern int add_aie_register_rd_insn(aie_register* it, char* iname);
extern int add_aie_register_wr_insn(aie_register* it, char* iname);
extern aie_register* is_aie_register_exist (char* symbol);

/* Global list info of register resource */
extern aie_register* aie_register_list_head;
extern aie_register* aie_register_list_tail;
extern int aie_register_list_length;


/****************************************************/
/* Declaration of instruction unit and it's methods */
/****************************************************/
struct _aie_insn_unit_ {
    //instruction type indication
    aie_itype_unit* itype;

    //name insn type or insn operation
    char name[MAX_STR_CONST];

    //insn group
    group insn_group;
    char group_str[9];

    unsigned int aie_ifield_list_length;
    aie_ifield* aie_ifield_list_head;
    aie_ifield* aie_ifield_list_tail;
    unsigned int defined_width[32];

    //coprocessor base value which is a uniq fixed value construct by opcode
    unsigned int base;
    unsigned int base_mask;

    //indicate andes operand is insert
    int is_andes_operand_insert;

    char syntax[MAX_STR_CONST];
    int syntax_symbol_count;
    unsigned int aie_stoken_list_length;
    aie_stoken* aie_stoken_list_head;
    aie_stoken* aie_stoken_list_tail;

    unsigned int is_def_group;
    unsigned int is_def_format;
    unsigned int is_def_decode;
    unsigned int is_def_syntax;

    unsigned int can_be_redef_group;
    unsigned int can_be_redef_format;
    unsigned int can_be_redef_decode;
    unsigned int can_be_redef_syntax;

    //used for pipeline modeling
    aie_pipe* pipe;

    //used to indicate the previous, next instruction unit
    aie_insn_unit* n;
    aie_insn_unit* group_n;
};

/* Global list info of instruction type */
extern aie_itype_unit* aie_itype_list_head;
extern aie_itype_unit* aie_itype_list_tail;
extern int aie_itype_list_length;

/* Global list info of instruction unit */
extern aie_insn_unit* aie_insn_list_head;
extern aie_insn_unit* aie_insn_list_tail;
extern int aie_insn_list_length;

extern aie_itype_unit* is_aie_itype_exist(char* name);
extern int add_aie_itype(aie_itype_unit** it, char* symbol);

//used to check used before typedef of aie insn
extern aie_insn_unit* is_aie_insn_valid(char* name);
//used to check multiple aie insn is created through instruction declaration/or single define directly
extern int is_multiple_aie_insn_create(char* name);

extern int define_aie_insn(aie_insn_unit** insn, char* operation);
extern int add_aie_insn(aie_insn_unit** insn, aie_itype_unit* itype, char* operation);
extern int add_insn_group(aie_insn_unit* insn, group g);
extern int add_insn_decode(aie_insn_unit* insn, char* symbol, unsigned int value);
extern int add_insn_sscycle(aie_insn_unit* insn, int cycle);
extern int add_insn_ssstage(aie_insn_unit* insn, char* stage);
extern int add_insn_pstage(aie_insn_unit* insn, char* stage);
extern int add_insn_cstage(aie_insn_unit* insn, char* stage);
/* Pipeline model related function */
extern int aie_get_stage_num(aie_cop* cop);
extern int aie_get_d2m(aie_cop* cop);
extern int aie_get_d2c(aie_cop* cop);
extern int aie_get_early_trap_ack(aie_cop* cop);
extern int aie_get_late_trap_ack(aie_cop* cop);
extern int aie_get_self_stall_cycle(aie_insn_unit* insn);
extern int aie_get_self_stall_stage(aie_insn_unit* insn);
extern int aie_get_produce_stage(aie_insn_unit* insn);
extern int aie_get_consume_stage(aie_insn_unit* insn);
extern int aie_get_sreg_num(aie_insn_unit* insn);
extern aie_register* aie_get_sreg(aie_insn_unit* insn, int idx);
extern int aie_get_sreg_idx(aie_insn_unit* insn, int idx, int immediate);
extern aie_register* aie_get_ssreg(aie_register* reg, int idx);
extern int aie_get_ssreg_idx(aie_register* reg, int seq, int idx);
extern unsigned long long int aie_get_time_stamp(aie_register* reg, int idx);
extern int aie_get_dreg_num(aie_insn_unit* insn);
extern aie_register* aie_get_dreg(aie_insn_unit* insn, int idx);
extern int aie_get_dreg_idx(aie_insn_unit* insn, int idx, int immediate);
extern aie_register* aie_get_sdreg(aie_register* reg, int idx);
extern int aie_get_sdreg_idx(aie_register* reg, int seq, int idx);
extern int aie_set_time_stamp(aie_register* reg, int idx, unsigned long long int cycle);
extern int aie_set_stage_num(aie_cop* cop, int num);
extern int aie_set_early_trap_ack(aie_cop* cop, int etrap);
extern int aie_set_late_trap_ack(aie_cop* cop, int ltrap);
extern int aie_set_self_stall_cycle(aie_insn_unit* insn, int cycle);
extern int aie_set_self_stall_stage(aie_insn_unit* insn, int stage);
extern int aie_set_produce_stage(aie_insn_unit* insn, int stage);
extern int aie_set_consume_stage(aie_insn_unit* insn, int stage);

extern aie_register* aie_get_sub_reg(aie_register* reg, int idx);
extern int aie_get_sub_reg_idx(aie_register* reg, int seq, int idx);

/* debugging function */
extern void dump_aie_state(void);

/* MISC */
extern void define_andes_gpr(void);
extern void aie_reset_list(void);

/* Global struct of cop pipeline parameter and cpid */
extern aie_cop* cop;

extern int check_syntax_string(aie_insn_unit* insn, char* str);
#endif
