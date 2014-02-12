#ifndef _AIE_UTILS_H_
#define _AIE_UTILS_H_
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#define MAX_STR_CONST  256
#define MAX_SCAN_LINE_SIZE 16384

/* Global version for Andes Copilot */
extern int Andes_Copilot_Ver;

/* Global variable for evaluation scanner */
extern char eval_token[MAX_STR_CONST];
extern char eval_syntax_line[MAX_SCAN_LINE_SIZE];
extern unsigned int eval_uint;

/* Token type returned from evaluation scanner */
typedef enum _syntax_token {
    /* 0 */ ERROR,
    /* 1 */ SYMBOL,
    /* 2 */ MYCHAR,
    /* 3 */ SPACE,
    /* 4 */ DIGIT,
    /* 5 */ NEGATIVE,
    /* 6 */ POSTIVE,
    /* 7 */ CROSSING,
    /* 8 */ EOL,
    /* 9 */ PPP,
    /* 10 */ SKIP_SYMBOL
} token_type;

typedef enum _stackop_token_type_ {
    /* 0 */ STACKOP_STACKEND,
    /* 1 */ STACKOP_LPAREN,
    /* 2 */ STACKOP_RPAREN,
    /* 3 */ STACKOP_POSTIVE,
    /* 4 */ STACKOP_NEGATIVE,
    /* 5 */ STACKOP_MUL,
    /* 6 */ STACKOP_DIV,
    /* 7 */ STACKOP_DIGIT,
    /* 8 */ STACKOP_IDX,
} stackop_token_type;

/* AIE component status */
typedef enum _aie_status_ {
    aie_error,
    aie_warn,
    aie_debug,
    aie_normal,
} aie_status;

/* AIE component type */
typedef enum _aie_component_ {
    aie_scanner,
    aie_parser,
    aie_translator,
} aie_component;

/* Global option flags for warning/debugging message */
extern unsigned int show_warn;
extern unsigned int show_debug;

/* Global option flags for generation function template */
extern unsigned int gen_func;
extern FILE* fptr;

/* Some AIE basic memory/string operation */
extern void aie_reset(void);
extern void* aie_malloc(int length);
extern int aie_strcmp(char* dest, char* src);
extern int aie_strcpy(char* dest, char* src);
extern int aie_strtol(const char* nprt, int base, long int* value, int bitwidth);
extern int aie_strtoul(const char* nprt, int base, unsigned long int* value, int bitwidth);
extern int aie_strtoll(const char* nprt, int base, long long int* value, int bitwidth);
extern int aie_strtoull(const char* nprt, int base, unsigned long long int* value, int bitwidth);
extern int aie_endian_convertion(int endian, char* reg_image);

/* Some message handling interfaces */
extern void aie_print(aie_component type, aie_status stat, va_list args, const char* format);
extern void aie_translator_print(aie_status stat, const char* format, ...);
extern void aie_scanner_print(aie_status stat, const char* format, ...);
extern void aie_parser_print(aie_status stat, const char* format, ...);
#define AIE_TPRINT(state, args...) aie_translator_print(state, args)
#define AIE_SPRINT(state, args...) aie_scanner_print(state, args)
#define AIE_PPRINT(state, args...) aie_parser_print(state, args)

#ifdef DEBUG_AIE_TRANSLATOR
    #define DTPRINT(args...) if(show_debug) {AIE_TPRINT(aie_debug, args);}
#else
    #define DTPRINT(args...)
#endif

#ifdef DEBUG_AIE_SCANNER
    #define DSPRINT(args...) if(show_debug) {AIE_SPRINT(aie_debug, args);}
#else
    #define DSPRINT(args...)
#endif

#ifdef DEBUG_AIE_PARSER
    #define DPPRINT(args...) if(show_debug) {AIE_PPRINT(aie_debug, args);}
#else
    #define DPPRINT(args...)
#endif

#define WTPRINT(args...) if(show_warn) {AIE_TPRINT(aie_warn, args);}
#define WSPRINT(args...) if(show_warn) {AIE_SPRINT(aie_warn, args);}
#define WPPRINT(args...) if(show_warn) {AIE_PPRINT(aie_warn, args);}

extern void aie_func_print(const char* format, ...);
#define AIE_FPRINT(args...) aie_func_print(args)
#define FPRINT(args...) if(gen_func) {AIE_FPRINT(args);}

/********************************/
/* AIE status related interface */
/********************************/
extern void inc_aie_scanner_line (void);
#define INC_SLINE() inc_aie_scanner_line();

/* AIE component state handle interface */
extern aie_status get_pstat (void);
extern aie_status get_sstat (void);
extern aie_status get_tstat (void);
extern void set_pstat (aie_status s);
extern void set_sstat (aie_status s);
extern void set_tstat (aie_status s);
#define GET_SSTAT() get_sstat()
#define GET_TSTAT() get_tstat()
#define GET_PSTAT() get_pstat()
#define SET_SSTAT(s) set_sstat(s)
#define SET_TSTAT(s) set_tstat(s)
#define SET_PSTAT(s) set_pstat(s)
#endif
