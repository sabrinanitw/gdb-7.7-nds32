/*
Notes:
This file is a merged version of the NDS32 AIE PARSER sources:
	nds32_aie_utils.c
	nds32_aie_typedef_insn.c
    nds32_aie_pipeline.h
	nds32_aie_pipeline.c
	nds32_aie_translator.c
	nds32_aie_translator.h
    nds32_aie_stackop.c
- mliu May.16,2013
*/

#include "nds32-aie.h"
/* nds32_aie_utils.h was renamed to nds32-aie-utils.h */
#include "nds32-aie-utils.h"
/* nds32_aie_typedef_insn.h was renamed to nds32-aie-typedef-insn.h */
#include "nds32-aie-typedef-insn.h"
/* nds32_aie_pipeline.h was renamed to nds32-aie-pipeline.h */
#include "nds32-aie-pipeline.h"



/* nds32_aie_translator.h */
#ifndef _AIE_TRANSLATOR_H_
#define _AIE_TRANSLATOR_H_
extern int match_ifield (aie_insn_unit * list_head, unsigned int imm);

#endif


extern void nds32_aie_eval_restart (FILE * ptr);
extern token_type nds32_aie_eval_lex (void);

/* nds32_aie_utils.c */

/* Global version for Andes Copilot */
int Andes_Copilot_Ver = 1;


/* Global variable for evaluation scanner */
unsigned int eval_uint;
char eval_token[MAX_STR_CONST];
char eval_syntax_line[MAX_SCAN_LINE_SIZE];

inline void print_allow_customized_ifield (aie_insn_unit * insn);

/* Privated aie scanner line info and is used as indication in user defined configuration file */
static unsigned int aie_scanner_line_num = 0;
#define SLINE aie_scanner_line_num
void
inc_aie_scanner_line (void)
{
  SLINE++;
}

/* Privated aie component status info 
 * if parser_state or scanner_state == aie_error --> skip aie feature
 * if translator_state == aie_error --> fatal error, aie_warn --> continue to assemble/disassemble */
static aie_status parser_state = aie_error;
static aie_status scanner_state = aie_error;
static aie_status translator_state = aie_error;
#define PSTAT parser_state
#define SSTAT scanner_state
#define TSTAT translator_state
aie_status
get_pstat (void)
{
  return PSTAT;
}

aie_status
get_sstat (void)
{
  return SSTAT;
}

aie_status
get_tstat (void)
{
  return TSTAT;
}

void
set_pstat (aie_status s)
{
  PSTAT = s;
}

void
set_sstat (aie_status s)
{
  SSTAT = s;
}

void
set_tstat (aie_status s)
{
  TSTAT = s;
}

unsigned int show_warn = 0;
unsigned int show_debug = 0;
unsigned int gen_func = 0;
FILE *fptr = NULL;

void
aie_reset (void)
{
  SLINE = 1;
  PSTAT = aie_normal;
  SSTAT = aie_normal;
}

/* Generic aie print function */
inline void
aie_print (aie_component type, aie_status stat, va_list args,
	   const char *format)
{
  fprintf (stdout, "[AIE ");

  if (aie_scanner == type)
    {
      fprintf (stdout, "SCAN] %d: ", SLINE);
    }
  else if (aie_translator == type)
    {
      fprintf (stdout, "TRANS]: ");
    }
  else
    {
      if (aie_error == stat)
	fprintf (stdout, "PARSE] %d: ", SLINE);
      else
	fprintf (stdout, "PARSE]: ");
    }

  switch (stat)
    {
    case aie_error:
      fprintf (stdout, "Error: ");
      break;
    case aie_warn:
      fprintf (stdout, "Warn: ");
      break;
    case aie_debug:
      fprintf (stdout, "Debug: ");
      break;
    case aie_normal:
      fprintf (stdout, "Info: ");
      break;
    }

  vfprintf (stdout, format, args);
  fprintf (stdout, "\n");
  va_end (args);
}

void
aie_func_print (const char *format, ...)
{
  va_list args;
  va_start (args, format);

  vfprintf (fptr, format, args);
  fprintf (fptr, "\n");
  va_end (args);
}

void
aie_scanner_print (aie_status stat, const char *format, ...)
{
  va_list args;
  va_start (args, format);

  /* Not update if error occur before */
  if (aie_error != SSTAT)
    {
      SSTAT = stat;
    }
  aie_print (aie_scanner, stat, args, format);
}

void
aie_parser_print (aie_status stat, const char *format, ...)
{
  va_list args;
  va_start (args, format);

  /* Not update if error occur before */
  if (aie_error != PSTAT)
    {
      PSTAT = stat;
    }
  aie_print (aie_parser, stat, args, format);
}

void
aie_translator_print (aie_status stat, const char *format, ...)
{
  va_list args;
  va_start (args, format);

  /* Not update if error occur before */
  if (aie_error != TSTAT)
    {
      TSTAT = stat;
    }
  aie_print (aie_translator, stat, args, format);
}

/* Generic aie memory/string function */
int
aie_strcpy (char *dest, char *src)
{
  if (MAX_STR_CONST < (strlen (src) + 1))
    {
      AIE_PPRINT (aie_error, "string is too long, smaller %d is required",
		  MAX_STR_CONST);
      return 0;
    }

  strcpy (dest, src);
  return 1;
}

int
aie_strcmp (char *dest, char *src)
{
  return strcmp (dest, src);
}

void *
aie_malloc (int length)
{
  void *itype = malloc (length);

  if (itype == NULL)
    AIE_PPRINT (aie_error,
		"AIE internal failure due to memory allocation fail");

  return itype;
}

int
aie_strtol (const char *nptr, int base, long int *value, int bitwidth)
{
  errno = 0;

  char *enptr = NULL;

  *value = strtol (nptr, &enptr, base);

  if (errno)
    return 0;

  if (0 != strcmp (enptr, ""))
    return 0;

  int width = bitwidth - 1;
  if (bitwidth && ((*value > ((1 << width) - 1) || *value < (-(1 << width)))))
    return 0;

  return 1;
}

int
aie_strtoul (const char *nptr, int base, unsigned long int *value,
	     int bitwidth)
{
  errno = 0;

  char *enptr = NULL;

  *value = strtoul (nptr, &enptr, base);

  if (errno)
    return 0;

  if (0 != strcmp (enptr, ""))
    return 0;

  if (bitwidth && (*value > (unsigned long int) ((1 << bitwidth) - 1)))
    return 0;

  return 1;
}

int
aie_strtoll (const char *nptr, int base, long long int *value, int bitwidth)
{
  errno = 0;

  char *enptr = NULL;

  *value = strtoll (nptr, &enptr, base);

  if (errno)
    return 0;

  if (0 != strcmp (enptr, ""))
    return 0;

  int width = bitwidth - 1;
  if (bitwidth && ((*value > ((1 << width) - 1) || *value < (-(1 << width)))))
    return 0;

  return 1;
}

int
aie_strtoull (const char *nptr, int base, unsigned long long int *value,
	      int bitwidth)
{
  errno = 0;

  char *enptr = NULL;

  *value = strtoull (nptr, &enptr, base);

  if (errno)
    return 0;

  if (0 != strcmp (enptr, ""))
    return 0;

  if (bitwidth && (*value > (unsigned long long int) ((1 << bitwidth) - 1)))
    return 0;

  return 1;
}

int
aie_endian_convertion (int endian, char *reg_image)
{
  char *sptr = reg_image;
  char tmp1[MAX_STR_CONST << 2] = "\0";
  char tmp2[MAX_STR_CONST << 2] = "\0";

  while (sptr[0] != '\0')
    {
      sprintf (tmp1, "%s", tmp2);

      /* LE */
      if (!endian)
	sprintf (tmp2, "%c%c%s", sptr[0], sptr[1], tmp1);
      else
	sprintf (tmp2, "%s%c%c", tmp1, sptr[0], sptr[1]);

      sptr += 2;
    }

  sprintf (reg_image, "%s", tmp2);
  return 1;
}


/* nds32_aie_pipeline.c */
int
init_aie_cop (aie_cop **this_cop)
{
  if (*this_cop)
    {
      free (*this_cop);
    }

  //new
  (*this_cop) = (aie_cop *) aie_malloc (sizeof (aie_cop));

  //Set default cpid to 1 due to 0 is andes fpu by default
  (*this_cop)->cpid = 1;
  (*this_cop)->cpid_str[0] = 'c';
  (*this_cop)->cpid_str[1] = 'p';
  (*this_cop)->cpid_str[2] = '1';
  (*this_cop)->cpid_str[3] = '\0';

  (*this_cop)->stage_num = 0;
  (*this_cop)->stage_nemonic_list = NULL;
  (*this_cop)->etrap = NULL;
  (*this_cop)->ltrap = NULL;
  (*this_cop)->d2m = NULL;
  (*this_cop)->d2c = NULL;

  (*this_cop)->is_def_cpid = 0;
  (*this_cop)->is_def_d2m = 0;
  (*this_cop)->is_def_d2c = 0;
  (*this_cop)->is_def_etrap = 0;
  (*this_cop)->is_def_ltrap = 0;
  (*this_cop)->is_def_stage = 0;

  (*this_cop)->can_be_redef_cpid = 0;
  (*this_cop)->can_be_redef_d2m = 0;
  (*this_cop)->can_be_redef_d2c = 0;
  (*this_cop)->can_be_redef_etrap = 0;
  (*this_cop)->can_be_redef_ltrap = 0;
  (*this_cop)->can_be_redef_stage = 0;

  return 1;
}

void
init_aie_pipe (aie_pipe ** pipe)
{
  if (!*pipe)
    {
      free (*pipe);
    }

  //new
  (*pipe) = (aie_pipe *) aie_malloc (sizeof (aie_pipe));

  (*pipe)->pstage = NULL;
  (*pipe)->cstage = NULL;
  (*pipe)->sscycle = -1;
  (*pipe)->ssstage = NULL;
  (*pipe)->sreg_num = 0;
  (*pipe)->dreg_num = 0;

  (*pipe)->can_be_redef_pstage = 0;
  (*pipe)->can_be_redef_cstage = 0;
  (*pipe)->can_be_redef_sscycle = 0;
  (*pipe)->can_be_redef_ssstage = 0;

  (*pipe)->is_def_pstage = 0;
  (*pipe)->is_def_cstage = 0;
  (*pipe)->is_def_sscycle = 0;
  (*pipe)->is_def_ssstage = 0;
}

int
is_cop_null (aie_cop *this_cop)
{
  if (NULL == this_cop)
    {
      AIE_PPRINT (aie_error,
		  "AIE internal failure due to NULL of coprocessor pipeline stage pointer");
      AIE_PPRINT (aie_error, "Possibly, stage is not defined!");
      return 1;
    }
  return 0;
}

stage_nemonic *
is_exist_stage (aie_cop *this_cop, char *stage)
{
  stage_nemonic *is_found = NULL;
  if (!this_cop || !this_cop->stage_nemonic_list)
    {
      return is_found;
    }
  else
    {
      is_found = this_cop->stage_nemonic_list;
      while (is_found)
	{
	  if (0 == strcmp (is_found->nemonic, stage))
	    break;
	  is_found = is_found->n;
	}
      return is_found;
    }
}

int
add_cop_cpid (aie_cop * this_cop, unsigned int cpid)
{
  if (is_cop_null (this_cop))
    return 0;
  if (!this_cop->is_def_cpid || this_cop->can_be_redef_cpid)
    {
      if (0 != cpid && 1 != cpid && 2 != cpid && 3 != cpid)
	{
	  AIE_PPRINT (aie_error, "Invalid coprocessor id ,0~3 only");
	  return 0;
	}

      sprintf (this_cop->cpid_str, "cp%d", cpid);
      this_cop->cpid = cpid;
      this_cop->is_def_cpid = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of cpid");
      return 0;
    }
  return 1;
}

int
add_cop_etrap (aie_cop *this_cop, char *stage)
{
  if (is_cop_null (this_cop))
    return 0;

  if (!this_cop->is_def_etrap || this_cop->can_be_redef_etrap)
    {
      stage_nemonic *s = is_exist_stage (this_cop, stage);
      if (!s)
	{
	  AIE_PPRINT (aie_error, "used before define stage `%s'", stage);
	  return 0;
	}

      if (this_cop->is_def_ltrap && this_cop->ltrap->stage >= s->stage)
	{
	  AIE_PPRINT (aie_error, "Only support scenario of etrap > ltrap");
	  return 0;
	}

      this_cop->etrap = s;
      this_cop->is_def_etrap = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of etrap");
      return 0;
    }
  return 1;
}

int
add_cop_ltrap (aie_cop *this_cop, char *stage)
{
  if (is_cop_null (this_cop))
    return 0;

  if (!this_cop->is_def_ltrap || this_cop->can_be_redef_ltrap)
    {
      stage_nemonic *s = is_exist_stage (this_cop, stage);
      if (!s)
	{
	  AIE_PPRINT (aie_error, "used before define stage `%s'", stage);
	  return 0;
	}

      if (this_cop->is_def_etrap && s->stage <= this_cop->etrap->stage)
	{
	  AIE_PPRINT (aie_error, "Only support scenario of etrap > ltrap");
	  return 0;
	}

      this_cop->ltrap = s;
      this_cop->is_def_ltrap = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of ltrap");
      return 0;
    }
  return 1;
}

int
add_cop_d2c (aie_cop *this_cop, char *stage)
{
  if (is_cop_null (this_cop))
    return 0;

  if (!this_cop->is_def_d2c || this_cop->can_be_redef_d2c)
    {
      stage_nemonic *s = is_exist_stage (this_cop, stage);
      if (!s)
	{
	  AIE_PPRINT (aie_error, "used before define stage `%s'", stage);
	  return 0;
	}
      this_cop->d2c = s;
      this_cop->is_def_d2c = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of d2c");
      return 0;
    }
  return 1;
}

int
add_cop_d2m (aie_cop *this_cop, char *stage)
{
  if (is_cop_null (this_cop))
    return 0;

  if (!this_cop->is_def_d2m || this_cop->can_be_redef_d2m)
    {
      stage_nemonic *s = is_exist_stage (this_cop, stage);
      if (!s)
	{
	  AIE_PPRINT (aie_error, "used before define stage `%s'", stage);
	  return 0;
	}
      this_cop->d2m = s;
      this_cop->is_def_d2m = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of d2m");
      return 0;
    }
  return 1;
}

