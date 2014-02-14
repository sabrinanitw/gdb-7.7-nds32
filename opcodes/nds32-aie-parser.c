/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         nds32_aie_parse
#define yylex           nds32_aie_lex
#define yyerror         nds32_aie_error
#define yylval          nds32_aie_lval
#define yychar          nds32_aie_char
#define yydebug         nds32_aie_debug
#define yynerrs         nds32_aie_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 4 "nds32-aie-parser.y"


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


/* Line 268 of yacc.c  */
#line 193 "nds32-aie-parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     END = 0,
     MINUS = 258,
     PLUS = 259,
     MUL = 260,
     DIV = 261,
     PERCENT = 262,
     COMMA = 263,
     SEMICOLON = 264,
     COLON = 265,
     DOT = 266,
     EQ = 267,
     LT = 268,
     GT = 269,
     LPAREN = 270,
     RPAREN = 271,
     LBRACK = 272,
     RBRACK = 273,
     LBRACE = 274,
     RBRACE = 275,
     DOLLAR = 276,
     CROSS = 277,
     NOT = 278,
     LOGIC_AND = 279,
     LOGIC_OR = 280,
     AT = 281,
     SQUARE = 282,
     SLASH = 283,
     SDOT = 284,
     RDOT = 285,
     SKIP = 286,
     PP = 287,
     INT = 288,
     STR = 289,
     INVALID_PRESENTABLE_TOKEN = 290,
     INVALID_IMPRESENTABLE_TOKEN = 291,
     TYPEDEF = 292,
     REGISTER = 293,
     PSEUDO = 294,
     PARTIAL_PSEUDO = 295,
     WIDTH = 296,
     REGNUM = 297,
     RD_INSN = 298,
     WR_INSN = 299,
     SYM_MAP = 300,
     SYMBOL_VALUE_MAP = 301,
     INSN = 302,
     GROUP = 303,
     CPE1 = 304,
     CPE2 = 305,
     CPE3 = 306,
     CPE4 = 307,
     MFCPD = 308,
     MTCPD = 309,
     MFCPW = 310,
     MTCPW = 311,
     MFCPPW = 312,
     MTCPPW = 313,
     CPLD_BI = 314,
     CPSD_BI = 315,
     CPLW_BI = 316,
     CPSW_BI = 317,
     LWC_BI = 318,
     SWC_BI = 319,
     LDC_BI = 320,
     SDC_BI = 321,
     CPLD = 322,
     CPSD = 323,
     CPLW = 324,
     CPSW = 325,
     LWC = 326,
     SWC = 327,
     LDC = 328,
     SDC = 329,
     CPID = 330,
     STAGE = 331,
     EACK = 332,
     LACK = 333,
     D2M = 334,
     D2C = 335,
     FORMAT = 336,
     SYNTAX = 337,
     DECODE = 338,
     ID = 339,
     PSTAGE = 340,
     CSTAGE = 341,
     SSSTAGE = 342,
     SSCYCLE = 343,
     SD = 344,
     SW = 345,
     SH = 346,
     SB = 347,
     UD = 348,
     UW = 349,
     UH = 350,
     UB = 351,
     SOURCE_OPERAND = 352,
     DESTINATION_OPERAND = 353
   };
#endif
/* Tokens.  */
#define END 0
#define MINUS 258
#define PLUS 259
#define MUL 260
#define DIV 261
#define PERCENT 262
#define COMMA 263
#define SEMICOLON 264
#define COLON 265
#define DOT 266
#define EQ 267
#define LT 268
#define GT 269
#define LPAREN 270
#define RPAREN 271
#define LBRACK 272
#define RBRACK 273
#define LBRACE 274
#define RBRACE 275
#define DOLLAR 276
#define CROSS 277
#define NOT 278
#define LOGIC_AND 279
#define LOGIC_OR 280
#define AT 281
#define SQUARE 282
#define SLASH 283
#define SDOT 284
#define RDOT 285
#define SKIP 286
#define PP 287
#define INT 288
#define STR 289
#define INVALID_PRESENTABLE_TOKEN 290
#define INVALID_IMPRESENTABLE_TOKEN 291
#define TYPEDEF 292
#define REGISTER 293
#define PSEUDO 294
#define PARTIAL_PSEUDO 295
#define WIDTH 296
#define REGNUM 297
#define RD_INSN 298
#define WR_INSN 299
#define SYM_MAP 300
#define SYMBOL_VALUE_MAP 301
#define INSN 302
#define GROUP 303
#define CPE1 304
#define CPE2 305
#define CPE3 306
#define CPE4 307
#define MFCPD 308
#define MTCPD 309
#define MFCPW 310
#define MTCPW 311
#define MFCPPW 312
#define MTCPPW 313
#define CPLD_BI 314
#define CPSD_BI 315
#define CPLW_BI 316
#define CPSW_BI 317
#define LWC_BI 318
#define SWC_BI 319
#define LDC_BI 320
#define SDC_BI 321
#define CPLD 322
#define CPSD 323
#define CPLW 324
#define CPSW 325
#define LWC 326
#define SWC 327
#define LDC 328
#define SDC 329
#define CPID 330
#define STAGE 331
#define EACK 332
#define LACK 333
#define D2M 334
#define D2C 335
#define FORMAT 336
#define SYNTAX 337
#define DECODE 338
#define ID 339
#define PSTAGE 340
#define CSTAGE 341
#define SSSTAGE 342
#define SSCYCLE 343
#define SD 344
#define SW 345
#define SH 346
#define SB 347
#define UD 348
#define UW 349
#define UH 350
#define UB 351
#define SOURCE_OPERAND 352
#define DESTINATION_OPERAND 353




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 117 "nds32-aie-parser.y"

 char* text;
 int value;



