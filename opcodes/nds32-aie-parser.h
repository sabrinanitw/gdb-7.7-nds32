/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
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

/* Line 2068 of yacc.c  */
#line 117 "nds32-aie-parser.y"

 char* text;
 int value;



/* Line 2068 of yacc.c  */
#line 255 "nds32-aie-parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE nds32_aie_lval;