int
add_cop_stage (aie_cop *this_cop, char *stage)
{
  //new and reset
  stage_nemonic *s = (stage_nemonic *) aie_malloc (sizeof (stage_nemonic));
  if (!this_cop->stage_nemonic_list)
    {
      this_cop->stage_nemonic_list = s;
    }
  else
    {
      stage_nemonic *it = this_cop->stage_nemonic_list;
      while (it)
	{
	  if (0 == strcmp (it->nemonic, stage))
	    {
	      AIE_PPRINT (aie_error, "redefinition of stage.%s", stage);
	      return 0;
	    }
	  if (!it->n)
	    {
	      it->n = s;
	      break;
	    }
	  it = it->n;
	}
    }

  if (!aie_strcpy (s->nemonic, stage))
    {
      return 0;
    }
  s->stage = ((++this_cop->stage_num));
  s->n = NULL;
  return 1;
}


/* nds32_aie_typedef_insn.c */

/* Global struct of cop pipeline parameter and cpid */
aie_cop *cop = 0;;

/* Reset global list info of symbol value mapping unit */
sym_val_map_unit *sym_val_map_list_head = NULL;
sym_val_map_unit *sym_val_map_list_tail = NULL;
int sym_val_map_list_length = 0;

/* Global list info of register resource */
aie_register *aie_register_list_head = NULL;
aie_register *aie_register_list_tail = NULL;
int aie_register_list_length = 0;

/* Reset global list info of instruction unit */
aie_insn_unit *aie_insn_list_head = NULL;
aie_insn_unit *aie_insn_list_tail = NULL;
int aie_insn_list_length = 0;

/* Reset global list info of instruction unit */
aie_itype_unit *aie_itype_list_head = NULL;
aie_itype_unit *aie_itype_list_tail = NULL;
int aie_itype_list_length = 0;

/* Global list info of objdump */
aie_list *objd_list = NULL;

/*******************************************/
/* Constructor for different AIE structure */
/*******************************************/
static inline void
init_aie_sym_val_unit (sym_val_unit * it)
{
  it->symbol[0] = '\0';
  it->value = 0;
  it->n = NULL;
}

static inline void
init_aie_sym_val_map_unit (sym_val_map_unit * it)
{
  it->name[0] = '\0';
  it->sym_val_list_head = NULL;
  it->sym_val_list_tail = NULL;
  it->sym_val_list_length = 0;
  it->total_unique_mapped_value_symbol = 0;
  it->n = NULL;
}

static inline void
init_aie_ifield (aie_ifield * it)
{
  it->symbol[0] = '\0';
  it->shift = 0;
  it->mask = 0;
  it->bit_width = 0;
  it->value = 0;
  it->imm_align_type = aie_ub;
  it->register_type = aie_s;
  it->valid = 0;
  it->is_mapped = 0;
  it->map_symbol[0] = '\0';
  it->map_unit = NULL;
  it->register_unit = NULL;
  it->n = NULL;
}

inline void
init_aie_stoken (aie_stoken * it)
{
  it->tok = EOL;
  it->c = ' ';
  it->value = 0;
  it->syntax[0] = '\0';
  it->map_unit = NULL;
  it->register_unit = NULL;
  it->format_unit = NULL;
  it->reg = NULL;
  it->n = NULL;
}

static inline void
init_pseudo_reg_idx (pseudo_reg_idx * it)
{
  it->idx = 0;
  it->mapped_physical_reg_idx = 0;
  it->n = 0;
}

static inline void
init_aie_register (aie_register * it)
{
  it->symbol[0] = '\0';
  it->index_symbol[0] = '\0';
  it->width = 0;
  it->regnum = 0;
  it->rd_insn[0] = '\0';
  it->wr_insn[0] = '\0';
  it->rd_insn_unit = NULL;
  it->wr_insn_unit = NULL;
  it->tstamp = NULL;
  it->is_pseudo = physical;
  it->index_list_head = NULL;
  it->index_list_tail = NULL;
  it->map_unit = NULL;
  it->phy_reg_map_list = NULL;
  it->num_dependency = 0;
  it->can_be_redef_mapping = 0;
  it->can_be_redef_regnum = 0;
  it->can_be_redef_width = 0;
  it->is_def_mapping = 0;
  it->is_def_regnum = 0;
  it->is_def_width = 0;
  it->is_def_rd_insn = 0;
  it->is_def_wr_insn = 0;
  it->n = NULL;
}

static inline void
init_aie_insn_unit (aie_insn_unit * it)
{
  //Instruction type
  it->itype = NULL;

  //Instruction name or type name
  it->name[0] = '\0';

  //Coprocessor instruction group
  it->insn_group = AIE_UNKNOWN;
  it->group_str[0] = 'u';
  it->group_str[1] = 'n';
  it->group_str[2] = 'k';
  it->group_str[3] = 'n';
  it->group_str[3] = 'o';
  it->group_str[3] = 'w';
  it->group_str[3] = 'n';
  it->group_str[4] = '\0';

  //Format list
  it->aie_ifield_list_length = 0;
  it->aie_ifield_list_head = NULL;
  it->aie_ifield_list_tail = NULL;
  it->defined_width[0] = 0;
  it->defined_width[10] = 0;
  it->defined_width[20] = 0;
  it->defined_width[30] = 0;
  it->defined_width[1] = 0;
  it->defined_width[11] = 0;
  it->defined_width[21] = 0;
  it->defined_width[31] = 0;
  it->defined_width[2] = 0;
  it->defined_width[12] = 0;
  it->defined_width[22] = 0;
  it->defined_width[3] = 0;
  it->defined_width[13] = 0;
  it->defined_width[23] = 0;
  it->defined_width[4] = 0;
  it->defined_width[14] = 0;
  it->defined_width[24] = 0;
  it->defined_width[5] = 0;
  it->defined_width[15] = 0;
  it->defined_width[25] = 0;
  it->defined_width[6] = 0;
  it->defined_width[16] = 0;
  it->defined_width[26] = 0;
  it->defined_width[7] = 0;
  it->defined_width[17] = 0;
  it->defined_width[27] = 0;
  it->defined_width[8] = 0;
  it->defined_width[18] = 0;
  it->defined_width[28] = 0;
  it->defined_width[9] = 0;
  it->defined_width[19] = 0;
  it->defined_width[29] = 0;

  it->base = 0xffffffff;
  it->base_mask = 0xffffffff;

  it->is_andes_operand_insert = -1;

  it->syntax[0] = '\0';
  it->syntax_symbol_count = 0;
  //Syntax list
  it->aie_stoken_list_length = 0;
  it->aie_stoken_list_head = NULL;
  it->aie_stoken_list_tail = NULL;

  //Already define configuration
  it->is_def_group = 0;
  it->is_def_format = 0;
  it->is_def_decode = 0;
  it->is_def_syntax = 0;

  //Re-define configuration
  it->can_be_redef_group = 0;
  it->can_be_redef_format = 0;
  it->can_be_redef_decode = 0;
  it->can_be_redef_syntax = 0;

  //Pipeline modeling
  init_aie_pipe (&(it->pipe));

  it->n = NULL;
  it->group_n = NULL;
}

int
add_aie_register_rd_insn (aie_register * it, char *iname)
{
  if (NULL == it)
    {
      AIE_PPRINT (aie_error, "Undefined reference to %d", it);
      return 0;
    }

  aie_strcpy (it->rd_insn, iname);
  it->is_def_rd_insn = 1;
  return 1;
}

int
add_aie_register_wr_insn (aie_register * it, char *iname)
{
  if (NULL == it)
    {
      AIE_PPRINT (aie_error, "Undefined reference to %d", it);
      return 0;
    }

  aie_strcpy (it->wr_insn, iname);
  it->is_def_wr_insn = 1;
  return 1;
}
 
void
init_pseudo_reg_mapping (pseudo_reg_mapping * pseudo_map)
{
  pseudo_map->physical_reg = NULL;
  pseudo_map->stok_list = NULL;
  pseudo_map->mask = 0LL;
  pseudo_map->shift = 0;
  pseudo_map->n = NULL;
}

/**************************/
/* Some privited function */
/**************************/

/* Stack evaluation of arithmetic expression */

/* expanse nds32_aie_stackop.c here! */
/* nds32_aie_stackop.c" */
static int stackop_val_idx = 0;
static int stackop_op_idx = 0;
static stackop_token stackop_val[MAX_STR_CONST];
static stackop_token stackop_op[MAX_STR_CONST];

static inline void
reset_stack (void)
{
  stackop_val_idx = 0;
  stackop_op_idx = 0;
}

static inline void
pushV (int digit)
{
  stackop_val[stackop_val_idx].digit = digit;
  stackop_val[stackop_val_idx].type = STACKOP_DIGIT;
  stackop_val_idx++;
}

static inline void
pushO (stackop_token_type type)
{
  stackop_op[stackop_op_idx].type = type;
  stackop_op_idx++;
}

static inline int
popV (void)
{
  stackop_val_idx--;
  return stackop_val[stackop_val_idx].digit;
}

static inline stackop_token_type
popO (void)
{
  stackop_op_idx--;
  return stackop_op[stackop_op_idx].type;
}

static inline stackop_token_type
topO (void)
{
  return stackop_op[stackop_op_idx - 1].type;
}

static inline stackop_token_type
topm1O (void)
{
  return stackop_op[stackop_op_idx - 2].type;
}

static inline int
getidx (stackop_token * list, int idx)
{

  reset_stack ();
  stackop_token *it = list;
  stackop_token_type type = it ? it->type : STACKOP_STACKEND;

  int return_idx = 0;

  while (it || stackop_op_idx)
    {

      switch (type)
	{
	case STACKOP_IDX:
	  pushV (idx);
	  break;
	case STACKOP_DIGIT:
	  pushV (it->digit);
	  break;

	  /* always push since maximum precedence */
	case STACKOP_LPAREN:
	  pushO (STACKOP_LPAREN);
	  break;

	case STACKOP_DIV:
	case STACKOP_MUL:
	  if (STACKOP_MUL == topO ())
	    {
	      return_idx =
		stackop_val[stackop_val_idx - 2].digit * stackop_val[stackop_val_idx - 1].digit;
	      stackop_val_idx -= 2;
	      stackop_op_idx -= 1;
	      pushV (return_idx);
	    }
	  else if (STACKOP_DIV == topO ())
	    {
	      return_idx =
		stackop_val[stackop_val_idx - 2].digit / stackop_val[stackop_val_idx - 1].digit;
	      stackop_val_idx -= 2;
	      stackop_op_idx -= 1;
	      pushV (return_idx);
	    }

	  /* pushback */
	  pushO (type);
	  break;

	case STACKOP_POSTIVE:
	case STACKOP_NEGATIVE:
	case STACKOP_RPAREN:
	case STACKOP_STACKEND:
	  if (STACKOP_POSTIVE == topO ())
	    {
	      return_idx =
		stackop_val[stackop_val_idx - 2].digit + stackop_val[stackop_val_idx - 1].digit;
	      stackop_val_idx -= 2;
	      stackop_op_idx -= 1;
	      pushV (return_idx);
	    }
	  else if (STACKOP_NEGATIVE == topO ())
	    {
	      return_idx =
		stackop_val[stackop_val_idx - 2].digit - stackop_val[stackop_val_idx - 1].digit;
	      stackop_val_idx -= 2;
	      stackop_op_idx -= 1;
	      pushV (return_idx);
	    }
	  else if (STACKOP_MUL == topO ())
	    {
	      return_idx =
		stackop_val[stackop_val_idx - 2].digit * stackop_val[stackop_val_idx - 1].digit;
	      stackop_val_idx -= 2;
	      stackop_op_idx -= 1;
	      pushV (return_idx);
	    }
	  else if (STACKOP_DIV == topO ())
	    {
	      return_idx =
		stackop_val[stackop_val_idx - 2].digit / stackop_val[stackop_val_idx - 1].digit;
	      stackop_val_idx -= 2;
	      stackop_op_idx -= 1;
	      pushV (return_idx);
	    }
	  else if (STACKOP_RPAREN == topO ())
	    {
	      type = popO ();
	      continue;
	    }

	  /* pushback */
	  if (STACKOP_STACKEND != type)
	    pushO (type);

	  if (STACKOP_RPAREN == topO () && STACKOP_LPAREN == topm1O ())
	    {
	      popO ();
	      popO ();
	    }
	  break;
	}

      if (it && it->n)
	{
	  it = it->n;
	  type = it->type;
	}
      else
	{
	  it = 0;
	  type = STACKOP_STACKEND;
	}
      DPPRINT ("%d", return_idx);
    }

  return return_idx;
}

/* end of nds32_aie_stackop.c */


/* second half of nds32_aie_typedef_insn.c */
static inline int
aie_ifield_2_aie_ifield (aie_ifield * it, aie_ifield * to_be_copy)
{
  if (!aie_strcpy (it->symbol, to_be_copy->symbol))
    {
      return 0;
    }
  it->shift = to_be_copy->shift;
  it->mask = to_be_copy->mask;
  it->bit_width = to_be_copy->bit_width;
  it->value = to_be_copy->value;
  it->imm_align_type = to_be_copy->imm_align_type;
  it->register_type = to_be_copy->register_type;
  it->valid = to_be_copy->valid;
  it->is_mapped = to_be_copy->is_mapped;
  if (!aie_strcpy (it->map_symbol, to_be_copy->map_symbol))
    {
      return 0;
    }
  it->n = to_be_copy->n;

  return 1;
}

static inline int
write_mask_determinate (unsigned int *mask, int value)
{
  if (0 >= value || value >= 20)
    {
      AIE_PPRINT (aie_error,
		  "AIE internal failure due to %d write mask value", value);
      return 0;
    }
  *mask = ((1 << value) - 1);
  return 1;
}

static inline int
is_itype_null (aie_itype_unit * itype)
{
  if (NULL == itype)
    {
      AIE_PPRINT (aie_error,
		  "AIE internal failure due to NULL of instruction type pointer");
      return 1;
    }
  return 0;
}

