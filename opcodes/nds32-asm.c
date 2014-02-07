/* NDS32-specific support for 32-bit ELF.
   Copyright (C) 2012-2013 Free Software Foundation, Inc.
   Contributed by Andes Technology Corporation.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */


#include <config.h>

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "safe-ctype.h"
#include "libiberty.h"
#include "hashtab.h"
#include "bfd.h"

#include "opcode/nds32.h"
#include "nds32-asm.h"

/* There at at most MAX_LEX_NUM lexical elements in a syntax.  */
#define MAX_LEX_NUM		32
/* A operand in syntax string should be at most this long.  */
#define MAX_LEX_LEN		64//32
/* The max length of a keyword can be.  */
#define MAX_KEYWORD_LEN		32
/* This LEX is a plain char or operand.  */
#define IS_LEX_CHAR(c)		(((c) >> 7) == 0)
#define LEX_SET_FIELD(c)	((c) | SYN_FIELD)
#define LEX_GET_FIELD(c)	operand_fields[((c) & 0xff)]
/* Get the char in this lexical element.  */
#define LEX_CHAR(c)		((c) & 0xff)

#define USRIDX(group, usr)	((group) | ((usr) << 5))
#define SRIDX(major, minor, ext) \
                           (((major) << 7) | ((minor) << 3) | (ext))

static int parse_re2 (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
		      char **str, int64_t *value);
static int parse_fe5 (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
		      char **str, int64_t *value);
static int parse_pi5 (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
		      char **str, int64_t *value);
static int parse_aext_reg (char **pstr, int *value, int hw_res);
static int parse_a30b20 (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			 char **str, int64_t *value);
static int parse_rt21 (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
		       char **str, int64_t *value);
static int parse_rte_start (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			    char **str, int64_t *value);
static int parse_rte_end (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			  char **str, int64_t *value);
static int parse_rte69_start (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			      char **str, int64_t *value);
static int parse_rte69_end (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			    char **str, int64_t *value);
static int parse_im5_ip (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			 char **str, int64_t *value);
static int parse_im5_mr (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			 char **str, int64_t *value);
static int parse_im6_ip (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			 char **str, int64_t *value);
static int parse_im6_iq (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			 char **str, int64_t *value);
static int parse_im6_mr (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			 char **str, int64_t *value);
static int parse_im6_ms (struct nds32_asm_desc *desc, struct nds32_asm_insn *insn,
			 char **str, int64_t *value);

/* These are operand prefixes for input/output semantic.

     %   input
     =   output
     &   both
     {}  optional operand

   Field table for operands and bit-fields.  */

const field_t operand_fields[] =
{
  {"rt",	20, 5, 0, HW_GPR, NULL},
  {"ra",	15, 5, 0, HW_GPR, NULL},
  {"rb",	10, 5, 0, HW_GPR, NULL},
  {"rd",	5, 5, 0, HW_GPR, NULL},
  {"fst",	20, 5, 0, HW_FSR, NULL},
  {"fsa",	15, 5, 0, HW_FSR, NULL},
  {"fsb",	10, 5, 0, HW_FSR, NULL},
  {"fdt",	20, 5, 0, HW_FDR, NULL},
  {"fda",	15, 5, 0, HW_FDR, NULL},
  {"fdb",	10, 5, 0, HW_FDR, NULL},
  {"cprt",	20, 5, 0, HW_CPR, NULL},
  {"cp",	13, 2, 0, HW_CP, NULL},
  {"sh",	5, 5, 0, HW_UINT, NULL},	/* sh in ALU instructions.  */
  {"sv",	8, 2, 0, HW_UINT, NULL},	/* sv in MEM instructions.  */
  {"dt",	21, 1, 0, HW_DXR, NULL},
  {"usr",	10, 10, 0, HW_USR, NULL},	/* User Special Registers.  */
  {"sr",	10, 10, 0, HW_SR, NULL},	/* System Registers.  */
  {"ridx",	10, 10, 0, HW_UINT, NULL},	/* Raw value for mfusr/mfsr.  */
  {"enb4",	6, 4, 0, HW_UINT, NULL},	/* Enable4 for LSMW.  */
  {"swid",	5, 15, 0, HW_UINT, NULL},
  {"stdby_st",	5, 2, 0, HW_STANDBY_ST, NULL},
  {"tlbop_st",	5, 5, 0, HW_TLBOP_ST, NULL},
  {"tlbop_stx",	5, 5, 0, HW_UINT, NULL},
  {"cctl_st0",	5, 5, 0, HW_CCTL_ST0, NULL},
  {"cctl_st1",	5, 5, 0, HW_CCTL_ST1, NULL},
  {"cctl_st2",	5, 5, 0, HW_CCTL_ST2, NULL},
  {"cctl_st3",	5, 5, 0, HW_CCTL_ST3, NULL},
  {"cctl_st4",	5, 5, 0, HW_CCTL_ST4, NULL},
  {"cctl_st5",	5, 5, 0, HW_CCTL_ST5, NULL},
  {"cctl_stx",	5, 5, 0, HW_UINT, NULL},
  {"cctl_lv",	10, 1, 0, HW_CCTL_LV, NULL},
  {"msync_st",	5, 3, 0, HW_MSYNC_ST, NULL},
  {"msync_stx",	5, 3, 0, HW_UINT, NULL},
  {"dpref_st",	20, 4, 0, HW_DPREF_ST, NULL},
  {"rt5",	5, 5, 0, HW_GPR, NULL},
  {"ra5",	0, 5, 0, HW_GPR, NULL},
  {"rt4",	5, 4, 0, HW_GPR, NULL},
  {"rt3",	6, 3, 0, HW_GPR, NULL},
  {"rt38",	8, 3, 0, HW_GPR, NULL},	/* rt3 used in 38 form.  */
  {"ra3",	3, 3, 0, HW_GPR, NULL},
  {"rb3",	0, 3, 0, HW_GPR, NULL},
  {"rt5e",	4, 4, 1, HW_GPR, NULL},	/* for movd44.  */
  {"ra5e",	0, 4, 1, HW_GPR, NULL},	/* for movd44.  */
  {"re2",	5, 2, 0, HW_GPR, parse_re2},	/* re in push25/pop25.  */
  {"fe5",	0, 5, 2, HW_UINT, parse_fe5},	/* imm5u in lwi45.fe.  */
  {"pi5",	0, 5, 0, HW_UINT, parse_pi5},	/* imm5u in movpi45.  */
  {"abdim",	2, 3, 0, HW_ABDIM, NULL},	/* Flags for LSMW.  */
  {"abm",	2, 3, 0, HW_ABM, NULL},	/* Flags for LSMWZB.  */
  {"dtiton",	8, 2, 0, HW_DTITON, NULL},
  {"dtitoff",	8, 2, 0, HW_DTITOFF, NULL},

  {"i5s",	0, 5, 0, HW_INT, NULL},
  {"i10s",	0, 10, 0, HW_INT, NULL},
  {"i15s",	0, 15, 0, HW_INT, NULL},
  {"i19s",	0, 19, 0, HW_INT, NULL},
  {"i20s",	0, 20, 0, HW_INT, NULL},
  {"i8s1",	0, 8, 1, HW_INT, NULL},
  {"i11br3",	8, 11, 0, HW_INT, NULL},
  {"i14s1",	0, 14, 1, HW_INT, NULL},
  {"i15s1",	0, 15, 1, HW_INT, NULL},
  {"i16s1",	0, 16, 1, HW_INT, NULL},
  {"i18s1",	0, 18, 1, HW_INT, NULL},
  {"i24s1",	0, 24, 1, HW_INT, NULL},
  {"i8s2",	0, 8, 2, HW_INT, NULL},
  {"i12s2",	0, 12, 2, HW_INT, NULL},
  {"i15s2",	0, 15, 2, HW_INT, NULL},
  {"i17s2",	0, 17, 2, HW_INT, NULL},
  {"i19s2",	0, 19, 2, HW_INT, NULL},
  {"i3u",	0, 3, 0, HW_UINT, NULL},
  {"i5u",	0, 5, 0, HW_UINT, NULL},
  {"ib5u",	10, 5, 0, HW_UINT, NULL},	/* imm5 field in ALU.  */
  {"ib5s",	10, 5, 0, HW_INT, NULL},	/* imm5 field in ALU.  */
  {"i9u",	0, 9, 0, HW_UINT, NULL},	/* for ex9.it.  */
  {"ia3u",	3, 3, 0, HW_UINT, NULL},	/* for bmski33, fexti33.  */
  {"i8u",	0, 8, 0, HW_UINT, NULL},
  {"ib8u",	7, 8, 0, HW_UINT, NULL},	/* for ffbi.  */
  {"i15u",	0, 15, 0, HW_UINT, NULL},
  {"i20u",	0, 20, 0, HW_UINT, NULL},
  {"i3u1",	0, 3, 1, HW_UINT, NULL},
  {"i9u1",	0, 9, 1, HW_UINT, NULL},
  {"i3u2",	0, 3, 2, HW_UINT, NULL},
  {"i6u2",	0, 6, 2, HW_UINT, NULL},
  {"i7u2",	0, 7, 2, HW_UINT, NULL},
  {"i5u3",	0, 5, 3, HW_UINT, NULL},	/* for pop25/pop25.  */
  {"i15s3",	0, 15, 3, HW_INT, NULL},	/* for dprefi.d.  */

  {"a_rt",	15, 5, 0, HW_GPR, NULL},  /* for audio-extension.  */
  {"a_ru",	10, 5, 0, HW_GPR, NULL},  /* for audio-extension.  */
  {"a_dx",	9, 1, 0, HW_DXR, NULL},  /* for audio-extension.  */
  {"a_a30",	16, 4, 0, HW_GPR, parse_a30b20},  /* for audio-extension.  */
  {"a_b20",	12, 4, 0, HW_GPR, parse_a30b20},  /* for audio-extension.  */
  {"a_rt21",	5, 7, 0, HW_GPR, parse_rt21},  /* for audio-extension.  */
  {"a_rte",	5, 7, 0, HW_GPR, parse_rte_start},  /* for audio-extension.  */
  {"a_rte1",	5, 7, 0, HW_GPR, parse_rte_end},  /* for audio-extension.  */
  {"a_rte69",	6, 4, 0, HW_GPR, parse_rte69_start},  /* for audio-extension.  */
  {"a_rte69_1",	6, 4, 0, HW_GPR, parse_rte69_end},  /* for audio-extension.  */
  {"dhy",	5, 2, 0, HW_AEXT_ACC, NULL},  /* for audio-extension.  */
  {"dxh",	15, 2, 0, HW_AEXT_ACC, NULL},  /* for audio-extension.  */
  {"aridx",	0, 5, 0, HW_AEXT_ARIDX, NULL},  /* for audio-extension.  */
  {"aridx16",	16, 4, 0, HW_AEXT_ARIDX, NULL},  /* for audio-extension.  */
  {"imm16",	0, 16, 0, HW_INT, NULL},  /* for audio-extension.  */
  {"im5_i",	0, 5, 0, HW_AEXT_IM_I, parse_im5_ip},  /* for audio-extension.  */
  {"im5_m",	0, 5, 0, HW_AEXT_IM_M, parse_im5_mr},  /* for audio-extension.  */
  {"im6_ip",	0, 2, 0, HW_AEXT_IM_I, parse_im6_ip},  /* for audio-extension.  */
  {"im6_iq",	0, 2, 0, HW_AEXT_IM_I, parse_im6_iq},  /* for audio-extension.  */
  {"im6_mr",	2, 2, 0, HW_AEXT_IM_M, parse_im6_mr},  /* for audio-extension.  */
  {"im6_ms",	4, 2, 0, HW_AEXT_IM_M, parse_im6_ms},  /* for audio-extension.  */
  {"cp45",	4, 2, 0, HW_CP, NULL},  /* for cop-extension.  */
  {"i12u",	8, 12, 0, HW_UINT, NULL},  /* for cop-extension.  */
  {"cpi19",	6, 19, 0, HW_UINT, NULL},  /* for cop-extension.  */
  {NULL, 0, 0, 0, 0, NULL}
};

#define OP6(op6)		(N32_OP6_ ## op6 << 25)
#define DEF_REG(r)		(__BIT (r))
#define USE_REG(r)		(__BIT (r))
#define RT(r)			(r << 20)
#define RA(r)			(r << 15)
#define RB(r)			(r << 10)
#define RA5(r)			(r)

