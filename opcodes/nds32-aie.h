#ifndef NDS32_AIE_H
#define NDS32_AIE_H

#include "nds32-aie-typedef-insn.h"

/* yyparse  */
char *nds32_aie_translator (char *s);
int nds32_aie_dislator (unsigned int immediate, char *aie_insn,
			unsigned int cpid, group g);
int nds32_aie_parse (void);
int check_aie_register_rd_wr_insn (void);

extern FILE *nds32_aie_scanner_in;

#endif