/* Line 293 of yacc.c  */
#line 434 "nds32-aie-parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 446 "nds32-aie-parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  232
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1321

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  99
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  95
/* YYNRULES -- Number of rules.  */
#define YYNRULES  489
/* YYNRULES -- Number of states.  */
#define YYNSTATES  635

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   353

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    28,    36,    41,    46,    51,
      56,    61,    65,    67,    69,    72,    74,    76,    77,    83,
      84,    92,    95,    97,    99,   101,   103,   105,   107,   109,
     112,   114,   118,   122,   126,   130,   134,   135,   143,   145,
     146,   150,   154,   157,   161,   163,   165,   169,   173,   175,
     176,   183,   185,   186,   191,   192,   197,   198,   203,   204,
     209,   210,   215,   217,   223,   224,   230,   231,   239,   242,
     244,   249,   264,   266,   268,   269,   276,   277,   286,   289,
     291,   293,   295,   297,   299,   301,   303,   305,   307,   309,
     311,   313,   315,   316,   322,   326,   327,   329,   332,   334,
     336,   338,   340,   342,   344,   346,   348,   350,   352,   354,
     356,   358,   360,   362,   364,   366,   368,   370,   372,   374,
     376,   378,   380,   382,   384,   386,   388,   390,   392,   394,
     396,   398,   400,   402,   404,   406,   408,   410,   412,   414,
     416,   418,   420,   422,   424,   426,   428,   430,   432,   434,
     436,   438,   440,   442,   444,   446,   448,   450,   452,   454,
     456,   458,   460,   462,   464,   466,   468,   470,   472,   474,
     476,   478,   480,   482,   484,   486,   488,   490,   492,   494,
     496,   498,   500,   502,   504,   506,   508,   510,   512,   514,
     516,   518,   520,   522,   524,   526,   527,   532,   534,   536,
     538,   540,   542,   544,   546,   548,   550,   552,   554,   560,
     561,   569,   572,   574,   576,   578,   580,   582,   584,   586,
     588,   590,   592,   594,   596,   598,   600,   602,   604,   606,
     608,   610,   612,   614,   616,   618,   620,   622,   624,   626,
     628,   630,   632,   634,   636,   638,   640,   642,   644,   646,
     648,   650,   652,   654,   656,   658,   660,   662,   664,   666,
     668,   670,   672,   674,   676,   678,   680,   682,   684,   686,
     688,   690,   692,   694,   696,   698,   700,   702,   704,   706,
     708,   710,   712,   714,   716,   718,   720,   722,   724,   726,
     728,   730,   732,   734,   736,   738,   740,   742,   744,   746,
     748,   750,   752,   754,   756,   758,   760,   762,   764,   766,
     768,   774,   775,   776,   785,   788,   792,   793,   800,   806,
     807,   813,   816,   820,   824,   826,   830,   831,   839,   840,
     844,   847,   848,   849,   854,   859,   864,   869,   874,   879,
     881,   883,   885,   887,   889,   891,   893,   895,   897,   899,
     901,   903,   905,   907,   909,   911,   913,   915,   917,   919,
     921,   923,   925,   927,   929,   931,   933,   936,   939,   942,
     945,   948,   951,   954,   957,   960,   961,   964,   967,   970,
     971,   973,   975,   977,   979,   981,   983,   985,   987,   989,
     991,   993,   995,   997,   999,  1001,  1003,  1005,  1007,  1009,
    1011,  1013,  1015,  1017,  1019,  1021,  1023,  1025,  1027,  1029,
    1031,  1033,  1035,  1037,  1039,  1041,  1043,  1045,  1047,  1049,
    1051,  1053,  1055,  1057,  1059,  1061,  1063,  1065,  1067,  1069,
    1071,  1073,  1075,  1077,  1079,  1081,  1083,  1085,  1087,  1089,
    1091,  1093,  1095,  1097,  1099,  1101,  1103,  1105,  1107,  1109,
    1111,  1113,  1115,  1117,  1119,  1121,  1123,  1125,  1127,  1129,
    1131,  1133,  1135,  1137,  1139,  1141,  1143,  1145,  1147,  1149,
    1151,  1153,  1155,  1157,  1159,  1161,  1163,  1165,  1167,  1169
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     100,     0,    -1,    -1,   101,    -1,   102,    -1,   101,   102,
      -1,   135,    -1,   103,    -1,   140,    -1,   144,    -1,   153,
      -1,   156,    -1,   107,    -1,     9,    -1,    -1,    76,   104,
      12,    19,   105,    20,     9,    -1,    75,    12,    33,     9,
      -1,    77,    12,   193,     9,    -1,    78,    12,   193,     9,
      -1,    80,    12,   193,     9,    -1,    79,    12,   193,     9,
      -1,   105,     8,   106,    -1,   106,    -1,   193,    -1,   108,
     109,    -1,    39,    -1,    40,    -1,    -1,    38,   193,    19,
      20,     9,    -1,    -1,    38,   193,    19,   110,   111,    20,
       9,    -1,   111,   112,    -1,   112,    -1,   113,    -1,   114,
      -1,   117,    -1,   118,    -1,   115,    -1,   116,    -1,    84,
      12,    -1,     9,    -1,    41,    12,    33,    -1,    42,    12,
      33,    -1,    43,    12,   193,    -1,    44,    12,   193,    -1,
      45,    12,   193,    -1,    -1,    84,   119,    15,   120,    16,
      12,   124,    -1,   193,    -1,    -1,   193,   121,   122,    -1,
     122,     8,   123,    -1,    12,   123,    -1,    33,    32,    33,
      -1,    33,    -1,   126,    -1,    15,   125,    16,    -1,   126,
       8,   125,    -1,   126,    -1,    -1,    84,    15,   127,   128,
      16,   134,    -1,    33,    -1,    -1,   128,     4,   129,   128,
      -1,    -1,   128,     3,   130,   128,    -1,    -1,   128,     5,
     131,   128,    -1,    -1,   128,     6,   132,   128,    -1,    -1,
      15,   133,   128,    16,    -1,    84,    -1,    17,    33,    10,
      33,    18,    -1,    -1,    46,   193,    19,    20,     9,    -1,
      -1,    46,   193,    19,   136,   137,    20,     9,    -1,   137,
     138,    -1,   138,    -1,    34,    12,    33,     9,    -1,    34,
      17,    33,    32,    33,    18,   139,    12,    17,    33,    32,
      33,    18,     9,    -1,     9,    -1,    34,    -1,    -1,    37,
      47,   193,    19,    20,     9,    -1,    -1,    37,    47,   193,
      19,   141,   142,    20,     9,    -1,   142,   143,    -1,   143,
      -1,   181,    -1,   163,    -1,   174,    -1,   169,    -1,   182,
      -1,   183,    -1,   185,    -1,   184,    -1,     9,    -1,   162,
      -1,   161,    -1,   160,    -1,    -1,    84,   149,   145,   146,
     148,    -1,   146,     8,   147,    -1,    -1,   149,    -1,    11,
     155,    -1,     9,    -1,   150,    -1,   151,    -1,   152,    -1,
      84,    -1,   190,    -1,   191,    -1,     7,    -1,    10,    -1,
       3,    -1,     4,    -1,     5,    -1,     6,    -1,    12,    -1,
      13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,
      18,    -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,
      25,    -1,    26,    -1,    27,    -1,    28,    -1,    29,    -1,
      30,    -1,    31,    -1,    32,    -1,    34,    -1,    37,    -1,
      38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,    -1,
      45,    -1,    43,    -1,    44,    -1,    46,    -1,    47,    -1,
      49,    -1,    50,    -1,    51,    -1,    52,    -1,    53,    -1,
      54,    -1,    55,    -1,    56,    -1,    57,    -1,    58,    -1,
      59,    -1,    60,    -1,    61,    -1,    62,    -1,    63,    -1,
      64,    -1,    65,    -1,    66,    -1,    67,    -1,    68,    -1,
      69,    -1,    70,    -1,    71,    -1,    72,    -1,    73,    -1,
      74,    -1,    75,    -1,    76,    -1,    77,    -1,    78,    -1,
      79,    -1,    80,    -1,    89,    -1,    90,    -1,    91,    -1,
      92,    -1,    93,    -1,    94,    -1,    48,    -1,    82,    -1,
      81,    -1,    83,    -1,    85,    -1,    86,    -1,    87,    -1,
      88,    -1,    19,    -1,    20,    -1,    95,    -1,    96,    -1,
      97,    -1,    98,    -1,    35,    -1,    -1,    84,    11,   154,
     155,    -1,   181,    -1,   169,    -1,   163,    -1,   174,    -1,
     182,    -1,   183,    -1,   185,    -1,   184,    -1,   162,    -1,
     161,    -1,   160,    -1,    47,   193,    19,    20,     9,    -1,
      -1,    47,   193,    19,   157,   158,    20,     9,    -1,   158,
     159,    -1,   159,    -1,   181,    -1,   163,    -1,   169,    -1,
     174,    -1,   182,    -1,   183,    -1,   185,    -1,   184,    -1,
       9,    -1,   162,    -1,   161,    -1,   160,    -1,   190,    -1,
     191,    -1,     7,    -1,     8,    -1,    10,    -1,     3,    -1,
       4,    -1,     5,    -1,     6,    -1,    11,    -1,    12,    -1,
      13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,
      18,    -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,
      25,    -1,    26,    -1,    27,    -1,    28,    -1,    29,    -1,
      30,    -1,    31,    -1,    32,    -1,    34,    -1,    37,    -1,
      38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,    -1,
      45,    -1,    43,    -1,    44,    -1,    46,    -1,    47,    -1,
      49,    -1,    50,    -1,    51,    -1,    52,    -1,    53,    -1,
      54,    -1,    55,    -1,    56,    -1,    57,    -1,    58,    -1,
      59,    -1,    60,    -1,    61,    -1,    62,    -1,    63,    -1,
      64,    -1,    65,    -1,    66,    -1,    67,    -1,    68,    -1,
      69,    -1,    70,    -1,    71,    -1,    72,    -1,    73,    -1,
      74,    -1,    75,    -1,    76,    -1,    77,    -1,    78,    -1,
      79,    -1,    80,    -1,    89,    -1,    90,    -1,    91,    -1,
      92,    -1,    93,    -1,    94,    -1,    95,    -1,    96,    -1,
      97,    -1,    98,    -1,    35,    -1,    84,    -1,    81,    12,
      15,    16,     9,    -1,    -1,    -1,    81,    12,    15,   164,
     166,   165,    16,     9,    -1,   167,   168,    -1,   167,   168,
       8,    -1,    -1,    84,    10,    33,    10,    33,   187,    -1,
      33,    10,    33,    10,    33,    -1,    -1,    83,   170,    12,
      15,   171,    -1,    16,     9,    -1,   172,    16,     9,    -1,
     172,     8,   173,    -1,   173,    -1,   193,    12,    33,    -1,
      -1,    82,   175,    12,    15,   176,    16,     9,    -1,    -1,
      34,   177,   178,    -1,   178,   179,    -1,    -1,    -1,     8,
     193,   180,   188,    -1,    48,    12,   186,     9,    -1,    85,
      12,   193,     9,    -1,    86,    12,   193,     9,    -1,    88,
      12,    33,     9,    -1,    87,    12,   193,     9,    -1,    49,
      -1,    50,    -1,    51,    -1,    52,    -1,    53,    -1,    54,
      -1,    55,    -1,    56,    -1,    57,    -1,    58,    -1,    67,
      -1,    68,    -1,    69,    -1,    70,    -1,    73,    -1,    71,
      -1,    74,    -1,    72,    -1,    59,    -1,    60,    -1,    61,
      -1,    62,    -1,    65,    -1,    63,    -1,    66,    -1,    64,
      -1,    84,    -1,    10,    89,    -1,    10,    90,    -1,    10,
      91,    -1,    10,    92,    -1,    10,    93,    -1,    10,    94,
      -1,    10,    95,    -1,    10,    96,    -1,    10,    84,    -1,
      -1,    10,    97,    -1,    10,    98,    -1,    10,    84,    -1,
      -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,     3,
      -1,     4,    -1,     5,    -1,     6,    -1,    11,    -1,    12,
      -1,    13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,
      -1,    18,    -1,    19,    -1,    20,    -1,    21,    -1,    22,
      -1,    23,    -1,    24,    -1,    25,    -1,    26,    -1,    27,
      -1,    28,    -1,    29,    -1,    30,    -1,    31,    -1,    32,
      -1,    34,    -1,    37,    -1,    38,    -1,    39,    -1,    40,
      -1,    41,    -1,    42,    -1,    45,    -1,    43,    -1,    44,
      -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,
      -1,    51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,
      -1,    56,    -1,    57,    -1,    58,    -1,    59,    -1,    60,
      -1,    61,    -1,    62,    -1,    63,    -1,    64,    -1,    65,
      -1,    66,    -1,    67,    -1,    68,    -1,    69,    -1,    70,
      -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1,    75,
      -1,    76,    -1,    77,    -1,    78,    -1,    79,    -1,    80,
      -1,    81,    -1,    82,    -1,    83,    -1,    85,    -1,    86,
      -1,    87,    -1,    88,    -1,    89,    -1,    90,    -1,    91,
      -1,    92,    -1,    93,    -1,    94,    -1,    95,    -1,    96,
      -1,    97,    -1,    98,    -1,    35,    -1,    36,    -1,    33,
      -1,    84,    -1,   189,    -1,   190,    -1,   191,    -1,   192,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   278,   278,   279,   286,   287,   290,   291,   292,   293,
     294,   295,   296,   297,   304,   303,   309,   313,   317,   321,
     325,   330,   330,   332,   342,   345,   346,   347,   350,   356,
     355,   405,   405,   407,   407,   407,   407,   407,   408,   409,
     414,   417,   426,   435,   444,   453,   463,   462,   531,   539,
     538,   566,   566,   568,   577,   584,   584,   585,   585,   588,
     587,   617,   619,   619,   622,   622,   625,   625,   628,   628,
     631,   631,   634,   644,   670,   683,   689,   688,   704,   704,
     706,   713,   808,   811,   818,   826,   832,   831,   852,   852,
     854,   855,   856,   857,   858,   859,   860,   861,   862,   863,
     868,   873,   885,   884,   902,   902,   904,   915,   915,   918,
     923,   928,   933,   935,   936,   938,   938,   938,   938,   938,
     938,   938,   938,   938,   938,   938,   939,   939,   939,   939,
     939,   939,   939,   939,   939,   939,   940,   940,   940,   940,
     940,   940,   940,   940,   940,   940,   940,   940,   941,   941,
     942,   942,   942,   942,   942,   942,   942,   942,   942,   942,
     942,   943,   943,   943,   943,   943,   943,   943,   943,   943,
     943,   943,   943,   944,   944,   944,   944,   944,   944,   944,
     944,   944,   944,   944,   944,   944,   944,   944,   944,   944,
     945,   945,   945,   945,   945,   945,   945,   945,   945,   945,
     946,   946,   946,   946,   946,   954,   953,   963,   964,   965,
     966,   967,   968,   969,   970,   971,   976,   981,   991,   997,
     996,  1016,  1016,  1018,  1019,  1020,  1021,  1022,  1023,  1024,
    1025,  1026,  1027,  1032,  1037,  1043,  1044,  1046,  1046,  1046,
    1046,  1046,  1046,  1046,  1046,  1046,  1046,  1046,  1046,  1046,
    1047,  1047,  1047,  1047,  1047,  1047,  1047,  1047,  1047,  1047,
    1048,  1048,  1048,  1048,  1048,  1048,  1048,  1048,  1048,  1048,
    1048,  1048,  1049,  1049,  1050,  1050,  1050,  1050,  1050,  1050,
    1050,  1050,  1050,  1050,  1050,  1051,  1051,  1051,  1051,  1051,
    1051,  1051,  1051,  1051,  1051,  1051,  1051,  1052,  1052,  1052,
    1052,  1052,  1052,  1052,  1052,  1052,  1052,  1052,  1052,  1052,
    1052,  1052,  1052,  1052,  1053,  1053,  1053,  1053,  1053,  1053,
    1059,  1065,  1094,  1064,  1138,  1139,  1139,  1141,  1153,  1171,
    1170,  1181,  1186,  1194,  1194,  1196,  1209,  1208,  1273,  1272,
    1285,  1285,  1288,  1287,  1316,  1338,  1355,  1372,  1390,  1404,
    1405,  1406,  1407,  1408,  1409,  1410,  1411,  1412,  1413,  1414,
    1415,  1416,  1417,  1418,  1419,  1420,  1421,  1422,  1423,  1424,
    1425,  1426,  1427,  1428,  1429,  1430,  1440,  1441,  1442,  1443,
    1444,  1445,  1446,  1447,  1448,  1453,  1456,  1457,  1458,  1464,
    1488,  1488,  1488,  1488,  1488,  1488,  1488,  1488,  1488,  1488,
    1488,  1488,  1488,  1488,  1489,  1489,  1489,  1489,  1489,  1489,
    1489,  1489,  1489,  1489,  1489,  1489,  1490,  1490,  1490,  1490,
    1490,  1490,  1490,  1490,  1490,  1490,  1490,  1490,  1491,  1491,
    1492,  1492,  1492,  1492,  1492,  1492,  1492,  1492,  1492,  1492,
    1492,  1492,  1493,  1493,  1493,  1493,  1493,  1493,  1493,  1493,
    1493,  1493,  1493,  1493,  1494,  1494,  1494,  1494,  1494,  1494,
    1494,  1494,  1494,  1494,  1494,  1494,  1494,  1494,  1495,  1495,
    1495,  1495,  1495,  1495,  1495,  1495,  1495,  1495,  1495,  1495,
    1495,  1496,  1496,  1497,  1498,  1499,  1502,  1507,  1512,  1517
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "\"-\"", "\"+\"", "\"*\"",
  "\"/\"", "\"%\"", "\",\"", "\";\"", "\":\"", "\".\"", "\"=\"", "\"<\"",
  "\">\"", "\"(\"", "\")\"", "\"[\"", "\"]\"", "\"{\"", "\"}\"", "\"$\"",
  "\"#\"", "\"!\"", "\"&\"", "\"|\"", "\"@\"", "\"^\"", "\"~\"", "\"`\"",
  "\"'\"", "\"unknown\"", "\"...\"", "\"integer\"", "\"string\"",
  "\"invalid token\"", "INVALID_IMPRESENTABLE_TOKEN", "\"typedef\"",
  "\"register\"", "\"pseduo\"", "\"partial_pseduo\"", "\"width\"",
  "\"regnum\"", "\"rd_insn\"", "\"wr_insn\"", "\"sym_map\"",
  "\"symbol_value_map\"", "\"insn\"", "\"group\"", "\"CPE1\"", "\"CPE2\"",
  "\"CPE3\"", "\"CPE4\"", "\"MFCPD\"", "\"MTCPF\"", "\"MFCPW\"",
  "\"MTCPW\"", "\"MFCPPW\"", "\"MTCPPW\"", "\"CPLD_BI\"", "\"CPSD_BI\"",
  "\"CPLW_BI\"", "\"CPSW_BI\"", "\"LWC_BI\"", "\"SWC_BI\"", "\"LDC_BI\"",
  "\"SDC_BI\"", "\"CPLD\"", "\"CPSD\"", "\"CPLW\"", "\"CPSW\"", "\"LWC\"",
  "\"SWC\"", "\"LDC\"", "\"SDC\"", "\"cpid\"", "\"stage\"", "\"eack\"",
  "\"lack\"", "\"d2m\"", "\"d2c\"", "\"format\"", "\"syntax\"",
  "\"decode\"", "\"identifier\"", "\"pstage\"", "\"cstage\"",
  "\"ssstage\"", "\"sscycle\"", "\"sd\"", "\"sw\"", "\"sh\"", "\"sb\"",
  "\"ud\"", "\"uw\"", "\"uh\"", "\"ub\"", "\"s\"", "\"d\"", "$accept",
  "re_input", "re_input1", "input", "pipe", "$@1", "stage_list",
  "stage_mnemonic", "hardware_register", "is_pseudo_register",
  "hardware_register_statement", "$@2", "re_hardware_register_statement",
  "aie_register_attr", "register_width", "register_num",
  "register_rd_insn", "register_wr_insn", "register_symbol",
  "register_reg_map", "$@3", "pseudo_register_index", "$@4",
  "re_pseudo_register_index", "pseudo_register_index_list",
  "physical_register_map", "re_physical_register", "physical_register",
  "$@5", "physical_register_index", "$@6", "$@7", "$@8", "$@9", "$@10",
  "physical_register_bit_range", "sym_val_struct", "$@11",
  "re_sym_val_exp", "sym_val_exp", "additional_str", "typedef_new_insn",
  "$@12", "insn_body", "insn_expression", "declare_insn", "$@13",
  "re_declare_instance", "possibly_declared_instance",
  "declare_and_overwrite_attribute", "is_valid_declared_instance",
  "invalid_impresentable_declared_instance",
  "invalid_presentable_declared_instance_digit",
  "invalid_presentable_declared_instance_literal",
  "assign_declared_insn_attribute", "$@14", "expression",
  "define_new_single_insn", "$@15", "re_define_insn_attribute",
  "define_insn_attribute", "invalid_impresentable_insn_attribute",
  "invalid_presentable_insn_attribute_digit",
  "invalid_presentable_insn_attribute_literal", "aie_insn_format", "$@16",
  "$@17", "re_insn_format", "insn_format", "format_statement",
  "aie_insn_decode", "$@18", "insn_decode", "re_decode_statement",
  "decode_statement", "aie_insn_syntax", "$@19", "re_insn_syntax", "$@20",
  "insn_syntax", "syntax_symbol", "$@21", "aie_insn_group",
  "aie_insn_pstage", "aie_insn_cstage", "aie_insn_sscycle",
  "aie_insn_ssstage", "insn_group", "field_descriptor",
  "operand_descriptor", "invalid_literal_id",
  "invalid_nonrepresentable_literal_id", "invalid_digit_id", "valid_id",
  "is_valid_id", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    99,   100,   100,   101,   101,   102,   102,   102,   102,
     102,   102,   102,   102,   104,   103,   103,   103,   103,   103,
     103,   105,   105,   106,   107,   108,   108,   108,   109,   110,
     109,   111,   111,   112,   112,   112,   112,   112,   112,   112,
     112,   113,   114,   115,   116,   117,   119,   118,   120,   121,
     120,   122,   122,   123,   123,   124,   124,   125,   125,   127,
     126,   128,   129,   128,   130,   128,   131,   128,   132,   128,
     133,   128,   128,   134,   134,   135,   136,   135,   137,   137,
     138,   138,   138,   139,   139,   140,   141,   140,   142,   142,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   145,   144,   146,   146,   147,   148,   148,   149,
     149,   149,   149,   150,   151,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   154,   153,   155,   155,   155,
     155,   155,   155,   155,   155,   155,   155,   155,   156,   157,
     156,   158,   158,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   160,   161,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     163,   164,   165,   163,   166,   167,   167,   168,   168,   170,
     169,   171,   171,   172,   172,   173,   175,   174,   177,   176,
     178,   178,   180,   179,   181,   182,   183,   184,   185,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   188,   188,   188,   188,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   190,   191,   192,   193,   193,   193,   193
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     7,     4,     4,     4,     4,
       4,     3,     1,     1,     2,     1,     1,     0,     5,     0,
       7,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     3,     3,     3,     3,     3,     0,     7,     1,     0,
       3,     3,     2,     3,     1,     1,     3,     3,     1,     0,
       6,     1,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     1,     5,     0,     5,     0,     7,     2,     1,
       4,    14,     1,     1,     0,     6,     0,     8,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     5,     3,     0,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     0,
       7,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       5,     0,     0,     8,     2,     3,     0,     6,     5,     0,
       5,     2,     3,     3,     1,     3,     0,     7,     0,     3,
       2,     0,     0,     4,     4,     4,     4,     4,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     0,     2,     2,     2,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,    13,     0,    25,    26,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     3,     4,     7,    12,     0,
       6,     8,     9,    10,    11,     0,   394,   395,   396,   397,
     390,   391,   392,   393,   398,   399,   400,   401,   402,   403,
     404,   405,   406,   407,   408,   409,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,   484,   420,   482,   483,
     421,   422,   423,   424,   425,   426,   428,   429,   427,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   465,   466,   467,   485,   468,   469,
     470,   471,   472,   473,   474,   475,   476,   477,   478,   479,
     480,   481,   486,   487,   488,   489,     0,     0,     0,     0,
       0,     0,     0,     0,   117,   118,   119,   120,   115,   116,
     205,   121,   122,   123,   124,   125,   126,   127,   198,   199,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   204,   141,   142,   143,   144,   145,   146,
     148,   149,   147,   150,   151,   190,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   192,   191,
     193,   112,   194,   195,   196,   197,   184,   185,   186,   187,
     188,   189,   200,   201,   202,   203,   102,   109,   110,   111,
     113,   114,     1,     5,     0,    24,     0,    76,   219,     0,
       0,     0,     0,     0,     0,     0,   105,     0,    86,     0,
       0,     0,     0,    16,     0,    17,    18,    20,    19,   240,
     241,   242,   243,   237,   238,   239,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   318,   265,   266,
     267,   268,   269,   270,   272,   273,   271,   274,   275,     0,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,     0,   336,   329,   319,     0,     0,     0,     0,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     206,   217,   216,   215,   209,   208,   210,   207,   211,   212,
     214,   213,   235,   236,     0,    29,     0,     0,    75,    82,
       0,     0,    79,   218,   231,     0,   222,   234,   233,   232,
     224,   225,   226,   223,   227,   228,   230,   229,     0,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     108,     0,   103,     0,     0,    85,    98,     0,    89,   101,
     100,    99,    91,    93,    92,    90,    94,    95,    97,    96,
       0,     0,     0,    78,     0,   221,     0,     0,   349,   350,
     351,   352,   353,   354,   355,   356,   357,   358,   367,   368,
     369,   370,   372,   374,   371,   373,   359,   360,   361,   362,
     364,   366,   363,   365,   375,     0,   321,     0,     0,     0,
       0,     0,     0,   104,   106,   107,    28,    40,     0,     0,
       0,     0,     0,    46,     0,    32,    33,    34,    37,    38,
      35,    36,     0,    88,     0,     0,    77,   220,    21,    15,
     344,     0,   326,     0,     0,   345,   346,   348,   347,     0,
       0,     0,     0,     0,    39,     0,     0,    31,    87,    80,
       0,   320,   322,     0,   338,     0,   403,   330,     0,   334,
       0,    41,    42,    43,    44,    45,     0,    30,     0,     0,
       0,     0,   324,   341,     0,   331,     0,     0,     0,     0,
      48,    84,     0,     0,     0,   325,   339,   337,   333,   332,
     335,     0,     0,    83,     0,   323,     0,     0,     0,   340,
       0,     0,    50,     0,     0,     0,   342,     0,     0,    47,
      55,    54,    52,     0,     0,   328,   385,   389,     0,    58,
      59,     0,    51,     0,     0,   327,     0,   343,    56,     0,
       0,    53,     0,   384,   376,   377,   378,   379,   380,   381,
     382,   383,   388,   386,   387,    57,    70,    61,    72,     0,
       0,     0,    64,    62,    66,    68,    74,     0,     0,     0,
       0,     0,     0,     0,    60,    81,    71,    65,    63,    67,
      69,     0,     0,     0,    73
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    14,    15,    16,    17,   129,   388,   389,    18,    19,
     235,   404,   474,   475,   476,   477,   478,   479,   480,   481,
     505,   539,   552,   562,   572,   569,   578,   579,   590,   609,
     620,   619,   621,   622,   611,   624,    20,   250,   371,   372,
     554,    21,   367,   407,   408,    22,   246,   364,   463,   402,
     226,   227,   228,   229,    23,   245,   350,    24,   252,   375,
     376,   351,   352,   353,   354,   492,   529,   512,   513,   532,
     355,   394,   517,   518,   519,   356,   393,   515,   533,   546,
     559,   577,   357,   358,   359,   360,   361,   455,   585,   587,
     122,   123,   124,   125,   390
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -586
static const yytype_int16 yypact[] =
{
      -7,  -586,   -43,  -586,  -586,   247,   247,     2,  -586,    31,
      64,    67,    73,   839,    61,    -7,  -586,  -586,  -586,    55,
    -586,  -586,  -586,  -586,  -586,   247,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,    84,    85,    72,    94,
     247,   247,   247,   247,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,   247,  -586,    88,    89,    90,    99,
      92,   103,   104,   105,   106,  1127,  -586,    97,    98,   108,
       4,   110,   935,  -586,   247,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,   123,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,   134,  -586,  -586,  -586,   137,   138,   140,   141,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,    49,   100,   113,  1031,  -586,  -586,
      74,     7,  -586,  -586,  -586,   647,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,    58,  -586,
    -586,   135,   139,   143,   144,   247,   247,   247,   124,  1223,
    -586,  1127,  -586,   149,     8,  -586,  -586,   743,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
     126,   127,   153,  -586,   156,  -586,   247,   157,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,   158,   152,   154,   160,   162,
     163,   169,   172,  -586,  -586,  -586,  -586,  -586,   170,   198,
     199,   200,   201,   202,     3,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,   206,  -586,   207,   185,  -586,  -586,  -586,  -586,
    -586,   209,  -586,   186,   551,  -586,  -586,  -586,  -586,   188,
     189,   247,   247,   247,  -586,   208,   215,  -586,  -586,  -586,
     192,  -586,  -586,   -30,  -586,   210,   218,  -586,    51,  -586,
     216,  -586,  -586,  -586,  -586,  -586,   247,  -586,   212,   217,
     221,   222,   226,  -586,   227,  -586,   247,   228,   205,   219,
     229,   211,   231,   214,   313,  -586,   235,  -586,  -586,  -586,
    -586,   232,   236,  -586,   335,  -586,   338,   339,   247,  -586,
     -10,   317,   343,   336,   319,   321,  -586,   271,   341,  -586,
    -586,   325,  -586,   317,   326,  -586,   348,   350,   346,   355,
    -586,   332,  -586,   334,     6,  -586,   -42,  -586,  -586,   271,
      -4,  -586,   337,  -586,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,    59,
     349,    -4,  -586,  -586,  -586,  -586,   351,   362,    78,    -4,
      -4,    -4,    -4,   340,  -586,  -586,  -586,    83,    83,  -586,
    -586,   364,   342,   358,  -586
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -586,  -586,  -586,   363,  -586,  -586,  -586,   -49,  -586,  -586,
    -586,  -586,  -586,   -95,  -586,  -586,  -586,  -586,  -586,  -586,
    -586,  -586,  -586,  -586,  -193,  -586,  -208,  -178,  -586,  -585,
    -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,  -586,    12,
    -586,  -586,  -586,  -586,   -23,  -586,  -586,  -586,  -586,  -586,
     -14,  -586,  -586,  -586,  -586,  -586,   -15,  -586,  -586,  -586,
      13,  -246,  -244,  -243,  -237,  -586,  -586,  -586,  -586,  -586,
    -234,  -586,  -586,  -586,  -149,  -233,  -586,  -586,  -586,  -586,
    -586,  -586,  -231,  -230,  -228,  -227,  -224,  -586,  -586,  -586,
    -586,    -6,    -3,  -586,    -5
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -50
static const yytype_int16 yytable[] =
{
     126,   127,     1,   530,    25,   567,   377,   230,   378,   379,
     231,   606,   467,   369,   128,   380,   369,   467,   381,   382,
     236,   383,   384,   506,   385,   386,   618,   422,   387,   607,
       2,   -27,     3,     4,   627,   628,   629,   630,   370,     5,
       6,   370,   602,   130,   468,   469,   470,   471,   472,   468,
     469,   470,   471,   472,   531,   603,   604,   399,   400,   536,
     401,   232,   612,   613,   614,   615,   426,   537,     7,     8,
       9,    10,    11,    12,   568,   616,   131,    13,   427,   132,
     608,   612,   613,   614,   615,   133,   420,   473,   614,   615,
     593,   421,   473,   234,   626,   594,   595,   596,   597,   598,
     599,   600,   601,   237,   238,   239,   240,   248,   253,   249,
     251,   254,   255,   256,   257,   258,   365,   368,   366,   373,
     403,   409,   405,   410,   411,   241,   242,   243,   244,   377,
     412,   378,   379,   413,   414,   391,   415,   416,   380,   417,
     418,   381,   382,   419,   383,   384,   392,   385,   386,   395,
     396,   387,   397,   398,   456,   457,   458,   462,   466,   484,
     485,   409,   486,   410,   411,   487,   489,   490,   491,   493,
     412,   495,   496,   413,   414,   494,   415,   416,   497,   417,
     418,   498,   499,   419,   428,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     500,   501,   502,   503,   504,   508,   509,   510,   511,   454,
     514,   521,   522,   526,   527,   528,   534,   535,   538,   247,
     541,   543,   544,   542,   545,   551,   547,   549,   550,   362,
     555,   -49,   363,   558,   560,   553,   362,   556,   561,   363,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   557,   563,   564,   565,
     571,   573,   575,   574,   576,   568,   580,   581,   584,   583,
     586,   362,   588,   589,   363,   591,   592,   617,   623,   362,
     610,   625,   363,   631,   632,   633,   634,   488,   233,   507,
     582,   605,   570,   423,   483,   464,   465,   548,   425,     0,
     459,   460,   461,   230,     0,   362,   231,     0,   363,     0,
       0,   362,     0,     0,   363,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   520,
       0,     0,     0,     0,     0,     0,   523,   524,   525,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   540,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   520,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   566,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,   516,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     259,   260,   261,   262,   263,   264,   374,   265,   266,   267,
     268,   269,   270,   271,   272,   273,     0,   424,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
      56,   286,   287,    59,   288,   289,   290,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   259,   260,   261,   262,
     263,   264,   406,   265,   266,   267,   268,   269,   270,   271,
     272,   273,     0,   482,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,    56,   286,   287,    59,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   347,
     348,   349,   134,   135,   136,   137,   138,     0,     0,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,    56,   162,   163,    59,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   259,   260,
     261,   262,   263,   264,   374,   265,   266,   267,   268,   269,
     270,   271,   272,   273,     0,     0,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,    56,   286,
     287,    59,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   259,   260,   261,   262,   263,   264,
     406,   265,   266,   267,   268,   269,   270,   271,   272,   273,
       0,     0,   274,   275,   276,   277,   278,   279,   280,   281,
     282,   283,   284,   285,    56,   286,   287,    59,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     259,   260,   261,   262,   263,   264,     0,   265,   266,   267,
     268,   269,   270,   271,   272,   273,     0,     0,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
      56,   286,   287,    59,   288,   289,   290,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   134,   135,   136,   137,
     138,     0,     0,   139,     0,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,    56,   162,   163,    59,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-586))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       5,     6,     9,    33,    47,    15,   252,    13,   252,   252,
      13,    15,     9,     9,    12,   252,     9,     9,   252,   252,
      25,   252,   252,    20,   252,   252,   611,    20,   252,    33,
      37,    38,    39,    40,   619,   620,   621,   622,    34,    46,
      47,    34,    84,    12,    41,    42,    43,    44,    45,    41,
      42,    43,    44,    45,    84,    97,    98,     8,     9,     8,
      11,     0,     3,     4,     5,     6,     8,    16,    75,    76,
      77,    78,    79,    80,    84,    16,    12,    84,    20,    12,
      84,     3,     4,     5,     6,    12,    12,    84,     5,     6,
      84,    17,    84,    38,    16,    89,    90,    91,    92,    93,
      94,    95,    96,    19,    19,    33,    12,    19,     9,    20,
      20,    19,     9,     9,     9,     9,    19,     9,    20,     9,
      20,   367,     9,   367,   367,   130,   131,   132,   133,   375,
     367,   375,   375,   367,   367,    12,   367,   367,   375,   367,
     367,   375,   375,   367,   375,   375,    12,   375,   375,    12,
      12,   375,    12,    12,    15,    12,    12,    33,     9,    33,
      33,   407,     9,   407,   407,     9,     9,     9,    16,    15,
     407,     9,     9,   407,   407,    15,   407,   407,     9,   407,
     407,     9,    12,   407,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      12,    12,    12,    12,    12,     9,     9,    32,     9,    84,
      34,    33,    33,    15,     9,    33,    16,     9,    12,   234,
      18,    10,    10,    16,     8,    16,     9,     9,    33,   245,
       9,    12,   245,     8,    12,    34,   252,    33,    12,   252,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    33,    12,    10,    10,
      33,     8,    33,    17,    33,    84,    15,    32,    10,    33,
      10,   367,    16,     8,   367,    33,    32,    18,    17,   375,
      33,     9,   375,    33,    10,    33,    18,   426,    15,   474,
     573,   589,   560,   371,   407,   399,   401,   536,   375,    -1,
     395,   396,   397,   399,    -1,   401,   399,    -1,   401,    -1,
      -1,   407,    -1,    -1,   407,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   494,
      -1,    -1,    -1,    -1,    -1,    -1,   501,   502,   503,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   526,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   536,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   558,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,     3,     4,     5,     6,     7,    -1,    -1,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    -1,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      -1,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
       3,     4,     5,     6,     7,     8,    -1,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     3,     4,     5,     6,
       7,    -1,    -1,    10,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     9,    37,    39,    40,    46,    47,    75,    76,    77,
      78,    79,    80,    84,   100,   101,   102,   103,   107,   108,
     135,   140,   144,   153,   156,    47,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,   189,   190,   191,   192,   193,   193,    12,   104,
      12,    12,    12,    12,     3,     4,     5,     6,     7,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    34,    35,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,   149,   150,   151,   152,
     190,   191,     0,   102,    38,   109,   193,    19,    19,    33,
      12,   193,   193,   193,   193,   154,   145,   193,    19,    20,
     136,    20,   157,     9,    19,     9,     9,     9,     9,     3,
       4,     5,     6,     7,     8,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    34,    35,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
     155,   160,   161,   162,   163,   169,   174,   181,   182,   183,
     184,   185,   190,   191,   146,    19,    20,   141,     9,     9,
      34,   137,   138,     9,     9,   158,   159,   160,   161,   162,
     163,   169,   174,   181,   182,   183,   184,   185,   105,   106,
     193,    12,    12,   175,   170,    12,    12,    12,    12,     8,
       9,    11,   148,    20,   110,     9,     9,   142,   143,   160,
     161,   162,   163,   169,   174,   181,   182,   183,   184,   185,
      12,    17,    20,   138,    20,   159,     8,    20,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    84,   186,    15,    12,    12,   193,
     193,   193,    33,   147,   149,   155,     9,     9,    41,    42,
      43,    44,    45,    84,   111,   112,   113,   114,   115,   116,
     117,   118,    20,   143,    33,    33,     9,     9,   106,     9,
       9,    16,   164,    15,    15,     9,     9,     9,     9,    12,
      12,    12,    12,    12,    12,   119,    20,   112,     9,     9,
      32,     9,   166,   167,    34,   176,    16,   171,   172,   173,
     193,    33,    33,   193,   193,   193,    15,     9,    33,   165,
      33,    84,   168,   177,    16,     9,     8,    16,    12,   120,
     193,    18,    16,    10,    10,     8,   178,     9,   173,     9,
      33,    16,   121,    34,   139,     9,    33,    33,     8,   179,
      12,    12,   122,    12,    10,    10,   193,    15,    84,   124,
     126,    33,   123,     8,    17,    33,    33,   180,   125,   126,
      15,    32,   123,    33,    10,   187,    10,   188,    16,     8,
     127,    33,    32,    84,    89,    90,    91,    92,    93,    94,
      95,    96,    84,    97,    98,   125,    15,    33,    84,   128,
      33,   133,     3,     4,     5,     6,    16,    18,   128,   130,
     129,   131,   132,    17,   134,     9,    16,   128,   128,   128,
     128,    33,    10,    33,    18
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 278 "nds32-aie-parser.y"
    { printf("Warning! The aie file is empty!\n"); }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 280 "nds32-aie-parser.y"
    {
           if (!check_aie_register_rd_wr_insn()) {
               YYERROR;
           }
        }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 304 "nds32-aie-parser.y"
    {
        if (!init_aie_cop(&cop)) {
            YYERROR;
        }
    }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 310 "nds32-aie-parser.y"
    {
        if (!add_cop_cpid(cop, (yyvsp[(3) - (4)].value))) { YYERROR; }
    }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 314 "nds32-aie-parser.y"
    {
        if (!add_cop_etrap(cop, (yyvsp[(3) - (4)].text))) { YYERROR; }
    }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 318 "nds32-aie-parser.y"
    {
        if (!add_cop_ltrap(cop, (yyvsp[(3) - (4)].text))) { YYERROR; }
    }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 322 "nds32-aie-parser.y"
    {
        if (!add_cop_d2c(cop, (yyvsp[(3) - (4)].text))) { YYERROR; }
    }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 326 "nds32-aie-parser.y"
    {
        if (!add_cop_d2m(cop, (yyvsp[(3) - (4)].text))) { YYERROR; }
    }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 333 "nds32-aie-parser.y"
    {
        if (!add_cop_stage(cop, (yyvsp[(1) - (1)].text))) {
            YYERROR;
        }
    }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 345 "nds32-aie-parser.y"
    {is_pseudo = pseudo;}
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 346 "nds32-aie-parser.y"
    {is_pseudo = partial_pseudo;}
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 347 "nds32-aie-parser.y"
    {is_pseudo = physical;}
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 351 "nds32-aie-parser.y"
    {
        yyerror("specify trivial register: `%s'", (yyvsp[(2) - (5)].text));
        YYERROR;
    }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 356 "nds32-aie-parser.y"
    {
        is_trivial_register = 1;
        is_define_pseudo_register_access_macro = 0;
        if (!add_aie_register(&aie_register_tmp, is_pseudo, (yyvsp[(2) - (3)].text))) {
            YYERROR;
        }
    }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 362 "nds32-aie-parser.y"
    {
        if (is_trivial_register) {
            yyerror("specify trivial register: `%s'", (yyvsp[(2) - (7)].text));
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

    }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 410 "nds32-aie-parser.y"
    {
        yyerror("only support register attribute: regnum, width, sym_map, reg_map macro");
        YYERROR;
    }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 418 "nds32-aie-parser.y"
    {
        is_trivial_register = 0;
        if (!add_aie_register_width(aie_register_tmp, (yyvsp[(3) - (3)].value))) {
            YYERROR;
        }
    }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 427 "nds32-aie-parser.y"
    {
        is_trivial_register = 0;
        if (!add_aie_register_regnum(aie_register_tmp, (yyvsp[(3) - (3)].value))) {
            YYERROR;
        }
    }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 436 "nds32-aie-parser.y"
    {
        is_trivial_register = 0;
        if (!add_aie_register_rd_insn(aie_register_tmp, (yyvsp[(3) - (3)].text))) {
            YYERROR;
        }
    }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 445 "nds32-aie-parser.y"
    {
        is_trivial_register = 0;
        if (!add_aie_register_wr_insn(aie_register_tmp, (yyvsp[(3) - (3)].text))) {
            YYERROR;
        }
    }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 454 "nds32-aie-parser.y"
    {
        is_trivial_register = 0;
        if (!add_aie_register_sym_val_mapping(aie_register_tmp, (yyvsp[(3) - (3)].text))) {
            YYERROR;
        }
    }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 463 "nds32-aie-parser.y"
    {
        is_trivial_register = 0;

        if (physical == aie_register_tmp->is_pseudo) {
            yyerror("try to define register access macro for non-pseudo register: `%s'", aie_register_tmp->symbol);
            YYERROR;
        }

        if (0 != strcmp (aie_register_tmp->symbol, (yyvsp[(1) - (1)].text))) {
            yyerror("invalid naming of pseudo register access macro `%s', %s is required", (yyvsp[(1) - (1)].text), aie_register_tmp->symbol);
            YYERROR;
        }
    }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 476 "nds32-aie-parser.y"
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
    }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 532 "nds32-aie-parser.y"
    {
        if (!aie_strcpy(aie_register_tmp->index_symbol, (yyvsp[(1) - (1)].text))) {
            yyerror("try to use unsupported index `%s'", (yyvsp[(1) - (1)].text));
            YYERROR;
        }
    }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 539 "nds32-aie-parser.y"
    {
        if (!aie_strcpy(aie_register_tmp->index_symbol, (yyvsp[(1) - (1)].text))) {
            yyerror("try to use unsupported index `%s'", (yyvsp[(1) - (1)].text));
            YYERROR;
        }
    }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 545 "nds32-aie-parser.y"
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
    }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 569 "nds32-aie-parser.y"
    {
        int i = 0;
        for(i = (yyvsp[(1) - (3)].value); i<= (yyvsp[(3) - (3)].value); i++) {
            if (!add_pseudo_reg_idx(aie_register_tmp, i)) {
                YYERROR;
            }
        }
    }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 578 "nds32-aie-parser.y"
    {
        if (!add_pseudo_reg_idx(aie_register_tmp, (yyvsp[(1) - (1)].value))) {
            YYERROR;
        }
    }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 588 "nds32-aie-parser.y"
    {
        aie_register* tmp_reg = is_aie_register_exist((yyvsp[(1) - (2)].text));
        if (!tmp_reg || 0 == strcmp(aie_register_tmp->symbol, (yyvsp[(1) - (2)].text))) {
            yyerror("undefined or self reference to register: `%s'", (yyvsp[(1) - (2)].text));
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

    }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 611 "nds32-aie-parser.y"
    {
        //update dependent physical register
        aie_register_tmp->num_dependency++;
    }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 617 "nds32-aie-parser.y"
    { add_stackop_token(pseudo_reg_tmp, STACKOP_DIGIT, (yyvsp[(1) - (1)].value)); }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 619 "nds32-aie-parser.y"
    { add_stackop_token(pseudo_reg_tmp, STACKOP_POSTIVE, 0); }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 622 "nds32-aie-parser.y"
    { add_stackop_token(pseudo_reg_tmp, STACKOP_NEGATIVE, 0); }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 625 "nds32-aie-parser.y"
    { add_stackop_token(pseudo_reg_tmp, STACKOP_MUL, 0); }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 628 "nds32-aie-parser.y"
    { add_stackop_token(pseudo_reg_tmp, STACKOP_DIV, 0); }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 631 "nds32-aie-parser.y"
    { add_stackop_token(pseudo_reg_tmp, STACKOP_LPAREN, 0); }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 632 "nds32-aie-parser.y"
    { add_stackop_token(pseudo_reg_tmp, STACKOP_RPAREN, 0); }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 635 "nds32-aie-parser.y"
    {
        if (0 != strcmp(aie_register_tmp->index_symbol, (yyvsp[(1) - (1)].text))) {
            yyerror("register access macro index: `%s' is not defined, %s is required ", (yyvsp[(1) - (1)].text), aie_register_tmp->index_symbol);
            YYERROR;
        }
        add_stackop_token(pseudo_reg_tmp, STACKOP_IDX, 0);
    }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 645 "nds32-aie-parser.y"
    {
        //physical register width
        int physical_reg_width = pseudo_reg_tmp->physical_reg->width;

        //check whether the range is out of corresponding physical reigster width
        if ((yyvsp[(2) - (5)].value) < (signed int )0 || (yyvsp[(2) - (5)].value) > (physical_reg_width - 1)) {
            yyerror("Bit `%d' is out of range, %s[%d:%d]", (yyvsp[(2) - (5)].value), pseudo_reg_tmp->physical_reg->symbol, physical_reg_width - 1, 0);
            YYERROR;
        }

        //check whether the range is out of corresponding physical reigster width
        if ((yyvsp[(4) - (5)].value) < (signed int)0 || (yyvsp[(4) - (5)].value) > (physical_reg_width - 1)) {
            yyerror("Bit `%d' is out of range, %s[%d:%d]", (yyvsp[(4) - (5)].value), pseudo_reg_tmp->physical_reg->symbol, physical_reg_width - 1, 0);
            YYERROR;
        }
        if ((yyvsp[(2) - (5)].value) < (yyvsp[(4) - (5)].value)) {
            yyerror("%s[%d:%d] is invalid, high<->low bit order is required", aie_register_tmp->symbol, (yyvsp[(2) - (5)].value), (yyvsp[(4) - (5)].value));
            YYERROR;
        }

        //update pyhsical reigster bit mask and bit shift
        pseudo_reg_tmp->mask = (yyvsp[(2) - (5)].value);
        pseudo_reg_tmp->shift = ((yyvsp[(2) - (5)].value) - (yyvsp[(4) - (5)].value) + 1);
    }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 670 "nds32-aie-parser.y"
    {
        //update pyhsical reigster bit mask and bit shift
        pseudo_reg_tmp->mask =
        (64 == pseudo_reg_tmp->physical_reg->width)? 0xffffffffffffffffLL :
        (32 == pseudo_reg_tmp->physical_reg->width) ? 0x00000000ffffffffLL :
        (16 == pseudo_reg_tmp->physical_reg->width) ? 0x000000000000ffffLL : 0x00000000000000ffLL;
        pseudo_reg_tmp->shift = pseudo_reg_tmp->physical_reg->width;
    }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 684 "nds32-aie-parser.y"
    {
        yyerror("specify trivial symbol_value_map: `%s'", (yyvsp[(2) - (5)].text));
        YYERROR;
    }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 689 "nds32-aie-parser.y"
    {
        //is this is a trivial_statement
        is_trivial_sym_val_map = 1;

        if (!add_sym_val_map(&sym_val_map_tmp, (yyvsp[(2) - (3)].text))) {
            YYERROR;
        }
    }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 697 "nds32-aie-parser.y"
    {
        if (is_trivial_sym_val_map) {
            yyerror("specify trivial symbol_value_map: `%s'", (yyvsp[(2) - (7)].text));
            YYERROR;
        }
    }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 707 "nds32-aie-parser.y"
    {
        if (!add_sym_val(sym_val_map_tmp, (yyvsp[(1) - (4)].text), (yyvsp[(3) - (4)].value))) {
            YYERROR;
        }
        is_trivial_sym_val_map = 0;
    }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 714 "nds32-aie-parser.y"
    {
        //[i...j] = [k...l]
        if (!aie_strcpy(eval_token, (yyvsp[(1) - (14)].text)))
            YYERROR;

        gi = (yyvsp[(3) - (14)].value);
        gj = (yyvsp[(5) - (14)].value);
        k = (yyvsp[(10) - (14)].value);
        l = (yyvsp[(12) - (14)].value);

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
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 812 "nds32-aie-parser.y"
    {
        if(!aie_strcpy(additional_string, (yyvsp[(1) - (1)].text))) {
            YYERROR;
        }
    }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 818 "nds32-aie-parser.y"
    {
        additional_string[0] = '\0';
    }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 827 "nds32-aie-parser.y"
    {
        yyerror("define trivial typedef: `%s'", (yyvsp[(3) - (6)].text));
        YYERROR;
    }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 832 "nds32-aie-parser.y"
    {
        is_trivial_typedef = 1;

        if (!add_aie_itype(&itype_tmp, (yyvsp[(3) - (4)].text))) {
            YYERROR;
        }

        //Set the following reference is itype_tmp not insn_tmp
        set_common_format = 1;

    }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 843 "nds32-aie-parser.y"
    {
        set_common_format = 0;

        if (is_trivial_typedef) {
            yyerror("define trivial typedef: `%s'", (yyvsp[(3) - (8)].text));
            YYERROR;
        }
    }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 854 "nds32-aie-parser.y"
    { is_trivial_typedef = 0; }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 855 "nds32-aie-parser.y"
    { is_trivial_typedef = 0; }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 856 "nds32-aie-parser.y"
    { is_trivial_typedef = 0; }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 857 "nds32-aie-parser.y"
    { is_trivial_typedef = 0; }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 858 "nds32-aie-parser.y"
    { is_trivial_typedef = 0; }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 859 "nds32-aie-parser.y"
    { is_trivial_typedef = 0; }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 860 "nds32-aie-parser.y"
    { is_trivial_typedef = 0; }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 861 "nds32-aie-parser.y"
    { is_trivial_typedef = 0; }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 864 "nds32-aie-parser.y"
    {
        yyerror("invalid attribute: `%s', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", (yyvsp[(1) - (1)].text));
        YYERROR;
    }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 869 "nds32-aie-parser.y"
    {
        yyerror("invalid attribute: `%d', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", (yyvsp[(1) - (1)].value));
        YYERROR;
    }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 874 "nds32-aie-parser.y"
    {
        yyerror("invalid attribute, support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only");
        YYERROR;
    }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 885 "nds32-aie-parser.y"
    {
        itype_tmp = is_aie_itype_exist((yyvsp[(1) - (2)].text));

        if (!itype_tmp) {
            yyerror("undefined reference to new instruction type: `%s'", (yyvsp[(1) - (2)].text));
            YYERROR;
        }

        if (is_multiple_aie_insn_create((yyvsp[(2) - (2)].text))) {
            yyerror("`%s' is already decalared", (yyvsp[(2) - (2)].text));
            YYERROR;
        }

        if (!add_aie_insn(&insn_tmp, itype_tmp, (yyvsp[(2) - (2)].text))) {
            YYERROR;
        }
    }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 902 "nds32-aie-parser.y"
    {}
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 905 "nds32-aie-parser.y"
    {
        if (is_multiple_aie_insn_create((yyvsp[(1) - (1)].text))) {
            yyerror("`%s' is already decalared", (yyvsp[(1) - (1)].text));
            YYERROR;
        }

        if (!add_aie_insn(&insn_tmp, itype_tmp, (yyvsp[(1) - (1)].text))) {
            YYERROR;
        }
    }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 919 "nds32-aie-parser.y"
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier");
        YYERROR;
    }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 924 "nds32-aie-parser.y"
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier: `%d'", (yyvsp[(1) - (1)].value));
        YYERROR;
    }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 929 "nds32-aie-parser.y"
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier: `%s'", (yyvsp[(1) - (1)].text));
        YYERROR;
    }
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 954 "nds32-aie-parser.y"
    {
        insn_tmp = is_aie_insn_valid((yyvsp[(1) - (2)].text));
        if (!insn_tmp) {
            yyerror("instruction type is not defined before declare new instruction: `%s'", (yyvsp[(1) - (2)].text));
            YYERROR;
        }
    }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 963 "nds32-aie-parser.y"
    { insn_tmp->can_be_redef_group = 0; }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 965 "nds32-aie-parser.y"
    { insn_tmp->can_be_redef_format = 0; }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 966 "nds32-aie-parser.y"
    { insn_tmp->can_be_redef_syntax = 0; }
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 972 "nds32-aie-parser.y"
    {
        yyerror("invalid attribute: `%s', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", (yyvsp[(1) - (1)].text));
        YYERROR;
    }
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 977 "nds32-aie-parser.y"
    {
        yyerror("invalid attribute: `%d', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", (yyvsp[(1) - (1)].value));
        YYERROR;
    }
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 982 "nds32-aie-parser.y"
    {
        yyerror("invalid attribute, support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only");
        YYERROR;
    }
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 992 "nds32-aie-parser.y"
    {
        yyerror("define trivial insn: `%s'", (yyvsp[(2) - (5)].text));
        YYERROR;
    }
    break;

  case 219:

/* Line 1806 of yacc.c  */
#line 997 "nds32-aie-parser.y"
    {
        is_trivial_def_insn = 1;

        if (is_multiple_aie_insn_create((yyvsp[(2) - (3)].text))) {
            yyerror("insn `%s' is already decalared", (yyvsp[(2) - (3)].text));
            YYERROR;
        }

        if (!define_aie_insn(&insn_tmp, (yyvsp[(2) - (3)].text))) {
            YYERROR;
        }
    }
    break;

  case 220:

/* Line 1806 of yacc.c  */
#line 1009 "nds32-aie-parser.y"
    {
        if (is_trivial_def_insn) {
            yyerror("define trivial insn: `%s'", (yyvsp[(2) - (7)].text));
            YYERROR;
        }
    }
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 1018 "nds32-aie-parser.y"
    { is_trivial_def_insn = 0; }
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 1019 "nds32-aie-parser.y"
    { is_trivial_def_insn = 0; }
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 1020 "nds32-aie-parser.y"
    { is_trivial_def_insn = 0; }
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 1021 "nds32-aie-parser.y"
    { is_trivial_def_insn = 0; }
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 1022 "nds32-aie-parser.y"
    { is_trivial_def_insn = 0; }
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 1023 "nds32-aie-parser.y"
    { is_trivial_def_insn = 0; }
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 1024 "nds32-aie-parser.y"
    { is_trivial_def_insn = 0; }
    break;

  case 230:

/* Line 1806 of yacc.c  */
#line 1025 "nds32-aie-parser.y"
    { is_trivial_def_insn = 0; }
    break;

  case 232:

/* Line 1806 of yacc.c  */
#line 1028 "nds32-aie-parser.y"
    {
        yyerror("invalid insn attribute: `%s', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", (yyvsp[(1) - (1)].text));
        YYERROR;
    }
    break;

  case 233:

/* Line 1806 of yacc.c  */
#line 1033 "nds32-aie-parser.y"
    {
        yyerror("invalid insn attribute: `%d', support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only", (yyvsp[(1) - (1)].value));
        YYERROR;
    }
    break;

  case 234:

/* Line 1806 of yacc.c  */
#line 1038 "nds32-aie-parser.y"
    {
        yyerror("invalid insn attribute, support group, format, syntax, decode, pstage, cstage, ssstage or sscycle only");
        YYERROR;
    }
    break;

  case 320:

/* Line 1806 of yacc.c  */
#line 1060 "nds32-aie-parser.y"
    {
        yyerror("specify trivial `format' in %s", gtmp->name);
        YYERROR;
    }
    break;

  case 321:

/* Line 1806 of yacc.c  */
#line 1065 "nds32-aie-parser.y"
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
    }
    break;

  case 322:

/* Line 1806 of yacc.c  */
#line 1094 "nds32-aie-parser.y"
    {

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
    }
    break;

  case 326:

/* Line 1806 of yacc.c  */
#line 1139 "nds32-aie-parser.y"
    {}
    break;

  case 327:

/* Line 1806 of yacc.c  */
#line 1142 "nds32-aie-parser.y"
    {                                         //                   2). reg:5:pos:imm_align_type
        unsigned int shf_mnt = (yyvsp[(5) - (6)].value);
        unsigned int bit_width = (yyvsp[(3) - (6)].value);
        unsigned int valid_value = 0;
        unsigned int valid = 0;

        if (!add_ifield(gtmp, (yyvsp[(1) - (6)].text), align_type, aie_s, shf_mnt, bit_width, valid_value, valid)) {
            YYERROR;
        }
        is_trivial_format = 0;
    }
    break;

  case 328:

/* Line 1806 of yacc.c  */
#line 1154 "nds32-aie-parser.y"
    {
        unsigned int shf_mnt = (yyvsp[(5) - (5)].value);
        unsigned int bit_width = (yyvsp[(3) - (5)].value);
        unsigned int valid_value = (yyvsp[(1) - (5)].value);
        unsigned int valid = 1;

        if (!add_ifield(gtmp, NULL, aie_ub, aie_s, shf_mnt, bit_width, valid_value, valid)) {
            YYERROR;
        }
        is_trivial_format = 0;
    }
    break;

  case 329:

/* Line 1806 of yacc.c  */
#line 1171 "nds32-aie-parser.y"
    {
        if (1 == set_common_format) {
            yyerror("only group, format are support within typedef");
            YYERROR;
        }
        is_trivial_decode = 1;
    }
    break;

  case 331:

/* Line 1806 of yacc.c  */
#line 1182 "nds32-aie-parser.y"
    {
        yyerror("specify trivial `decode' in %s", insn_tmp->name);
        YYERROR;
    }
    break;

  case 332:

/* Line 1806 of yacc.c  */
#line 1187 "nds32-aie-parser.y"
    {
        if (is_trivial_decode) {
            yyerror("specify trivial `decode' in %s", gtmp->name);
            YYERROR;
        }
    }
    break;

  case 335:

/* Line 1806 of yacc.c  */
#line 1197 "nds32-aie-parser.y"
    {
        if (!add_insn_decode(insn_tmp, (yyvsp[(1) - (3)].text), (yyvsp[(3) - (3)].value))) {
            YYERROR;
        }
        is_trivial_decode = 0;
    }
    break;

  case 336:

/* Line 1806 of yacc.c  */
#line 1209 "nds32-aie-parser.y"
    {
        if (1 == set_common_format) {
            yyerror("only group, format are support within typedef");
            YYERROR;
        }
    }
    break;

  case 337:

/* Line 1806 of yacc.c  */
#line 1216 "nds32-aie-parser.y"
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
    }
    break;

  case 338:

/* Line 1806 of yacc.c  */
#line 1273 "nds32-aie-parser.y"
    {
        if (!check_syntax_string(insn_tmp, (yyvsp[(1) - (1)].text))) {
            YYERROR;
        }
    }
    break;

  case 339:

/* Line 1806 of yacc.c  */
#line 1278 "nds32-aie-parser.y"
    {
        if (0 != insn_tmp->syntax_symbol_count) {
            yyerror("syntax error! in field list, there is not corresponding field to customized operand");
            YYERROR;
        }
    }
    break;

  case 341:

/* Line 1806 of yacc.c  */
#line 1285 "nds32-aie-parser.y"
    {}
    break;

  case 342:

/* Line 1806 of yacc.c  */
#line 1288 "nds32-aie-parser.y"
    {
        //actually syntax symbol is 0
        if (0 == insn_tmp->syntax_symbol_count) {
            yyerror("syntax error due to unmapped operand from %s", (yyvsp[(2) - (2)].text));
            YYERROR;
        }

        //support overwriting alignment/register type attribute, need to preset from old value
        aie_ifield* it = is_ifield_exist(insn_tmp, (yyvsp[(2) - (2)].text));
        if (NULL == it) {
            yyerror("try to resolve `%s' before define `%s.format.%s'",(yyvsp[(2) - (2)].text), insn_tmp->name, (yyvsp[(2) - (2)].text));
            YYERROR;
        }

        align_type = it->imm_align_type;
        rtype = it->register_type;
    }
    break;

  case 343:

