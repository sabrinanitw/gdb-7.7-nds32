/*  
    Andes Coprocessor Extension Language Syntax for GNU Flex
*/
%{
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "nds32-aie-parser.h"

#include "nds32-aie-utils.h"

#define yylval          nds32_aie_lval

#define MAX_CODE_CONST  256
#define MAX_BLOCK_CONST  4096
#define MAX_INCLUDE_DEPTH 10

YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;
void debug_scanner_state(int state);
#define DSPRINT_STATE(state) debug_scanner_state(state) 
%}

DIGIT    [0-9]
ID       [_a-zA-Z][_a-zA-Z0-9]*
HEXA_DIGIT [0-9A-Fa-f]

%x COMMENTA
%x COMMENTB
%x STRING


%option noyywrap

%%

 char string_buf[MAX_STR_CONST];
 char* string_buf_ptr = string_buf;
 int return_state = YY_START;
 
 /********************************************/
 /* Regular expressions for primitive tokens */
 /********************************************/

<INITIAL>[ \t\r] {
    DSPRINT("Eating spaces: \"%s\"", yytext);
    DSPRINT_STATE(YY_START);
}

<INITIAL>"#include" {
    DSPRINT("Including file: %s", yytext);
    DSPRINT_STATE(YY_START);
    //BEGIN(INCLUDE);
}