static inline char *
insn_group2str (group g)
{
  switch (g)
    {
    case AIE_CPE1:
      return "cpe1";
    case AIE_CPE2:
      return "cpe2";
    case AIE_CPE3:
      return "cpe3";
    case AIE_CPE4:
      return "cpe4";
    case AIE_MFCPD:
      return "mfcpd";
    case AIE_MTCPD:
      return "mtcpd";
    case AIE_MFCPW:
      return "mfcpw";
    case AIE_MTCPW:
      return "mtcpw";
    case AIE_MFCPPW:
      return "mfcppw";
    case AIE_MTCPPW:
      return "mtcppw";

    case AIE_CPSD:
      return "cpsd";
    case AIE_CPSW:
      return "cpsw";
    case AIE_CPLD:
      return "cpld";
    case AIE_CPLW:
      return "cplw";
    case AIE_SDC:
      return "cpsdi";
    case AIE_SWC:
      return "cpswi";
    case AIE_LDC:
      return "cpldi";
    case AIE_LWC:
      return "cplwi";

    case AIE_CPSD_BI:
      return "cpsd.bi";
    case AIE_CPSW_BI:
      return "cpsw.bi";
    case AIE_CPLD_BI:
      return "cpld.bi";
    case AIE_CPLW_BI:
      return "cplw.bi";
    case AIE_SDC_BI:
      return "cpsdi.bi";
    case AIE_SWC_BI:
      return "cpswi.bi";
    case AIE_LDC_BI:
      return "cpldi.bi";
    case AIE_LWC_BI:
      return "cplwi.bi";

    default:
      return "unknown";
    }
}

static inline int
is_insn_null (aie_insn_unit * insn)
{
  if (NULL == insn)
    {
      AIE_PPRINT (aie_error,
		  "AIE internal failure due to NULL of instruction pointer");
      return 1;
    }
  return 0;
}

static inline int
chain_classify (aie_insn_unit ** sub_head, aie_insn_unit ** sub_tail,
		group g)
{
  //set up global head
  aie_insn_unit *it = aie_insn_list_head;

  while (NULL != it)
    {
      if (it->insn_group == g)
	{

	  if (NULL == (*sub_head))
	    {
	      (*sub_head) = it;
	    }
	  else
	    {
	      (*sub_tail)->group_n = it;
	    }

	  (*sub_tail) = it;
	  it->group_n = NULL;
	}
      it = it->n;
    }

  return 1;
}

/************************/
/* Some public function */
/************************/
inline aie_itype_unit *
is_aie_itype_exist (char *name)
{
  aie_itype_unit *it = aie_itype_list_head;
  while (NULL != it)
    {
      if (0 == strcmp (it->name, name))
	{
	  break;
	}
      it = it->n;
    }
  return it;
}

inline sym_val_map_unit *
is_sym_val_map_exist (char *symbol)
{
  sym_val_map_unit *it = sym_val_map_list_head;

  while (NULL != it)
    {
      if (0 == strcmp (it->name, symbol))
	{
	  break;
	}
      it = it->n;
    }

  return it;
}

inline sym_val_unit *
is_sym_val_exist (sym_val_map_unit * unit, char *symbol)
{
  sym_val_unit *it = unit->sym_val_list_head;

  while (NULL != it)
    {
      if (0 == strcmp (it->symbol, symbol))
	{
	  break;
	}
      it = it->n;
    }

  return it;
}

inline aie_register *
is_aie_register_exist (char *symbol)
{
  aie_register *it = aie_register_list_head;

  while (NULL != it)
    {
      if (0 == strcmp (it->symbol, symbol))
	{
	  break;
	}
      it = it->n;
    }

  return it;
}

int
is_multiple_aie_insn_create (char *name)
{
  aie_insn_unit *it = aie_insn_list_head;
  while (NULL != it)
    {
      if (0 == strcmp (it->name, name))
	{
	  return 1;
	}
      it = it->n;
    }
  return 0;
}

aie_insn_unit *
is_aie_insn_valid (char *name)
{
  aie_insn_unit *it = aie_insn_list_head;
  while (NULL != it)
    {
      if (0 == strcmp (it->name, name) && NULL != it->itype)
	{
	  return it;
	}
      it = it->n;
    }
  return NULL;
}

//This is used to conver aie syntax: 
//insn id {
//  group = ..;
//  format = ..;
//  syntax = ..;
//  decode = ..;
//};
int
define_aie_insn (aie_insn_unit ** insn, char *operation)
{

  *insn = (aie_insn_unit *) aie_malloc (sizeof (aie_insn_unit));

  if (NULL == *insn)
    return 0;

  //Reset
  init_aie_insn_unit (*insn);

  if (!aie_strcpy ((*insn)->name, operation))
    return 0;

  (*insn)->itype = 0;

  (*insn)->can_be_redef_group = 0;
  (*insn)->can_be_redef_format = 0;
  (*insn)->can_be_redef_decode = 0;
  (*insn)->can_be_redef_syntax = 0;
  (*insn)->pipe->can_be_redef_pstage = 1;
  (*insn)->pipe->can_be_redef_cstage = 1;
  (*insn)->pipe->can_be_redef_ssstage = 1;
  (*insn)->pipe->can_be_redef_sscycle = 1;

  if (NULL == aie_insn_list_head)
    {
      aie_insn_list_head = (*insn);
    }
  else
    {
      aie_insn_list_tail->n = (*insn);
    }
  (*insn)->n = NULL;
  aie_insn_list_tail = (*insn);

  return 1;
}

int
add_aie_insn (aie_insn_unit ** insn, aie_itype_unit * itype, char *operation)
{

  *insn = (aie_insn_unit *) aie_malloc (sizeof (aie_insn_unit));

  if (NULL == *insn)
    return 0;

  //Reset
  init_aie_insn_unit (*insn);

  if (!aie_strcpy ((*insn)->name, operation))
    return 0;

  if (is_itype_null (itype))
    return 0;

  //Update instruction type
  (*insn)->itype = itype;

  //Overwrite some default resets
  (*insn)->is_def_group = itype->is_def_group;
  (*insn)->is_def_format = itype->is_def_format;
  (*insn)->is_def_decode = 0;
  (*insn)->is_def_syntax = 0;
  (*insn)->pipe->is_def_pstage = itype->pipe->is_def_pstage;
  (*insn)->pipe->is_def_cstage = itype->pipe->is_def_cstage;
  (*insn)->pipe->is_def_ssstage = itype->pipe->is_def_ssstage;
  (*insn)->pipe->is_def_sscycle = itype->pipe->is_def_sscycle;

  (*insn)->can_be_redef_group = 1;
  (*insn)->can_be_redef_format = 1;
  (*insn)->can_be_redef_decode = 0;
  (*insn)->can_be_redef_syntax = 0;

  (*insn)->pipe->can_be_redef_pstage = 1;
  (*insn)->pipe->can_be_redef_cstage = 1;
  (*insn)->pipe->can_be_redef_ssstage = 1;
  (*insn)->pipe->can_be_redef_sscycle = 1;

  (*insn)->is_andes_operand_insert = itype->is_andes_operand_insert;

  //Preset
  if ((*insn)->is_def_group)
    {
      (*insn)->insn_group = itype->insn_group;

      if (!aie_strcpy
	  ((*insn)->group_str, insn_group2str (itype->insn_group)))
	{
	  return 0;
	}
    }

  if ((*insn)->is_def_format)
    {
      aie_ifield *to_be_copy = itype->aie_ifield_list_head;

      while (NULL != to_be_copy)
	{

	  aie_ifield *it = (aie_ifield *) aie_malloc (sizeof (aie_ifield));

	  if (!aie_ifield_2_aie_ifield (it, to_be_copy))
	    {
	      return 0;
	    }

	  if (NULL == (*insn)->aie_ifield_list_head)
	    {
	      (*insn)->aie_ifield_list_head = it;
	    }
	  else
	    {
	      (*insn)->aie_ifield_list_tail->n = it;
	    }
	  (*insn)->aie_ifield_list_tail = it;

	  to_be_copy = to_be_copy->n;
	}

      (*insn)->aie_ifield_list_length = itype->aie_ifield_list_length;
    }

  //for pipeline modeling
  if ((*insn)->pipe->is_def_pstage)
    {
      (*insn)->pipe->pstage = itype->pipe->pstage;
    }

  if ((*insn)->pipe->is_def_cstage)
    {
      (*insn)->pipe->cstage = itype->pipe->cstage;
    }

  if ((*insn)->pipe->is_def_ssstage)
    {
      (*insn)->pipe->ssstage = itype->pipe->ssstage;
    }

  if ((*insn)->pipe->is_def_sscycle)
    {
      (*insn)->pipe->sscycle = itype->pipe->sscycle;
    }

  if (NULL == aie_insn_list_head)
    {
      aie_insn_list_head = (*insn);
    }
  else
    {
      aie_insn_list_tail->n = (*insn);
    }
  (*insn)->n = NULL;
  aie_insn_list_tail = (*insn);

  return 1;
}

int
add_insn_group (aie_insn_unit * insn, group g)
{
  if (is_insn_null (insn))
    return 0;

  if (!insn->is_def_group || insn->can_be_redef_group)
    {
      insn->insn_group = g;

      if (!aie_strcpy (insn->group_str, insn_group2str (g)))
	{
	  return 0;
	}

      insn->is_def_group = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of `%s.group'", insn->name);
      return 0;
    }
  return 1;
}

int
add_insn_pstage (aie_insn_unit * insn, char *stage)
{
  if (is_insn_null (insn) || is_cop_null (cop))
    return 0;

  if (!insn->pipe->is_def_pstage || insn->pipe->can_be_redef_pstage)
    {
      stage_nemonic *s = is_exist_stage (cop, stage);
      if (!s)
	{
	  AIE_PPRINT (aie_error, "used before define stage `%s'", stage);
	  return 0;
	}

      if (insn->pipe->is_def_cstage && insn->pipe->cstage->stage >= s->stage)
	{
	  AIE_PPRINT (aie_error,
		      "Only support scenario of %s.pstage > %s.cstage",
		      insn->name, insn->name);
	  return 0;
	}

      insn->pipe->pstage = s;
      insn->pipe->is_def_pstage = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of %s.pstage", insn->name);
      return 0;
    }
  return 1;
}

int
add_insn_cstage (aie_insn_unit * insn, char *stage)
{
  if (is_insn_null (insn))
    return 0;
  if (!insn->pipe->is_def_cstage || insn->pipe->can_be_redef_cstage)
    {
      stage_nemonic *s = is_exist_stage (cop, stage);
      if (!s)
	{
	  AIE_PPRINT (aie_error, "used before define stage `%s'", stage);
	  return 0;
	}

      if (insn->pipe->is_def_pstage && insn->pipe->pstage->stage <= s->stage)
	{
	  AIE_PPRINT (aie_error,
		      "Only support scenario of %s.pstage > %s.cstage",
		      insn->name, insn->name);
	  return 0;
	}

      insn->pipe->cstage = s;
      insn->pipe->is_def_cstage = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of %s.cstage", insn->name);
      return 0;
    }
  return 1;
}

int
add_insn_ssstage (aie_insn_unit * insn, char *stage)
{
  if (is_insn_null (insn))
    return 0;
  if (!insn->pipe->is_def_ssstage || insn->pipe->can_be_redef_ssstage)
    {
      stage_nemonic *s = is_exist_stage (cop, stage);
      if (!s)
	{
	  AIE_PPRINT (aie_error, "used before define stage `%s'", stage);
	  return 0;
	}

      insn->pipe->ssstage = s;
      insn->pipe->is_def_ssstage = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of %s.ssstage", insn->name);
      return 0;
    }
  return 1;
}

int
add_insn_sscycle (aie_insn_unit * insn, int sscycle)
{
  if (is_insn_null (insn))
    return 0;
  if (!insn->pipe->is_def_sscycle || insn->pipe->can_be_redef_sscycle)
    {
      if (!aie_set_self_stall_cycle (insn, sscycle))
	{
	  AIE_PPRINT (aie_error, "Invalid cycle `%d', > 0 is required",
		      sscycle);
	  return 0;
	}

      if (0 != sscycle && -1 == aie_get_self_stall_stage (insn))
	{
	  AIE_PPRINT (aie_error,
		      "Specify self-stall cycle `%lld' of non-blocking instruction `%s'",
		      insn->pipe->sscycle, insn->name);
	  AIE_PPRINT (aie_error,
		      "Either specifying %s.ssstage first or reseting %s.sscycle to 0",
		      insn->name, insn->name);
	  return 0;
	}

      insn->pipe->is_def_sscycle = 1;
    }
  else
    {
      AIE_PPRINT (aie_error, "redefinition of %s.sscycle", insn->name);
      return 0;
    }
  return 1;
}