/* Line 1806 of yacc.c  */
#line 1306 "nds32-aie-parser.y"
    {
        if (!find_ifield_for_symbol_stok((yyvsp[(2) - (4)].text), insn_tmp, align_type, rtype)) {
            YYERROR;
        }
    }
    break;

  case 344:

/* Line 1806 of yacc.c  */
#line 1317 "nds32-aie-parser.y"
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
    }
    break;

  case 345:

/* Line 1806 of yacc.c  */
#line 1339 "nds32-aie-parser.y"
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (!add_insn_pstage(gtmp, (yyvsp[(3) - (4)].text))) {
            YYERROR;
        }
    }
    break;

  case 346:

/* Line 1806 of yacc.c  */
#line 1356 "nds32-aie-parser.y"
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (!add_insn_cstage(gtmp, (yyvsp[(3) - (4)].text))) {
            YYERROR;
        }
    }
    break;

  case 347:

/* Line 1806 of yacc.c  */
#line 1373 "nds32-aie-parser.y"
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (!add_insn_sscycle(gtmp, (yyvsp[(3) - (4)].value))) {
            YYERROR;
        }
    }
    break;

  case 348:

/* Line 1806 of yacc.c  */
#line 1391 "nds32-aie-parser.y"
    {
        //select proper chain to be added (itype chain or sepcific insn chain)
        if (set_common_format)
            gtmp = itype_tmp;
        else
            gtmp = insn_tmp;

        if (!add_insn_ssstage(gtmp, (yyvsp[(3) - (4)].text))) {
            YYERROR;
        }
    }
    break;

  case 349:

/* Line 1806 of yacc.c  */
#line 1404 "nds32-aie-parser.y"
    {g = AIE_CPE1;}
    break;

  case 350:

/* Line 1806 of yacc.c  */
#line 1405 "nds32-aie-parser.y"
    {g = AIE_CPE2;}
    break;

  case 351:

/* Line 1806 of yacc.c  */
#line 1406 "nds32-aie-parser.y"
    {g = AIE_CPE3;}
    break;

  case 352:

/* Line 1806 of yacc.c  */
#line 1407 "nds32-aie-parser.y"
    {g = AIE_CPE4;}
    break;

  case 353:

/* Line 1806 of yacc.c  */
#line 1408 "nds32-aie-parser.y"
    {g = AIE_MFCPD;}
    break;

  case 354:

/* Line 1806 of yacc.c  */
#line 1409 "nds32-aie-parser.y"
    {g = AIE_MTCPD;}
    break;

  case 355:

/* Line 1806 of yacc.c  */
#line 1410 "nds32-aie-parser.y"
    {g = AIE_MFCPW;}
    break;

  case 356:

/* Line 1806 of yacc.c  */
#line 1411 "nds32-aie-parser.y"
    {g = AIE_MTCPW;}
    break;

  case 357:

/* Line 1806 of yacc.c  */
#line 1412 "nds32-aie-parser.y"
    {g = AIE_MFCPPW;}
    break;

  case 358:

/* Line 1806 of yacc.c  */
#line 1413 "nds32-aie-parser.y"
    {g = AIE_MTCPPW;}
    break;

  case 359:

/* Line 1806 of yacc.c  */
#line 1414 "nds32-aie-parser.y"
    {g = AIE_CPLD;}
    break;

  case 360:

/* Line 1806 of yacc.c  */
#line 1415 "nds32-aie-parser.y"
    {g = AIE_CPSD;}
    break;

  case 361:

/* Line 1806 of yacc.c  */
#line 1416 "nds32-aie-parser.y"
    {g = AIE_CPLW;}
    break;

  case 362:

/* Line 1806 of yacc.c  */
#line 1417 "nds32-aie-parser.y"
    {g = AIE_CPSW;}
    break;

  case 363:

/* Line 1806 of yacc.c  */
#line 1418 "nds32-aie-parser.y"
    {g = AIE_LDC;}
    break;

  case 364:

/* Line 1806 of yacc.c  */
#line 1419 "nds32-aie-parser.y"
    {g = AIE_LWC;}
    break;

  case 365:

/* Line 1806 of yacc.c  */
#line 1420 "nds32-aie-parser.y"
    {g = AIE_SDC;}
    break;

  case 366:

/* Line 1806 of yacc.c  */
#line 1421 "nds32-aie-parser.y"
    {g = AIE_SWC;}
    break;

  case 367:

/* Line 1806 of yacc.c  */
#line 1422 "nds32-aie-parser.y"
    {g = AIE_CPLD_BI;}
    break;

  case 368:

/* Line 1806 of yacc.c  */
#line 1423 "nds32-aie-parser.y"
    {g = AIE_CPSD_BI;}
    break;

  case 369:

/* Line 1806 of yacc.c  */
#line 1424 "nds32-aie-parser.y"
    {g = AIE_CPLW_BI;}
    break;

  case 370:

/* Line 1806 of yacc.c  */
#line 1425 "nds32-aie-parser.y"
    {g = AIE_CPSW_BI;}
    break;

  case 371:

/* Line 1806 of yacc.c  */
#line 1426 "nds32-aie-parser.y"
    {g = AIE_LDC_BI;}
    break;

  case 372:

/* Line 1806 of yacc.c  */
#line 1427 "nds32-aie-parser.y"
    {g = AIE_LWC_BI;}
    break;

  case 373:

/* Line 1806 of yacc.c  */
#line 1428 "nds32-aie-parser.y"
    {g = AIE_SDC_BI;}
    break;

  case 374:

/* Line 1806 of yacc.c  */
#line 1429 "nds32-aie-parser.y"
    {g = AIE_SWC_BI;}
    break;

  case 375:

/* Line 1806 of yacc.c  */
#line 1431 "nds32-aie-parser.y"
    {
        yyerror("invalid group: `%s', support:", (yyvsp[(1) - (1)].text));
        yyerror("CPE1, CPE2, CPE3, CPE4, MFCPD, MTCPD, MFCPW, MTCPW, MFCPPW, MTCPPW,");
        yyerror("CPLD, CPSD, CPLW, CPSW, LDC, LWC, SDC, SWC, CPLD_BI, CPSD_BI,");
        yyerror("CPLW_BI, CPSW_BI, LDC_BI, LWC_BI, SDC_BI and SWC_BI");
        YYERROR;
    }
    break;

  case 376:

/* Line 1806 of yacc.c  */
#line 1440 "nds32-aie-parser.y"
    {align_type = aie_sd;}
    break;

  case 377:

/* Line 1806 of yacc.c  */
#line 1441 "nds32-aie-parser.y"
    {align_type = aie_sw;}
    break;

  case 378:

/* Line 1806 of yacc.c  */
#line 1442 "nds32-aie-parser.y"
    {align_type = aie_sh;}
    break;

  case 379:

/* Line 1806 of yacc.c  */
#line 1443 "nds32-aie-parser.y"
    {align_type = aie_sb;}
    break;

  case 380:

/* Line 1806 of yacc.c  */
#line 1444 "nds32-aie-parser.y"
    {align_type = aie_ud;}
    break;

  case 381:

/* Line 1806 of yacc.c  */
#line 1445 "nds32-aie-parser.y"
    {align_type = aie_uw;}
    break;

  case 382:

/* Line 1806 of yacc.c  */
#line 1446 "nds32-aie-parser.y"
    {align_type = aie_uh;}
    break;

  case 383:

/* Line 1806 of yacc.c  */
#line 1447 "nds32-aie-parser.y"
    {align_type = aie_ub;}
    break;

  case 384:

/* Line 1806 of yacc.c  */
#line 1449 "nds32-aie-parser.y"
    {
        yyerror("invalid field descriptor: `%s', support sd, sw, sh, sb, ud, uw, uh and ub only", (yyvsp[(2) - (2)].text));
        YYERROR;
    }
    break;

  case 385:

/* Line 1806 of yacc.c  */
#line 1453 "nds32-aie-parser.y"
    {align_type = aie_ub;}
    break;

  case 386:

/* Line 1806 of yacc.c  */
#line 1456 "nds32-aie-parser.y"
    { rtype = aie_s; }
    break;

  case 387:

/* Line 1806 of yacc.c  */
#line 1457 "nds32-aie-parser.y"
    { rtype = aie_d; }
    break;

  case 388:

/* Line 1806 of yacc.c  */
#line 1459 "nds32-aie-parser.y"
    {
        yyerror("invalid operand descriptor `%s', support s and d", (yyvsp[(2) - (2)].text));
        YYERROR;
    }
    break;

  case 389:

/* Line 1806 of yacc.c  */
#line 1464 "nds32-aie-parser.y"
    {
        rtype = aie_s;
    }
    break;

  case 486:

/* Line 1806 of yacc.c  */
#line 1503 "nds32-aie-parser.y"
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier: `%s'", (yyvsp[(1) - (1)].text));
        YYERROR;
    }
    break;

  case 487:

/* Line 1806 of yacc.c  */
#line 1508 "nds32-aie-parser.y"
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier");
        YYERROR;
    }
    break;

  case 488:

/* Line 1806 of yacc.c  */
#line 1513 "nds32-aie-parser.y"
    {
        yyerror("use invalid char, reserved word, integer or too long string as the identifier: `%d'", (yyvsp[(1) - (1)].value));
        YYERROR;
    }
    break;



/* Line 1806 of yacc.c  */
#line 4349 "nds32-aie-parser.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 1519 "nds32-aie-parser.y"


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

