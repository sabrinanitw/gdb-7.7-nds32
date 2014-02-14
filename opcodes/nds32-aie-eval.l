/*
    Andes Coprocessor Extension Language Syntax for GNU Flex
*/
%{
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "nds32-aie-utils.h"

extern char eval_syntax_line[MAX_SCAN_LINE_SIZE];
extern char eval_token[MAX_STR_CONST];
extern unsigned int eval_uint;

static int return_state = 0;

token_type nds32_aie_eval_lex(void);

#ifndef YY_INPUT
#define YY_INPUT(buf, result, max_size) \
    strncpy(buf, eval_syntax_line, max_size); \
    result = max_size;
#endif

#define YY_DECL token_type nds32_aie_eval_lex ()

%}
%x HEX

%option noyywrap
%option nounput
%option noinput



%%

 /********************************************/
 /* Regular expressions for primitive tokens */
 /********************************************/


<INITIAL>"%"[_a-zA-Z][_a-zA-Z0-9]+ {

    /* we should skip first "%" */
    if (!aie_strcpy(eval_token, yytext+1)) {
        return ERROR;
    }

    return SYMBOL;
}

<INITIAL>"%" {
    if (!aie_strcpy(eval_token, yytext)) {
        return ERROR;
    }

    return SKIP_SYMBOL;
}

<INITIAL>[ \t\r]+ {
    if (!aie_strcpy(eval_token, yytext)) {
        return ERROR;
    }
    return SPACE;
}

 /********************************************/
 /* Regular expressions for general char     */
 /********************************************/

<INITIAL,HEX>"\0" {
    return EOL;
}

<INITIAL>"0x"|"0X" {
    return_state = YY_START;
    BEGIN(HEX);
}

<HEX>[0-9A-Fa-f]* {
    unsigned long int tmp = 0;

    if (!aie_strtoul(yytext, 16, &tmp, 0)) {
        AIE_PPRINT(aie_error, "Can't not conver `%s' to 'unsigned long int', possible it is not a representable value", yytext);
        if (!aie_strcpy(eval_token, yytext)) {
            return ERROR;
        }
        return ERROR;
    }

    eval_uint = (unsigned int)tmp;

    BEGIN(return_state);

    return DIGIT;
}

<INITIAL>[0-9]* {
    unsigned long int tmp = 0;
    if (!aie_strtoul(yytext, 0, &tmp, 0)) {
        AIE_PPRINT(aie_error, "Can't not conver `%s' to 'unsigned long int', possible it is not a representable value", yytext);
        if (!aie_strcpy(eval_token, yytext)) {
            return ERROR;
        }
        return ERROR;
    }

    eval_uint = (unsigned int)tmp;

    return DIGIT;
}

<INITIAL>"-" {
    if (!aie_strcpy(eval_token, yytext)) {
        return ERROR;
    }
    return NEGATIVE;
}

<INITIAL>"+" {
    if (!aie_strcpy(eval_token, yytext)) {
        return ERROR;
    }
    return POSTIVE;
}

<INITIAL>"#" {
    if (!aie_strcpy(eval_token, yytext)) {
        return ERROR;
    }
    return CROSSING;
}


<INITIAL>\.\.+ {
    if (!aie_strcpy(eval_token, yytext)) {
        return ERROR;
    }
    return PPP;
}


<INITIAL>. {
    if (!aie_strcpy(eval_token, yytext)) {
        return ERROR;
    }
    return MYCHAR;
}

%%

#ifdef EVAL_MAIN
int main(void) {
    //aie_strcpy(eval_syntax_line, "aes128d %cpreg, %cpreg, %cpreg");
    //token_type status = syntax_scanner();

    //while(EOL != status && ERROR != status) {
    //    printf("%s\n",eval_token);

    //    status = syntax_scanner();
    //}

//<INITIAL>";" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"'" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"!" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"/*" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"*/" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"/" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"@" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"~" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"`" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"$" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"^" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"&" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}
//
//<INITIAL>"*" {
//    if (!aie_strcpy(eval_token, yytext)) {
//        return ERROR;
//    }
//    return ERROR;
//}

    return 0;
}
#endif