inline void
print_allow_customized_ifield (aie_insn_unit * insn)
{

  AIE_PPRINT (aie_error,
	      "+-----------------------------------------------------------------------+");
  AIE_PPRINT (aie_error,
	      "|31    28|27    24|23    20|19    16|15    12|11    08| 7     4| 3     0|");
  AIE_PPRINT (aie_error,
	      "+-----------------------------------------------------------------------+");

  switch (insn->insn_group)
    {
    case AIE_CPE1:
    case AIE_CPE2:
    case AIE_CPE3:
    case AIE_CPE4:
      AIE_PPRINT (aie_error,
		  "                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
      AIE_PPRINT (aie_error,
		  "                24                                        6");
      break;
    case AIE_MFCPD:
    case AIE_MTCPD:
    case AIE_MFCPW:
    case AIE_MTCPW:
    case AIE_MFCPPW:
    case AIE_MTCPPW:
      AIE_PPRINT (aie_error,
		  "                            ^^^^^^^^^^^^^^^^^^^^^^^^^^");
      AIE_PPRINT (aie_error,
		  "                            19                      08");
      break;
    default:
      AIE_PPRINT (aie_error, "                ^^^^^^^^^^^");
      AIE_PPRINT (aie_error, "                24       20");
      break;
    }
  AIE_PPRINT (aie_error,
	      "^: allowed customized coprocessor instruction bit field");
  AIE_PPRINT (aie_error, "");
}

int ifield_check (aie_insn_unit * insn, char *symbol, int shfmnt, int width);

//use to check valid ified such as: (rt:5:20, ra:5:15,...)
int
ifield_check (aie_insn_unit * insn, char *symbol, int shfmnt, int width)
{
  unsigned int tmp = 0;
  int tmp_field[32];
  int i = -1;
  int j = -1;
  int idx = 0;

  //Add write mask
  if (!write_mask_determinate (&tmp, width))
    return 0;

  tmp <<= shfmnt;
  for (idx = 0; idx <= 31; idx++)
    {
      if (1 == ((tmp >> idx) & 0x1))
	tmp_field[idx] = 1;
      else
	tmp_field[idx] = 0;
    }

  switch (insn->insn_group)
    {
    case AIE_CPE1:
    case AIE_CPE2:
    case AIE_CPE3:
    case AIE_CPE4:
      i = 24;
      j = 6;
      break;
    case AIE_MFCPD:
    case AIE_MTCPD:
    case AIE_MFCPW:
    case AIE_MTCPW:
    case AIE_MFCPPW:
    case AIE_MTCPPW:
      i = 19;
      j = 8;
      break;
    default:
      i = 24;
      j = 20;
      break;
    }

  for (idx = 31; idx >= 0; idx--)
    {
      int field = insn->defined_width[idx];
      int incoming_field = tmp_field[idx];

      if (((idx > i) || (idx < j)) && 1 == incoming_field)
	{
	  print_allow_customized_ifield (insn);
	  AIE_PPRINT (aie_error, "bit[%d] in %s is not for customized", idx,
		      symbol);
	  return 0;
	}
      else if (idx >= j && idx <= i)
	{
	  if (1 == field && 1 == incoming_field)
	    {
	      insn->defined_width[idx] = 0;
	    }
	  else if (0 == field && 1 == incoming_field)
	    {
	      print_allow_customized_ifield (insn);
	      AIE_PPRINT (aie_error, "redefinition of bit[%d] in %s", idx,
			  symbol);
	      return 0;
	    }
	}
    }
  return 1;
}

int
add_ifield (aie_insn_unit * insn, char *symbol, imm_type type,
	    aie_reg_type reg_type, int shfmnt, int bit_width, int value,
	    int valid)
{
  //Check whether the format is exist or not
  if (NULL != symbol && NULL != is_ifield_exist (insn, symbol))
    {
      AIE_PPRINT (aie_error, "redefinition of %s.format.%s", insn->name,
		  symbol);
      return 0;
    }

  //Check validation of field position & width
  if (0 > shfmnt)
    {
      AIE_PPRINT (aie_error, "invalid fields position:%d (>=0 is required)",
		  shfmnt);
      return 0;
    }

  if (0 >= bit_width)
    {
      AIE_PPRINT (aie_error, "invalid fields width:%d (>0 is required)",
		  bit_width);
      return 0;
    }

  if (!ifield_check (insn, symbol, shfmnt, bit_width))
    return 0;

  //New format
  aie_ifield *format = (aie_ifield *) aie_malloc (sizeof (aie_ifield));
  if (NULL == format)
    return 0;

  //Initiate to default value
  init_aie_ifield (format);

  //Add some settings
  format->shift = shfmnt;
  format->bit_width = bit_width;
  format->valid = valid;
  format->value = value;

  //Add write mask
  if (!write_mask_determinate (&(format->mask), format->bit_width))
    return 0;

  //Add symbol
  if (NULL != symbol && !aie_strcpy (format->symbol, symbol))
    return 0;

  //Add sign
  format->imm_align_type = type;

  //Add register type
  format->register_type = reg_type;

  //Add to chain
  if (NULL == insn->aie_ifield_list_head)
    insn->aie_ifield_list_head = format;
  else
    insn->aie_ifield_list_tail->n = format;
  insn->aie_ifield_list_tail = format;
  insn->aie_ifield_list_length++;

  return 1;
}

int
add_insn_decode (aie_insn_unit * insn, char *symbol, unsigned int value)
{
  if (is_insn_null (insn))
    return 0;

  aie_ifield *it = is_ifield_exist (insn, symbol);

  if (NULL == it)
    {
      AIE_PPRINT (aie_error,
		  "try to resolve `%s' before define `%s.format.%s'", symbol,
		  insn->name, symbol);
      return 0;
    }

  if (!it->valid && it->map_unit)
    {
      AIE_PPRINT (aie_error,
		  "try to decode %s.fotmat.%s since it has been treat as field of operand `%s%s'",
		  insn->name, symbol, "%", it->register_unit->symbol);
      AIE_PPRINT (aie_error, "does `%s' really need to be decode?", symbol);
      return 0;
    }

  it->valid = 1;
  it->value = (value & it->mask);
  return 1;
}

aie_ifield *
is_ifield_exist (aie_insn_unit * insn, char *symbol)
{
  aie_ifield *it = insn->aie_ifield_list_head;

  while (NULL != it)
    {
      if (0 == strcmp (it->symbol, symbol))
	{
	  return it;
	}
      it = it->n;
    }
  return NULL;
}

int
find_ifield_for_symbol_stok (char *symbol, aie_insn_unit * insn_tmp,
			     imm_type align_type, aie_reg_type register_type)
{

  aie_ifield *it = is_ifield_exist (insn_tmp, symbol);
  if (NULL == it)
    {
      AIE_PPRINT (aie_error, "undefined reference to %s.format.%s",
		  insn_tmp->name, symbol);
      return 0;
    }

  aie_stoken *tok = insn_tmp->aie_stoken_list_head;
  if (NULL == tok)
    {
      AIE_PPRINT (aie_error, "syntax error due to unmapped of %s", symbol);
      return 0;
    }

  while (NULL != tok)
    {
      //Sequential add, no id identification mechanism!!
      if (SYMBOL == tok->tok && NULL == tok->format_unit)
	{
	  tok->format_unit = it;

	  //reverse mapping for objdump
	  tok->format_unit->map_unit = tok->map_unit;

	  tok->format_unit->register_unit = tok->register_unit;

	  //complain if field length is smaller than associated register operand
	  //example:
	  // symbol_value_map test {
	  //     "$c1r"[0..31]=[0..31];
	  // };
	  // register c1reg {
	  //     regnum = 32;
	  //     mapping = test;
	  // };
	  // 
	  // typedef insn cpe {
	  //     group = CPE1;
	  //     format = (ra:10:15,rb:5:10,op:4:6);
	  // };
	  // 
	  // cpe aes128e,aes192e, aes256e.syntax = ("aes256e %c1reg,%c1reg,%c1reg", ra,rb,op);
	  // 
	  //
	  // ==> assembly:
	  //         aes256e $c1r0,$c1r0,$c1r31
	  // 
	  // ==> disassembly will become:
	  //         aes256e $c1r0,$c1r0,$c1r"15"
	  //
	  // since we have 32 symbols but only have 4 bit for "op" encode/decode

	  if ((unsigned int) (1 << tok->format_unit->bit_width) <
	      tok->format_unit->register_unit->regnum)
	    {
	      AIE_PPRINT (aie_error,
			  "customized operand, `%s%s' may map to %d symbols,",
			  "%", tok->format_unit->register_unit->symbol,
			  tok->format_unit->register_unit->regnum);
	      AIE_PPRINT (aie_error,
			  "however, `%s.format.%s' only have %d bits for encode/decode",
			  insn_tmp->name, tok->format_unit->symbol,
			  tok->format_unit->bit_width);
	      return 0;
	    }
	  break;
	}
      tok = tok->n;
    }

  insn_tmp->syntax_symbol_count--;

  //support overwriting alignment/reigster type attribute
  it->register_type = register_type;
  it->imm_align_type = align_type;

  //find out which register_type is invalid (such as it is not a register),
  //for immediate/const field tok->reg is NULL, for user defined reg/nds reg it should be non-NULL value
  //and then we know the source/destination operand number
  if (aie_s == it->register_type && NULL != tok->reg)
    {
      insn_tmp->pipe->sreg_num++;
    }

  if (aie_d == it->register_type && NULL != tok->reg)
    {
      insn_tmp->pipe->dreg_num++;
    }

  return 1;
}

int
add_sym_val_map (sym_val_map_unit ** it, char *symbol)
{
  //Check previous definitions
  if (NULL != is_sym_val_map_exist (symbol))
    {
      AIE_PPRINT (aie_error, "redefnition of %s", symbol);
      return 0;
    }

  (*it) = (sym_val_map_unit *) aie_malloc (sizeof (sym_val_map_unit));
  if (NULL == (*it))
    {
      return 0;
    }

  //apply reset values
  init_aie_sym_val_map_unit (*it);

  //Chain to global map unit list
  if (NULL == sym_val_map_list_head)
    {
      sym_val_map_list_head = (*it);
    }
  else
    {
      sym_val_map_list_tail->n = (*it);
    }
  sym_val_map_list_tail = (*it);
  sym_val_map_list_length++;

  //Copy id
  if (!aie_strcpy ((*it)->name, symbol))
    return 0;

  return 1;
}

int
add_sym_val (sym_val_map_unit * unit, char *symbol, unsigned int value)
{
  //Check previous definitions
  if (NULL != is_sym_val_exist (unit, symbol))
    {
      AIE_PPRINT (aie_error, "redefinition of %s.%s", unit->name, symbol);
      return 0;
    }

  //Also check the symbol-value pair is already defined in other symbol value map struct
  sym_val_map_unit *it2 = sym_val_map_list_head;
  while (NULL != it2)
    {
      if (NULL != is_sym_val_exist (it2, symbol))
	{
	  AIE_PPRINT (aie_error, "definition conflict to %s.%s", it2->name,
		      symbol);
	  return 0;
	}
      it2 = it2->n;
    }

  //update the "total_unique_mapped_value_symbol" if it is unique mapped value symbol 
  sym_val_unit *tmp = unit->sym_val_list_head;
  int is_unique = 1;
  while (tmp)
    {
      if (value == tmp->value)
	{
	  is_unique = 0;
	  break;
	}

      tmp = tmp->n;
    }

  if (is_unique)
    {
      unit->total_unique_mapped_value_symbol++;
    }

  sym_val_unit *it = (sym_val_unit *) aie_malloc (sizeof (sym_val_unit));
  if (NULL == it)
    {
      return 0;
    }

  //apply reset values
  init_aie_sym_val_unit (it);

  //Chain to unit list
  if (NULL == unit->sym_val_list_head)
    {
      unit->sym_val_list_head = it;
    }
  else
    {
      unit->sym_val_list_tail->n = it;
    }
  unit->sym_val_list_tail = it;

  //Copy id
  if (!aie_strcpy (it->symbol, symbol))
    return 0;

  //Copy symbol value
  it->value = value;

  return 1;
}

int
add_aie_itype (aie_itype_unit ** it, char *symbol)
{
  if (NULL != is_aie_itype_exist (symbol))
    {
      AIE_PPRINT (aie_error, "redefinition of %s", symbol);
      return 0;
    }

  (*it) = (aie_itype_unit *) aie_malloc (sizeof (aie_itype_unit));

  if (NULL == (*it))
    return 0;

  //apply reset values
  init_aie_insn_unit ((*it));

  //Chain to itype list
  if (NULL == aie_itype_list_head)
    {
      aie_itype_list_head = (*it);
    }
  else
    {
      aie_itype_list_tail->n = (*it);
    }
  aie_itype_list_tail = (*it);

  if (!aie_strcpy ((*it)->name, symbol))
    return 0;

  return 1;
}

int
add_stoken (aie_insn_unit * it, token_type s, char c,
	    sym_val_map_unit * map_unit, aie_register * reg)
{
  aie_stoken *tok = (aie_stoken *) aie_malloc (sizeof (aie_stoken));
  if (NULL == tok)
    {
      return 0;
    }

  init_aie_stoken (tok);

  //chain to list
  if (NULL == it->aie_stoken_list_head)
    {
      it->aie_stoken_list_head = tok;
    }
  else
    {
      it->aie_stoken_list_tail->n = tok;
    }

  it->aie_stoken_list_tail = tok;

  //copy tok
  tok->tok = s;

  //copy char if tok is not a SYMBOL
  if (MYCHAR == s || NEGATIVE == s || POSTIVE == s || CROSSING == s)
    {
      tok->c = c;
    }
  else if (DIGIT == s)
    {
      tok->value = eval_uint;
    }

  if (NULL != map_unit)
    {
      tok->map_unit = map_unit;
      tok->register_unit = reg;
      it->syntax_symbol_count++;
    }

  if (NULL != reg)
    {
      tok->reg = reg;
    }

  //format_unit pointer should be add latter
  return 1;
}

int
add_pseudo_reg_idx (aie_register * it, int idx)
{

  if (physical == it->is_pseudo)
    {
      AIE_PPRINT (aie_error,
		  "specify pseudo register access macro for non-pseudo register: `%s'",
		  it->symbol);
      return 0;
    }

  pseudo_reg_idx *reg_idx =
    (pseudo_reg_idx *) aie_malloc (sizeof (pseudo_reg_idx));
  init_pseudo_reg_idx (reg_idx);
  reg_idx->idx = idx;

  if (!it->index_list_head)
    {
      it->index_list_head = reg_idx;
      it->index_list_tail = reg_idx;
    }
  else
    {
      pseudo_reg_idx *tmp = it->index_list_head;
      while (tmp)
	{
	  if (tmp->idx == idx)
	    {
	      AIE_PPRINT (aie_error,
			  "redefine index: `%d' in (partial)pseudo register access macro",
			  idx);
	      return 0;
	    }
	  tmp = tmp->n;
	}
      it->index_list_tail->n = reg_idx;
      it->index_list_tail = reg_idx;
    }

  return 1;
}

int
add_aie_register (aie_register ** it, register_mapping is_pseudo,
		  char *symbol)
{
  if (NULL != is_aie_register_exist (symbol))
    {
      AIE_PPRINT (aie_error, "redefinition of %s", symbol);
      return 0;
    }

  (*it) = (aie_register *) aie_malloc (sizeof (aie_register));

  if (NULL == (*it))
    return 0;

  //apply reset values
  init_aie_register ((*it));

  //Chain to itype list
  if (NULL == aie_register_list_head)
    {
      aie_register_list_head = (*it);
    }
  else
    {
      aie_register_list_tail->n = (*it);
    }
  aie_register_list_tail = (*it);

  if (!aie_strcpy ((*it)->symbol, symbol))
    return 0;

  (*it)->is_pseudo = is_pseudo;

  return 1;
}

int
add_aie_register_width (aie_register * it, unsigned int value)
{
  if (NULL == it)
    {
      AIE_PPRINT (aie_error, "Undefined reference to %d", it);
      return 0;
    }

  if (!it->can_be_redef_width && it->is_def_width)
    {
      AIE_PPRINT (aie_error, "redefinition of `%s.width'", it->symbol);
      return 0;
    }

  if (0 == value || value > 1024)
    {
      AIE_PPRINT (aie_error, "invalid register width `%d' (1~1024)", value);
      return 0;
    }

  if (physical == it->is_pseudo)
    {
      if (8 != value && 16 != value && 32 != value && 64 != value)
	{
	  AIE_PPRINT (aie_error,
		      "unsupport register width `%d', 8/16/32/64 is support for non-pseudo register",
		      value);
	  return 0;
	}
    }

  it->is_def_width = 1;
  it->width = value;
  return 1;
}

int
add_aie_register_regnum (aie_register * it, int value)
{
  if (NULL == it)
    {
      AIE_PPRINT (aie_error, "Undefined reference to %d", it);
      return 0;
    }

  if (!it->can_be_redef_regnum && it->is_def_regnum)
    {
      AIE_PPRINT (aie_error, "redefinition of `%s.regnum'", it->symbol);
      return 0;
    }

  if (0 == value || value > 128 || value < 0)
    {
      AIE_PPRINT (aie_error, "invalid register number `%d' (1~128)", value);
      return 0;
    }
  it->regnum = value;

  /* malloc time stamp for pipeline modeling */
  if (it->tstamp)
    free (it->tstamp);
  it->tstamp =
    (unsigned long long int *) malloc (sizeof (unsigned long long int) *
				       it->regnum);

  unsigned int i = 0;
  for (i = 0; i < it->regnum; i++)
    {
      it->tstamp[i] = 0;
    }

  it->is_def_regnum = 1;
  return 1;
}

int
add_aie_register_sym_val_mapping (aie_register * it, char *symbol)
{
  sym_val_map_unit *map_unit;

  if (NULL == it)
    {
      AIE_PPRINT (aie_error, "undefined reference to %d", it);
      return 0;
    }

  if (!it->can_be_redef_mapping && it->is_def_mapping)
    {
      AIE_PPRINT (aie_error, "redefinition of `%s.sym_map'", it->symbol);
      return 0;
    }

  // if sym_map is "NULL", register encoding is implied in read/write insn.
  // this is the only case for a map_unit to be NULL.
  if (0 != strcasecmp("null", symbol)) {
	  map_unit = is_sym_val_map_exist(symbol);
	  if(NULL == map_unit) {
		  AIE_PPRINT(aie_error, "undefined reference to symbol_value_map: `%s'", symbol);
		  return 0;
	  }
	  it->map_unit = map_unit;
  }

  it->is_def_mapping = 1;
  return 1;
}

int
check_aie_register_rd_wr_insn (void)
{
  aie_register *reg;
  aie_insn_unit *insn;
  unsigned int found;

  // Check rd_insn and wr_insn for registers
  reg = aie_register_list_head;
  insn = aie_insn_list_head;
  while (NULL != reg)
    {
      // rd_insn is assigned to register.
      if (reg->is_def_rd_insn)
	{
	  // check if rd insn is declared or not
	  found = 0;
	  while ((NULL != insn) && !found)
	    {
	      if (!strcmp (reg->rd_insn, insn->name))
		found = 1;
	      else
		insn = insn->n;
	    }
	  if (!found)
	    {
	      AIE_PPRINT (aie_error,
			  "%s register defined rd_insn %s is not delcared!",
			  reg->symbol, reg->rd_insn);
	      return 0;
	    }
	  else
	    {
	      reg->rd_insn_unit = insn;
	    }
	}

      // wr_insn is assigned to register.
      if (reg->is_def_wr_insn)
	{
	  // check if wr insn is declared or not insn = aie_insn_list_head;
	  // found = 0;
	  while ((NULL != insn) && !found)
	    {
	      if (!strcmp (reg->wr_insn, insn->name))
		found = 1;
	      else
		insn = insn->n;
	    }
	  if (!found)
	    {
	      AIE_PPRINT (aie_error,
			  "%s register defined wr_insn %s is not delcared!",
			  reg->symbol, reg->wr_insn);
	      return 0;
	    }
	  else
	    reg->wr_insn_unit = insn;

	}
      // move to next register
      reg = reg->n;
    }

  return 1;
}


void
free_stoken (aie_stoken * it)
{
  if (NULL == it)
    return;

  aie_stoken *current;
  while (NULL != it)
    {
      current = it;
      it = it->n;
      free (current);
    }
}

void
free_ifield (aie_ifield * it)
{
  if (NULL == it)
    return;

  aie_ifield *current;
  while (NULL != it)
    {
      current = it;
      it = it->n;
      free (current);
    }
}

int
install_objd_list (void)
{
  group g;
  objd_list = (aie_list *) aie_malloc (sizeof (aie_list) * 26);

  if (!objd_list)
    {
      return 0;
    }
  else
    {
      for (g = AIE_CPE1; g <= AIE_SDC_BI; g++)
	{
	  //some reset
	  objd_list[g].head = NULL;
	  objd_list[g].tail = NULL;
	  objd_list[g].length = 0;

	  if (!chain_classify (&(objd_list[g].head), &(objd_list[g].tail), g))
	    {
	      return 0;
	    }
	}
      return 1;
    }
}

void
dump_aie_state (void)
{
  if (aie_error == GET_TSTAT () || aie_error == GET_PSTAT ())
    return;

  DPPRINT ("%s", "-------------------------------");
  DPPRINT ("%s", "AIE coprocessor info");
  DPPRINT ("%s", "-------------------------------");
  DPPRINT ("cpid   = %d", cop->cpid);
  stage_nemonic *stage = cop->stage_nemonic_list;
  char stage_list[MAX_STR_CONST] = "\0";
  while (stage)
    {
      sprintf (stage_list, "%s %s", stage_list, stage->nemonic);
      stage = stage->n;
    }
  DPPRINT ("stage  = {%s}", stage_list);
  DPPRINT ("#stage = %d", cop->stage_num);
  DPPRINT ("etrap  = %9s, %d", cop->etrap->nemonic, cop->etrap->stage);
  DPPRINT ("ltrap  = %9s, %d", cop->ltrap->nemonic, cop->ltrap->stage);
  DPPRINT ("d2m    = %9s, %d", cop->d2m->nemonic, cop->d2m->stage);
  DPPRINT ("d2c    = %9s, %d", cop->d2c->nemonic, cop->d2c->stage);

  sym_val_map_unit *it0 = sym_val_map_list_head;
  DPPRINT ("%s", "-------------------------------");
  DPPRINT ("%s", "AIE symbol value mapping");
  DPPRINT ("%s", "-------------------------------");
  while (NULL != it0)
    {
      sym_val_unit *it = it0->sym_val_list_head;
      while (NULL != it)
	{
	  DPPRINT ("%15s.%8s = 0x%08x", it0->name, it->symbol, it->value);
	  it = it->n;
	}
      it0 = it0->n;
    }

  aie_register *reg = aie_register_list_head;
  DPPRINT ("%s", "-------------------------------");
  DPPRINT ("%s", "AIE register");
  DPPRINT ("%s", "-------------------------------");
  while (NULL != reg)
    {
      DPPRINT ("%10s.w     %d", reg->symbol, reg->width);
      DPPRINT ("%10s.#     %d", reg->symbol, reg->regnum);
      DPPRINT ("%10s.map  (0x%08x)", reg->symbol, reg->map_unit);
      DPPRINT ("%10s.sdo?  %s", reg->symbol,
	       physical == reg->is_pseudo ? "physical" : pseudo ==
	       reg->is_pseudo ? "pseudo" : "partial_pseudo");
      if (pseudo == reg->is_pseudo || partial_pseudo == reg->is_pseudo)
	{
	  DPPRINT ("%10s.i     %s", reg->symbol, reg->index_symbol);
	  DPPRINT ("%10s.n     %d", reg->symbol, reg->num_dependency);

	  pseudo_reg_mapping *it = reg->phy_reg_map_list;
	  while (it)
	    {
	      char tmp[MAX_STR_CONST];
	      stackop_token *tok = it->stok_list;
	      debug_stackop_token (tok, tmp);
	      DPPRINT ("%10s.r     name=%s msk=0x%08llx sft=%03d stok=%s",
		       reg->symbol, it->physical_reg->symbol, it->mask,
		       it->shift, tmp);
	      it = it->n;
	    }
	}
      DPPRINT ("");
      reg = reg->n;
    }

  aie_itype_unit *it = aie_itype_list_head;
  DPPRINT ("%s", "-------------------------------");
  DPPRINT ("%s", "AIE instruction type");
  DPPRINT ("%s", "-------------------------------");
  while (NULL != it)
    {
      DPPRINT ("%10s.g   %9s", it->name, it->group_str);
      DPPRINT ("%10s.ba  %9d", it->name, it->base);
      DPPRINT ("%10s.bm  %9d", it->name, it->base_mask);
      DPPRINT ("%10s.#s  %9d", it->name, cop->stage_num);
      DPPRINT ("%10s.et  %9s", it->name, cop->etrap->nemonic);
      DPPRINT ("%10s.lt  %9s", it->name, cop->ltrap->nemonic);
      DPPRINT ("%10s.d2m %9s", it->name, cop->d2m->nemonic);
      DPPRINT ("%10s.d2c %9s", it->name, cop->d2c->nemonic);
      DPPRINT ("%10s.ps  %9s", it->name, it->pipe->pstage->nemonic);
      DPPRINT ("%10s.cs  %9s", it->name, it->pipe->cstage->nemonic);
      DPPRINT ("%10s.ss  %9s", it->name, it->pipe->ssstage->nemonic);
      DPPRINT ("%10s.sc  %9d", it->name, it->pipe->sscycle);
      DPPRINT ("%10s.#sr %9d", it->name, it->pipe->sreg_num);
      DPPRINT ("%10s.#dr %9d", it->name, it->pipe->dreg_num);


      aie_ifield *it3 = it->aie_ifield_list_head;
      while (NULL != it3)
	{
	  DPPRINT
	    ("%10s.f %9s shf=%2d msk=0x%04x bw=%2d val=%3d imm_type=%d reg_type=%d valid=%d is_map=%d map_s=%s",
	     it->name, it3->symbol, it3->shift, it3->mask, it3->bit_width,
	     it3->value, it3->imm_align_type, it3->register_type, it3->valid,
	     it3->is_mapped, it3->map_symbol);
	  it3 = it3->n;
	}
      it = it->n;
      DPPRINT ("%s", "");
    }

  aie_insn_unit *it2 = aie_insn_list_head;
  DPPRINT ("%s", "");
  DPPRINT ("%s", "-------------------------------");
  DPPRINT ("%s", "AIE instruction ");
  DPPRINT ("%s", "-------------------------------");
  while (NULL != it2)
    {
      DPPRINT ("%10s.g   %9s", it2->name, it2->group_str);
      DPPRINT ("%10s.ba  %9d", it2->name, it2->base);
      DPPRINT ("%10s.bm  %9d", it2->name, it2->base_mask);
      DPPRINT ("%10s.#s  %9d", it2->name, cop->stage_num);
      DPPRINT ("%10s.et  %9s", it2->name, cop->etrap->nemonic);
      DPPRINT ("%10s.lt  %9s", it2->name, cop->ltrap->nemonic);
      DPPRINT ("%10s.d2m %9s", it2->name, cop->d2m->nemonic);
      DPPRINT ("%10s.d2c %9s", it2->name, cop->d2c->nemonic);
      DPPRINT ("%10s.ps  %9s", it2->name, it2->pipe->pstage->nemonic);
      DPPRINT ("%10s.cs  %9s", it2->name, it2->pipe->cstage->nemonic);
      DPPRINT ("%10s.ss  %9s", it2->name, it2->pipe->ssstage->nemonic);
      DPPRINT ("%10s.sc  %9d", it2->name, it2->pipe->sscycle);
      DPPRINT ("%10s.#sr %9d", it2->name, it2->pipe->sreg_num);
      DPPRINT ("%10s.#dr %9d", it2->name, it2->pipe->dreg_num);

      aie_ifield *it3 = it2->aie_ifield_list_head;

      while (NULL != it3)
	{
	  DPPRINT
	    ("%10s.f %9s shf=%2d msk=0x%04x bw=%2d val=%3d imm_type=%d reg_type=%d valid=%d is_map=%d map_s=%s",
	     it2->name, it3->symbol, it3->shift, it3->mask, it3->bit_width,
	     it3->value, it3->imm_align_type, it3->register_type, it3->valid,
	     it3->is_mapped, it3->map_symbol);
	  it3 = it3->n;
	}

      DPPRINT ("%10s.s %5s%s", it2->name, "", it2->syntax);

      aie_stoken *it5 = it2->aie_stoken_list_head;
      while (NULL != it5)
	{
	  if (DIGIT == it5->tok)
	    {
	      DPPRINT
		("%10s.s %5d (%d) map(0x%08x) ifield(0x%08x) reg(0x%08x)",
		 it2->name, it5->value, it5->tok, it5->map_unit,
		 it5->format_unit, it5->reg);
	    }
	  else
	    {
	      DPPRINT
		("%10s.s %5c (%d) map(0x%08x) ifield(0x%08x) reg(0x%08x)",
		 it2->name, it5->c, it5->tok, it5->map_unit, it5->format_unit,
		 it5->reg);
	    }
	  it5 = it5->n;
	}

      it2 = it2->n;
      DPPRINT ("%s", "");
    }
}

void
define_andes_gpr (void)
{

  int i = 0;

  sym_val_map_unit *sym_val_map_tmp = NULL;

  if (!add_sym_val_map (&sym_val_map_tmp, "imm"))
    AIE_TPRINT (aie_error,
		"predefine andes signed immediate 12bit with word align error");

  if (!add_sym_val_map (&sym_val_map_tmp, "nds_gpridx"))
    AIE_TPRINT (aie_error,
		"predefined andes general purpose register index error");

  for (i = 0; i <= 31; i++)
    {

      char tmp[MAX_STR_CONST] = "\0";
      sprintf (tmp, "%s%d", "$r", i);

      if (!add_sym_val (sym_val_map_tmp, tmp, i))
	{
	  AIE_TPRINT (aie_error,
		      "predefined andes general purpose register index error");
	}

      if (i <= 5)
	{
	  tmp[0] = '\0';
	  sprintf (tmp, "%s%d", "$a", i);

	  if (!add_sym_val (sym_val_map_tmp, tmp, i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (6 <= i && i <= 14)
	{
	  tmp[0] = '\0';
	  sprintf (tmp, "%s%d", "$s", i - 6);

	  if (!add_sym_val (sym_val_map_tmp, tmp, i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (i <= 7)
	{
	  tmp[0] = '\0';
	  sprintf (tmp, "%s%d", "$o", i);

	  if (!add_sym_val (sym_val_map_tmp, tmp, i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (i <= 11)
	{
	  tmp[0] = '\0';
	  sprintf (tmp, "%s%d", "$h", i);

	  if (!add_sym_val (sym_val_map_tmp, tmp, i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (16 <= i && i <= 19)
	{
	  tmp[0] = '\0';
	  sprintf (tmp, "%s%d", "$h", i - 4);

	  if (!add_sym_val (sym_val_map_tmp, tmp, i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}


      if (15 == i)
	{
	  if (!add_sym_val (sym_val_map_tmp, "$ta", 15))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (16 <= i && i <= 25)
	{
	  tmp[0] = '\0';
	  sprintf (tmp, "%s%d", "$t", i - 16);

	  if (!add_sym_val (sym_val_map_tmp, tmp, i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (26 == i)
	{
	  if (!add_sym_val (sym_val_map_tmp, "$p0", i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (27 == i)
	{
	  if (!add_sym_val (sym_val_map_tmp, "$p1", i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (28 == i)
	{
	  if (!add_sym_val (sym_val_map_tmp, "$s9", i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }

	  if (!add_sym_val (sym_val_map_tmp, "$fp", i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (29 == i)
	{
	  if (!add_sym_val (sym_val_map_tmp, "$gp", i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (30 == i)
	{
	  if (!add_sym_val (sym_val_map_tmp, "$lp", i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}

      if (31 == i)
	{
	  if (!add_sym_val (sym_val_map_tmp, "$sp", i))
	    {
	      AIE_TPRINT (aie_error,
			  "predefined andes general purpose register index error");
	    }
	}
    }

  aie_register *reg;
  if (!add_aie_register (&reg, 0, "nds_gpr"))
    AIE_TPRINT (aie_error, "predefine andes general purpose register error");

  reg->width = 32;
  reg->regnum = 32;
  //code sequence is matter
  reg->map_unit = sym_val_map_tmp;
}

void
aie_reset_list (void)
{
  /* allow user redefin configuration file through cmd line again */
  /* symbol value map */
  if (sym_val_map_list_head)
    {
      free (sym_val_map_list_head);
      sym_val_map_list_head = NULL;
    }
  if (sym_val_map_list_tail)
    {
      free (sym_val_map_list_tail);
      sym_val_map_list_tail = NULL;
    }

  /* hardware register */
  if (aie_register_list_head)
    {
      free (aie_register_list_head);
      aie_register_list_head = NULL;
    }
  if (aie_register_list_tail)
    {
      free (aie_register_list_tail);
      aie_register_list_tail = NULL;
    }

  /* instruction type */
  if (aie_itype_list_head)
    {
      free (aie_itype_list_head);
      aie_itype_list_head = NULL;
    }
  if (aie_itype_list_tail)
    {
      free (aie_itype_list_tail);
      aie_itype_list_tail = NULL;
    }

  /* global instruction list */
  if (aie_insn_list_head)
    {
      free (aie_insn_list_head);
      aie_insn_list_head = NULL;
    }
  if (aie_insn_list_tail)
    {
      free (aie_insn_list_tail);
      aie_insn_list_tail = NULL;
    }

  /* chain list for disassemble */
  /* cprocessor execution */
  if (objd_list)
    {
      group g = AIE_CPE1;
      for (g = AIE_CPE1; g <= AIE_SDC_BI; g++)
	{
	  if (objd_list[g].head)
	    {
	      free (objd_list[g].head);
	      objd_list[g].head = NULL;
	    }

	  if (objd_list[g].tail)
	    {
	      free (objd_list[g].tail);
	      objd_list[g].tail = NULL;
	    }

	  objd_list[g].length = 0;
	}
    }
}

int
add_stackop_token (pseudo_reg_mapping * reg, stackop_token_type type,
		   int stackop_digit)
{
  if (!reg)
    return 0;

  stackop_token *tmp = (stackop_token *) malloc (sizeof (stackop_token));
  tmp->type = type;
  tmp->n = NULL;
  if (STACKOP_DIGIT == type)
    tmp->digit = stackop_digit;

  if (!reg->stok_list)
    {
      reg->stok_list = tmp;
    }
  else
    {
      stackop_token *it = reg->stok_list;
      while (it->n)
	{
	  it = it->n;
	}
      it->n = tmp;
    }
  return 1;
}

void
init_stackop_token (stackop_token * it)
{
  it->type = 0;
  it->n = NULL;
  it->digit = 0;
}

void
debug_stackop_token (stackop_token * it, char *buf)
{
  buf[0] = '\0';
  while (it)
    {
      switch (it->type)
	{
	case STACKOP_DIGIT:
	  sprintf (buf, "%s%d", buf, it->digit);
	  break;
	case STACKOP_IDX:
	  sprintf (buf, "%si", buf);
	  break;
	case STACKOP_POSTIVE:
	  sprintf (buf, "%s+", buf);
	  break;
	case STACKOP_NEGATIVE:
	  sprintf (buf, "%s-", buf);
	  break;
	case STACKOP_DIV:
	  sprintf (buf, "%s/", buf);
	  break;
	case STACKOP_MUL:
	  sprintf (buf, "%s*", buf);
	  break;
	case STACKOP_LPAREN:
	  sprintf (buf, "%s(", buf);
	  break;
	case STACKOP_RPAREN:
	  sprintf (buf, "%s)", buf);
	  break;
	case STACKOP_STACKEND:
	  sprintf (buf, "%s(END)\n", buf);
	  break;
	}
      it = it->n;
    }
  strcat (buf, "\0");
}

//For global coprocessor config
int
aie_get_stage_num (aie_cop * insn)
//			     ^^^^ bug ???  Sign. I don't want to touch it now.
{
  if (!insn)
    return -1;
  return cop->stage_num;
}

int
aie_get_early_trap_ack (aie_cop * this_cop)
{
  if (!this_cop || !this_cop->etrap)
    return -1;
  return this_cop->etrap->stage;
}

int
aie_get_late_trap_ack (aie_cop * this_cop)
{
  if (!this_cop || !this_cop->ltrap)
    return -1;
  return this_cop->ltrap->stage;
}

int
aie_get_d2m (aie_cop * this_cop)
{
  if (!this_cop || !this_cop->d2m)
    return -1;
  return this_cop->d2m->stage;
}

int
aie_get_d2c (aie_cop * this_cop)
{
  if (!this_cop || !this_cop->d2c)
    return -1;
  return this_cop->d2c->stage;
}

//For instruction based
int
aie_get_self_stall_cycle (aie_insn_unit * insn)
{
  if (!insn || !insn->pipe)
    return -1;
  return insn->pipe->sscycle;
}

int
aie_get_self_stall_stage (aie_insn_unit * insn)
{
  if (!insn || !insn->pipe || !insn->pipe->ssstage)
    return -1;
  return insn->pipe->ssstage->stage;
}

int
aie_get_produce_stage (aie_insn_unit * insn)
{
  if (!insn || !insn->pipe || !insn->pipe->pstage)
    return -1;
  return insn->pipe->pstage->stage;
}

int
aie_get_consume_stage (aie_insn_unit * insn)
{
  if (!insn || !insn->pipe || !insn->pipe->cstage)
    return -1;
  return insn->pipe->cstage->stage;
}

//For register based
unsigned long long int
aie_get_time_stamp (aie_register * reg, int idx)
{
  if (!reg || (unsigned int) idx > reg->regnum)
    return 0;
  return reg->tstamp[idx];
}

//For source operand
int
aie_get_sreg_num (aie_insn_unit * insn)
{
  if (!insn)
    return -1;
  return insn->pipe->sreg_num;
}

aie_register *
aie_get_sreg (aie_insn_unit * insn, int idx)
{
  if (!insn)
    return 0;

  aie_stoken *tok = insn->aie_stoken_list_head;
  if (!tok)
    return 0;

  aie_register *reg = 0;
  int seq_idx = 0;
  while (NULL != tok)
    {
      //condition: a symbol, a register, a source register, a source register with idx
      if ((SYMBOL == tok->tok) && (tok->reg)
	  && aie_s == (tok->format_unit)->register_type)
	{
	  seq_idx++;
	  if (idx == seq_idx)
	    {
	      reg = tok->reg;
	      break;
	    }
	}
      tok = tok->n;
    }
  return reg;
}

int
aie_get_sreg_idx (aie_insn_unit * insn, int idx, int immediate)
{
  if (!insn)
    return -1;

  aie_stoken *tok = insn->aie_stoken_list_head;
  if (!tok)
    return -1;

  int sreg_idx = -1;
  int seq_idx = 0;
  while (NULL != tok)
    {
      //condition: a symbol, a register, a source register, a source register with idx
      if ((SYMBOL == tok->tok) && (tok->reg)
	  && aie_s == (tok->format_unit)->register_type)
	{
	  seq_idx++;
	  if (idx == seq_idx)
	    {
	      unsigned int shift = tok->format_unit->shift;
	      unsigned int mask = tok->format_unit->mask;
	      sreg_idx = (immediate >> shift) & mask;
	      break;
	    }
	}
      tok = tok->n;
    }

  return sreg_idx;
}

//For destination operand
int
aie_get_dreg_num (aie_insn_unit * insn)
{
  if (!insn)
    return -1;
  return insn->pipe->dreg_num;
}

aie_register *
aie_get_dreg (aie_insn_unit * insn, int idx)
{
  if (!insn)
    return 0;

  aie_stoken *tok = insn->aie_stoken_list_head;
  if (!tok)
    return 0;

  aie_register *reg = 0;
  int seq_idx = 0;
  while (NULL != tok)
    {
      //condition: a symbol, a register, a destination register, a destination register with idx
      if ((SYMBOL == tok->tok) && (tok->reg)
	  && aie_d == (tok->format_unit)->register_type)
	{
	  seq_idx++;
	  if (idx == seq_idx)
	    {
	      reg = tok->reg;
	      break;
	    }
	}
      tok = tok->n;
    }
  return reg;
}

int
aie_get_dreg_idx (aie_insn_unit * insn, int idx, int immediate)
{
  if (!insn)
    return -1;

  aie_stoken *tok = insn->aie_stoken_list_head;
  if (!tok)
    return -1;

  int dreg_idx = -1;
  int seq_idx = 0;
  while (NULL != tok)
    {
      //condition: a symbol, a register, a destination register, a destination register with idx
      if ((SYMBOL == tok->tok) && (tok->reg)
	  && aie_d == (tok->format_unit)->register_type)
	{
	  seq_idx++;
	  if (idx == seq_idx)
	    {
	      unsigned int shift = tok->format_unit->shift;
	      unsigned int mask = tok->format_unit->mask;
	      dreg_idx = (immediate >> shift) & mask;
	      break;
	    }
	}
      tok = tok->n;
    }

  return dreg_idx;
}

//                         map to
//for sub pseudo register --------> physical register(means ssreg)
aie_register *
aie_get_sub_reg (aie_register * reg, int idx)
{
  if (!reg || physical == reg->is_pseudo)
    return 0;

  int seq_idx = 0;
  pseudo_reg_mapping *it = reg->phy_reg_map_list;
  aie_register *physical_reg = 0;
  while (it)
    {
      seq_idx++;
      if (seq_idx == idx)
	{
	  physical_reg = it->physical_reg;
	}
      it = it->n;
    }

  return physical_reg;
}

int
aie_get_sub_reg_idx (aie_register * reg, int seq, int idx)
{
  if (!reg || physical == reg->is_pseudo)
    return -1;

  int seq_idx = 0;
  pseudo_reg_mapping *it = reg->phy_reg_map_list;
  int return_idx = -1;
  while (it)
    {
      seq_idx++;
      if (seq_idx == seq)
	{
	  return_idx = getidx (it->stok_list, idx);
	}
      it = it->n;
    }

  return return_idx;
}

int
aie_set_stage_num (aie_cop *this_cop, int num)
{
  if (!this_cop || num <= 0)
    return 0;
  this_cop->stage_num = num;
  return 1;
}

int
aie_set_early_trap_ack (aie_cop *this_cop, int etrap)
{
  if (!this_cop || etrap <= 0)
    return 0;
  if (etrap >= this_cop->ltrap->stage)
    return 0;
  this_cop->etrap->stage = etrap;
  return 1;
}

int
aie_set_late_trap_ack (aie_cop *this_cop, int ltrap)
{
  if (!this_cop || ltrap <= 0)
    return 0;
  if (ltrap <= this_cop->ltrap->stage)
    return 0;
  this_cop->ltrap->stage = ltrap;
  return 1;
}

int
aie_set_self_stall_cycle (aie_insn_unit * insn, int cycle)
{
  if (!insn || cycle <= 0)
    return 0;
  insn->pipe->sscycle = (unsigned long long int) cycle;
  return 1;
}

int
aie_set_self_stall_stage (aie_insn_unit * insn, int stage)
{
  if (!insn || stage <= 0)
    return 0;
  insn->pipe->ssstage->stage = stage;
  return 1;
}

int
aie_set_produce_stage (aie_insn_unit * insn, int stage)
{
  if (!insn || stage <= 0)
    return 0;
  insn->pipe->pstage->stage = stage;
  return 1;
}

int
aie_set_consume_stage (aie_insn_unit * insn, int stage)
{
  if (!insn || stage <= 0)
    return 0;
  insn->pipe->cstage->stage = stage;
  return 1;
}

int
aie_set_time_stamp (aie_register * reg, int idx, unsigned long long int cycle)
{
  if (!reg || (unsigned int) idx > reg->regnum)
    return 0;
  reg->tstamp[idx] = cycle;
  return 1;
}

int
add_pseudo_reg (aie_register * insn, pseudo_reg_mapping * reg)
{
  if (!insn)
    return 0;

  pseudo_reg_mapping *it = insn->phy_reg_map_list;
  if (!it)
    {
      insn->phy_reg_map_list = reg;
    }
  else
    {
      while (it)
	{
	  if (!it->n)
	    {
	      it->n = reg;
	      break;
	    }
	  it = it->n;
	}
    }
  return 1;
}


/* used to check syntax string defined in syntax */
int
check_syntax_string (aie_insn_unit * insn, char *str)
{

  if (insn->is_def_syntax && !insn->can_be_redef_syntax)
    {
      AIE_PPRINT (aie_error, "redefinition of %s.syntax", insn->name);
      return 0;
    }

  if (insn->can_be_redef_syntax)
    {
      //Free memory
      free_stoken (insn->aie_stoken_list_head);

      insn->aie_stoken_list_head = NULL;
      insn->aie_stoken_list_tail = NULL;
      insn->aie_stoken_list_length = 0;
    }

  if (0 == strcmp (str, ""))
    {
      AIE_PPRINT (aie_error, "define trivial `syntax' in %s", insn->name);
      return 0;
    }

  //Copy to YY_INPUT
  if (!aie_strcpy (eval_syntax_line, str))
    return 0;

  //Have redefined YY_INPUT, thus, restart with NULL FILE pointer
  nds32_aie_eval_restart (NULL);

  //Start to get a token
  token_type s = nds32_aie_eval_lex ();

  char mnemonic[MAX_STR_CONST] = "\0";
  char *tmp = mnemonic;
  int meet_first_space = 0;
  int already_define_multiple_reserve_operand_type = 0;
  int already_define_symbol = 0;
  aie_register *reg = 0;
  int is_first_char_in_mnemonic = 1;

  do
    {
      if (ERROR == s)
	{
	  goto invalid_char_in_syntax_string;
	}

      if (!meet_first_space && MYCHAR != s && DIGIT != s && SPACE != s)
	{
	  goto invalid_char_in_mnemonic;
	}

      if (already_define_multiple_reserve_operand_type)
	{
	  goto any_tok_after_skip_operator;
	}

      if (SKIP_SYMBOL == s
	  && (((insn->insn_group <= AIE_CPE4 || insn->insn_group >= AIE_CPLD))
	      || ((insn->insn_group > AIE_CPE4 || insn->insn_group < AIE_CPLD)
		  && already_define_symbol)))
	{
	  goto skip_symbol_should_be_1st_operand_for_mftcppwd;
	}

      if (PPP == s)
	{
	  already_define_multiple_reserve_operand_type = 1;

	  if (insn->insn_group <= AIE_MTCPPW
	      || insn->insn_group == AIE_UNKNOWN)
	    goto not_support_multiple_reserve_operand_type;
	}
      else if (SPACE == s)
	{
	  if (!meet_first_space)
	    {
	      meet_first_space = 1;
	      if (!add_stoken (insn, s, eval_token[0], NULL, NULL))
		{
		  return 0;
		}
	    }
	}
      else if (SYMBOL == s)
	{
	  reg = is_aie_register_exist (eval_token);

	  if (NULL == reg)
	    {
	      goto undefined_reg;
	    }

	  if (NULL == reg->map_unit)
	    {
	      goto undefined_reg_map;
	    }

	  already_define_symbol = 1;
	}
      else
	{
	  if (!meet_first_space && MYCHAR == s)
	    {
	      char c = eval_token[0];
	      if (c == ';' || c == '\'' || c == '!' || c == '/' || c == '*'
		  || c == '@' || c == '~' || c == '`' || c == '$' || c == '^'
		  || c == '&')
		{
		  goto invalid_char_in_mnemonic_string;
		}
	      *tmp = eval_token[0];
	      tmp++;
	      is_first_char_in_mnemonic = 0;
	    }
	  else if (!meet_first_space && DIGIT == s)
	    {
	      if (is_first_char_in_mnemonic)
		{
		  goto invalid_1st_literal_in_mnemonic_string;
		}
	      sprintf (tmp, "%s%d", tmp, eval_uint);
	      tmp = strchr (tmp, '\0');
	    }
	}

      if (SPACE != s && SYMBOL != s)
	{
	  if (!add_stoken (insn, s, eval_token[0], NULL, NULL))
	    {
	      return 0;
	    }
	}

      if (SYMBOL == s)
	{
	  if (!add_stoken (insn, s, eval_token[0], reg->map_unit, reg))
	    {
	      return 0;
	    }
	  reg = 0;
	}

      //should be the last one
      //corner case: ... (1st char is white space)
      //            ^
      if (meet_first_space && 0 == strcmp (mnemonic, ""))
	{
	  goto no_valid_mnemonic;
	}

      s = nds32_aie_eval_lex ();
    }
  while (s != EOL);

  if (!aie_strcpy (insn->syntax, str))
    return 0;

  return 1;

not_support_multiple_reserve_operand_type:
  AIE_PPRINT (aie_error,
	      "multiple reserved operand type '...' is for the last operand and only support for");
  AIE_PPRINT (aie_error, "%s.group = CP[LS][DW](_BI)?, [LS][DW]C(_BI)?",
	      insn->name);
  return 0;
undefined_reg:
  AIE_PPRINT (aie_error, "use undefined register `%s' as operand",
	      eval_token);
  AIE_PPRINT (aie_error,
	      "first white space is used as seperator between mnemonic and operands(optional)");
  return 0;
undefined_reg_map:
  AIE_PPRINT(aie_error, "Line 2737: undefined reference to `%s.sym_map'", eval_token);
  AIE_PPRINT (aie_error,
	      "first white space is used as seperator between mnemonic and operands(optional)");
  return 0;
invalid_char_in_mnemonic_string:
  AIE_PPRINT (aie_error, "mnemonic contain invalid/unsupport character: `%c'",
	      eval_token[0]);
  AIE_PPRINT (aie_error,
	      "first white space is used as seperator between mnemonic and operands(optional)");
  return 0;
invalid_char_in_syntax_string:
  AIE_PPRINT (aie_error, "syntax contain invalid/unsupport character: `%c'",
	      eval_token[0]);
  AIE_PPRINT (aie_error,
	      "first white space is used as seperator between mnemonic and operands(optional)");
  return 0;
no_valid_mnemonic:
  AIE_PPRINT (aie_error, "possibly not specify any valid mnemonic `%s'", str);
  AIE_PPRINT (aie_error,
	      "first white space is used as seperator between mnemonic and operands(optional)");
  return 0;
invalid_char_in_mnemonic:
  AIE_PPRINT (aie_error,
	      "mnemonic contains invalid/unsupport character/string `%s'",
	      eval_token);
  AIE_PPRINT (aie_error,
	      "first white space is used as seperator between mnemonic and operands(optional)");
  return 0;
any_tok_after_skip_operator:
  AIE_PPRINT (aie_error,
	      "found `%c' specified after multiple reserved operand: `...'",
	      eval_token[0]);
  AIE_PPRINT (aie_error, "use single reserved operand `%s' instead of `...'",
	      "%");
  return 0;
skip_symbol_should_be_1st_operand_for_mftcppwd:
  AIE_PPRINT (aie_error,
	      "reserved operand type '%c' is for 1st operand which corresponding bit field",
	      '%');
  AIE_PPRINT (aie_error,
	      "is non-customized and only support for %s.group = MFCPD, MFCPW, MTCPD,",
	      insn->name);
  AIE_PPRINT (aie_error, "MTCPW, MFCPPW or MTCPPW");
  return 0;
invalid_1st_literal_in_mnemonic_string:
  AIE_PPRINT (aie_error, "leading digit in mnemonic is invalid: `%d'",
	      eval_uint);
  AIE_PPRINT (aie_error,
	      "first white space is used as seperator between mnemonic and operands(optional)");
  return 0;
}

/* end of nds32_aie_typedef_insn.c */


/* nds32_aie_translator.c */

static aie_stoken *mad_s_head = NULL;
static aie_stoken *mad_s_tail = NULL;
int scan_offset = 2;

/* prototyping */

static inline int
look_up_symbol_value_map_list (aie_stoken * syntax_token, char *symbol)
{
  sym_val_unit *it = syntax_token->map_unit->sym_val_list_head;

  //Immediate handle
  if (0 == strcmp (syntax_token->map_unit->name, "imm"))
    {

      imm_type align = syntax_token->format_unit->imm_align_type;
      int bit_width = syntax_token->format_unit->bit_width;

      switch (align)
	{
	case aie_sd:
	case aie_sw:
	case aie_sh:
	case aie_sb:
	  {
	    long int value = 0;
	    if (!aie_strtol
		(symbol, 10, &value, syntax_token->format_unit->bit_width))
	      {
		if (bit_width)
		  {
		    WTPRINT
		      ("Can't not conver `%s' to %d bits 'long int', possible it is not a representable value",
		       symbol, bit_width);
		  }
		else
		  {
		    WTPRINT
		      ("Can't not conver `%s' to 'long int', possible it is not a representable value",
		       symbol);
		  }
		return 0;
	      }

	    syntax_token->format_unit->value = (unsigned int) value;
	    break;
	  }
	case aie_ud:
	case aie_uw:
	case aie_uh:
	case aie_ub:
	  {
	    unsigned long int value = 0;
	    if (!aie_strtoul
		(symbol, 10, &value, syntax_token->format_unit->bit_width))
	      {
		if (bit_width)
		  {
		    WTPRINT
		      ("Can't not conver `%s' to %d bits 'unsigned long int', possible it is not a representable value",
		       symbol, bit_width);
		  }
		else
		  {
		    WTPRINT
		      ("Can't not conver `%s' to 'unsigned long int', possible it is not a representable value",
		       symbol);
		  }
		return 0;
	      }

	    syntax_token->format_unit->value = (unsigned int) value;
	    break;
	  }
	default:
	  return 0;
	}

      //alignment check..
      if (align <= aie_ud && (syntax_token->format_unit->value & 0x7))
	{
	  WTPRINT ("Invalid immediate `%s' due to non-double word align",
		   symbol);
	  return 0;
	}
      else if (align <= aie_uw && (syntax_token->format_unit->value & 0x3))
	{
	  WTPRINT ("Invalid immediate `%s' due to non-word align", symbol);
	  return 0;
	}
      else if (align <= aie_uh && (syntax_token->format_unit->value & 0x1))
	{
	  WTPRINT ("Invalid immediate `%s' due to non-half word align",
		   symbol);
	  return 0;
	}

      syntax_token->format_unit->is_mapped = 1;
      return 1;
    }

  int found = 0;
  while (NULL != it)
    {
      if (0 == strcmp (it->symbol, symbol))
	{
	  found = 1;
	  break;
	}
      it = it->n;
    }

  if (!found)
    return 0;

  if (!aie_strcpy (syntax_token->format_unit->map_symbol, symbol))
    {
      return 0;
    }

  syntax_token->format_unit->is_mapped = 1;
  syntax_token->format_unit->value = it->value;

  return 1;
}

static inline int
step_until_and_look_up_sym_val_map_unit (aie_insn_unit *it __attribute__ ((__unused__)),
					 aie_stoken **stok,
					 aie_stoken **mad_stok,
					 char terminal)
{

  char symbol[MAX_STR_CONST] = "\0";
  char *symbol_tmp = symbol;

  while (NULL != (*mad_stok) && terminal != (*mad_stok)->c)
    {
      if (DIGIT == (*mad_stok)->tok)
	{
	  sprintf (symbol, "%s%d", symbol, (*mad_stok)->value);

	  //update symbol_tmp
	  symbol_tmp = strchr (symbol, '\0');
	}
      else
	{
	  //update symbol
	  *symbol_tmp = (*mad_stok)->c;

	  //update symbol_tmp
	  symbol_tmp++;
	}

      (*mad_stok) = (*mad_stok)->n;
    }

  if (symbol == symbol_tmp)
    return 0;

  DTPRINT ("extract \"%-s\"", symbol);

  //The *stok should point to token_type with SYMBOL type
  if (!look_up_symbol_value_map_list ((*stok), symbol))
    {
      return 0;
    }

  //Skip current SYMBOL syntax_token
  (*stok) = (*stok)->n;

  DTPRINT ("found %-s", symbol);
  return 1;
}

static inline int
match_insn_syntax (aie_insn_unit * it)
{

  aie_stoken *stok = it->aie_stoken_list_head;

  //global list
  aie_stoken *mad_stok = mad_s_head;

  if (NULL == stok)
    {
      return 0;
    }

  while (NULL != stok && NULL != mad_stok)
    {

#ifdef DEBUG_AIE_TRANSLATOR
      if (DIGIT == (stok->tok) && DIGIT == (mad_stok->tok))
	{
	  DTPRINT
	    ("compare %-s %4d(%d) : %4d(%d)  [0:x 1:sym 2:char 3:space 4:dig 5:- 6:+ 7:# 8:eol 9:ppp 10:%]",
	     it->name, stok->value, stok->tok, mad_stok->value,
	     mad_stok->tok);
	}
      else if (DIGIT == (stok->tok) && MYCHAR == (mad_stok->tok))
	{
	  DTPRINT
	    ("compare %-s %4d(%d) : %4c(%d)  [0:x 1:sym 2:char 3:space 4:dig 5:- 6:+ 7:# 8:eol 9:ppp 10:%]",
	     it->name, stok->value, stok->tok, mad_stok->c, mad_stok->tok);
	}
      else if (MYCHAR == (stok->tok) && DIGIT == (mad_stok->tok))
	{
	  DTPRINT
	    ("compare %-s %4c(%d) : %4d(%d)  [0:x 1:sym 2:char 3:space 4:dig 5:- 6:+ 7:# 8:eol 9:ppp 10:%]",
	     it->name, stok->c, stok->tok, mad_stok->value, mad_stok->tok);
	}
      else
	{
	  DTPRINT
	    ("compare %-s %4c(%d) : %4c(%d)  [0:x 1:sym 2:char 3:space 4:dig 5:- 6:+ 7:# 8:eol 9:ppp 10:%]",
	     it->name, stok->c, stok->tok, mad_stok->c, mad_stok->tok);
	}
#endif

      switch (stok->tok)
	{
	case SPACE:
	case NEGATIVE:
	case POSTIVE:
	case CROSSING:
	case MYCHAR:
	  if (stok->tok != mad_stok->tok || stok->c != mad_stok->c)
	    {
	      return 0;
	    }
	  break;
	case SYMBOL:
	  {
	    char terminal = '\0';

	    //"...,%cpreg"
	    //           ^
	    //           EOL 
	    if (NULL == stok->n)
	      {
		terminal = '\0';
	      }

	    //"...,%cpreg,"
	    //           ^
	    //           this is not a symbol
	    else if (SYMBOL != stok->n->tok)
	      {
		terminal = stok->n->c;
	      }
	    else
	      {
		DTPRINT ("Not handle the case");
		return 0;
	      }

	    if (!step_until_and_look_up_sym_val_map_unit
		(it, &stok, &mad_stok, terminal))
	      {
		return 0;
	      }

	    // Have already moved to the next token in current iteration.
	    // So, continue to the next iteration.
	    continue;
	  }
	case PPP:
	  {
	    //just skip all literal character until end
	    //check mad_stok->n == 0 because outter loop will check it again
	    char *tmp = stok->syntax;
	    while (NULL != mad_stok)
	      {
		switch (mad_stok->tok)
		  {
		  case DIGIT:
		    sprintf (tmp, "%d", mad_stok->value);
		    tmp = strchr (tmp, '\0');
		    break;
		  default:
		    *tmp = mad_stok->c;
		    tmp++;
		  }
		mad_stok = mad_stok->n;
	      }
	    // Terminate the PPP syntax string with a null byte.
	    *tmp = '\0';
	    return 1;
	  }
	  break;
	case SKIP_SYMBOL:
	  {
	    // Use the next token of stok as the termination point for skipping symbols.
	    // The termination point may be end of line (EOL) if all custimized opcodes
	    // are fully specified with constants. For example, "fmtcsr $r0" has no custimized
	    // operand. The coprocessor operand is implied in the instruction mnemonic.
	    //
	    // Step forward a next token of stok.
	    token_type terminal_token = (stok->n) ? stok->n->tok : EOL;
	    char terminal_char = (stok->n) ? stok->n->c : '\0';
	    unsigned int terminal_value = (stok->n) ? stok->n->value : 0;
	    int found = 0;

	    // Initialize the skipped-symbol syntax buffer
	    memset (stok->syntax, '\0', sizeof (stok->syntax));

	    char *tmp = stok->syntax;
	    while (NULL != mad_stok)
	      {
		switch (mad_stok->tok)
		  {
		  case SPACE:
		  case NEGATIVE:
		  case POSTIVE:
		  case CROSSING:
		  case MYCHAR:
		    if (terminal_token == mad_stok->tok
			&& terminal_char == mad_stok->c)
		      {
			found = 1;
		      }
		    else
		      {
			// copy a skipped character to stok's syntax buffer.
			*tmp = mad_stok->c;
			tmp++;
		      }
		    break;
		  case DIGIT:
		    if (terminal_token == mad_stok->tok
			&& terminal_value == mad_stok->value)
		      {
			found = 1;
		      }
		    else
		      {
			// copy a skipped number to stok's syntax buffer.
			sprintf (tmp, "%d", mad_stok->value);
			tmp = strchr (tmp, '\0');
		      }
		    break;
		  default:
		    AIE_PPRINT (aie_error, "unmatched case in skip symbol");
		    return 0;
		  }

		if (found)
		  break;

		mad_stok = mad_stok->n;

		// if the termination point is EOL, we need to handle the NULL mad_stok here instead
		// of in the while condition above.
		if ((NULL == mad_stok) && (EOL == terminal_token))
		  {
		    // Since the termination point is EOL, the skipped symbol is found and 
		    // we can return here directly.
		    return 1;
		  }
	      }

	    if (found)
	      stok = stok->n;
	    else
	      return 0;
	  }
	  break;
	case DIGIT:
	  if (stok->tok != mad_stok->tok || stok->value != mad_stok->value)
	    {
	      return 0;
	    }
	  break;
	default:
	  return 0;
	}

      stok = stok->n;
      mad_stok = mad_stok->n;
    }

  DTPRINT ("stok is %s.", stok ? "NOT NULL" : "NULL");
  DTPRINT ("mad_stok is %s.", mad_stok ? "NOT NULL" : "NULL");
  /*
     while (NULL != stok) {
     DTPRINT("stok token: %d", stok->tok);
     DTPRINT("stok char: %c", stok->c);
     DTPRINT("stok value: %d", stok->value);

     stok = stok->n;
     } */

  // if one string is exhausted while the other string is not,
  // it is obvious that they do not match. 
  // In this case, the match is failed (i.e. return 0).
  if ((NULL == stok) && (NULL == mad_stok))
    return 1;
  else
    return 0;
}

static inline void
free_mad_s (aie_stoken * it)
{
  if (NULL == it)
    return;

  aie_stoken *current = NULL;
  while (NULL != it)
    {
      current = it;
      it = it->n;
      free (current);
    }
}

static inline int
find_imm (aie_insn_unit * target)
{
  aie_ifield *it = target->aie_ifield_list_head;

  int imm = 0;
  while (NULL != it)
    {
      if (it->valid || it->is_mapped)
	{
	  imm |= ((it->value & it->mask) << it->shift);

	  //always turn off the run time bit for the new iteration
	  it->is_mapped = 0;
	}
      else
	{
	  WTPRINT ("Unrecognized operand `%s'", it->map_symbol);
	  return -1;
	}

      it = it->n;
    }

  return imm;
}

char *
nds32_aie_translator (char *syntax)
{

  DTPRINT ("Assemble: %s", syntax);

  if (aie_error == GET_TSTAT ())
    return syntax;

  if (!aie_strcpy (eval_syntax_line, syntax))
    {
      //FIXME
      WTPRINT ("Unrecognized opcode/operand `%s', ignoring the insn", syntax);
      return syntax;
    }

  if (NULL != mad_s_head)
    {
      free_mad_s (mad_s_head);
      mad_s_head = NULL;
      mad_s_tail = NULL;
    }

  //Have redefined YY_INPUT, thus, restart with NULL FILE pointer
  nds32_aie_eval_restart (NULL);
  eval_token[0] = '\0';

  token_type tok_type = nds32_aie_eval_lex ();

  //First token_type should not end of line or error
  if (tok_type == EOL || tok_type == ERROR)
    {
      WTPRINT ("Error while parse `%s'", syntax);
      return syntax;
    }

  //First token_type should not SYMBOL
  if (tok_type == SYMBOL)
    {
      WTPRINT ("Not support \\% in syntax `%s'", syntax);
      return syntax;
    }

  //Convert incoming syntax into aie_stoken and chain into list point by mad_s_head
  do
    {
      aie_stoken *it = (aie_stoken *) aie_malloc (sizeof (aie_stoken));
      if (NULL == it)
	{
	  return syntax;
	}

      //add to chain
      if (NULL == mad_s_head)
	mad_s_head = it;
      else
	mad_s_tail->n = it;
      mad_s_tail = it;

      //apply default resets
      init_aie_stoken (it);

      it->tok = tok_type;
      it->c = eval_token[0];
      it->value = eval_uint;

      tok_type = nds32_aie_eval_lex ();

      //tok_type should not SYMBOL
      if (tok_type == SYMBOL)
	{
	  WTPRINT ("Not support \\% in syntax `%s'", syntax);
	  return syntax;
	}

    }
  while (tok_type != EOL && tok_type != ERROR);

  if (tok_type == ERROR)
    {
      WTPRINT ("Error while parse `%s'", syntax);
      return syntax;
    }

#ifdef DEBUG_AIE_TRANSLATOR
  aie_stoken *mad_s_it = mad_s_head;
  while (NULL != mad_s_it)
    {
      if (DIGIT != mad_s_it->tok)
	{
	  DTPRINT
	    ("%4c (%d)   [0:x 1:sym 2:char 3:space 4:dig 5:- 6:+ 7:# 8:eol 9:ppp 10:%]",
	     mad_s_it->c, mad_s_it->tok);
	}
      else if (DIGIT == mad_s_it->tok)
	{
	  DTPRINT
	    ("%4d (%d)   [0:x 1:sym 2:char 3:space 4:dig 5:- 6:+ 7:# 8:eol 9:ppp 10:%]",
	     mad_s_it->value, mad_s_it->tok);
	}
      mad_s_it = mad_s_it->n;
    }
  DTPRINT ("end...");
#endif

  aie_insn_unit *target = NULL;
  aie_insn_unit *it = aie_insn_list_head;

  //First hit, first report for performance issue
  while (NULL != it)
    {

      int found = 1;

      if (!match_insn_syntax (it))
	{
	  found = 0;
	}

      if (found)
	{
	  target = it;
	  break;
	}

      it = it->n;
    }

  if (NULL != target)
    {
      //Reset first
      eval_token[0] = '\0';

      //Extract immediate according to Andes COP ISA Spec
      int imm = find_imm (target);
      if (-1 == imm)
	{
	  WTPRINT ("Unrecognized opcode/operand `%s', ignoring the insn",
		   syntax);
	  return syntax;
	}

      //Caution! Code sequence is matter!
      //format convert
      group g = target->insn_group;
      if (AIE_CPE4 >= g)
	{
	  imm >>= 6;
	  sprintf (eval_token, "%s %s,#0x%x", target->group_str,
		   cop->cpid_str, imm);
	}
      else if (AIE_MTCPPW >= g)
	{
	  imm >>= 8;

	  //extract 1st SKIP_SYMBOL for non-customized field [24:20]
	  aie_stoken *skip_symbol = target->aie_stoken_list_head;
	  while (0 != skip_symbol)
	    {
	      if (SKIP_SYMBOL == skip_symbol->tok)
		break;
	      skip_symbol = skip_symbol->n;
	    }

	  sprintf (eval_token, "%s %s,%s,#0x%x", target->group_str,
		   cop->cpid_str, skip_symbol->syntax, (imm & 0xfff));
	}
      else if (AIE_SDC_BI >= g)
	{
	  int cpr_idx = (imm >> 20) & 0x1f;

	  //extract 1st PPP for non-customized field [19:0]
	  aie_stoken *ppp_symbol = target->aie_stoken_list_head;
	  while (0 != ppp_symbol)
	    {
	      if (PPP == ppp_symbol->tok)
		break;
	      ppp_symbol = ppp_symbol->n;
	    }

	  sprintf (eval_token, "%s %s,$cpr%d,%s", target->group_str,
		   cop->cpid_str, cpr_idx, ppp_symbol->syntax);
	}
      else
	{
	  return syntax;
	}

      return eval_token;
    }
  else
    {
      WTPRINT ("Unrecognized opcode/operand `%s', ignoring the insn", syntax);
      return syntax;
    }
}

static inline int
look_up_symbol (aie_ifield * it, unsigned int imm)
{

  sym_val_map_unit *map_unit = it->map_unit;

  //Can't find any map_unit, possibily undefiend
  if (NULL == it->map_unit)
    return 0;

  //Handle immediate
  if (0 == strcmp (it->map_unit->name, "imm"))
    {

      imm_type align = it->imm_align_type;

      if (aie_sd == align || aie_sw == align || aie_sh == align
	  || aie_sb == align)
	{
	  if (imm & (1 << (it->bit_width - 1)))
	    {
	      imm = imm | (~((1 << it->bit_width) - 1));
	      sprintf (it->map_symbol, "%d", (int) imm);

	      it->is_mapped = 1;
	      return 1;
	    }
	}

      sprintf (it->map_symbol, "%d", imm);
      it->is_mapped = 1;
      return 1;
    }

  sym_val_unit *unit = map_unit->sym_val_list_head;

  while (NULL != unit)
    {
      if (unit->value == imm)
	{
	  if (!aie_strcpy (it->map_symbol, unit->symbol))
	    {
	      return 0;
	    }
	  else
	    {
	      it->is_mapped = 1;
	      return 1;
	    }
	}

      unit = unit->n;
    }

  return 0;
}

int
match_ifield (aie_insn_unit * list_head, unsigned int imm)
{

  aie_ifield *it = list_head->aie_ifield_list_head;

  while (NULL != it)
    {
      unsigned int tmp = (imm >> (it->shift)) & (it->mask);

      //have to look up symbol_value_mapping
      if (!it->valid)
	{
	  if (!look_up_symbol (it, tmp))
	    return 0;
	}
      else
	{
	  if (it->value != tmp)
	    return 0;
	}
      it = it->n;
    }

  return 1;
}

int
nds32_aie_dislator (unsigned int immediate, char *aie_insn, unsigned int cpid __attribute__ ((__unused__)), group g)
{

  //Fail due to parse command line/config file error
  if (aie_error == GET_TSTAT ())
    return 0;

  aie_insn_unit *list_head = NULL;

  aie_insn[0] = '\0';

  list_head = objd_list[g].head;

  int found = 0;
  while (NULL != list_head)
    {

      if (match_ifield (list_head, immediate))
	{
	  found = 1;
	  break;
	}

      list_head = list_head->group_n;
    }

  if (found)
    {
      aie_stoken *stok = list_head->aie_stoken_list_head;

      while (NULL != stok)
	{

	  if (SYMBOL == stok->tok && 1 == stok->format_unit->is_mapped)
	    {
	      stok->format_unit->is_mapped = 0;
	      strcat (aie_insn, stok->format_unit->map_symbol);
	    }
	  else if (SKIP_SYMBOL == stok->tok && g <= AIE_MTCPPW
		   && g >= AIE_MFCPD)
	    {
	      sprintf (aie_insn, "%s$r%d", aie_insn,
		       (immediate >> 20) & 0x1f);
	    }
	  else if (PPP == stok->tok && g <= AIE_CPSW && g >= AIE_CPLD)
	    {
	      int ra = (immediate >> 15) & 0x1f;
	      int rb = (immediate >> 10) & 0x1f;
	      int sv = (immediate >> 8) & 0x3;
	      sprintf (aie_insn, "%s[$r%d+($r%d<<%d)]", aie_insn, ra, rb, sv);
	    }
	  else if (PPP == stok->tok && g <= AIE_CPSW_BI && g >= AIE_CPLD_BI)
	    {
	      int ra = (immediate >> 15) & 0x1f;
	      int rb = (immediate >> 10) & 0x1f;
	      int sv = (immediate >> 8) & 0x3;
	      sprintf (aie_insn, "%s[$r%d],($r%d<<%d)]", aie_insn, ra, rb,
		       sv);
	    }
	  else if (PPP == stok->tok && g <= AIE_SDC && g >= AIE_LWC)
	    {
	      int ra = (immediate >> 15) & 0x1f;
	      int simm =
		(immediate & 0x2000) ? (immediate & 0x1fff) | 0xffffe000
		: (immediate & 0x3fff);
	      sprintf (aie_insn, "%s[$r%d+(#%d)]", aie_insn, ra, simm);
	    }
	  else if (PPP == stok->tok && g <= AIE_SDC_BI && g >= AIE_LWC_BI)
	    {
	      int ra = (immediate >> 15) & 0x1f;
	      int simm =
		(immediate & 0x2000) ? (immediate & 0x1fff) | 0xffffe000
		: (immediate & 0x3fff);
	      sprintf (aie_insn, "%s[$r%d],(#%d)", aie_insn, ra, simm);
	    }
	  else
	    {
	      if (DIGIT == stok->tok)
		sprintf (aie_insn, "%s%d", aie_insn, stok->value);
	      else
		strncat (aie_insn, &(stok->c), sizeof (char));
	    }
	  stok = stok->n;
	}
    }
  return found;
}

/* end of nds32_aie_translator.c */