<INITIAL>"%" {
    DSPRINT("(PERCENT): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return PERCENT;
}

<INITIAL>"," {
    DSPRINT("(COMMA): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return COMMA;
}

<INITIAL>";" {
    DSPRINT("(SEMICOLON): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SEMICOLON;
}

<INITIAL>":" {
    DSPRINT("(COLON): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return COLON;
}

<INITIAL>"-" {
    DSPRINT("(MINUS): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return MINUS;
}

<INITIAL>"+" {
    DSPRINT("(PLUS): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return PLUS;
}

<INITIAL>"*" {
    DSPRINT("(MUL): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return MUL;
}

<INITIAL>"/" {
    DSPRINT("(DIV): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return DIV;
}

<INITIAL>"." {
    DSPRINT("(DOT): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return DOT;
}

<INITIAL>"=" {
    DSPRINT("(EQUAL): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return EQ;
}

<INITIAL>"<" {
    DSPRINT("(LT): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LT;
}

<INITIAL>">" {
    DSPRINT("(GT): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return GT;
}

<INITIAL>"(" {
    DSPRINT("(LPAREN): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LPAREN;
}

<INITIAL>")" {
    DSPRINT("(RPAREN): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return RPAREN;
}

<INITIAL>"[" {
    DSPRINT("(LBRACK): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LBRACK;
}

<INITIAL>"]" {
    DSPRINT("(RBRACK): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return RBRACK;
}

<INITIAL>"{" {
    DSPRINT("(LBRACE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LBRACE;
}

<INITIAL>"}" {
    DSPRINT("(RBRACE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return RBRACE;
}

<INITIAL>"$" {
    DSPRINT("(DOLLAR): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return DOLLAR;
}

<INITIAL>"#" {
    DSPRINT("(CROSS): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CROSS;
}

<INITIAL>\.\.+ {
    DSPRINT("(PP): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return PP;
}

<INITIAL>! {
    DSPRINT("NOT: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return NOT;
}

<INITIAL>& {
    DSPRINT("logic and: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LOGIC_AND;
}

<INITIAL>"|" {
    DSPRINT("logic or: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LOGIC_OR;
}

<INITIAL>"@" {
    DSPRINT("AT: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return AT;
}

<INITIAL>"^" {
    DSPRINT("SQUARE: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SQUARE;
}

<INITIAL>"~" {
    DSPRINT("SLASH: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SLASH;
}

<INITIAL>"`" {
    DSPRINT("SDOT: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SDOT;
}

<INITIAL>"'" {
    DSPRINT("RDOT: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return RDOT;
}

 /********************************************/
 /* Regular expressions for AIE symbol value */
 /********************************************/

<INITIAL>"typedef" {
    DSPRINT("(TYPEDEF): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return TYPEDEF;
}

<INITIAL>"register" {
    DSPRINT("(REGISTER): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return REGISTER;
}

<INITIAL>"pseudo" {
    DSPRINT("(PSEUDO): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return PSEUDO;
}

<INITIAL>"partial_pseudo" {
    DSPRINT("(PARTIAL_PSEUDO): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return PARTIAL_PSEUDO;
}

<INITIAL>"width" {
    DSPRINT("(WIDTH): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return WIDTH;
}

<INITIAL>"regnum" {
    DSPRINT("(REGNUM): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return REGNUM;
}

<INITIAL>"rd_insn" {
   DSPRINT("(RD_INSN): %s", yytext);
   DSPRINT_STATE(YY_START);

   yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
   if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
   return RD_INSN;
}

<INITIAL>"wr_insn" {
   DSPRINT("(WR_INSN): %s", yytext);
   DSPRINT_STATE(YY_START);

   yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
   if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
   return WR_INSN;
}

<INITIAL>"sym_map" {
    DSPRINT("(SYM_MAP): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SYM_MAP;
}

<INITIAL>"symbol_value_map" {
    DSPRINT("(AIE_SYMBOL_VALUE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SYMBOL_VALUE_MAP;
}

<INITIAL>"insn" {
    DSPRINT("(AIE_INSN): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return INSN;
}


<INITIAL>"group" {
    DSPRINT("(GROUP): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return GROUP;
}

<INITIAL>"cpid" {
    DSPRINT("(CPID): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPID;
}

<INITIAL>"stage" {
    DSPRINT("(STAGE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return STAGE;
}

<INITIAL>"eack" {
    DSPRINT("(EACK): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return EACK;
}

<INITIAL>"lack" {
    DSPRINT("(LACK): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LACK;
}

<INITIAL>"d2c" {
    DSPRINT("(D2C): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return D2C;
}

<INITIAL>"d2m" {
    DSPRINT("(D2M): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return D2M;
}

<INITIAL>"format" {
    DSPRINT("(FORMAT): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return FORMAT;
}

<INITIAL>"syntax" {
    DSPRINT("(SYNTAX): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SYNTAX;
}

<INITIAL>"decode" {
    DSPRINT("(DECODE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return DECODE;
}
<INITIAL>"pstage" {
    DSPRINT("(PSTAGE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return PSTAGE;
}
<INITIAL>"cstage" {
    DSPRINT("(CSTAGE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CSTAGE;
}
<INITIAL>"ssstage" {
    DSPRINT("(SSSTAGE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SSSTAGE;
}
<INITIAL>"sscycle" {
    DSPRINT("(SSCYCLE): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SSCYCLE;
}

<INITIAL>CPE1 {
    DSPRINT("(CPE1): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPE1;
}

<INITIAL>CPE2 {
    DSPRINT("(CPE2): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPE2;
}

<INITIAL>CPE3 {
    DSPRINT("(CPE3): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPE3;
}

<INITIAL>CPE4 {
    DSPRINT("(CPE4): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPE4;
}

<INITIAL>MFCPD {
    DSPRINT("(MFCPD): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return MFCPD;
}

<INITIAL>MTCPD {
    DSPRINT("(MTCPD): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return MTCPD;
}

<INITIAL>MFCPW {
    DSPRINT("(MFCPW): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return MFCPW;
}

<INITIAL>MTCPW {
    DSPRINT("(MTCPW): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return MTCPW;
}

<INITIAL>MFCPPW {
    DSPRINT("(MFCPPW): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return MFCPPW;
}

<INITIAL>MTCPPW {
    DSPRINT("(MTCPPW): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return MTCPPW;
}

<INITIAL>CPLD_BI {
    DSPRINT("(CPLD_BI): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPLD_BI;
}

<INITIAL>CPSD_BI {
    DSPRINT("(CPSD_BI): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPSD_BI;
}

<INITIAL>CPLW_BI {
    DSPRINT("(CPLW_BI): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPLW_BI;
}

<INITIAL>CPSW_BI {
    DSPRINT("(CPSW_BI): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPSW_BI;
}

<INITIAL>LWC_BI {
    DSPRINT("(LWC_BI): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LWC_BI;
}

<INITIAL>SWC_BI {
    DSPRINT("(SWC_BI): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SWC_BI;
}

<INITIAL>LDC_BI {
    DSPRINT("(LDC_BI): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LDC_BI;
}

<INITIAL>SDC_BI {
    DSPRINT("(SDC_BI): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SDC_BI;
}

<INITIAL>CPLD {
    DSPRINT("(CPLD): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPLD;
}

<INITIAL>CPSD {
    DSPRINT("(CPSD): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPSD;
}

<INITIAL>CPLW {
    DSPRINT("(CPLW): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPLW;
}

<INITIAL>CPSW {
    DSPRINT("(CPSW): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return CPSW;
}

<INITIAL>LWC {
    DSPRINT("(LWC): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LWC;
}

<INITIAL>SWC {
    DSPRINT("(SWC): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SWC;
}

<INITIAL>LDC {
    DSPRINT("(LDC): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return LDC;
}

<INITIAL>SDC {
    DSPRINT("(SDC): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SDC;
}

<INITIAL>sd {
    DSPRINT("(sd): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SD;
}

<INITIAL>sw {
    DSPRINT("(sw): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SW;
}

<INITIAL>sh {
    DSPRINT("(sh): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SH;
}

<INITIAL>sb {
    DSPRINT("(sb): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SB;
}

<INITIAL>ud {
    DSPRINT("(ud): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return UD;
}

<INITIAL>uw {
    DSPRINT("(uw): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return UW;
}

<INITIAL>uh {
    DSPRINT("(uh): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return UH;
}

<INITIAL>ub {
    DSPRINT("(ub): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return UB;
}

<INITIAL>s {
    DSPRINT("(source operand): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SOURCE_OPERAND;
}

<INITIAL>d {
    DSPRINT("(destination operand): %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return DESTINATION_OPERAND;
}

 /***********************************/
 /* Regular expressions for Strings */
 /***********************************/

<INITIAL>\" {
    string_buf[0] = '\0';
    string_buf_ptr = string_buf;
    return_state = YY_START;

    BEGIN(STRING);

    DSPRINT("%s", yytext);
    DSPRINT_STATE(YY_START);
}

<STRING>\" {
    BEGIN(return_state);
    
    DSPRINT("%s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(string_buf) + 1));
    strcpy(yylval.text, string_buf);
    return STR;
}

<STRING>. {
    *string_buf_ptr++ = *yytext;
    *string_buf_ptr = '\0';

    DSPRINT("(STR): %s", string_buf);
    DSPRINT_STATE(YY_START);
}

<STRING>\n {
    *string_buf_ptr = '\0';
    AIE_SPRINT(aie_error, "Unterminate string constant:%s", string_buf);
    yyterminate();
}

<STRING><<EOF>> {
    *string_buf_ptr = '\0';
    AIE_SPRINT(aie_error, "Unexpected end of file");
    yyterminate();
}

 /********************************************/
 /* Regular expressions for comment stytle A */
 /********************************************/

<INITIAL>"//" {
    DSPRINT("Entering comment");

    return_state = YY_START;
    BEGIN(COMMENTA);
}

<COMMENTA>\n {
    INC_SLINE();
    BEGIN(return_state);

    DSPRINT("Leaving comment");
    DSPRINT_STATE(YY_START);
}

<COMMENTA>. {
    DSPRINT("%s", yytext);
    DSPRINT_STATE(YY_START);
}

<COMMENTA><<EOF>> {
    AIE_SPRINT(aie_error, "Unexpected end of file");
    yyterminate();
}

 /********************************************/
 /* Regular expressions for comment stytle B */
 /********************************************/

<INITIAL>"/*" {
    DSPRINT("Entering comment");
    return_state = YY_START;
    BEGIN(COMMENTB);
}

<COMMENTB>"*/" {
    BEGIN(return_state);

    DSPRINT("Leaving comment");
    DSPRINT_STATE(YY_START);
}

<COMMENTB>\n {
    INC_SLINE();
    DSPRINT("New line");
    DSPRINT_STATE(YY_START);
}

<COMMENTB>. {
    DSPRINT("%s", yytext);
    DSPRINT_STATE(YY_START);
}

<COMMENTB><<EOF>> {
    AIE_SPRINT(aie_error, "Unexpected end of file");
    yyterminate();
}

 /*---------------------------------*/
 /* Regular expressions for numbers */
 /*---------------------------------*/

<INITIAL>[0-9a-zA-Z_]+ {
    unsigned long int tmp = 0;
    
    //priority of dec is higher than hex
    if (aie_strtoul(yytext, 0, &tmp, 0)) {
        DSPRINT("(INT): %s = (%u)", yytext, tmp);
        DSPRINT_STATE(YY_START);
        yylval.value = (unsigned int)tmp;
        return INT;
    }
    else if ('0' == yytext[0] && ('x' == yytext[1] || 'X' == yytext[1]) && (aie_strtoul(yytext, 16, &tmp, 0))) {
        DSPRINT("(INT): %s = (%u)", yytext, tmp);
        DSPRINT_STATE(YY_START);
        yylval.value = (unsigned int)tmp;
        return INT;
    }
    else if ('0' == yytext[0] || '1' == yytext[0] || '2' == yytext[0] || '3' == yytext[0] || '4' == yytext[0] || '5' == yytext[0] || '6' == yytext[0] || '7' == yytext[0] || '8' == yytext[0] || '9' == yytext[0]) {
        AIE_SPRINT(aie_error, "Can't not conver `%s' to 'unsigned long int', possible it is not a representable value", yytext);
        yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
        if(!aie_strcpy(yylval.text, yytext)) { return INVALID_IMPRESENTABLE_TOKEN;};
        return INVALID_PRESENTABLE_TOKEN;
    }
    else {
        DSPRINT("(ID): %s", yytext);
        DSPRINT_STATE(YY_START);

        yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
        if(!aie_strcpy(yylval.text, yytext)) { return INVALID_IMPRESENTABLE_TOKEN;};
        return ID;

    }
}

<INITIAL>\n {
    DSPRINT("New line");
    DSPRINT_STATE(YY_START); 
    INC_SLINE();
}

<INITIAL>. {
    DSPRINT("Skip: %s", yytext);
    DSPRINT_STATE(YY_START);

    yylval.text = (char*) malloc(sizeof(char) * (strlen(yytext) + 1));
    if(!aie_strcpy(yylval.text, yytext)) { return INVALID_PRESENTABLE_TOKEN;};
    return SKIP;
}

%%

void debug_scanner_state(int state) {
    switch (state) {
        case COMMENTA:
            DSPRINT("SState: COMMENTA");
            break;
        case COMMENTB:
            DSPRINT("SState: COMMENTB");
            break;
        case STRING:
            DSPRINT("SState: STRING");
            break;
        case INITIAL:
            DSPRINT("SState: INITIAL");
            break;
        default:
            DSPRINT("SState: Uknown");
            break;
    }
}