struct nds32_opcode nds32_opcodes[] =
{
  /* opc6_encoding table OPC_6.  */
  {"lbi", "=rt,[%ra{+%i15s}]", OP6 (LBI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhi", "=rt,[%ra{+%i15s1}]", OP6 (LHI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lwi", "=rt,[%ra{+%i15s2}]", OP6 (LWI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* mark "ldi", "=rt,[%ra{+%i15s2}]" as unsupported instruction.  */
  {NULL, NULL, OP6 (LDI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lbi.bi", "=rt,[%ra],%i15s", OP6 (LBI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhi.bi", "=rt,[%ra],%i15s1", OP6 (LHI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lwi.bi", "=rt,[%ra],%i15s2", OP6 (LWI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* mark "ldi.bi", "=rt,[%ra],%i15s2" as unsupported instruction.  */
  {NULL, NULL, OP6 (LDI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sbi", "%rt,[%ra{+%i15s}]", OP6 (SBI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"shi", "%rt,[%ra{+%i15s1}]", OP6 (SHI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"swi", "%rt,[%ra{+%i15s2}]", OP6 (SWI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* mark "sdi", "%rt,[%ra{+%i15s2}]" as unsupported instruction.  */
  {NULL, NULL, OP6 (SDI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sbi.bi", "%rt,[%ra],%i15s", OP6 (SBI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"shi.bi", "%rt,[%ra],%i15s1", OP6 (SHI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"swi.bi", "%rt,[%ra],%i15s2", OP6 (SWI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* mark "sdi.bi", "%rt,[%ra],%i15s2" as unsupported instruction.  */
  {NULL, NULL, OP6 (SDI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},

  {"lbsi", "=rt,[%ra{+%i15s}]", OP6 (LBSI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhsi", "=rt,[%ra{+%i15s1}]", OP6 (LHSI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* mark "lwsi", "=rt,[%ra{+%i15s2}]" as unsupported instruction.  */
  {NULL, NULL, OP6 (LWSI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"dprefi", NULL, 0, 0, ATTR_V3MEX_V1, 0, NULL, 0, NULL, (24 << 24) | (0x01 << 16) | OPCODE_BASE_DPREFI},
  {"lbsi.bi", "=rt,[%ra],%i15s", OP6 (LBSI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhsi.bi", "=rt,[%ra],%i15s1", OP6 (LHSI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* mark "lwsi.bi", "=rt,[%ra],%i15s2" as unsupported instruction.  */
  {NULL, NULL, OP6 (LWSI_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lbgp", NULL, 0, 0, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, (19 << 24) | (0x01 << 16) | OPCODE_BASE_LBGP},
  {"lwc", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (12 << 24) | (0x01 << 16) | OPCODE_BASE_LWC},
  {"swc", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (12 << 24) | (0x01 << 16) | OPCODE_BASE_SWC},
  {"ldc", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (12 << 24) | (0x01 << 16) | OPCODE_BASE_LDC},
  {"sdc", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (12 << 24) | (0x01 << 16) | OPCODE_BASE_SDC},
  {"mem", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (0 << 24) | (0x08 << 16) | OPCODE_BASE_MEM},
  {"lsmw", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (0 << 24) | (0x02 << 16) | OPCODE_BASE_LSMW},
  {"hwgp", NULL, 0, 0, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, (17 << 24) | (0x03 << 16) | OPCODE_BASE_HWGP},
  {"sbgp", NULL, 0, 0, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, (19 << 24) | (0x01 << 16) | OPCODE_BASE_SBGP},

  {"alu1", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (0 << 24) | (0x05 << 16) | OPCODE_BASE_ALU1},
  {"alu2", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (0 << 24) | (0x06 << 16) | OPCODE_BASE_ALU2},
  {"movi", "=rt,%i20s", OP6 (MOVI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sethi", "=rt,%i20u", OP6 (SETHI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"ji", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (24 << 24) | (0x01 << 16) | OPCODE_BASE_JI},
  {"jreg", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (0 << 24) | (0x04 << 16) | OPCODE_BASE_JREG},
  {"br1", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (14 << 24) | (0x01 << 16) | OPCODE_BASE_BR1},
  {"br2", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (16 << 24) | (0x04 << 16) | OPCODE_BASE_BR2},
  {"addi", "=rt,%ra,%i15s", OP6 (ADDI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"subri", "=rt,%ra,%i15s", OP6 (SUBRI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"andi", "=rt,%ra,%i15u", OP6 (ANDI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"xori", "=rt,%ra,%i15u", OP6 (XORI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"ori", "=rt,%ra,%i15u", OP6 (ORI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"br3", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (19 << 24) | (0x01 << 16) | OPCODE_BASE_BR3},
  {"slti", "=rt,%ra,%i15s", OP6 (SLTI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sltsi", "=rt,%ra,%i15s", OP6 (SLTSI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},

  {"aext", NULL, OP6 (AEXT), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (20 << 24) | (0x05 << 16) | OPCODE_BASE_AEXT},
  {"cext", NULL, OP6 (CEXT), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"misc", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (0 << 24) | (0x05 << 16) | OPCODE_BASE_MISC},
  {"bitci", "=rt,%ra,%i15u", OP6 (BITCI), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"cop0", NULL, OP6 (COP), 0, ATTR_ALL, 0, NULL, 0, NULL, (0 << 24) | (0x04 << 16) | OPCODE_BASE_COP},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"simd", NULL, 0, 0, ATTR (PERF2_EXT), 0, NULL, 0, NULL, (0 << 24) | (0x01 << 16) | OPCODE_BASE_SIMD},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},

  /* seg-DPREFI.  */
  {"dprefi.w", "%dpref_st,[%ra{+%i15s2]}", OP6 (DPREFI), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"dprefi.d", "%dpref_st,[%ra{+%i15s3]}", OP6 (DPREFI) | __BIT (24), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  /* seg-LBGP.  */
  {"lbi.gp", "=rt,[+%i19s]", OP6 (LBGP), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"lbsi.gp", "=rt,[+%i19s]", OP6 (LBGP) | __BIT (19), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  /* seg-LWC/0.  */
  {"cplwi", "%cp,=cprt,[%ra{+%i12s2}]", OP6 (LWC), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_LWC0},
  {"cplwi.bi", "%cp,=cprt,[%ra],%i12s2", OP6 (LWC) | __BIT (12), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_LWC0 + 1},
  /* seg-SWC/0.  */
  {"cpswi", "%cp,=cprt,[%ra{+%i12s2}]", OP6 (SWC), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_SWC0},
  {"cpswi.bi", "%cp,=cprt,[%ra],%i12s2", OP6 (SWC) | __BIT (12), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_SWC0 + 1},
  /* seg-LDC/0.  */
  {"cpldi", "%cp,%cprt,[%ra{+%i12s2}]", OP6 (LDC), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_LDC0},
  {"cpldi.bi", "%cp,%cprt,[%ra],%i12s2", OP6 (LDC) | __BIT (12), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_LDC0 + 1},
  /* seg-SDC/0.  */
  {"cpsdi", "%cp,%cprt,[%ra{+%i12s2}]", OP6 (SDC), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_SDC0},
  {"cpsdi.bi", "%cp,%cprt,[%ra],%i12s2", OP6 (SDC) | __BIT (12), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_SDC0 + 1},
  /* seg-LSMW.  */
  #define LSMW(sub)    (OP6 (LSMW) | N32_LSMW_ ## sub)
  {"lmw", "%abdim %rt,[%ra],%rb{,%enb4}", LSMW (LSMW), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_LSMW_SMW},
  {"lmwa", "%abdim %rt,[%ra],%rb{,%enb4}", LSMW (LSMWA), 4, ATTR_V3MEX_V2, 0, NULL, 0, NULL, OPCODE_BASE_LSMW_SMWA},
  {"lmwzb", "%abm %rt,[%ra],%rb{,%enb4}", LSMW (LSMWZB), 4, ATTR (STR_EXT), 0, NULL, 0, NULL, OPCODE_BASE_LSMW_SMWZB},
  {"smw", "%abdim %rt,[%ra],%rb{,%enb4}", LSMW (LSMW) | __BIT (5), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"smwa", "%abdim %rt,[%ra],%rb{,%enb4}", LSMW (LSMWA) | __BIT (5), 4, ATTR_V3MEX_V2, 0, NULL, 0, NULL, 0},
  {"smwzb", "%abm %rt,[%ra],%rb{,%enb4}", LSMW (LSMWZB) | __BIT (5), 4, ATTR (STR_EXT), 0, NULL, 0, NULL, 0},
  /* seg-HWGP.  */
  {"lhi.gp", "=rt,[+%i18s1]", OP6 (HWGP), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"lhi.gp", "=rt,[+%i18s1]", OP6 (HWGP), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"lhsi.gp", "=rt,[+%i18s1]", OP6 (HWGP) | (2 << 17), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"lhsi.gp", "=rt,[+%i18s1]", OP6 (HWGP) | (2 << 17), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"shi.gp", "%rt,[+%i18s1]", OP6 (HWGP) | (4 << 17), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"shi.gp", "%rt,[+%i18s1]", OP6 (HWGP) | (4 << 17), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"lwi.gp", "=rt,[+%i17s2]", OP6 (HWGP) | (6 << 17), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"swi.gp", "%rt,[+%i17s2]", OP6 (HWGP) | (7 << 17), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  /* seg-SBGP.  */
  {"sbi.gp", "%rt,[+%i19s]", OP6 (SBGP), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  {"addi.gp", "=rt,%i19s", OP6 (SBGP) | __BIT (19), 4, ATTR (GPREL) | ATTR_V2UP, USE_REG (29), NULL, 0, NULL, 0},
  /* seg-JI.  */
  {"j", "%i24s1", OP6 (JI), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"jal", "%i24s1", OP6 (JI) | __BIT (24), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-JREG.  */
  #define JREG(sub)    (OP6 (JREG) | N32_JREG_ ## sub)
  #define JREG_RET    (1 << 5)
  #define JREG_IFC    (1 << 6)
  {"jr", "%rb", JREG (JR), 4, ATTR (BRANCH) | ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_JREG_RET},
  {"jral", "%rt,%rb", JREG (JRAL), 4, ATTR (BRANCH) | ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_JREG_JRAL_RT30},
  {"jrnez", "%rb", JREG (JRNEZ), 4, ATTR (BRANCH) | ATTR_V3, 0, NULL, 0, NULL, 0},
  {"jralnez", "%rt,%rb", JREG (JRALNEZ), 4, ATTR (BRANCH) | ATTR_V3, 0, NULL, 0, NULL, OPCODE_BASE_JREG_JRALNEZ_RT30},
  {"ret", "%rb", JREG (JR) | JREG_RET, 4, ATTR (BRANCH) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"ifret", "", JREG (JR) | JREG_IFC | JREG_RET, 4, ATTR (BRANCH) | ATTR (IFC_EXT), 0, NULL, 0, NULL, 0},
  {"jral", "%rb", JREG (JRAL) | RT (30), 4, ATTR (BRANCH) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"jralnez", "%rb", JREG (JRALNEZ) | RT (30), 4, ATTR (BRANCH) | ATTR_V3, 0, NULL, 0, NULL, 0},
  {"ret", "", JREG (JR) | JREG_RET | RB (30), 4, ATTR (BRANCH) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"jr", "%dtitoff %rb", JREG (JR), 4, ATTR (BRANCH) | ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"ret", "%dtitoff %rb", JREG (JR) | JREG_RET, 4, ATTR (BRANCH) | ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"jral", "%dtiton %rt,%rb", JREG (JRAL), 4, ATTR (BRANCH) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"jral", "%dtiton %rb", JREG (JRAL) | RT (30), 4, ATTR (BRANCH) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-BR1.  */
  {"beq", "%rt,%ra,%i14s1", OP6 (BR1), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bne", "%rt,%ra,%i14s1", OP6 (BR1) | __BIT (14), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-BR2.  */
  #define BR2(sub)    (OP6 (BR2) | (N32_BR2_ ## sub << 16))
  {"ifcall", "%i16s1", BR2 (IFCALL), 4, ATTR (IFC_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"beqz", "%rt,%i16s1", BR2 (BEQZ), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bnez", "%rt,%i16s1", BR2 (BNEZ), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bgez", "%rt,%i16s1", BR2 (BGEZ), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bltz", "%rt,%i16s1", BR2 (BLTZ), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bgtz", "%rt,%i16s1", BR2 (BGTZ), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"blez", "%rt,%i16s1", BR2 (BLEZ), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"bgezal", "%rt,%i16s1", BR2 (BGEZAL), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bltzal", "%rt,%i16s1", BR2 (BLTZAL), 4, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-BR3.  */
  {"beqc", "%rt,%i11br3,%i8s1", OP6 (BR3), 4, ATTR_PCREL | ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  {"bnec", "%rt,%i11br3,%i8s1", OP6 (BR3) | __BIT (19), 4, ATTR_PCREL | ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  /* seg-SIMD.  */
  #define SIMD(sub)    (OP6 (SIMD) | N32_SIMD_ ## sub)
  {"pbsad", "%rt,%rb,%ra", SIMD (PBSAD), 4, ATTR (PERF2_EXT), 0, NULL, 0, NULL, 0},
  {"pbsada", "%rt,%rb,%ra", SIMD (PBSADA), 4, ATTR (PERF2_EXT), 0, NULL, 0, NULL, 0},
  /* seg-ALU1.  */
  #define ALU1(sub)    (OP6 (ALU1) | N32_ALU1_ ## sub)
  {"add", "=rt,%ra,%rb", ALU1 (ADD), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_ALU1_ADDSLLI},
  {"sub", "=rt,%ra,%rb", ALU1 (SUB), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_ALU1_SUBSLLI},
  {"and", "=rt,%ra,%rb", ALU1 (AND), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_ALU1_ANDSLLI},
  {"xor", "=rt,%ra,%rb", ALU1 (XOR), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_ALU1_XORSLLI},
  {"or", "=rt,%ra,%rb", ALU1 (OR), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_ALU1_ORSLLI},
  {"nor", "=rt,%ra,%rb", ALU1 (NOR), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"slt", "=rt,%ra,%rb", ALU1 (SLT), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"slts", "=rt,%ra,%rb", ALU1 (SLTS), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"slli", "=rt,%ra,%ib5u", ALU1 (SLLI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"srli", "=rt,%ra,%ib5u", ALU1 (SRLI), 4, ATTR_ALL, 0, NULL, 0, NULL, OPCODE_BASE_ALU1_NOP},
  {"srai", "=rt,%ra,%ib5u", ALU1 (SRAI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"rotri", "=rt,%ra,%ib5u", ALU1 (ROTRI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sll", "=rt,%ra,%rb", ALU1 (SLL), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"srl", "=rt,%ra,%rb", ALU1 (SRL), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sra", "=rt,%ra,%rb", ALU1 (SRA), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"rotr", "=rt,%ra,%rb", ALU1 (ROTR), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"seb", "=rt,%ra", ALU1 (SEB), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"seh", "=rt,%ra", ALU1 (SEH), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bitc", "=rt,%ra,%rb", ALU1 (BITC), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"zeh", "=rt,%ra", ALU1 (ZEH), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"wsbh", "=rt,%ra", ALU1 (WSBH), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"or_srli", "=rt,%ra,%rb,%sh", ALU1 (OR_SRLI), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"divsr", "=rt,=rd,%ra,%rb", ALU1 (DIVSR), 4, ATTR (DIV) | ATTR_V2UP, 0, NULL, 0, NULL, 0},
  {"divr", "=rt,=rd,%ra,%rb", ALU1 (DIVR), 4, ATTR (DIV) | ATTR_V2UP, 0, NULL, 0, NULL, 0},
  {"sva", "=rt,%ra,%rb", ALU1 (SVA), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"svs", "=rt,%ra,%rb", ALU1 (SVS), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"cmovz", "=rt,%ra,%rb", ALU1 (CMOVZ), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"cmovn", "=rt,%ra,%rb", ALU1 (CMOVN), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"add_srli", "=rt,%ra,%rb,%sh", ALU1 (ADD_SRLI), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"sub_srli", "=rt,%ra,%rb,%sh", ALU1 (SUB_SRLI), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"and_srli", "=rt,%ra,%rb,%sh", ALU1 (AND_SRLI), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"xor_srli", "=rt,%ra,%rb,%sh", ALU1 (XOR_SRLI), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"add_slli", "=rt,%ra,%rb,%sh", ALU1 (ADD), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"sub_slli", "=rt,%ra,%rb,%sh", ALU1 (SUB), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"and_slli", "=rt,%ra,%rb,%sh", ALU1 (AND), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"xor_slli", "=rt,%ra,%rb,%sh", ALU1 (XOR), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"or_slli", "=rt,%ra,%rb,%sh", ALU1 (OR), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"nop", "", ALU1 (SRLI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-ALU2.  */
  #define ALU2(sub)    (OP6 (ALU2) | N32_ALU2_ ## sub)
  {"max", "=rt,%ra,%rb", ALU2 (MAX), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"min", "=rt,%ra,%rb", ALU2 (MIN), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"ave", "=rt,%ra,%rb", ALU2 (AVE), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"abs", "=rt,%ra", ALU2 (ABS), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"clips", "=rt,%ra,%ib5s", ALU2 (CLIPS), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"clip", "=rt,%ra,%ib5u", ALU2 (CLIP), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"clo", "=rt,%ra", ALU2 (CLO), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"clz", "=rt,%ra", ALU2 (CLZ), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"bset", "=rt,%ra,%ib5u", ALU2 (BSET), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"bclr", "=rt,%ra,%ib5u", ALU2 (BCLR), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"btgl", "=rt,%ra,%ib5u", ALU2 (BTGL), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"btst", "=rt,%ra,%ib5u", ALU2 (BTST), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"bse", "=rt,%ra,=rb", ALU2 (BSE), 4, ATTR (PERF2_EXT), 0, NULL, 0, NULL, 0},
  {"bsp", "=rt,%ra,=rb", ALU2 (BSP), 4, ATTR (PERF2_EXT), 0, NULL, 0, NULL, 0},
  {"ffb", NULL, 0, 0, ATTR (STR_EXT), 0, NULL, 0, NULL, (6 << 24) | (0x01 << 16) | OPCODE_BASE_ALU2_FFBI},
  {"ffmism", NULL, 0, 0, ATTR (STR_EXT), 0, NULL, 0, NULL, (6 << 24) | (0x01 << 16) | OPCODE_BASE_ALU2_FLMISM},
  {"add_sc", NULL, ALU2 (ADD_SC), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"sub_sc", NULL, ALU2 (SUB_SC), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"add_wc", NULL, ALU2 (ADD_WC), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"sub_wc", NULL, ALU2 (SUB_WC), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"ffzmism", "=rt,%ra,%rb", ALU2 (FFZMISM), 4, ATTR (STR_EXT), 0, NULL, 0, NULL, 0},
  {"qadd", NULL, ALU2 (QADD), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {"qsub", NULL, ALU2 (QSUB), 4, ATTR (PERF_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"mfusr", "=rt,%usr", ALU2 (MFUSR), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"mtusr", "%rt,%usr", ALU2 (MTUSR), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"mfusr", "=rt,%ridx", ALU2 (MFUSR), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"mtusr", "%rt,%ridx", ALU2 (MTUSR), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"mul", "=rt,%ra,%rb", ALU2 (MUL), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"mults64", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (6 << 24) | (0x01 << 16) | OPCODE_BASE_ALU2_MULSR64},
  {"mult64", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (6 << 24) | (0x01 << 16) | OPCODE_BASE_ALU2_MULR64},
  {"madds64", "=dt,%ra,%rb", ALU2 (MADDS64), 4, ATTR (MAC) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"madd64", "=dt,%ra,%rb", ALU2 (MADD64), 4, ATTR (MAC) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"msubs64", "=dt,%ra,%rb", ALU2 (MSUBS64), 4, ATTR (MAC) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"msub64", "=dt,%ra,%rb", ALU2 (MSUB64), 4, ATTR (MAC) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"divs", "=dt,%ra,%rb", ALU2 (DIVS), 4, ATTR (DIV) | ATTR (DXREG), 0, NULL, 0, NULL, 0},
  {"div", "=dt,%ra,%rb", ALU2 (DIV), 4, ATTR (DIV) | ATTR (DXREG), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"mult32", "=dt,%ra,%rb", ALU2 (MULT32), 4, ATTR (DXREG) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"madd32", NULL, 0, 0, ATTR (MAC) | ATTR (DXREG) | ATTR_ALL, 0, NULL, 0, NULL, (6 << 24) | (0x01 << 16) | OPCODE_BASE_ALU2_MADDR32},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"msub32", NULL, 0, 0, ATTR (MAC) | ATTR (DXREG) | ATTR_ALL, 0, NULL, 0, NULL, (6 << 24) | (0x01 << 16) | OPCODE_BASE_ALU2_MSUBR32},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},

  /* seg-ALU2_FFBI.  */
  {"ffb", "=rt,%ra,%rb", ALU2 (FFB), 4, ATTR (STR_EXT), 0, NULL, 0, NULL, 0},
  {"ffbi", "=rt,%ra,%ib8u", ALU2 (FFBI) | __BIT (6), 4, ATTR (STR_EXT), 0, NULL, 0, NULL, 0},
  /* seg-ALU2_FLMISM.  */
  {"ffmism", "=rt,%ra,%rb", ALU2 (FFMISM), 4, ATTR (STR_EXT), 0, NULL, 0, NULL, 0},
  {"flmism", "=rt,%ra,%rb", ALU2 (FLMISM) | __BIT (6), 4, ATTR (STR_EXT), 0, NULL, 0, NULL, 0},
  /* seg-ALU2_MULSR64.  */
  {"mults64", "=dt,%ra,%rb", ALU2 (MULTS64), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"mulsr64", "=rt,%ra,%rb", ALU2 (MULSR64) |  __BIT (6), 4, ATTR_V3MEX_V2, 0, NULL, 0, NULL, 0},
  /* seg-ALU2_MULR64.  */
  {"mult64", "=dt,%ra,%rb", ALU2 (MULT64), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"mulr64", "=rt,%ra,%rb", ALU2 (MULR64) | __BIT (6), 4, ATTR_V3MEX_V2, 0, NULL, 0, NULL, 0},
  /* seg-ALU2_MADDR32.  */
  {"madd32", "=dt,%ra,%rb", ALU2 (MADD32), 4, ATTR (MAC) | ATTR (DXREG) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"maddr32", "=rt,%ra,%rb", ALU2 (MADDR32) | __BIT (6), 4, ATTR (MAC) | ATTR_V2UP, 0, NULL, 0, NULL, 0},
  /* seg-ALU2_MSUBR32.  */
  {"msub32", "=dt,%ra,%rb", ALU2 (MSUB32), 4, ATTR (MAC) | ATTR (DXREG) | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"msubr32", "=rt,%ra,%rb", ALU2 (MSUBR32) | __BIT (6), 4, ATTR (MAC) | ATTR_V2UP, 0, NULL, 0, NULL, 0},

  /* seg-MISC.  */
  #define MISC(sub)    (OP6 (MISC) | N32_MISC_ ## sub)
  {"standby", "%stdby_st", MISC (STANDBY), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"cctl", NULL, 0, 0, ATTR_V3MEX_V1, 0, NULL, 0, NULL, (5 << 24) | (0x05 << 16) | OPCODE_BASE_MISC_CCTL},
  {"mfsr", "=rt,%sr", MISC (MFSR), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"mtsr", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (5 << 24) | (0x05 << 16) | OPCODE_BASE_MISC_MTSR},
  {"iret", "", MISC (IRET), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"trap", "%swid", MISC (TRAP), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"teqz", "%rt,%swid", MISC (TEQZ), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"tnez", "%rt,%swid", MISC (TNEZ), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"dsb", "", MISC (DSB), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"isb", "", MISC (ISB), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"break", "%swid", MISC (BREAK), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"syscall", "%swid", MISC (SYSCALL), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"msync", "%msync_st", MISC (MSYNC), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"isync", "%rt", MISC (ISYNC), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"tlbop", "%ra,%tlbop_st", MISC (TLBOP), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-MISC_MTSR.  */
  {"mtsr", "%rt,%sr", MISC (MTSR), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"setend", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (20 << 24) | (0x01 << 16) | OPCODE_BASE_MISC_SETEND},
  {"setgie", NULL, 0, 0, ATTR_ALL, 0, NULL, 0, NULL, (20 << 24) | (0x01 << 16) | OPCODE_BASE_MISC_SETGIE},
  /* seg-MISC_SETEND.  */
  {"setend.l", "", MISC (MTSR) | (SRIDX (1, 0, 0) << 10) | __BIT (5), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"setend.b", "", MISC (MTSR) | (SRIDX (1, 0, 0) << 10) | __BIT (5) | __BIT (20), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-MISC_SETGIE.  */
  {"setgie.d", "", MISC (MTSR) | (SRIDX (1, 0, 0) << 10) | __BIT (6), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"setgie.e", "", MISC (MTSR) | (SRIDX (1, 0, 0) << 10) | __BIT (6) | __BIT (20), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-MISC_CCTL.  */
  {"cctl", "%ra,l1d_ix_inval", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1d_ix_wb", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1d_ix_wbinval", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "=rt,%ra,l1d_ix_rtag", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "=rt,%ra,l1d_ix_rwd", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%rb,%ra,l1d_ix_wtag", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%rb,%ra,l1d_ix_wwd", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "l1d_invalall", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1d_va_inval{,%cctl_lv}", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1d_va_wb{,%cctl_lv}", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1d_va_wbinval{,%cctl_lv}", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1d_va_fillck", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1d_va_ulck", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"cctl", "l1d_wball{,%cctl_lv}", MISC (CCTL), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1i_ix_inval", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"cctl", "=rt,%ra,l1i_ix_rtag", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "=rt,%ra,l1i_ix_rwd", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%rb,%ra,l1i_ix_wtag", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%rb,%ra,l1i_ix_wwd", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1i_va_inval{,%cctl_lv}", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1i_va_fillck", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,l1i_va_ulck", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-MISC_TLBOP.  */
  {"tlbop", "%ra,%tlbop_stx", MISC (TLBOP), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"tlbop", "flua", MISC (TLBOP) | (7 << 5), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"mfsr", "=rt,%ridx", MISC (MFSR), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"mtsr", "%rt,%ridx", MISC (MTSR), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"trap", "", MISC (TRAP), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"break", "", MISC (BREAK), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"msync", "", MISC (MSYNC), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"msync", "%msync_stx", MISC (MSYNC), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},

  /* seg-MEM.  */
  #define MEM(sub)    (OP6 (MEM) | N32_MEM_ ## sub)
  {"lb", "=rt,[%ra+(%rb<<%sv)]", MEM (LB), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lh", "=rt,[%ra+(%rb<<%sv)]", MEM (LH), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lw", "=rt,[%ra+(%rb<<%sv)]", MEM (LW), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"ld", "=rt,[%ra+(%rb<<%sv)]", MEM (LD), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lb.bi", "=rt,[%ra],(%rb<<%sv)", MEM (LB_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lh.bi", "=rt,[%ra],(%rb<<%sv)", MEM (LH_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lw.bi", "=rt,[%ra],(%rb<<%sv)", MEM (LW_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"ld.bi", "=rt,[%ra],(%rb<<%sv)", MEM (LD_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sb", "=rt,[%ra+(%rb<<%sv)]", MEM (SB), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sh", "=rt,[%ra+(%rb<<%sv)]", MEM (SH), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sw", "=rt,[%ra+(%rb<<%sv)]", MEM (SW), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sd", "=rt,[%ra+(%rb<<%sv)]", MEM (SD), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sb.bi", "=rt,[%ra],(%rb<<%sv)", MEM (SB_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sh.bi", "=rt,[%ra],(%rb<<%sv)", MEM (SH_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sw.bi", "=rt,[%ra],(%rb<<%sv)", MEM (SW_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sd.bi", "=rt,[%ra],(%rb<<%sv)", MEM (SD_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},

  {"lbs", "=rt,[%ra+(%rb<<%sv)]", MEM (LBS), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhs", "=rt,[%ra+(%rb<<%sv)]", MEM (LHS), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lws", "=rt,[%ra+(%rb<<%sv)]", MEM (LWS), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"dpref", "%dpref_st,[%ra+(%rb<<%sv)]", MEM (DPREF), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"lbs.bi", "=rt,[%ra],(%rb<<%sv)", MEM (LBS_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhs.bi", "=rt,[%ra],(%rb<<%sv)", MEM (LHS_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lws.bi", "=rt,[%ra],(%rb<<%sv)", MEM (LWS_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"llw", "=rt,[%ra+(%rb<<%sv)]", MEM (LLW), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"scw", "%rt,[%ra+(%rb<<%sv)]", MEM (SCW), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},

  {"lbup", "=rt,[%ra+(%rb<<%sv)]", MEM (LBUP), 4, ATTR_V3MEX_V2, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"lwup", "=rt,[%ra+(%rb<<%sv)]", MEM (LWUP), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"sbup", "%rt,[%ra+(%rb<<%sv)]", MEM (SBUP), 4, ATTR_V3MEX_V2, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"swup", "%rt,[%ra+(%rb<<%sv)]", MEM (SWUP), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},

  {"lb", "=rt,[%ra+%rb{<<%sv}]", MEM (LB), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lh", "=rt,[%ra+%rb{<<%sv}]", MEM (LH), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lw", "=rt,[%ra+%rb{<<%sv}]", MEM (LW), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lb.bi", "=rt,[%ra],%rb{<<%sv}", MEM (LB_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lh.bi", "=rt,[%ra],%rb{<<%sv}", MEM (LH_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lw.bi", "=rt,[%ra],%rb{<<%sv}", MEM (LW_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sb", "%rt,[%ra+%rb{<<%sv}]", MEM (SB), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sh", "%rt,[%ra+%rb{<<%sv}]", MEM (SH), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sw", "%rt,[%ra+%rb{<<%sv}]", MEM (SW), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sb.bi", "%rt,[%ra],%rb{<<%sv}", MEM (SB_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sh.bi", "%rt,[%ra],%rb{<<%sv}", MEM (SH_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sw.bi", "%rt,[%ra],%rb{<<%sv}", MEM (SW_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lbs", "=rt,[%ra+%rb{<<%sv}]", MEM (LBS), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhs", "=rt,[%ra+%rb{<<%sv}]", MEM (LHS), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lbs.bi", "=rt,[%ra],%rb{<<%sv}", MEM (LBS_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhs.bi", "=rt,[%ra],%rb{<<%sv}", MEM (LHS_BI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},

  {"llw", "=rt,[%ra+%rb{<<%sv}]", MEM (LLW), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"scw", "%rt,[%ra+%rb{<<%sv}]", MEM (SCW), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"lbup", "=rt,[%ra+%rb{<<%sv}]", MEM (LBUP), 4, ATTR_V3MEX_V2, 0, NULL, 0, NULL, 0},
  {"lwup", "=rt,[%ra+%rb{<<%sv}]", MEM (LWUP), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"sbup", "%rt,[%ra+%rb{<<%sv}]", MEM (SBUP), 4, ATTR_V3MEX_V2, 0, NULL, 0, NULL, 0},
  {"swup", "%rt,[%ra+%rb{<<%sv}]", MEM (SWUP), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"dpref", "%dpref_st,[%ra+%rb{<<%sv}]", MEM (DPREF), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},

  /* for Missing-Operand-load/store instructions.  */
  {"lb", "=rt,[%ra]", OP6 (LBI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lh", "=rt,[%ra]", OP6 (LHI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lw", "=rt,[%ra]", OP6 (LWI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lbs", "=rt,[%ra]", OP6 (LBSI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhs", "=rt,[%ra]", OP6 (LHSI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sb", "%rt,[%ra]", OP6 (SBI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sh", "%rt,[%ra]", OP6 (SHI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sw", "%rt,[%ra]", OP6 (SWI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-LWC0.  */
  #define FPU_RA_IMMBI(sub)    (OP6 (sub) | __BIT (12))
  {"flsi", "=fst,[%ra{+%i12s2}]", OP6(LWC), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"flsi.bi", "=fst,[%ra],%i12s2", FPU_RA_IMMBI(LWC), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  /* seg-SWC0.  */
  {"fssi", "=fst,[%ra{+%i12s2}]", OP6(SWC), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fssi.bi", "=fst,[%ra],%i12s2", FPU_RA_IMMBI(SWC), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  /* seg-LDC0.  */
  {"fldi", "=fdt,[%ra{+%i12s2}]", OP6(LDC), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fldi.bi", "=fdt,[%ra],%i12s2", FPU_RA_IMMBI(LDC), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  /* seg-SDC0.  */
  {"fsdi", "=fdt,[%ra{+%i12s2}]", OP6(SDC), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fsdi.bi", "=fdt,[%ra],%i12s2", FPU_RA_IMMBI(SDC), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  /* seg-COP.  */
  {"cpe1", "%cp45,%cpi19", OP6 (COP) | 0x00, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"mfcpx", NULL, 0, 0, 0, 0, NULL, 0, NULL, (6 << 24) | (0x02 << 16) | OPCODE_BASE_COP_MFCPX},
  {"cplwx", NULL, 0, 0, 0, 0, NULL, 0, NULL, (7 << 24) | (0x01 << 16) | OPCODE_BASE_COP_CPLW},
  {"cpldx", NULL, 0, 0, 0, 0, NULL, 0, NULL, (7 << 24) | (0x01 << 16) | OPCODE_BASE_COP_CPLD},
  {"cpe2", "%cp45,%cpi19", OP6 (COP) | 0x04, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},

  {"cpe3", "%cp45,%cpi19", OP6 (COP) | 0x08, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"mtcpx", NULL, 0, 0, 0, 0, NULL, 0, NULL, (6 << 24) | (0x02 << 16) | OPCODE_BASE_COP_MTCPX},
  {"cpswx", NULL, 0, 0, 0, 0, NULL, 0, NULL, (7 << 24) | (0x01 << 16) | OPCODE_BASE_COP_CPSW},
  {"cpsdx", NULL, 0, 0, 0, 0, NULL, 0, NULL, (7 << 24) | (0x01 << 16) | OPCODE_BASE_COP_CPSD},
  {"cpe4", "%cp45,%cpi19", OP6 (COP) | 0x0C, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-FPU (COP0).  */
  {"fs1", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x04 << 16) | OPCODE_BASE_FPU_FS1},
  {"mfcp",NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x04 << 16) | OPCODE_BASE_FPU_MFCP},
  {"fls", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x02 << 16) | OPCODE_BASE_FPU_FLS},
  {"fld", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x02 << 16) | OPCODE_BASE_FPU_FLD},
  {"fs2", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x04 << 16) | OPCODE_BASE_FPU_FS2},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},

  {"fd1", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x04 << 16) | OPCODE_BASE_FPU_FD1},
  {"mtcp",NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x04 << 16) | OPCODE_BASE_FPU_MTCP},
  {"fss", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x02 << 16) | OPCODE_BASE_FPU_FSS},
  {"fsd", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x02 << 16) | OPCODE_BASE_FPU_FSD},
  {"fd2", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (6 << 24) | (0x04 << 16) | OPCODE_BASE_FPU_FD2},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},

  /* seg-COP-MFCPX.  */
  {"mfcpw", "%cp45,=rt,%i12u", OP6 (COP) | 0x01, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"mfcpd", "%cp45,=rt,%i12u", OP6 (COP) | 0x41, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"mfcppw", "%cp45,=rt,%i12u", OP6 (COP) | 0xc1, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-COP-CPLW.  */
  {"cplw", "%cp45,%cprt,[%ra+%rb<<%sv]", OP6 (COP) | 0x02, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"cplw.bi", "%cp45,%cprt,[%ra],%rb<<%sv", OP6 (COP) | 0x82, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-COP-CPLD.  */
  {"cpld", "%cp45,%cprt,[%ra+%rb<<%sv]", OP6 (COP) | 0x03, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"cpld.bi", "%cp45,%cprt,[%ra],%rb<<%sv", OP6 (COP) | 0x83, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-COP-MTCPX.  */
  {"mtcpw", "%cp45,%rt,%i12u", OP6 (COP) | 0x09, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"mtcpd", "%cp45,%rt,%i12u", OP6 (COP) | 0x49, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"mtcppw", "%cp45,%rt,%i12u", OP6 (COP) | 0xc9, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-COP-CPSW.  */
  {"cpsw", "%cp45,%cprt,[%ra+%rb<<%sv]", OP6 (COP) | 0x0a, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"cpsw.bi", "%cp45,%cprt,[%ra],%rb<<%sv", OP6 (COP) | 0x8a, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* seg-COP-CPSD.  */
  {"cpsd", "%cp45,%cprt,[%ra+%rb<<%sv]", OP6 (COP) | 0x0b, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"cpsd.bi", "%cp45,%cprt,[%ra],%rb<<%sv", OP6 (COP) | 0x8b, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},

  /* seg-FPU_FS1.  */
  #define FS1(sub)    (OP6 (COP) | N32_FPU_FS1 | (N32_FPU_FS1_ ## sub << 6))
  {"fadds", "=fst,%fsa,%fsb", FS1 (FADDS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fsubs", "=fst,%fsa,%fsb", FS1 (FSUBS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcpynss", "=fst,%fsa,%fsb", FS1 (FCPYNSS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcpyss", "=fst,%fsa,%fsb", FS1 (FCPYSS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fmadds", "=fst,%fsa,%fsb", FS1 (FMADDS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fmsubs", "=fst,%fsa,%fsb", FS1 (FMSUBS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmovns", "=fst,%fsa,%fsb", FS1 (FCMOVNS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmovzs", "=fst,%fsa,%fsb", FS1 (FCMOVZS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fnmadds", "=fst,%fsa,%fsb", FS1 (FNMADDS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fnmsubs", "=fst,%fsa,%fsb", FS1 (FNMSUBS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fmuls", "=fst,%fsa,%fsb", FS1 (FMULS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fdivs", "=fst,%fsa,%fsb", FS1 (FDIVS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"f2op", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (10 << 24) | (0x05 << 16) | OPCODE_BASE_FPU_FS1_F2OP},
  /* seg-FPU_FS1_F2OP.  */
  #define FS1_F2OP(sub)    (OP6 (COP) | N32_FPU_FS1 | (N32_FPU_FS1_F2OP << 6) \
             | (N32_FPU_FS1_F2OP_ ## sub << 10))
  {"fs2d", "=fdt,%fsa", FS1_F2OP (FS2D), 4, ATTR (FPU) | ATTR (FPU_SP_EXT) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fsqrts", "=fst,%fsa", FS1_F2OP (FSQRTS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fabss", "=fst,%fsa", FS1_F2OP (FABSS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fui2s", "=fst,%fsa", FS1_F2OP (FUI2S), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fsi2s", "=fst,%fsa", FS1_F2OP (FSI2S), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fs2ui", "=fst,%fsa", FS1_F2OP (FS2UI), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fs2ui.z", "=fst,%fsa", FS1_F2OP (FS2UI_Z), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fs2si", "=fst,%fsa", FS1_F2OP (FS2SI), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fs2si.z", "=fst,%fsa", FS1_F2OP (FS2SI_Z), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-FPU_FS2.  */
  #define FS2(sub)    (OP6 (COP) | N32_FPU_FS2 | (N32_FPU_FS2_ ## sub << 6))
  {"fcmpeqs", "=fst,%fsa,%fsb", FS2 (FCMPEQS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpeqs.e", "=fst,%fsa,%fsb", FS2 (FCMPEQS_E), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmplts", "=fst,%fsa,%fsb", FS2 (FCMPLTS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmplts.e", "=fst,%fsa,%fsb", FS2 (FCMPLTS_E), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmples", "=fst,%fsa,%fsb", FS2 (FCMPLES), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmples.e", "=fst,%fsa,%fsb", FS2 (FCMPLES_E), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpuns", "=fst,%fsa,%fsb", FS2 (FCMPUNS), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpuns.e", "=fst,%fsa,%fsb", FS2 (FCMPUNS_E), 4, ATTR (FPU) | ATTR (FPU_SP_EXT), 0, NULL, 0, NULL, 0},
  /* seg-FPU_FD1.  */
  #define FD1(sub)    (OP6 (COP) | N32_FPU_FD1 | (N32_FPU_FD1_ ## sub << 6))
  {"faddd", "=fdt,%fda,%fdb", FD1 (FADDD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fsubd", "=fdt,%fda,%fdb", FD1 (FSUBD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcpynsd", "=fdt,%fda,%fdb", FD1 (FCPYNSD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcpysd", "=fdt,%fda,%fdb", FD1 (FCPYSD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fmaddd", "=fdt,%fda,%fdb", FD1 (FMADDD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fmsubd", "=fdt,%fda,%fdb", FD1 (FMSUBD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmovnd", "=fdt,%fda,%fsb", FD1 (FCMOVND), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmovzd", "=fdt,%fda,%fsb", FD1 (FCMOVZD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fnmaddd", "=fdt,%fda,%fdb", FD1 (FNMADDD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fnmsubd", "=fdt,%fda,%fdb", FD1 (FNMSUBD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fmuld", "=fdt,%fda,%fdb", FD1 (FMULD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fdivd", "=fdt,%fda,%fdb", FD1 (FDIVD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"f2op", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (10 << 24) | (0x05 << 16) | OPCODE_BASE_FPU_FD1_F2OP},
  /* seg-FPU_FD1_F2OP.  */
  #define FD1_F2OP(sub)    (OP6 (COP) | N32_FPU_FD1 | (N32_FPU_FD1_F2OP << 6) \
             | (N32_FPU_FD1_F2OP_ ## sub << 10))
  {"fd2s", "=fst,%fda", FD1_F2OP (FD2S), 4, ATTR (FPU) | ATTR (FPU_SP_EXT) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fsqrtd", "=fdt,%fda", FD1_F2OP (FSQRTD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fabsd", "=fdt,%fda", FD1_F2OP (FABSD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fui2d", "=fdt,%fsa", FD1_F2OP (FUI2D), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fsi2d", "=fdt,%fsa", FD1_F2OP (FSI2D), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fd2ui", "=fst,%fda", FD1_F2OP (FD2UI), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fd2ui.z", "=fst,%fda", FD1_F2OP (FD2UI_Z), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fd2si", "=fst,%fda", FD1_F2OP (FD2SI), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fd2si.z", "=fst,%fda", FD1_F2OP (FD2SI_Z), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-FPU_FD2.  */
  #define FD2(sub)    (OP6 (COP) | N32_FPU_FD2 | (N32_FPU_FD2_ ## sub << 6))
  {"fcmpeqd", "=fst,%fda,%fdb", FD2 (FCMPEQD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpeqd.e", "=fst,%fda,%fdb", FD2 (FCMPEQD_E), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpltd", "=fst,%fda,%fdb", FD2 (FCMPLTD), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpltd.e", "=fst,%fda,%fdb", FD2 (FCMPLTD_E), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpled", "=fst,%fda,%fdb", FD2 (FCMPLED), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpled.e", "=fst,%fda,%fdb", FD2 (FCMPLED_E), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpund", "=fst,%fda,%fdb", FD2 (FCMPUND), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  {"fcmpund.e", "=fst,%fda,%fdb", FD2 (FCMPUND_E), 4, ATTR (FPU) | ATTR (FPU_DP_EXT), 0, NULL, 0, NULL, 0},
  /* seg-FPU_MFCP.  */
  #define MFCP(sub)    (OP6 (COP) | N32_FPU_MFCP | (N32_FPU_MFCP_ ## sub << 6))
  {"fmfsr", "=rt,%fsa", MFCP (FMFSR), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fmfdr", "=rt,%fda", MFCP (FMFDR), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"mfcpxr", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (10 << 24) | (0x05 << 16) | OPCODE_BASE_FPU_MFCP_XR},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-FPU_MFCP_XR.  */
  #define MFCP_XR(sub)    (OP6 (COP) | N32_FPU_MFCP | (N32_FPU_MFCP_XR << 6) \
             | (N32_FPU_MFCP_XR_ ## sub << 10))
  {"fmfcfg", "=rt", MFCP_XR(FMFCFG), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fmfcsr", "=rt", MFCP_XR(FMFCSR), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  /* seg-FPU_MTCP.  */
  #define MTCP(sub)    (OP6 (COP) | N32_FPU_MTCP | (N32_FPU_MTCP_ ## sub << 6))
  {"fmtsr", "%rt,=fsa", MTCP (FMTSR), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fmtdr", "%rt,=fda", MTCP (FMTDR), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"mtcpxr", NULL, 0, 0, ATTR (FPU), 0, NULL, 0, NULL, (10 << 24) | (0x05 << 16) | OPCODE_BASE_FPU_MTCP_XR},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-FPU_MTCP_XR.  */
  #define MTCP_XR(sub)    (OP6 (COP) | N32_FPU_MTCP | (N32_FPU_MTCP_XR << 6) \
             | (N32_FPU_MTCP_XR_ ## sub << 10))
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fmtcsr", "%rt", MTCP_XR(FMTCSR), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  #define FPU_MEM(sub)    (OP6 (COP) | N32_FPU_ ## sub)
  #define FPU_MEMBI(sub)  (OP6 (COP) | N32_FPU_ ## sub | 0x2 << 6)
  /* seg-FPU_FLS.  */
  {"fls", "=fst,[%ra+(%rb<<%sv)]", FPU_MEM (FLS), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fls.bi", "=fst,[%ra],(%rb<<%sv)", FPU_MEMBI (FLS), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-FPU_FLD.  */
  {"fld", "=fdt,[%ra+(%rb<<%sv)]", FPU_MEM (FLD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fld.bi", "=fdt,[%ra],(%rb<<%sv)", FPU_MEMBI (FLD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-FPU_FSS.  */
  {"fss", "=fst,[%ra+(%rb<<%sv)]", FPU_MEM (FSS), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fss.bi", "=fst,[%ra],(%rb<<%sv)", FPU_MEMBI (FSS), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* seg-FPU_FSD.  */
  {"fsd", "=fdt,[%ra+(%rb<<%sv)]", FPU_MEM (FSD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fsd.bi", "=fdt,[%ra],(%rb<<%sv)", FPU_MEMBI (FSD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"fls", "=fst,[%ra+%rb{<<%sv}]", FPU_MEM (FLS), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fls.bi", "=fst,[%ra],%rb{<<%sv}", FPU_MEMBI (FLS), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fld", "=fdt,[%ra+%rb{<<%sv}]", FPU_MEM (FLD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fld.bi", "=fdt,[%ra],%rb{<<%sv}", FPU_MEMBI (FLD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fss", "=fst,[%ra+%rb{<<%sv}]", FPU_MEM (FSS), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fss.bi", "=fst,[%ra],%rb{<<%sv}", FPU_MEMBI (FSS), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fsd", "=fdt,[%ra+%rb{<<%sv}]", FPU_MEM (FSD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"fsd.bi", "=fdt,[%ra],%rb{<<%sv}", FPU_MEMBI (FSD), 4, ATTR (FPU), 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,%cctl_st0", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%ra,%cctl_st1{,%cctl_lv}", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "=rt,%ra,%cctl_st2", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%rb,%ra,%cctl_st3", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%cctl_st4", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  {"cctl", "%cctl_st5{,%cctl_lv}", MISC (CCTL), 4, ATTR_V3, 0, NULL, 0, NULL, 0},
  {"cctl", "=rt,%ra,%cctl_stx,%cctl_lv", MISC (CCTL), 4, ATTR_V3MEX_V1, 0, NULL, 0, NULL, 0},
  /* seg-Alias instructions.  */
  {"neg", "=rt,%ra", OP6 (SUBRI), 4, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"zeb", "=rt,%ra", OP6 (ANDI) | 0xff, 4, ATTR_ALL, 0, NULL, 0, NULL, 0},

  /* 16-bit instructions.  */
  /* get bit14~bit11 of 16-bit instruction.  */
  {"seg00", NULL, 0x8000, 0, 0, 0, NULL, 0, NULL, (10 << 24) | (0x01 << 16) | BIT16_BASE_SEG00},
  {"seg01", NULL, 0x8800, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG01},
  {"seg02", NULL, 0x9000, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG02},
  {"seg03", NULL, 0x9800, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG03},
  {"seg04", NULL, 0xa000, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG04},
  {"seg05", NULL, 0xa800, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG05},
  {"seg06", NULL, 0xb000, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG06},
  {"seg07", NULL, 0xb800, 0, 0, 0, NULL, 0, NULL, (7 << 24) | (0x01 << 16) | BIT16_BASE_SEG07},
  {"beqz38", "%rt38,%i8s1", 0xc000, 2, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bnez38", "%rt38,%i8s1", 0xc800, 2, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"beqs38", "%rt38,%i8s1", 0xd000, 2, ATTR_PCREL | ATTR_ALL, USE_REG (5), NULL, 0, NULL, BIT16_BASE_SEG10_1},
  {"bnes38", "%rt38,%i8s1", 0xd800, 2, ATTR_PCREL | ATTR_ALL, USE_REG (5), NULL, 0, NULL, BIT16_BASE_SEG11_1},
  {"seg12", NULL, 0xe000, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG12},
  {"seg13", NULL, 0xe800, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG13},
  {"seg14", NULL, 0xf000, 0, 0, 0, NULL, 0, NULL, (7 << 24) | (0x01 << 16) | BIT16_BASE_SEG14},
  {"seg15", NULL, 0xf800, 0, 0, 0, NULL, 0, NULL, (9 << 24) | (0x02 << 16) | BIT16_BASE_SEG15},

  /* SEG00, get bit10.  */
  {"mov55", "=rt5,%ra5", 0x8000, 2, ATTR_ALL, 0, NULL, 0, NULL, BIT16_BASE_IFRET16},
  {"movi55", "=rt5,%i5s", 0x8400, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* SEG01  bit10~bit9.  */
  {"add45", "=rt4,%ra5", 0x8800, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sub45", "=rt4,%ra5", 0x8a00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"addi45", "=rt4,%i5u", 0x8c00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"subi45", "=rt4,%i5u", 0x8e00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* SEG02  bit10~bit9.  */
  {"srai45", "=rt4,%i5u", 0x9000, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"srli45", "=rt4,%i5u", 0x9200, 2, ATTR_ALL, 0, NULL, 0, NULL, BIT16_BASE_NOP16},
  {"slli333", "=rt3,%ra3,%i3u", 0x9400, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bfmi33", NULL, 0x9600, 0, 0, 0, NULL, 0, NULL, (0 << 24) | (0x03 << 16) | BIT16_BASE_BFMI333},
  /* SEG03  bit10~bit9.  */
  {"add333", "=rt3,%ra3,%rb3", 0x9800, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sub333", "=rt3,%ra3,%rb3", 0x9a00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"addi333", "=rt3,%ra3,%i3u", 0x9c00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"subi333", "=rt3,%ra3,%i3u", 0x9e00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* SEG04  bit10~bit9.  */
  {"lwi333", "=rt3,[%ra3{+%i3u2}]", 0xa000, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lwi333.bi", "=rt3,[%ra3],%i3u2", 0xa200, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lhi333", "=rt3,[%ra3{+%i3u1}]", 0xa400, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"lbi333", "=rt3,[%ra3{+%i3u}]", 0xa600, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* SEG05  bit10~bit9.  */
  {"swi333", "%rt3,[%ra3{+%i3u2}]", 0xa800, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"swi333.bi", "%rt3,[%ra3],%i3u2", 0xaa00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"shi333", "%rt3,[%ra3{+%i3u1}]", 0xac00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"sbi333", "%rt3,[%ra3{+%i3u}]", 0xae00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* SEG06  bit10~bit9.  */
  {"addri36.sp", "%rt3,%i6u2", 0xb000, 2, ATTR_V3MUP, USE_REG (31), NULL, 0, NULL, 0},
  {"lwi45.fe", "=rt4,%fe5", 0xb200, 2, ATTR_V3MUP, USE_REG (8), NULL, 0, NULL, 0},
  {"lwi450", "=rt4,[%ra5]", 0xb400, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"swi450", "%rt4,[%ra5]", 0xb600, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* SEG07  bit7.  */
  {"lwi37", "=rt38,[$fp{+%i7u2}]", 0xb800, 2, ATTR_ALL, USE_REG (28), NULL, 0, NULL, 0},
  {"swi37", "%rt38,[$fp{+%i7u2}]", 0xb880, 2, ATTR_ALL, USE_REG (28), NULL, 0, NULL, 0},
  /* SEG10_1 if Rt3=5.  */
  {"j8", "%i8s1", 0xd500, 2, ATTR_PCREL | ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* SEG11_1 if Rt3=5.  */
  {"s13_1", NULL, 0xdd00, 0, 0, 0, NULL, 0, NULL, (5 << 24) | (0x03 << 16) | BIT16_BASE_SEG11_2},
  /* SEG11_2  bit7~bit5.  */
  {"jr5", "%ra5", 0xdd00, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"jral5", "%ra5", 0xdd20, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"ex9.it", "%i5u", 0xdd40, 2, ATTR (EX9_EXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0xdd60, 0, 0, 0, NULL, 0, NULL, 0},
  {"ret5", "%ra5", 0xdd80, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"add5.pc", "%ra5", 0xdda0, 2, ATTR_V3, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0xddc0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0xdde0, 0, 0, 0, NULL, 0, NULL, 0},
  /* SEG11_3  if Ra5=30.  */
  {"ret5", "", 0xdd80 | RA5 (30), 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  /* SEG12  bit10~bit9.  */
  {"slts45", "%rt4,%ra5", 0xe000, 2, ATTR_ALL, DEF_REG (15), NULL, 0, NULL, 0},
  {"slt45", "%rt4,%ra5", 0xe200, 2, ATTR_ALL, DEF_REG (15), NULL, 0, NULL, 0},
  {"sltsi45", "%rt4,%i5u", 0xe400, 2, ATTR_ALL, DEF_REG (15), NULL, 0, NULL, 0},
  {"slti45", "%rt4,%i5u", 0xe600, 2, ATTR_ALL, DEF_REG (15), NULL, 0, NULL, 0},
  /* SEG13  bit10~bit9.  */
  {"s13_1", NULL, 0xe800, 0, 0, 0, NULL, 0, NULL, (8 << 24) | (0x01 << 16) | BIT16_BASE_SEG13_1},
  {"break16", "%i5u", 0xea00, 2, ATTR_ALL, 0, NULL, 0, NULL, BIT16_BASE_SEG13_2},
  {"addi10.sp", "%i10s", 0xec00, 2, ATTR_V2UP, USE_REG (31) | DEF_REG (31), NULL, 0, NULL, 0},
  {"addi10.sp", "%i10s", 0xec00, 2, ATTR_V2UP, USE_REG (31) | DEF_REG (31), NULL, 0, NULL, 0},
  /* SEG13_1  bit8.  */
  {"beqzs8", "%i8s1", 0xe800, 2, ATTR_PCREL | ATTR_ALL, USE_REG (5), NULL, 0, NULL, 0},
  {"bnezs8", "%i8s1", 0xe900, 2, ATTR_PCREL | ATTR_ALL, USE_REG (5), NULL, 0, NULL, 0},
  /* SEG13_2  bit8~bit5.  */
  {"ex9.it", "%i9u", 0xea00, 2, ATTR (EX9_EXT), 0, NULL, 0, NULL, 0},
  /* SEG14  bit7.  */
  {"lwi37.sp", "=rt38,[+%i7u2]", 0xf000, 2, ATTR_V2UP, USE_REG (31), NULL, 0, NULL, 0},
  {"swi37.sp", "%rt38,[+%i7u2]", 0xf080, 2, ATTR_V2UP, USE_REG (31), NULL, 0, NULL, 0},
  /* SEG15  bit10~bit9.  */
  {"ifcall9", "%i9u1", 0xf800, 2, ATTR (IFC_EXT), 0, NULL, 0, NULL, 0},
  {"movpi45", "=rt4,%pi5", 0xfa00, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  {"s15_1", NULL, 0xfc00, 0, 0, 0, NULL, 0, NULL, (8 << 24) | (0x01 << 16) | BIT16_BASE_SEG15_1},
  {"misc33", NULL, 0xfe00, 0, 0, 0, NULL, 0, NULL, (0 << 24) | (0x03 << 16) | BIT16_BASE_MISC33},
  /* SEG15_1  bit8.  */
  {"pushpop25", NULL, 0xfc00, 0, 0, 0, NULL, 0, NULL, (7 << 24) | (0x02 << 16) | BIT16_BASE_PUSHPOP25},
  {"movd44", "=rt5e,%ra5e", 0xfd00, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},

  /* SEG-BFMI333 bit2~bit0.  */
  {"zeb33", "=rt3,%ra3", 0x9600, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"zeh33", "=rt3,%ra3", 0x9601, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"seb33", "=rt3,%ra3", 0x9602, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"seh33", "=rt3,%ra3", 0x9603, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"xlsb33", "=rt3,%ra3", 0x9604, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"x11b33", "=rt3,%ra3", 0x9605, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"bmski33", "=rt3,%ia3u", 0x9606, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  {"fexti33", "=rt3,%ia3u", 0x9607, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  /* SEG-PUSHPOP25 bit8~bit7.  */
  {"push25", "%re2,%i5u3", 0xfc00, 2, ATTR_V3MUP, USE_REG (31) | DEF_REG (31), NULL, 0, NULL, 0},
  {"pop25", "%re2,%i5u3", 0xfc80, 2, ATTR_V3MUP, USE_REG (31) | DEF_REG (31), NULL, 0, NULL, 0},
  /* SEG-MISC33 bit2~bit0.  */
  {NULL, NULL, 0xfe00, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0xfe01, 0, 0, 0, NULL, 0, NULL, 0},
  {"neg33", "=rt3,%ra3", 0xfe02, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  {"not33", "=rt3,%ra3", 0xfe03, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  {"mul33", "=rt3,%ra3", 0xfe04, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  {"xor33", "=rt3,%ra3", 0xfe05, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  {"and33", "=rt3,%ra3", 0xfe06, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  {"or33", "=rt3,%ra3", 0xfe07, 2, ATTR_V3MUP, 0, NULL, 0, NULL, 0},
  /* SEG-Alias instructions.  */
  {"nop16", "", 0x9200, 2, ATTR_ALL, 0, NULL, 0, NULL, 0},
  {"ifret16", "", 0x83ff, 2, ATTR (IFC_EXT), 0, NULL, 0, NULL, 0},

  /* Audio ext. instructions.  */
  #define AUDIO(sub)    (OP6 (AEXT) | (N32_AEXT_ ## sub << 20))

  {"aext00", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMADD},
  {"aext01", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMSUB},
  {"aext02", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMULT},
  {"aext03", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (5 << 24) | (0x02 << 16) | OPCODE_BASE_AEXT_AMFAR},
  {"aext04", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMADDS},
  {"aext05", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMSUBS},
  {"aext06", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMULTS},
  {"aext07", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMNEGS},
  {"aext08", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (5 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AADDL},
  {"amtari", "%aridx16,%imm16", AUDIO (AMTARI), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"aext12", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMAWBS},
  {"aext13", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMAWTS},
  {"aext14", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMWBS},
  {"aext15", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMWTS},
  {"aext16", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMABBS},
  {"aext17", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMABTS},
  {"aext18", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMATBS},
  {"aext19", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMATTS},
  {"aext20", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMBBS},
  {"aext21", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMBTS},
  {"aext22", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMTBS},
  {"aext23", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (6 << 24) | (0x03 << 16) | OPCODE_BASE_AEXT_AMTTS},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMADD.  */
  {"amadd00", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (5 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMADD_1},
  {"alr2", "=a_rt,=a_ru,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMADD) | (0x02 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMADD) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMADD) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMADD) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMADD) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddsa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMADD) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddsa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMADD) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amadd_1", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (0 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMADD_2},
  {"alr", "=a_rt,[%im5_i],%im5_m", AUDIO (AMADD) | (0x01 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amadd", "=a_dx,%ra,%rb", AUDIO (AMADD), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabbs", "=a_dx,%ra,%rb", AUDIO (AMADD) | 0x01, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMSUB.  */
  {"amsub00", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (5 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMSUB_1},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amsubl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMSUB) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMSUB) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMSUB) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMSUB) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubsa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMSUB) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubsa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMSUB) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsub_1", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (0 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMSUB_2},
  {"asr", "%ra,[%im5_i],%im5_m", AUDIO (AMSUB) | (0x01 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsub", "=a_dx,%ra,%rb", AUDIO (AMSUB), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabts", "=a_dx,%ra,%rb", AUDIO (AMSUB) | 0x01, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMULT.  */
  {"amult00", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (5 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMULT_1},
  {"amult01", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (5 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMULT_3},
  {"amultl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMULT) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMULT) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMULT) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMULT) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultsa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMULT) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultsa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMULT) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amult_1", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (0 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMULT_2},
  {"ala", "=dxh,[%im5_i],%im5_m", AUDIO (AMULT) | (0x01 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amult", "=a_dx,%ra,%rb", AUDIO (AMULT), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amatbs", "=a_dx,%ra,%rb", AUDIO (AMULT) | 0x01, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"asats48", "=a_dx", AUDIO (AMULT) | (0x02 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"awext", "%ra,%a_dx,%i5s", AUDIO (AMULT) | (0x03 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMFAR.  */
  {"amatts", "=a_dx,%ra,%rb", AUDIO (AMFAR) | 0x01, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"asa", "=dxh,[%im5_i],%im5_m", AUDIO (AMFAR) | (0x01 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amfar02", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (9 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMFAR_1},
  {"amfar03", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (9 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMFAR_2},
  {"amtar", "%ra,%aridx", AUDIO (AMFAR) | (0x02 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amtar2", "%ra,%aridx", AUDIO (AMFAR) | (0x12 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amfar", "=ra,%aridx", AUDIO (AMFAR) | (0x03 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amfar2", "=ra,%aridx", AUDIO (AMFAR) | (0x13 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMADDS.  */
  {"amadds00", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (5 << 24) | (0x01 << 16) | OPCODE_BASE_AEXT_AMADDS_1},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amaddsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMADDS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMADDS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMADDS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMADDS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMADDS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amaddssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMADDS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amadds_1", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (0 << 24) | (0x02 << 16) | OPCODE_BASE_AEXT_AMADDS_2},
  {"aupi", "%im5_i,%im5_m", AUDIO (AMADDS) | (0x01 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amadds", "=a_dx,%ra,%rb", AUDIO (AMADDS), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambbs", "=a_dx,%ra,%rb", AUDIO (AMADDS) | 0x01, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawbs", "=a_dx,%ra,%rb", AUDIO (AMADDS) | 0x02, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMSUBS.  */
  {"amsubs00", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (0 << 24) | (0x02 << 16) | OPCODE_BASE_AEXT_AMSUBS_1},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amsubsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMSUBS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMSUBS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMSUBS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMSUBS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMSUBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMSUBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amsubs", "=a_dx,%ra,%rb", AUDIO (AMSUBS), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambts", "=a_dx,%ra,%rb", AUDIO (AMSUBS) | 0x01, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawts", "=a_dx,%ra,%rb", AUDIO (AMSUBS) | 0x02, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMULTS.  */
  {"amults00", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (0 << 24) | (0x02 << 16) | OPCODE_BASE_AEXT_AMULTS_1},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amultsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMULTS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMULTS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMULTS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMULTS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMULTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amultssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMULTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amults", "=a_dx,%ra,%rb", AUDIO (AMULTS), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amtbs", "=a_dx,%ra,%rb", AUDIO (AMULTS) | 0x01, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwbs", "=a_dx,%ra,%rb", AUDIO (AMULTS) | 0x02, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMNEGS.  */
  {"amnegs00", NULL, 0, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, (0 << 24) | (0x02 << 16) | OPCODE_BASE_AEXT_AMNEGS_1},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amnegsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMNEGS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amnegsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMNEGS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amnegsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMNEGS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amnegsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMNEGS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amnegssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMNEGS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amnegssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMNEGS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amnegs", "=a_dx,%ra,%rb", AUDIO (AMNEGS), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amtts", "=a_dx,%ra,%rb", AUDIO (AMNEGS) | 0x01, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwts", "=a_dx,%ra,%rb", AUDIO (AMNEGS) | 0x02, 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AADDL.  */
  {"aaddl", "=a_rte69,%ra,%rb,%a_rte69_1,[%im5_i],%im5_m", AUDIO (AADDL), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"asubl", "=a_rte69,%ra,%rb,%a_rte69_1,[%im5_i],%im5_m", AUDIO (AADDL) | (0x01 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMAWBS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amawbsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMAWBS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawbsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMAWBS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawbsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMAWBS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawbsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMAWBS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMAWBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMAWBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMAWTS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amawtsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMAWTS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawtsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMAWTS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawtsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMAWTS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawtsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMAWTS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawtssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMAWTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amawtssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMAWTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMWBS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amwbsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMWBS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwbsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMWBS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwbsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMWBS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwbsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMWBS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMWBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMWBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMWTS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amwtsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMWTS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwtsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMWTS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwtsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMWTS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwtsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMWTS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwtssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMWTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amwtssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMWTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMABBS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amabbsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMABBS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabbsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMABBS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabbsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMABBS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabbsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMABBS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMABBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMABBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMABTS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amabtsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMABTS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabtsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMABTS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabtsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMABTS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabtsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMABTS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabtssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMABTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amabtssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMABTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMATBS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amatbsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMATBS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amatbsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMATBS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amatbsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMATBS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amatbsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMATBS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amatbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMATBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amatbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMATBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMATTS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amattsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMATTS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amattsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMATTS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amattsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMATTS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amattsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMATTS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amattssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMATTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amattssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMATTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMBBS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"ambbsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMBBS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambbsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMBBS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambbsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMBBS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambbsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMBBS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMBBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMBBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMBTS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"ambtsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMBTS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambtsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMBTS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambtsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMBTS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambtsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMBTS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambtssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMBTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"ambtssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMBTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMTBS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amtbsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMTBS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amtbsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMTBS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amtbsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMTBS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amtbsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMTBS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amtbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMTBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amtbssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMTBS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  /* N32_AEXT_AMTTS.  */
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
  {"amttsl.s", "=a_dx,%ra,%rb,[%im5_i],%im5_m", AUDIO (AMTTS) | (0x04 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amttsl.l", "=a_dx,%a_a30,%a_b20,%a_rt21,[%im5_i],%im5_m", AUDIO (AMTTS) | (0x06 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amttsl2.s", "=a_dx,%ra,%rb,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMTTS) | (0x08 << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amttsl2.l", "=a_dx,%a_a30,%a_b20,%a_rte,%a_rte1,[%im6_ip],[%im6_iq],%im6_mr,%im6_ms", AUDIO (AMTTS) | (0x0A << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amttssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMTTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {"amttssa", "=a_dx,%ra,%rb,%dhy,[%im5_i],%im5_m", AUDIO (AMTTS) | (0x0C << 5), 4, ATTR (AUDIO_ISAEXT), 0, NULL, 0, NULL, 0},
  {NULL, NULL, 0, 0, 0, 0, NULL, 0, NULL, 0},
};

const keyword_t keyword_gpr[] =
{
    /* Standard names.  */
    {"r0", 0, ATTR (RDREG)}, {"r1", 1, ATTR (RDREG)}, {"r2", 2, ATTR (RDREG)}, {"r3", 3, ATTR (RDREG)},
    {"r4", 4, ATTR (RDREG)}, {"r5", 5, ATTR (RDREG)}, {"r6", 6, ATTR (RDREG)}, {"r7", 7, ATTR (RDREG)},
    {"r8", 8, ATTR (RDREG)}, {"r9", 9, ATTR (RDREG)}, {"r10", 10, ATTR (RDREG)}, {"r11", 11, 0},
    {"r12", 12, 0}, {"r13", 13, 0}, {"r14", 14, 0}, {"r15", 15, ATTR (RDREG)},
    {"r16", 16, 0}, {"r17", 17, 0}, {"r18", 18, 0}, {"r19", 19, 0},
    {"r20", 20, 0}, {"r21", 21, 0}, {"r22", 22, 0}, {"r23", 23, 0},
    {"r24", 24, 0}, {"r25", 25, 0}, {"r26", 26, 0}, {"r27", 27, 0},
    {"fp", 28, ATTR (RDREG)}, {"gp", 29, ATTR (RDREG)}, {"lp", 30, ATTR (RDREG)}, {"sp", 31, ATTR (RDREG)},
    {"r28", 28, ATTR (RDREG)}, {"r29", 29, ATTR (RDREG)}, {"r30", 30, ATTR (RDREG)}, {"r31", 31, ATTR (RDREG)},
    /* Names for parameter passing.  */
    {"a0", 0, ATTR (RDREG)}, {"a1", 1, ATTR (RDREG)}, {"a2", 2, ATTR (RDREG)}, {"a3", 3, ATTR (RDREG)},
    {"a4", 4, ATTR (RDREG)}, {"a5", 5, ATTR (RDREG)},
    /* Names reserved for 5-bit addressing only.  */
    {"s0", 6, ATTR (RDREG)}, {"s1", 7, ATTR (RDREG)}, {"s2", 8, ATTR (RDREG)}, {"s3", 9, ATTR (RDREG)},
    {"s4", 10, ATTR (RDREG)}, {"s5", 11, 0}, {"s6", 12, 0}, {"s7", 13, 0},
    {"s8", 14, 0}, {"s9", 28, ATTR (RDREG)},
    {"ta", 15, ATTR (RDREG)},
    {"t0", 16, 0}, {"t1", 17, 0}, {"t2", 18, 0}, {"t3", 19, 0},
    {"t4", 20, 0}, {"t5", 21, 0}, {"t6", 22, 0}, {"t7", 23, 0},
    {"t8", 24, 0}, {"t9", 25, 0},
    {"p0", 26, 0}, {"p1", 27, 0},
    /* Names reserved for 4-bit addressing only.  */
    {"h0", 0, ATTR (RDREG)}, {"h1", 1, ATTR (RDREG)}, {"h2", 2, ATTR (RDREG)}, {"h3", 3, ATTR (RDREG)},
    {"h4", 4, ATTR (RDREG)}, {"h5", 5, ATTR (RDREG)}, {"h6", 6, ATTR (RDREG)}, {"h7", 7, ATTR (RDREG)},
    {"h8", 8, ATTR (RDREG)}, {"h9", 9, ATTR (RDREG)}, {"h10", 10, ATTR (RDREG)}, {"h11", 11, 0},
    {"h12", 16, 0}, {"h13", 17, 0}, {"h14", 18, 0}, {"h15", 19, 0},
    /* Names reserved for 3-bit addressing only.  */
    {"o0", 0, ATTR (RDREG)}, {"o1", 1, ATTR (RDREG)}, {"o2", 2, ATTR (RDREG)}, {"o3", 3, ATTR (RDREG)},
    {"o4", 4, ATTR (RDREG)}, {"o5", 5, ATTR (RDREG)}, {"o6", 6, ATTR (RDREG)}, {"o7", 7, ATTR (RDREG)},
    {NULL, 0, 0}
};

const keyword_t keyword_usr[] =
{
  {"d0.lo", USRIDX (0, 0), 0},
  {"d0.hi", USRIDX (0, 1), 0},
  {"d1.lo", USRIDX (0, 2), 0},
  {"d1.hi", USRIDX (0, 3), 0},
  {"itb", USRIDX (0, 28), 0},
  {"ifc_lp", USRIDX (0, 29), 0},
  {"pc", USRIDX (0, 31), 0},

  {"dma_cfg", USRIDX (1, 0), 0},
  {"dma_gcsw", USRIDX (1, 1), 0},
  {"dma_chnsel", USRIDX (1, 2), 0},
  {"dma_act", USRIDX (1, 3), 0},
  {"dma_setup", USRIDX (1, 4), 0},
  {"dma_isaddr", USRIDX (1, 5), 0},
  {"dma_esaddr", USRIDX (1, 6), 0},
  {"dma_tcnt", USRIDX (1, 7), 0},
  {"dma_status", USRIDX (1, 8), 0},
  {"dma_2dset", USRIDX (1, 9), 0},
  {"dma_rcnt", USRIDX (1, 23), 0},
  {"dma_hstatus", USRIDX (1, 24), 0},
  {"dma_2dsctl", USRIDX (1, 25), 0},

  {"pfmc0", USRIDX (2, 0), 0},
  {"pfmc1", USRIDX (2, 1), 0},
  {"pfmc2", USRIDX (2, 2), 0},
  {"pfm_ctl", USRIDX (2, 4), 0},

  {NULL, 0, 0}
};

const keyword_t keyword_dxr[] =
{
  {"d0", 0, 0}, {"d1", 1, 0}, {NULL, 0, 0}
};

const keyword_t keyword_sr[] =
{
  {"cr0", SRIDX (0, 0, 0), 0}, {"cpu_ver", SRIDX (0, 0, 0), 0},
  {"cr1", SRIDX (0, 1, 0), 0}, {"icm_cfg", SRIDX (0, 1, 0), 0},
  {"cr2", SRIDX (0, 2, 0), 0}, {"dcm_cfg", SRIDX (0, 2, 0), 0},
  {"cr3", SRIDX (0, 3, 0), 0}, {"mmu_cfg", SRIDX (0, 3, 0), 0},
  {"cr4", SRIDX (0, 4, 0), 0}, {"msc_cfg", SRIDX (0, 4, 0), 0},
  {"cr5", SRIDX (0, 0, 1), 0}, {"core_id", SRIDX (0, 0, 1), 0},
  {"cr6", SRIDX (0, 5, 0), 0}, {"fucop_exist", SRIDX (0, 5, 0), 0},

  {"ir0", SRIDX (1, 0, 0), 0}, {"psw", SRIDX (1, 0, 0), 0},
  {"ir1", SRIDX (1, 0, 1), 0}, {"ipsw", SRIDX (1, 0, 1), 0},
  {"ir2", SRIDX (1, 0, 2), 0}, {"p_ipsw", SRIDX (1, 0, 2), 0},
  {"ir3", SRIDX (1, 1, 1), 0}, {"ivb", SRIDX (1, 1, 1), 0},
  {"ir4", SRIDX (1, 2, 1), 0}, {"p_eva", SRIDX (1, 2, 2), 0},
  {"ir5", SRIDX (1, 2, 2), 0}, {"eva", SRIDX (1, 2, 1), 0},
  {"ir6", SRIDX (1, 3, 1), 0}, {"itype", SRIDX (1, 3, 1), 0},
  {"ir7", SRIDX (1, 3, 2), 0}, {"p_itype", SRIDX (1, 3, 2), 0},
  {"ir8", SRIDX (1, 4, 1), 0}, {"merr", SRIDX (1, 4, 1), 0},
  {"ir9", SRIDX (1, 5, 1), 0}, {"ipc", SRIDX (1, 5, 1), 0},
  {"ir10", SRIDX (1, 5, 2), 0}, {"p_ipc", SRIDX (1, 5, 2), 0},
  {"ir11", SRIDX (1, 5, 3), 0}, {"oipc", SRIDX (1, 5, 3), 0},
  {"ir12", SRIDX (1, 6, 2), 0}, {"p_p0", SRIDX (1, 6, 2), 0},
  {"ir13", SRIDX (1, 7, 2), 0}, {"p_p1", SRIDX (1, 7, 2), 0},
  {"ir14", SRIDX (1, 8, 0), 0}, {"int_mask", SRIDX (1, 8, 0), 0},
  {"ir15", SRIDX (1, 9, 0), 0}, {"int_pend", SRIDX (1, 9, 0), 0},
  {"ir16", SRIDX (1, 10, 0), 0}, {"sp_usr", SRIDX (1, 10, 0), 0},
  {"ir17", SRIDX (1, 10, 1), 0}, {"sp_priv", SRIDX (1, 10, 1), 0},
  {"ir18", SRIDX (1, 11, 0), 0}, {"int_pri", SRIDX (1, 11, 0), 0},
  {"ir19", SRIDX (1, 1, 2), 0}, {"int_ctrl", SRIDX (1, 1, 2), 0},
  {"ir20", SRIDX (1, 10, 2), 0}, {"sp_usr1", SRIDX (1, 10, 2), 0},
  {"ir21", SRIDX (1, 10, 3), 0}, {"sp_priv1", SRIDX (1, 10, 3), 0},
  {"ir22", SRIDX (1, 10, 4), 0}, {"sp_usr2", SRIDX (1, 10, 4), 0},
  {"ir23", SRIDX (1, 10, 5), 0}, {"sp_priv2", SRIDX (1, 10, 5), 0},
  {"ir24", SRIDX (1, 10, 6), 0}, {"sp_usr3", SRIDX (1, 10, 6), 0},
  {"ir25", SRIDX (1, 10, 7), 0}, {"sp_priv3", SRIDX (1, 10, 7), 0},
  {"ir26", SRIDX (1, 8, 1), 0}, {"int_mask2", SRIDX (1, 8, 1), 0},
  {"ir27", SRIDX (1, 9, 1), 0}, {"int_pend2", SRIDX (1, 9, 1), 0},
  {"ir28", SRIDX (1, 11, 1), 0}, {"int_pri2", SRIDX (1, 11, 1), 0},
  {"ir29", SRIDX (1, 9, 4), 0}, {"int_trigger", SRIDX (1, 9, 4), 0},
  {"ir30", SRIDX (1, 1, 3), 0},

  {"mr0", SRIDX (2, 0, 0), 0}, {"mmu_ctl", SRIDX (2, 0, 0), 0},
  {"mr1", SRIDX (2, 1, 0), 0}, {"l1_pptb", SRIDX (2, 1, 0), 0},
  {"mr2", SRIDX (2, 2, 0), 0}, {"tlb_vpn", SRIDX (2, 2, 0), 0},
  {"mr3", SRIDX (2, 3, 0), 0}, {"tlb_data", SRIDX (2, 3, 0), 0},
  {"mr4", SRIDX (2, 4, 0), 0}, {"tlb_misc", SRIDX (2, 4, 0), 0},
  {"mr5", SRIDX (2, 5, 0), 0}, {"vlpt_idx", SRIDX (2, 5, 0), 0},
  {"mr6", SRIDX (2, 6, 0), 0}, {"ilmb", SRIDX (2, 6, 0), 0},
  {"mr7", SRIDX (2, 7, 0), 0}, {"dlmb", SRIDX (2, 7, 0), 0},
  {"mr8", SRIDX (2, 8, 0), 0}, {"cache_ctl", SRIDX (2, 8, 0), 0},
  {"mr9", SRIDX (2, 9, 0), 0}, {"hsmp_saddr", SRIDX (2, 9, 0), 0},
  {"mr10", SRIDX (2, 9, 1), 0}, {"hsmp_eaddr", SRIDX (2, 9, 1), 0},
  {"mr11", SRIDX (2, 0, 1), 0}, {"bg_region", SRIDX (2, 0, 1), 0},

  {"pfr0", SRIDX (4, 0, 0), 0}, {"pfmc0", SRIDX (4, 0, 0), 0},
  {"pfr1", SRIDX (4, 0, 1), 0}, {"pfmc1", SRIDX (4, 0, 1), 0},
  {"pfr2", SRIDX (4, 0, 2), 0}, {"pfmc2", SRIDX (4, 0, 2), 0},
  {"pfr3", SRIDX (4, 1, 0), 0}, {"pfm_ctl", SRIDX (4, 1, 0), 0},

  {"dmar0", SRIDX (5, 0, 0), 0}, {"dma_cfg", SRIDX (5, 0, 0), 0},
  {"dmar1", SRIDX (5, 1, 0), 0}, {"dma_gcsw", SRIDX (5, 1, 0), 0},
  {"dmar2", SRIDX (5, 2, 0), 0}, {"dma_chnsel", SRIDX (5, 2, 0), 0},
  {"dmar3", SRIDX (5, 3, 0), 0}, {"dma_act", SRIDX (5, 3, 0), 0},
  {"dmar4", SRIDX (5, 4, 0), 0}, {"dma_setup", SRIDX (5, 4, 0), 0},
  {"dmar5", SRIDX (5, 5, 0), 0}, {"dma_isaddr", SRIDX (5, 5, 0), 0},
  {"dmar6", SRIDX (5, 6, 0), 0}, {"dma_esaddr", SRIDX (5, 6, 0), 0},
  {"dmar7", SRIDX (5, 7, 0), 0}, {"dma_tcnt", SRIDX (5, 7, 0), 0},
  {"dmar8", SRIDX (5, 8, 0), 0}, {"dma_status", SRIDX (5, 8, 0), 0},
  {"dmar9", SRIDX (5, 9, 0), 0}, {"dma_2dset", SRIDX (5, 9, 0), 0},
  {"dmar10", SRIDX (5, 9, 1), 0}, {"dma_2dsctl", SRIDX (5, 9, 1), 0},
  {"dmar11", SRIDX (5, 7, 1), 0}, {"dma_rcnt", SRIDX (5, 7, 1), 0},
  {"dmar12", SRIDX (5, 8, 1), 0}, {"dma_hstatus", SRIDX (5, 8, 1), 0},

  {"idr0", SRIDX (2, 15, 0), 0}, {"sdz_ctl", SRIDX (2, 15, 0), 0},
  {"idr1", SRIDX (2, 15, 1), 0}, {"n12misc_ctl", SRIDX (2, 15, 1), 0},
                  {"misc_ctl", SRIDX (2, 15, 1), 0},

  {"secur0", SRIDX (6, 0, 0), 0}, {"sfcr", SRIDX (6, 0, 0), 0},

  {"prusr_acc_ctl", SRIDX (4, 4, 0), 0},
  {"fucpr", SRIDX (4, 5, 0), 0}, {"fucop_ctl", SRIDX (4, 5, 0), 0},

  {"dr0", SRIDX (3, 0, 0), 0}, {"bpc0", SRIDX (3, 0, 0), 0},
  {"dr1", SRIDX (3, 0, 1), 0}, {"bpc1", SRIDX (3, 0, 1), 0},
  {"dr2", SRIDX (3, 0, 2), 0}, {"bpc2", SRIDX (3, 0, 2), 0},
  {"dr3", SRIDX (3, 0, 3), 0}, {"bpc3", SRIDX (3, 0, 3), 0},
  {"dr4", SRIDX (3, 0, 4), 0}, {"bpc4", SRIDX (3, 0, 4), 0},
  {"dr5", SRIDX (3, 0, 5), 0}, {"bpc5", SRIDX (3, 0, 5), 0},
  {"dr6", SRIDX (3, 0, 6), 0}, {"bpc6", SRIDX (3, 0, 6), 0},
  {"dr7", SRIDX (3, 0, 7), 0}, {"bpc7", SRIDX (3, 0, 7), 0},
  {"dr8", SRIDX (3, 1, 0), 0}, {"bpa0", SRIDX (3, 1, 0), 0},
  {"dr9", SRIDX (3, 1, 1), 0}, {"bpa1", SRIDX (3, 1, 1), 0},
  {"dr10", SRIDX (3, 1, 2), 0}, {"bpa2", SRIDX (3, 1, 2), 0},
  {"dr11", SRIDX (3, 1, 3), 0}, {"bpa3", SRIDX (3, 1, 3), 0},
  {"dr12", SRIDX (3, 1, 4), 0}, {"bpa4", SRIDX (3, 1, 4), 0},
  {"dr13", SRIDX (3, 1, 5), 0}, {"bpa5", SRIDX (3, 1, 5), 0},
  {"dr14", SRIDX (3, 1, 6), 0}, {"bpa6", SRIDX (3, 1, 6), 0},
  {"dr15", SRIDX (3, 1, 7), 0}, {"bpa7", SRIDX (3, 1, 7), 0},
  {"dr16", SRIDX (3, 2, 0), 0}, {"bpam0", SRIDX (3, 2, 0), 0},
  {"dr17", SRIDX (3, 2, 1), 0}, {"bpam1", SRIDX (3, 2, 1), 0},
  {"dr18", SRIDX (3, 2, 2), 0}, {"bpam2", SRIDX (3, 2, 2), 0},
  {"dr19", SRIDX (3, 2, 3), 0}, {"bpam3", SRIDX (3, 2, 3), 0},
  {"dr20", SRIDX (3, 2, 4), 0}, {"bpam4", SRIDX (3, 2, 4), 0},
  {"dr21", SRIDX (3, 2, 5), 0}, {"bpam5", SRIDX (3, 2, 5), 0},
  {"dr22", SRIDX (3, 2, 6), 0}, {"bpam6", SRIDX (3, 2, 6), 0},
  {"dr23", SRIDX (3, 2, 7), 0}, {"bpam7", SRIDX (3, 2, 7), 0},
  {"dr24", SRIDX (3, 3, 0), 0}, {"bpv0", SRIDX (3, 3, 0), 0},
  {"dr25", SRIDX (3, 3, 1), 0}, {"bpv1", SRIDX (3, 3, 1), 0},
  {"dr26", SRIDX (3, 3, 2), 0}, {"bpv2", SRIDX (3, 3, 2), 0},
  {"dr27", SRIDX (3, 3, 3), 0}, {"bpv3", SRIDX (3, 3, 3), 0},
  {"dr28", SRIDX (3, 3, 4), 0}, {"bpv4", SRIDX (3, 3, 4), 0},
  {"dr29", SRIDX (3, 3, 5), 0}, {"bpv5", SRIDX (3, 3, 5), 0},
  {"dr30", SRIDX (3, 3, 6), 0}, {"bpv6", SRIDX (3, 3, 6), 0},
  {"dr31", SRIDX (3, 3, 7), 0}, {"bpv7", SRIDX (3, 3, 7), 0},
  {"dr32", SRIDX (3, 4, 0), 0}, {"bpcid0", SRIDX (3, 4, 0), 0},
  {"dr33", SRIDX (3, 4, 1), 0}, {"bpcid1", SRIDX (3, 4, 1), 0},
  {"dr34", SRIDX (3, 4, 2), 0}, {"bpcid2", SRIDX (3, 4, 2), 0},
  {"dr35", SRIDX (3, 4, 3), 0}, {"bpcid3", SRIDX (3, 4, 3), 0},
  {"dr36", SRIDX (3, 4, 4), 0}, {"bpcid4", SRIDX (3, 4, 4), 0},
  {"dr37", SRIDX (3, 4, 5), 0}, {"bpcid5", SRIDX (3, 4, 5), 0},
  {"dr38", SRIDX (3, 4, 6), 0}, {"bpcid6", SRIDX (3, 4, 6), 0},
  {"dr39", SRIDX (3, 4, 7), 0}, {"bpcid7", SRIDX (3, 4, 7), 0},
  {"dr40", SRIDX (3, 5, 0), 0}, {"edm_cfg", SRIDX (3, 5, 0), 0},
  {"dr41", SRIDX (3, 6, 0), 0}, {"edmsw", SRIDX (3, 6, 0), 0},
  {"dr42", SRIDX (3, 7, 0), 0}, {"edm_ctl", SRIDX (3, 7, 0), 0},
  {"dr43", SRIDX (3, 8, 0), 0}, {"edm_dtr", SRIDX (3, 8, 0), 0},
  {"dr44", SRIDX (3, 9, 0), 0}, {"bpmtc", SRIDX (3, 9, 0), 0},
  {"dr45", SRIDX (3, 10, 0), 0}, {"dimbr", SRIDX (3, 10, 0), 0},
  {"dr46", SRIDX (3, 14, 0), 0}, {"tecr0", SRIDX (3, 14, 0), 0},
  {"dr47", SRIDX (3, 14, 1), 0}, {"tecr1", SRIDX (3, 14, 1), 0},
  {NULL,0 ,0}
};

const keyword_t keyword_cp[] =
{
  {"cp0", 0, 0}, {"cp1", 1, 0}, {"cp2", 2, 0}, {"cp3", 3, 0}, {NULL, 0, 0}
};

const keyword_t keyword_cpr[] =
{
  {"cpr0", 0, 0}, {"cpr1", 1, 0}, {"cpr2", 2, 0}, {"cpr3", 3, 0}, {"cpr4", 4, 0},
  {"cpr5", 5, 0}, {"cpr6", 6, 0}, {"cpr7", 7, 0}, {"cpr8", 8, 0}, {"cpr9", 9, 0},
  {"cpr10", 10, 0}, {"cpr11", 11, 0}, {"cpr12", 12, 0}, {"cpr13", 13, 0},
  {"cpr14", 14, 0}, {"cpr15", 15, 0}, {"cpr16", 16, 0}, {"cpr17", 17, 0},
  {"cpr18", 18, 0}, {"cpr19", 19, 0}, {"cpr20", 20, 0}, {"cpr21", 21, 0},
  {"cpr22", 22, 0}, {"cpr23", 23, 0}, {"cpr24", 24, 0}, {"cpr25", 25, 0},
  {"cpr26", 26, 0}, {"cpr27", 27, 0}, {"cpr28", 28, 0}, {"cpr29", 29, 0},
  {"cpr30", 30, 0}, {"cpr31", 31, 0}, {NULL, 0, 0}
};

const keyword_t keyword_fsr[] =
{
  {"fs0", 0, 0}, {"fs1", 1, 0}, {"fs2", 2, 0}, {"fs3", 3, 0}, {"fs4", 4, 0},
  {"fs5", 5, 0}, {"fs6", 6, 0}, {"fs7", 7, 0}, {"fs8", 8, 0}, {"fs9", 9, 0},
  {"fs10", 10, 0}, {"fs11", 11, 0}, {"fs12", 12, 0}, {"fs13", 13, 0},
  {"fs14", 14, 0}, {"fs15", 15, 0}, {"fs16", 16, 0}, {"fs17", 17, 0},
  {"fs18", 18, 0}, {"fs19", 19, 0}, {"fs20", 20, 0}, {"fs21", 21, 0},
  {"fs22", 22, 0}, {"fs23", 23, 0}, {"fs24", 24, 0}, {"fs25", 25, 0},
  {"fs26", 26, 0}, {"fs27", 27, 0}, {"fs28", 28, 0}, {"fs29", 29, 0},
  {"fs30", 30, 0}, {"fs31", 31, 0}, {NULL, 0 ,0}
};

const keyword_t keyword_fdr[] =
{
  {"fd0", 0, 0}, {"fd1", 1, 0}, {"fd2", 2, 0}, {"fd3", 3, 0}, {"fd4", 4, 0},
  {"fd5", 5, 0}, {"fd6", 6, 0}, {"fd7", 7, 0}, {"fd8", 8, 0}, {"fd9", 9, 0},
  {"fd10", 10, 0}, {"fd11", 11, 0}, {"fd12", 12, 0}, {"fd13", 13, 0},
  {"fd14", 14, 0}, {"fd15", 15, 0}, {"fd16", 16, 0}, {"fd17", 17, 0},
  {"fd18", 18, 0}, {"fd19", 19, 0}, {"fd20", 20, 0}, {"fd21", 21, 0},
  {"fd22", 22, 0}, {"fd23", 23, 0}, {"fd24", 24, 0}, {"fd25", 25, 0},
  {"fd26", 26, 0}, {"fd27", 27, 0}, {"fd28", 28, 0}, {"fd29", 29, 0},
  {"fd30", 30, 0}, {"fd31", 31, 0}, {NULL, 0, 0}
};

const keyword_t keyword_abdim[] =
{
  {"bi", 0, 0}, {"bim", 1, 0}, {"bd", 2, 0}, {"bdm", 3, 0},
  {"ai", 4, 0}, {"aim", 5, 0}, {"ad", 6, 0}, {"adm", 7, 0},
  {NULL, 0, 0}
};

const keyword_t keyword_abm[] =
{
  {"b", 0, 0}, {"bm", 1, 0}, {"bx", 2, 0}, {"bmx", 3, 0},
  {"a", 4, 0}, {"am", 5, 0}, {"ax", 6, 0}, {"amx", 7, 0},
  {NULL, 0, 0}
};

static const keyword_t keyword_dtiton[] =
{
  {"iton", 1, 0}, {"ton", 3, 0}, {NULL, 0, 0}
};

static const keyword_t keyword_dtitoff[] =
{
  {"itoff", 1, 0}, {"toff", 3, 0}, {NULL, 0, 0}
};

const keyword_t keyword_dpref_st[] =
{
  {"srd", 0, 0}, {"mrd", 1, 0}, {"swr", 2, 0}, {"mwr", 3, 0},
  {"pte", 4, 0}, {"clwr", 5, 0}, {NULL, 0, 0}
};

/* CCTL Ra, SubType.  */
static const keyword_t keyword_cctl_st0[] =
{
  {"l1d_ix_inval", 0X0, 0}, {"l1d_ix_wb", 0X1, 0}, {"l1d_ix_wbinval", 0X2, 0},
  {"l1d_va_fillck", 0XB, 0}, {"l1d_va_ulck", 0XC, 0}, {"l1i_ix_inval", 0X10, 0},
  {"l1i_va_fillck", 0X1B, 0}, {"l1i_va_ulck", 0X1C, 0},
  {NULL, 0, 0}
};

/* CCTL Ra, SubType, level.  */
static const keyword_t keyword_cctl_st1[] =
{
  {"l1d_va_inval", 0X8, 0}, {"l1d_va_wb", 0X9, 0},
  {"l1d_va_wbinval", 0XA, 0}, {"l1i_va_inval", 0X18, 0},
  {NULL, 0, 0}
};

/* CCTL Rt, Ra, SubType.  */
static const keyword_t keyword_cctl_st2[] =
{
  {"l1d_ix_rtag", 0X3, 0}, {"l1d_ix_rwd", 0X4, 0},
  {"l1i_ix_rtag", 0X13, 0}, {"l1i_ix_rwd", 0X14, 0},
  {NULL, 0, 0}
};

/* CCTL Rb, Ra, SubType.  */
static const keyword_t keyword_cctl_st3[] =
{
  {"l1d_ix_wtag", 0X5, 0}, {"l1d_ix_wwd", 0X6, 0},
  {"l1i_ix_wtag", 0X15, 0}, {"l1i_ix_wwd", 0X16, 0},
  {NULL, 0, 0}
};

/* CCTL L1D_INVALALL.  */
static const keyword_t keyword_cctl_st4[] =
{
  {"l1d_invalall", 0x7, 0}, {NULL, 0, 0}
};

/* CCTL L1D_WBALL, level.  */
static const keyword_t keyword_cctl_st5[] =
{
  {"l1d_wball", 0xf, 0}, {NULL, 0, 0}
};

const keyword_t keyword_cctl_lv[] =
{
  {"1level", 0, 0}, {"alevel", 1, 0}, {"0", 0, 0}, {"1", 1, 0},
  {NULL, 0, 0},
};

static const keyword_t keyword_tlbop_st[] =
{
  {"trd", 0, 0},
  {"twr", 1, 0},
  {"rwr", 2, 0},
  {"rwlk", 3, 0},
  {"unlk", 4, 0},
  {"pb", 5, 0},
  {"inv", 6, 0},
  {"targetread", 0, 0},
  {"targetwrite", 1, 0},
  {"rwrite", 2, 0},
  {"rwritelock", 3, 0},
  {"unlock", 4, 0},
  {"probe", 5, 0},
  {"invalidate", 6, 0},
  {NULL, 0, 0},
  /* "pb" requries two operand and "flua" requires none.  */
  /* {"pb", 5, 0}, {"probe", 5, 0},
     {"flua", 7, 0}, {"flushall", 0},.  */
};

const keyword_t keyword_standby_st[] =
{
  {"no_wake_grant", 0, 0},
  {"wake_grant", 1, 0},
  {"wait_done", 2, 0},
  {"0", 0, 0},
  {"1", 1, 0},
  {"2", 2, 0},
  {"3", 3, 0},
  {NULL, 0, 0},
};

const keyword_t keyword_msync_st[] =
{
  {"all", 0, 0}, {"store", 1, 0},
  {NULL, 0, 0}
};

const keyword_t keyword_im5_i[] =
{
  {"i0", 0, 0}, {"i1", 1, 0}, {"i2", 2, 0}, {"i3", 3, 0},
  {"i4", 4, 0}, {"i5", 5, 0}, {"i6", 6, 0}, {"i7", 7, 0},
  {NULL, 0, 0}
};

const keyword_t keyword_im5_m[] =
{
  {"m0", 0, 0}, {"m1", 1, 0}, {"m2", 2, 0}, {"m3", 3, 0},
  {"m4", 4, 0}, {"m5", 5, 0}, {"m6", 6, 0}, {"m7", 7, 0},
  {NULL, 0, 0}
};

const keyword_t keyword_accumulator[] =
{
  {"d0.lo", 0, 0}, {"d0.hi", 1, 0}, {"d1.lo", 2, 0}, {"d1.hi", 3, 0},
  {NULL, 0, 0}
};

const keyword_t keyword_aridx[] =
{
  {"i0", 0, 0}, {"i1", 1, 0}, {"i2", 2, 0}, {"i3", 3, 0},
  {"i4", 4, 0}, {"i5", 5, 0}, {"i6", 6, 0}, {"i7", 7, 0},
  {"mod", 8, 0}, {"m1", 9, 0}, {"m2", 10, 0}, {"m3",11, 0},
  {"m4",12, 0}, {"m5",13, 0}, {"m6",14, 0}, {"m7",15, 0},
  {"d0.l24", 16, 0}, {"d1.l24", 17, 0},
  {"shft_ctr0", 18, 0}, {"shft_ctr1", 19, 0},
  {"aridx20", 20, 0}, {"aridx21", 21, 0},
  {"aridx22", 22, 0}, {"aridx23", 23, 0},
  {"lb", 24, 0}, {"le", 25, 0}, {"lc", 26, 0}, {"adm_vbase", 27, 0},
  {NULL, 0, 0}
};

const keyword_t *keywords[_HW_LAST] =
{
  keyword_gpr, keyword_usr, keyword_dxr, keyword_sr, keyword_fsr,
  keyword_fdr, keyword_cp, keyword_cpr, keyword_abdim, keyword_abm,
  keyword_dtiton, keyword_dtitoff, keyword_dpref_st,
  keyword_cctl_st0, keyword_cctl_st1, keyword_cctl_st2,
  keyword_cctl_st3, keyword_cctl_st4, keyword_cctl_st5,
  keyword_cctl_lv, keyword_tlbop_st, keyword_standby_st,
  keyword_msync_st,
  keyword_im5_i, keyword_im5_m,
  keyword_accumulator, keyword_aridx,
};

const int keywords_maxidx[_HW_LAST] =
{
  sizeof (keyword_gpr) / sizeof (keyword_t) - 1,
  sizeof (keyword_usr) / sizeof (keyword_t) - 1,
  sizeof (keyword_dxr) / sizeof (keyword_t) - 1,
  sizeof (keyword_sr) / sizeof (keyword_t) - 1,
  sizeof (keyword_fsr) / sizeof (keyword_t) - 1,
  sizeof (keyword_fdr) / sizeof (keyword_t) - 1,
  sizeof (keyword_cp) / sizeof (keyword_t) - 1,
  sizeof (keyword_cpr) / sizeof (keyword_t) - 1,
  sizeof (keyword_abdim) / sizeof (keyword_t) - 1,
  sizeof (keyword_abm) / sizeof (keyword_t) - 1,
  sizeof (keyword_dtiton) / sizeof (keyword_t) - 1,
  sizeof (keyword_dtitoff) / sizeof (keyword_t) - 1,
  sizeof (keyword_dpref_st) / sizeof (keyword_t) - 1,
  sizeof (keyword_cctl_st0) / sizeof (keyword_t) - 1,
  sizeof (keyword_cctl_st1) / sizeof (keyword_t) - 1,
  sizeof (keyword_cctl_st2) / sizeof (keyword_t) - 1,
  sizeof (keyword_cctl_st3) / sizeof (keyword_t) - 1,
  sizeof (keyword_cctl_st4) / sizeof (keyword_t) - 1,
  sizeof (keyword_cctl_st5) / sizeof (keyword_t) - 1,
  sizeof (keyword_cctl_lv) / sizeof (keyword_t) - 1,
  sizeof (keyword_tlbop_st) / sizeof (keyword_t) - 1,
  sizeof (keyword_standby_st) / sizeof (keyword_t) - 1,
  sizeof (keyword_msync_st) / sizeof (keyword_t) - 1,
  sizeof (keyword_im5_i) / sizeof (keyword_im5_i) - 1,
  sizeof (keyword_im5_m) / sizeof (keyword_im5_m) - 1,
  sizeof (keyword_accumulator) / sizeof (keyword_accumulator) - 1,
  sizeof (keyword_aridx) / sizeof (keyword_aridx) - 1,
};


/* Hash table for syntax lex.   */
static htab_t field_htab;
/* Hash table for opcodes.  */
static htab_t opcode_htab;
/* Hash table for hardware resources.  */
static htab_t hw_ktabs[_HW_LAST];

static hashval_t
htab_hash_hash (const void *p)
{
  struct nds32_hash_entry *h = (struct nds32_hash_entry *) p;

  return htab_hash_string (h->name);
}

static int
htab_hash_eq (const void *p, const void *q)
{
  struct nds32_hash_entry *h = (struct nds32_hash_entry *) p;
  const char *name = (const char *) q;

  return strcmp (name, h->name) == 0;
}


/* Build a hash table for array BASE.  Each element is in size of SIZE,
   and it's first element is a pointer to the key of string.
   It stops inserting elements until reach an NULL key.  */

static htab_t
build_hash_table (const void *base, size_t size)
{
  htab_t htab;
  hashval_t hash;
  const char *p;

  htab = htab_create_alloc (128, htab_hash_hash, htab_hash_eq,
			    NULL, xcalloc, free);

  p = base;
  while (1)
    {
      struct nds32_hash_entry **slot;
      struct nds32_hash_entry *h;

      h = (struct nds32_hash_entry *) p;

      if (h->name == NULL)
	break;

      hash = htab_hash_string (h->name);
      slot = (struct nds32_hash_entry **)
	htab_find_slot_with_hash (htab, h->name, hash, INSERT);

      assert (slot != NULL && *slot == NULL);

      *slot = h;

      p = p + size;
    }

  return htab;
}

/* Build the syntax for a given opcode OPC.  It parses the string
   pointed by INSTRUCTION and store the result on SYNTAX, so
   when we assemble an instruction, we don't have to parse the syntax
   again.  */

static void
build_opcode_syntax (struct nds32_opcode *opc)
{
  char odstr[MAX_LEX_LEN];
  const char *str;
  const char *end;
  lex_t *plex;
  int len;
  hashval_t hash;
  field_t *fd;
  int opt = 0;

  /* Check whether it has been initialized.  */
  if (opc->syntax)
    return;

  opc->syntax = xmalloc (MAX_LEX_NUM * sizeof (lex_t));

  str = opc->instruction;
  plex = opc->syntax;
  while (*str)
    {
      int fidx;

      switch (*str)
	{
	case '%': *plex = SYN_INPUT; break;
	case '=': *plex = SYN_OUTPUT; break;
	case '&': *plex = SYN_INPUT | SYN_OUTPUT; break;
	case '{':
		  *plex++ = SYN_LOPT;
		  opt++;
		  str++;
		  continue;
	case '}':
		  *plex++ = SYN_ROPT;
		  str++;
		  continue;
	default:
		  *plex++ = *str++;
		  continue;
	}
      str++;

      /* Extract operand.  */
      end = str;
      while (ISALNUM (*end) || *end == '_')
	end++;
      len = end - str;
      memcpy (odstr, str, len);
      odstr[len] = '\0';

      hash = htab_hash_string (odstr);
      fd = (field_t *) htab_find_with_hash (field_htab, odstr, hash);
      fidx = fd - operand_fields;

      if (fd == NULL)
	{
	  fprintf (stderr, "Internal error: Unknown operand, %s\n", str);
	}
      assert (fd && fidx >= 0 && fidx < (int) ARRAY_SIZE (operand_fields));
      *plex |= LEX_SET_FIELD (fidx);

      str += len;
      plex++;
    }

  *plex = 0;
  opc->variant = opt;
  return;

  fprintf (stderr, "Unknown lex in assembly syntax, %s.\n", str);
  abort ();
}

/* Initialize the assembler.  It must be called before assembling.  */

void
nds32_asm_init (nds32_asm_desc_t *pdesc, int flags)
{
  int i;
  hashval_t hash;

  pdesc->flags = flags;
  pdesc->mach = flags & NASM_OPEN_ARCH_MASK;

  /* Build keyword tables.  */
  field_htab = build_hash_table (operand_fields,
				 sizeof (operand_fields[0]));

  for (i = 0; i < _HW_LAST; i++)
    hw_ktabs[i] = build_hash_table (keywords[i], sizeof (keyword_t));

  /* Build opcode table.  */
  opcode_htab = htab_create_alloc (128, htab_hash_hash, htab_hash_eq,
				   NULL, xcalloc, free);

  for (i = 0; i < (int) ARRAY_SIZE (nds32_opcodes); i++)
    {
      struct nds32_opcode **slot;
      struct nds32_opcode *opc;

      opc = &nds32_opcodes[i];
      if ((opc->opcode != NULL) && (opc->instruction != NULL))
	{
	  hash = htab_hash_string (opc->opcode);
	  slot = (struct nds32_opcode **)
	    htab_find_slot_with_hash (opcode_htab, opc->opcode, hash, INSERT);

#define NDS32_PREINIT_SYNTAX
#if defined (NDS32_PREINIT_SYNTAX)
	  /* Initial SYNTAX when build opcode table, so bug in syntax can be
	     found when initialized rather than used.  */
	  build_opcode_syntax (opc);
#endif

	  if (*slot == NULL)
	    {
	      /* This is the new one.  */
	      *slot = opc;
	    }
	  else
	    {
	      /* Already exists.  Append to the list.  */
	      opc = *slot;
	      while (opc->next)
		opc = opc->next;
	      opc->next = &nds32_opcodes[i];
	    }
	}
    }
}

/* Parse the input and store operand keyword string in ODSTR.
   This function is only used for parsing keywords,
   HW_INT/HW_UINT are parsed parse_operand callback handler.  */

static char *
parse_to_delimiter (char *str, char odstr[MAX_KEYWORD_LEN])
{
  char *outp = odstr;

  while (ISALNUM (*str) || *str == '.' || *str == '_')
    *outp++ = TOLOWER (*str++);

  *outp = '\0';
  return str;
}

/* Parse the operand of push25/pop25.  */

static int
parse_re2 (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	   struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	   char **pstr, int64_t *value)
{
  char *end = *pstr;
  char odstr[MAX_KEYWORD_LEN];
  keyword_t *k;
  hashval_t hash;

  if (*end == '$')
    end++;
  end = parse_to_delimiter (end, odstr);

  hash = htab_hash_string (odstr);
  k = htab_find_with_hash (hw_ktabs[HW_GPR], odstr, hash);

  if (k == NULL)
    return NASM_ERR_OPERAND;

  if (k->value == 6)
    *value = 0;
  else if (k->value == 8)
    *value = 1;
  else if (k->value == 10)
    *value = 2;
  else if (k->value == 14)
    *value = 3;
  else
    return NASM_ERR_OPERAND;

  *pstr = end;
  return NASM_R_CONST;
}

/* Parse the operand of lwi45.fe.  */

static int
parse_fe5 (struct nds32_asm_desc *pdesc, struct nds32_asm_insn *pinsn,
	   char **pstr, int64_t *value)
{
  int r;

  r = pdesc->parse_operand (pdesc, pinsn, pstr, value);
  if (r != NASM_R_CONST)
    return r;

  /* 128 == 32 << 2.  Leave the shift to parse_opreand,
     so it can check whether it is a multiple of 4.  */
  *value = 128 + *value;
  return r;
}

/* Parse the operand of movpi45.  */

static int
parse_pi5 (struct nds32_asm_desc *pdesc, struct nds32_asm_insn *pinsn,
	   char **pstr, int64_t *value)
{
  int r;

  r = pdesc->parse_operand (pdesc, pinsn, pstr, value);
  if (r != NASM_R_CONST)
    return r;

  *value -= 16;
  return r;
}

int aext_a30b20 = 0;
int aext_rte = 0;
int aext_im5_ip = 0;
int aext_im6_ip = 0;
/* Parse the operand of audio ext.  */
static int
parse_aext_reg (char **pstr, int *value, int hw_res)
{
  char *end = *pstr;
  char odstr[MAX_KEYWORD_LEN];
  keyword_t *k;
  hashval_t hash;

  if (*end == '$')
    end++;
  end = parse_to_delimiter (end, odstr);

  hash = htab_hash_string (odstr);
  k = htab_find_with_hash (hw_ktabs[hw_res], odstr, hash);

  if (k == NULL)
    return NASM_ERR_OPERAND;

  *value = k->value;
  *pstr = end;
  return NASM_R_CONST;
}

static int
parse_a30b20 (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	      struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	      char **pstr, int64_t *value)
{
  int rt_value, ret;

  ret = parse_aext_reg (pstr, &rt_value, HW_GPR);

  if ((ret == NASM_ERR_OPERAND) || (rt_value > 15))
    return NASM_ERR_OPERAND;

  *value = rt_value;
  aext_a30b20 = rt_value;
  return NASM_R_CONST;
}

static int
parse_rt21 (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	    struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	    char **pstr, int64_t *value)
{
  int rt_value, ret, tmp_value, tmp1, tmp2;

  ret = parse_aext_reg (pstr, &rt_value, HW_GPR);

  if ((ret == NASM_ERR_OPERAND) || (rt_value > 15))
    return NASM_ERR_OPERAND;
  tmp1 = (aext_a30b20 & 0x08);
  tmp2 = (rt_value & 0x08);
  if (tmp1 != tmp2)
    return NASM_ERR_OPERAND;

  /* Rt=CONCAT(c, t21, t0), t21:bit11-10, t0:bit5.  */
  tmp_value = (rt_value & 0x06) << 4;
  tmp_value |= (rt_value & 0x01);
  *value = tmp_value;
  return NASM_R_CONST;
}

static int
parse_rte_start (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
		 struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
		 char **pstr, int64_t *value)
{
  int rt_value, ret, tmp1, tmp2;

  ret = parse_aext_reg (pstr, &rt_value, HW_GPR);

  if ((ret == NASM_ERR_OPERAND) || (rt_value > 15)
      || (rt_value & 0x01))
    return NASM_ERR_OPERAND;
  tmp1 = (aext_a30b20 & 0x08);
  tmp2 = (rt_value & 0x08);
  if (tmp1 != tmp2)
    return NASM_ERR_OPERAND;

  aext_rte = rt_value;
  /* Rt=CONCAT(c, t21, 0), t21:bit11-10.  */
  rt_value = (rt_value & 0x06) << 4;
  *value = rt_value;
  return NASM_R_CONST;
}

static int
parse_rte_end (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	       struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	       char **pstr, int64_t *value)
{
  int rt_value, ret, tmp1, tmp2;

  ret = parse_aext_reg (pstr, &rt_value, HW_GPR);
  if ((ret == NASM_ERR_OPERAND) || (rt_value > 15)
      || ((rt_value & 0x01) == 0)
      || (rt_value != (aext_rte + 1)))
    return NASM_ERR_OPERAND;
  tmp1 = (aext_a30b20 & 0x08);
  tmp2 = (rt_value & 0x08);
  if (tmp1 != tmp2)
    return NASM_ERR_OPERAND;
  /* Rt=CONCAT(c, t21, 0), t21:bit11-10.  */
  rt_value = (rt_value & 0x06) << 4;
  *value = rt_value;
  return NASM_R_CONST;
}

static int
parse_rte69_start (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
		   struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
		   char **pstr, int64_t *value)
{
  int rt_value, ret;

  ret = parse_aext_reg (pstr, &rt_value, HW_GPR);
  if ((ret == NASM_ERR_OPERAND)
      || (rt_value & 0x01))
    return NASM_ERR_OPERAND;
  aext_rte = rt_value;
  rt_value = (rt_value >> 1);
  *value = rt_value;
  return NASM_R_CONST;
}

static int
parse_rte69_end (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
		 struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
		 char **pstr, int64_t *value)
{
  int rt_value, ret;

  ret = parse_aext_reg (pstr, &rt_value, HW_GPR);
  if ((ret == NASM_ERR_OPERAND)
      || ((rt_value & 0x01) == 0)
      || (rt_value != (aext_rte + 1)))
    return NASM_ERR_OPERAND;
  aext_rte = rt_value;
  rt_value = (rt_value >> 1);
  *value = rt_value;
  return NASM_R_CONST;
}

static int
parse_im5_ip (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	      struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	      char **pstr, int64_t *value)
{
  int rt_value, ret, new_value;

  ret = parse_aext_reg (pstr, &rt_value, HW_AEXT_IM_I);
  if (ret == NASM_ERR_OPERAND)
    return NASM_ERR_OPERAND;
  /* p = bit[4].bit[1:0], r = bit[4].bit[3:2].  */
  new_value = (rt_value & 0x04) << 2;
  new_value |= (rt_value & 0x03);
  *value = new_value;
  aext_im5_ip = new_value;
  return NASM_R_CONST;
}

static int
parse_im5_mr (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	      struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	      char **pstr, int64_t *value)
{
  int rt_value, ret, new_value, tmp1, tmp2;

  ret = parse_aext_reg (pstr, &rt_value, HW_AEXT_IM_M);
  if (ret == NASM_ERR_OPERAND)
    return NASM_ERR_OPERAND;
  /* p = bit[4].bit[1:0], r = bit[4].bit[3:2].  */
  new_value = (rt_value & 0x07) << 2;
  tmp1 = (aext_im5_ip & 0x10);
  tmp2 = (new_value & 0x10);
  if (tmp1 != tmp2)
    return NASM_ERR_OPERAND;
  *value = new_value;
  return NASM_R_CONST;
}

static int
parse_im6_ip (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	      struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	      char **pstr, int64_t *value)
{
  int rt_value, ret;

  ret = parse_aext_reg (pstr, &rt_value, HW_AEXT_IM_I);
  if ((ret == NASM_ERR_OPERAND) || (rt_value > 3))
    return NASM_ERR_OPERAND;
  /* p = 0.bit[1:0].  */
  aext_im6_ip = rt_value;
  *value = aext_im6_ip;
  return NASM_R_CONST;
}

static int
parse_im6_iq (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	      struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	      char **pstr, int64_t *value)
{
  int rt_value, ret;

  ret = parse_aext_reg (pstr, &rt_value, HW_AEXT_IM_I);
  if ((ret == NASM_ERR_OPERAND) || (rt_value < 4))
    return NASM_ERR_OPERAND;
  /* q = 1.bit[1:0].  */
  if ((rt_value & 0x03) != aext_im6_ip)
    return NASM_ERR_OPERAND;
  *value = aext_im6_ip;
  return NASM_R_CONST;
}

static int
parse_im6_mr (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	      struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	      char **pstr, int64_t *value)
{
  int rt_value, ret;

  ret = parse_aext_reg (pstr, &rt_value, HW_AEXT_IM_M);
  if ((ret == NASM_ERR_OPERAND) || (rt_value > 3))
    return NASM_ERR_OPERAND;
  /* r = 0.bit[3:2].  */
  *value = (rt_value & 0x03);
  return NASM_R_CONST;
}

static int
parse_im6_ms (struct nds32_asm_desc *pdesc ATTRIBUTE_UNUSED,
	      struct nds32_asm_insn *pinsn ATTRIBUTE_UNUSED,
	      char **pstr, int64_t *value)
{
  int rt_value, ret;

  ret = parse_aext_reg (pstr, &rt_value, HW_AEXT_IM_M);
  if ((ret == NASM_ERR_OPERAND) || (rt_value < 4))
    return NASM_ERR_OPERAND;
  /* s = 1.bit[5:4].  */
  *value = (rt_value & 0x03);
  return NASM_R_CONST;
}

/* Generic operand parse base on the information provided by the field.  */

static int
parse_operand (nds32_asm_desc_t *pdesc, nds32_asm_insn_t *pinsn,
	       char **str, int syn)
{
  char odstr[MAX_KEYWORD_LEN];
  char *end;
  hashval_t hash;
  const field_t *fld = &LEX_GET_FIELD (syn);
  keyword_t *k;
  int64_t value = 0;    /* 0x100000000; Big enough to overflow.  */
  int r;
  uint64_t modifier = 0;

  end = *str;

  if (fld->parse)
    {
      r = fld->parse (pdesc, pinsn, &end, &value);
      if (r == NASM_ERR_OPERAND)
	{
	  pdesc->result = NASM_ERR_OPERAND;
	  return 0;
	}
      goto done;
    }

  if (fld->hw_res < _HW_LAST)
    {
      /* Parse the operand in assembly code.  */
      if (*end == '$')
	end++;
      end = parse_to_delimiter (end, odstr);

      hash = htab_hash_string (odstr);
      k = htab_find_with_hash (hw_ktabs[fld->hw_res], odstr, hash);

      if (k == NULL)
	{
	  pdesc->result = NASM_ERR_OPERAND;
	  return 0;
	}

      if (fld->hw_res == HW_GPR && (pdesc->flags & NASM_OPEN_REDUCED_REG)
	  && (k->attr & ATTR (RDREG)) == 0)
	{
	  /* Register not allowed in reduced register.  */
	  pdesc->result = NASM_ERR_REG_REDUCED;
	  return 0;
	}

      if (fld->hw_res == HW_GPR)
	{
	  if (syn & SYN_INPUT)
	    pinsn->defuse |= USE_REG (k->value);
	  if (syn & SYN_OUTPUT)
	    pinsn->defuse |= DEF_REG (k->value);
	}

      value = k->value;
      if (fld->hw_res == HW_GPR && (fld->bitsize + fld->shift) == 4)
	value = nds32_r54map[value];
    }
  else if (fld->hw_res == HW_INT || fld->hw_res == HW_UINT)
    {
      if (*end == '#')
	end++;

      /* Handle modifiers.  Do we need to make a table for modifiers?
	 Do we need to check unknown modifier?  */
      if (strncasecmp (end, "hi20(", 5) == 0)
	{
	  modifier |= NASM_ATTR_HI20;
	  end += 5;
	}
      else if (strncasecmp (end, "lo12(", 5) == 0)
	{
	  modifier |= NASM_ATTR_LO12;
	  end += 5;
	}
      else if (strncasecmp (end, "lo20(", 5) == 0)
	{
	  /* e.g., movi.  */
	  modifier |= NASM_ATTR_LO20;
	  end += 5;
	}

      r = pdesc->parse_operand (pdesc, pinsn, &end, &value);
      if (modifier)
	{
	  /* Consume the ')' of modifier.  */
	  end++;
	  pinsn->attr |= modifier;
	}

      switch (r)
	{
	case NASM_R_ILLEGAL:
	  pdesc->result = NASM_ERR_OPERAND;
	  return 0;
	case NASM_R_SYMBOL:
	  /* This field needs special fix-up.  */
	  pinsn->field = fld;
	  break;
	case NASM_R_CONST:
	  if (modifier & NASM_ATTR_HI20)
	    value = (value >> 12) & 0xfffff;
	  else if (modifier & NASM_ATTR_LO12)
	    value = value & 0xfff;
	  else if (modifier & NASM_ATTR_LO20)
	    value = value & 0xfffff;
	  break;
	default:
	  fprintf (stderr, "Internal error: Don't know how to handle "
		   "parsing results.\n");
	  abort ();
	}
    }
  else
    {
      fprintf (stderr, "Internal error: Unknown hardware resource.\n");
      abort ();
    }

done:
  /* Don't silently discarding bits.  */
  if (value & __MASK (fld->shift))
    {
      pdesc->result = NASM_ERR_OUT_OF_RANGE;
      return 0;
    }

  /* Check the range of signed or unsigned result.  */
  if (fld->hw_res != HW_INT && (value >> (fld->bitsize + fld->shift)))
    {
      pdesc->result = NASM_ERR_OUT_OF_RANGE;
      return 0;
    }
  else if (fld->hw_res == HW_INT)
    {
      /* Sign-ext the value.  */
      if (((value >> 32) == 0) && (value & 0x80000000))
	value |= (int64_t) -1 << 31;


      /* Shift the value to positive domain.  */
      if ((value + (1 << (fld->bitsize + fld->shift - 1)))
	  >> (fld->bitsize + fld->shift))
	{
	  pdesc->result = NASM_ERR_OUT_OF_RANGE;
	  return 0;
	}
    }

  pinsn->insn |= (((value >> fld->shift) & __MASK (fld->bitsize)) << fld->bitpos);
  *str = end;
  return 1;
}

/* Try to parse an instruction string based on opcode syntax.  */

static int
parse_insn (nds32_asm_desc_t *pdesc, nds32_asm_insn_t *pinsn,
	    char *str, struct nds32_opcode *opc)
{
  int variant = 0;
  char *p = NULL;

  /* A syntax may has optional operands, so we have to try each possible
     combination to see if the input is accepted.  In order to do so,
     bit-N represent whether optional-operand-N is used in this combination.
     That is, if bit-N is set, optional-operand-N is not used.

     For example, there are 2 optional operands in this syntax,

     "a{,b}{,c}"

     we can try it 4 times (i.e., 1 << 2)

     0 (b00): "a,b,c"
     1 (b01): "a,c"
     2 (b10): "a,b"
     3 (b11): "a"
   */

  /* The outer do-while loop is used to try each possible optional
     operand combination, and VARIANT is the bit mask.  The inner loop
     iterates each lexeme in the syntax.  */

  do
    {
      /* OPT is the number of optional operands we've seen.  */
      int opt = 0;
      lex_t *plex;

      /* PLEX is the syntax iterator and P is the iterator for input
	 string.  */
      plex = opc->syntax;
      p = str;
      /* Initial the base value.  */
      pinsn->insn = opc->value;

      while (*plex)
	{
	  if (IS_LEX_CHAR (*plex))
	    {
	      /* If it's a plain char, just compare it.  */
	      if (LEX_CHAR (*plex) != *p)
		{
		  pdesc->result = NASM_ERR_SYNTAX;
		  goto reject;
		}
	      p++;
	    }
	  else if (*plex & SYN_LOPT)
	    {
	      /* If it's '{' and it's not used in this iteration,
		 just skip the whole optional operand.  */
	      if ((1 << (opt++)) & variant)
		{
		  while ((*plex & SYN_ROPT) == 0)
		    plex++;
		}
	    }
	  else if (*plex & SYN_ROPT)
	    {
	      /* ignore.  */
	    }
	  else
	    {
	      /* If it's a operand, parse the input operand from input.  */
	      if (!parse_operand (pdesc, pinsn, &p, *plex))
		goto reject;
	    }
	  plex++;
	}

      /* Check whether this syntax is accepted.  */
      if (*plex == 0 && (*p == '\0' || *p == '!' || *p == '#'))
	return 1;

reject:
      /* If not accepted, try another combination.  */
      variant++;
    }
  while (variant < (1 << opc->variant));

  return 0;
}

void
nds32_assemble (nds32_asm_desc_t *pdesc, nds32_asm_insn_t *pinsn,
		char *str)
{
  struct nds32_opcode *opc;
  char *s;
  char *mnemoic;
  char *dot;
  hashval_t hash;

  /* Duplicate the string, so we can modify it for convenience.  */
  s = strdup (str);
  mnemoic = s;
  str = s;

  /* Find opcode mnemoic.  */
  while (*s != ' ' && *s != '\t' && *s != '\0')
    s++;
  if (*s != '\0')
    *s++ = '\0';
  dot = strchr (mnemoic, '.');

retry_dot:
  /* Lookup the opcode syntax.  */
  hash = htab_hash_string (mnemoic);
  opc = (struct nds32_opcode *)
    htab_find_with_hash (opcode_htab, mnemoic, hash);

  /* If we cannot find a match syntax, try it again without `.'.
     For example, try "lmw.adm" first and then try "lmw" again.  */
  if (opc == NULL && dot != NULL)
    {
      *dot = '\0';
      s[-1] = ' ';
      s = dot + 1;
      dot = NULL;
      goto retry_dot;
    }
  else if (opc == NULL)
    {
      pdesc->result = NASM_ERR_UNKNOWN_OP;
      goto out;
    }

  /* There may be multiple syntaxes for a given opcode.
     Try each one until a match is found.  */
  for (; opc; opc = opc->next)
    {
      /* Build opcode syntax, if it's not been initialized yet.  */
      if (opc->syntax == NULL)
	build_opcode_syntax (opc);

      /* Reset status before assemble.  */
      pinsn->defuse = opc->defuse;
      pinsn->insn = 0;
      pinsn->field = NULL;
      /* Use opcode attributes to initial instruction attributes.  */
      pinsn->attr = opc->attr;
      if (parse_insn (pdesc, pinsn, s, opc))
	break;
    }

  pinsn->opcode = opc;
  if (opc == NULL)
    {
      pdesc->result = NASM_ERR_SYNTAX;
      goto out;
    }

  /* A matched opcode is found.  Write the result to instruction buffer.  */
  pdesc->result = NASM_OK;

out:
  free (str);
}
