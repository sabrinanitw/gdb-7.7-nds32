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


#ifndef NDS32_ASM_H
#define NDS32_ASM_H

/* Constant values for assembler.  */
enum
{
  /* Error code for assembling an instruction.  */
  NASM_OK = 0,
  NASM_ERR_UNKNOWN_OP,
  NASM_ERR_SYNTAX,
  NASM_ERR_OPERAND,
  NASM_ERR_OUT_OF_RANGE,
  NASM_ERR_REG_REDUCED,
  NASM_ERR_JUNK_EOL,

  /* Results of parse_operand.  */
  NASM_R_CONST,
  NASM_R_SYMBOL,
  NASM_R_ILLEGAL,

  /* Flags for open description.  */
  NASM_OPEN_ARCH_V1		= 0x0,
  NASM_OPEN_ARCH_V2		= 0x1,
  NASM_OPEN_ARCH_V3		= 0x2,
  NASM_OPEN_ARCH_V3M		= 0x3,
  NASM_OPEN_ARCH_MASK		= 0xf,
  NASM_OPEN_REDUCED_REG		= 0x10,

  /* Common attributes.  */
  NASM_ATTR_ISA_V1		= 0x01,
  NASM_ATTR_ISA_V2		= 0x02,
  NASM_ATTR_ISA_V3		= 0x04,
  NASM_ATTR_ISA_V3M		= 0x08,
  NASM_ATTR_ISA_ALL		= 0x0f,

  /* Attributes for instructions.  */
  NASM_ATTR_MAC			= 0x0000100,
  NASM_ATTR_DIV			= 0x0000200,
  NASM_ATTR_FPU			= 0x0000400,
  NASM_ATTR_FPU_SP_EXT		= 0x0000800,
  NASM_ATTR_FPU_DP_EXT		= 0x0001000,
  NASM_ATTR_STR_EXT		= 0x0002000,
  NASM_ATTR_PERF_EXT		= 0x0004000,
  NASM_ATTR_PERF2_EXT		= 0x0008000,
  NASM_ATTR_AUDIO_ISAEXT	= 0x0010000,
  NASM_ATTR_IFC_EXT		= 0x0020000,
  NASM_ATTR_EX9_EXT		= 0x0040000,
  NASM_ATTR_FPU_FMA		= 0x0080000,
  NASM_ATTR_DXREG		= 0x0100000,
  NASM_ATTR_BRANCH		= 0x0200000,
  NASM_ATTR_RELAXABLE		= 0x0400000,
  NASM_ATTR_PCREL		= 0x0800000,
  NASM_ATTR_GPREL		= 0x1000000,

  /* Attributes for relocations.  */
  NASM_ATTR_HI20		= 0x10000000,
  NASM_ATTR_LO12		= 0x20000000,
  NASM_ATTR_LO20		= 0x40000000,

  /* Attributes for registers.  */
  NASM_ATTR_RDREG		= 0x000100,

};

enum
{
  /* This is a field (operand) of just a separator char.  */
  SYN_FIELD = 0x100,

  /* This operand is used for input or output.  (define or use)  */
  SYN_INPUT = 0x1000,
  SYN_OUTPUT = 0x2000,
  SYN_LOPT = 0x4000,
  SYN_ROPT = 0x8000,

  /* Hardware resources.  */
  HW_GPR = 0,
  HW_USR,
  HW_DXR,
  HW_SR,
  HW_FSR,
  HW_FDR,
  HW_CP,	/* Co-processor ID.  */
  HW_CPR,	/* Co-processor registers.  */
  HW_ABDIM,	/* [ab][di]m? flag for LSMWA?.  */
  HW_ABM,	/* [ab]m? flag for LSMWZB.  */
  HW_DTITON,
  HW_DTITOFF,
  HW_DPREF_ST,
  HW_CCTL_ST0,
  HW_CCTL_ST1,
  HW_CCTL_ST2,
  HW_CCTL_ST3,
  HW_CCTL_ST4,
  HW_CCTL_ST5,
  HW_CCTL_LV,
  HW_TLBOP_ST,
  HW_STANDBY_ST,
  HW_MSYNC_ST,
  HW_AEXT_IM_I,
  HW_AEXT_IM_M,
  HW_AEXT_ACC,
  HW_AEXT_ARIDX,
  _HW_LAST,
  /* TODO: Maybe we should add a new type to distinguish address and
	   const int.  Only the former allows symbols and relocations.  */
  HW_INT,
  HW_UINT,
};

/* for audio-extension.  */
enum
{
  N32_AEXT_AMADD = 0,
  N32_AEXT_AMSUB,
  N32_AEXT_AMULT,
  N32_AEXT_AMFAR,
  N32_AEXT_AMADDS,
  N32_AEXT_AMSUBS,
  N32_AEXT_AMULTS,
  N32_AEXT_AMNEGS,
  N32_AEXT_AADDL,
  N32_AEXT_AMTARI,
  N32_AEXT_AMAWBS = 0x0c,
  N32_AEXT_AMAWTS,
  N32_AEXT_AMWBS,
  N32_AEXT_AMWTS,
  N32_AEXT_AMABBS,
  N32_AEXT_AMABTS,
  N32_AEXT_AMATBS,
  N32_AEXT_AMATTS,
  N32_AEXT_AMBBS,
  N32_AEXT_AMBTS,
  N32_AEXT_AMTBS,
  N32_AEXT_AMTTS,
};

#define OPCODE_BASE_DPREFI  64
#define OPCODE_BASE_LBGP    (OPCODE_BASE_DPREFI+2)

#define OPCODE_BASE_LWC     (OPCODE_BASE_LBGP+2)
#define OPCODE_BASE_SWC     (OPCODE_BASE_LWC+2)
#define OPCODE_BASE_LDC     (OPCODE_BASE_SWC+2)
#define OPCODE_BASE_SDC     (OPCODE_BASE_LDC+2)

#define OPCODE_BASE_LSMW    (OPCODE_BASE_SDC+2)

#define OPCODE_BASE_LSMW_SMW     (OPCODE_BASE_LSMW+3)
#define OPCODE_BASE_LSMW_SMWA    (OPCODE_BASE_LSMW_SMW+1)
#define OPCODE_BASE_LSMW_SMWZB   (OPCODE_BASE_LSMW_SMWA+1)

#define OPCODE_BASE_HWGP    (OPCODE_BASE_LSMW+6) /* 16 */
#define OPCODE_BASE_SBGP    (OPCODE_BASE_HWGP+8)
#define OPCODE_BASE_JI      (OPCODE_BASE_SBGP+2)
#define OPCODE_BASE_JREG    (OPCODE_BASE_JI+2)

#define OPCODE_BASE_JREG_RET          (OPCODE_BASE_JREG+4)
#define OPCODE_BASE_JREG_IFRET        (OPCODE_BASE_JREG_RET+1)
#define OPCODE_BASE_JREG_JRAL_RT30    (OPCODE_BASE_JREG_IFRET+1)
#define OPCODE_BASE_JREG_JRALNEZ_RT30 (OPCODE_BASE_JREG_JRAL_RT30+1)
#define OPCODE_BASE_JREG_RET_RB30     (OPCODE_BASE_JREG_JRALNEZ_RT30+1)

#define OPCODE_BASE_BR1     (OPCODE_BASE_JREG+13)
#define OPCODE_BASE_BR2     (OPCODE_BASE_BR1+2)
#define OPCODE_BASE_BR3     (OPCODE_BASE_BR2+16)
#define OPCODE_BASE_SIMD    (OPCODE_BASE_BR3+2)

#define OPCODE_BASE_ALU1          (OPCODE_BASE_SIMD+2)
#define OPCODE_BASE_ALU1_ADDSLLI  (OPCODE_BASE_ALU1+32)
#define OPCODE_BASE_ALU1_SUBSLLI  (OPCODE_BASE_ALU1_ADDSLLI+1)
#define OPCODE_BASE_ALU1_ANDSLLI  (OPCODE_BASE_ALU1_SUBSLLI+1)
#define OPCODE_BASE_ALU1_XORSLLI  (OPCODE_BASE_ALU1_ANDSLLI+1)
#define OPCODE_BASE_ALU1_ORSLLI   (OPCODE_BASE_ALU1_XORSLLI+1)
#define OPCODE_BASE_ALU1_NOP      (OPCODE_BASE_ALU1_ORSLLI+1)
#define OPCODE_BASE_ALU1_END      (OPCODE_BASE_ALU1+32+6)

#define OPCODE_BASE_ALU2          OPCODE_BASE_ALU1_END
#define OPCODE_BASE_ALU2_FFBI     (OPCODE_BASE_ALU2+0x38)
#define OPCODE_BASE_ALU2_FLMISM   (OPCODE_BASE_ALU2_FFBI+2)
#define OPCODE_BASE_ALU2_MULSR64  (OPCODE_BASE_ALU2_FLMISM+2)
#define OPCODE_BASE_ALU2_MULR64   (OPCODE_BASE_ALU2_MULSR64+2)
#define OPCODE_BASE_ALU2_MADDR32  (OPCODE_BASE_ALU2_MULR64+2)
#define OPCODE_BASE_ALU2_MSUBR32  (OPCODE_BASE_ALU2_MADDR32+2)
#define OPCODE_BASE_ALU2_END      (OPCODE_BASE_ALU2_MSUBR32+2)

#define OPCODE_BASE_MISC          OPCODE_BASE_ALU2_END
#define OPCODE_BASE_MISC_MTSR     (OPCODE_BASE_MISC+16)
#define OPCODE_BASE_MISC_SETEND   (OPCODE_BASE_MISC_MTSR+3)
#define OPCODE_BASE_MISC_SETGIE   (OPCODE_BASE_MISC_SETEND+2)
#define OPCODE_BASE_MISC_CCTL     (OPCODE_BASE_MISC_SETGIE+2)
#define OPCODE_BASE_MISC_TLBOP    (OPCODE_BASE_MISC_CCTL+32)
#define OPCODE_BASE_MISC_END      (OPCODE_BASE_MISC_TLBOP+8)

#define OPCODE_BASE_MEM           OPCODE_BASE_MISC_END
#define OPCODE_BASE_MEM_END       (OPCODE_BASE_MEM+71+8)

#define OPCODE_BASE_LWC0          (OPCODE_BASE_MEM_END)
#define OPCODE_BASE_SWC0          (OPCODE_BASE_LWC0+2)
#define OPCODE_BASE_LDC0          (OPCODE_BASE_SWC0+2)
#define OPCODE_BASE_SDC0          (OPCODE_BASE_LDC0+2)
#define OPCODE_BASE_COP           (OPCODE_BASE_SDC0+2)
#define OPCODE_BASE_FPU           (OPCODE_BASE_COP+16)

#define OPCODE_BASE_COP_MFCPX     (OPCODE_BASE_FPU+16)
#define OPCODE_BASE_COP_CPLW      (OPCODE_BASE_COP_MFCPX+4)
#define OPCODE_BASE_COP_CPLD      (OPCODE_BASE_COP_CPLW+2)
#define OPCODE_BASE_COP_MTCPX     (OPCODE_BASE_COP_CPLD+2)
#define OPCODE_BASE_COP_CPSW      (OPCODE_BASE_COP_MTCPX+4)
#define OPCODE_BASE_COP_CPSD      (OPCODE_BASE_COP_CPSW+2)

#define OPCODE_BASE_FPU_FS1       (OPCODE_BASE_COP_CPSD+2)
#define OPCODE_BASE_FPU_FS1_F2OP  (OPCODE_BASE_FPU_FS1+16)
#define OPCODE_BASE_FPU_FS2       (OPCODE_BASE_FPU_FS1_F2OP+32)
#define OPCODE_BASE_FPU_FD1       (OPCODE_BASE_FPU_FS2+8)
#define OPCODE_BASE_FPU_FD1_F2OP  (OPCODE_BASE_FPU_FD1+16)
#define OPCODE_BASE_FPU_FD2       (OPCODE_BASE_FPU_FD1_F2OP+32)

#define OPCODE_BASE_FPU_MFCP      (OPCODE_BASE_FPU_FD2+8)
#define OPCODE_BASE_FPU_MFCP_XR   (OPCODE_BASE_FPU_MFCP+16)
#define OPCODE_BASE_FPU_MTCP      (OPCODE_BASE_FPU_MFCP_XR+2)
#define OPCODE_BASE_FPU_MTCP_XR   (OPCODE_BASE_FPU_MTCP+16)

#define OPCODE_BASE_FPU_FLS       (OPCODE_BASE_FPU_MTCP_XR+2)
#define OPCODE_BASE_FPU_FLD       (OPCODE_BASE_FPU_FLS+4)
#define OPCODE_BASE_FPU_FSS       (OPCODE_BASE_FPU_FLD+4)
#define OPCODE_BASE_FPU_FSD       (OPCODE_BASE_FPU_FSS+4)

#define OPCODE_BASE_ALIAS         (OPCODE_BASE_FPU_FSD+19)
#define BIT16_BASE_START          (OPCODE_BASE_ALIAS+2)

#define BIT16_BASE_SEG00          (BIT16_BASE_START+16)
#define BIT16_BASE_SEG01          (BIT16_BASE_SEG00+2)
#define BIT16_BASE_SEG02          (BIT16_BASE_SEG01+4)
#define BIT16_BASE_SEG03          (BIT16_BASE_SEG02+4)
#define BIT16_BASE_SEG04          (BIT16_BASE_SEG03+4)
#define BIT16_BASE_SEG05          (BIT16_BASE_SEG04+4)
#define BIT16_BASE_SEG06          (BIT16_BASE_SEG05+4)
#define BIT16_BASE_SEG07          (BIT16_BASE_SEG06+4)
#define BIT16_BASE_SEG08          (BIT16_BASE_SEG07+2)
#define BIT16_BASE_SEG09          (BIT16_BASE_SEG08+0)
#define BIT16_BASE_SEG10          (BIT16_BASE_SEG09+0)
#define BIT16_BASE_SEG10_1        (BIT16_BASE_SEG10+0)
#define BIT16_BASE_SEG11          (BIT16_BASE_SEG10+1)
#define BIT16_BASE_SEG11_1        (BIT16_BASE_SEG11+0)
#define BIT16_BASE_SEG11_2        (BIT16_BASE_SEG11_1+1)
#define BIT16_BASE_SEG11_3        (BIT16_BASE_SEG11_2+8)

#define BIT16_BASE_SEG12          (BIT16_BASE_SEG11+10)
#define BIT16_BASE_SEG13          (BIT16_BASE_SEG12+4)
#define BIT16_BASE_SEG13_1        (BIT16_BASE_SEG13+4)
#define BIT16_BASE_SEG13_2        (BIT16_BASE_SEG13_1+2)

#define BIT16_BASE_SEG14          (BIT16_BASE_SEG13+7)
#define BIT16_BASE_SEG15          (BIT16_BASE_SEG14+2)
#define BIT16_BASE_SEG15_1        (BIT16_BASE_SEG15+4)

#define BIT16_BASE_BFMI333        (BIT16_BASE_SEG15+6)
#define BIT16_BASE_PUSHPOP25      (BIT16_BASE_BFMI333+8)
#define BIT16_BASE_MISC33         (BIT16_BASE_PUSHPOP25+2)
#define BIT16_BASE_NOP16          (BIT16_BASE_MISC33+8)
#define BIT16_BASE_IFRET16        (BIT16_BASE_NOP16+1)


#define OPCODE_BASE_AEXT            (BIT16_BASE_IFRET16+1)
#define OPCODE_BASE_AEXT_AMADD      (OPCODE_BASE_AEXT+32)
#define OPCODE_BASE_AEXT_AMADD_1    (OPCODE_BASE_AEXT_AMADD+8)
#define OPCODE_BASE_AEXT_AMADD_2    (OPCODE_BASE_AEXT_AMADD_1+2)
#define OPCODE_BASE_AEXT_AMSUB      (OPCODE_BASE_AEXT_AMADD_2+2)
#define OPCODE_BASE_AEXT_AMSUB_1    (OPCODE_BASE_AEXT_AMSUB+8)
#define OPCODE_BASE_AEXT_AMSUB_2    (OPCODE_BASE_AEXT_AMSUB_1+2)
#define OPCODE_BASE_AEXT_AMULT      (OPCODE_BASE_AEXT_AMSUB_2+2)
#define OPCODE_BASE_AEXT_AMULT_1    (OPCODE_BASE_AEXT_AMULT+8)
#define OPCODE_BASE_AEXT_AMULT_2    (OPCODE_BASE_AEXT_AMULT_1+2)
#define OPCODE_BASE_AEXT_AMULT_3    (OPCODE_BASE_AEXT_AMULT_2+2)
#define OPCODE_BASE_AEXT_AMFAR      (OPCODE_BASE_AEXT_AMULT_3+2)
#define OPCODE_BASE_AEXT_AMFAR_1    (OPCODE_BASE_AEXT_AMFAR+4)
#define OPCODE_BASE_AEXT_AMFAR_2    (OPCODE_BASE_AEXT_AMFAR_1+2)
#define OPCODE_BASE_AEXT_AMADDS     (OPCODE_BASE_AEXT_AMFAR_2+2)
#define OPCODE_BASE_AEXT_AMADDS_1   (OPCODE_BASE_AEXT_AMADDS+8)
#define OPCODE_BASE_AEXT_AMADDS_2   (OPCODE_BASE_AEXT_AMADDS_1+2)
#define OPCODE_BASE_AEXT_AMSUBS     (OPCODE_BASE_AEXT_AMADDS_2+4)
#define OPCODE_BASE_AEXT_AMSUBS_1   (OPCODE_BASE_AEXT_AMSUBS+8)
#define OPCODE_BASE_AEXT_AMULTS     (OPCODE_BASE_AEXT_AMSUBS_1+4)
#define OPCODE_BASE_AEXT_AMULTS_1   (OPCODE_BASE_AEXT_AMULTS+8)
#define OPCODE_BASE_AEXT_AMNEGS     (OPCODE_BASE_AEXT_AMULTS_1+4)
#define OPCODE_BASE_AEXT_AMNEGS_1   (OPCODE_BASE_AEXT_AMNEGS+8)
#define OPCODE_BASE_AEXT_AADDL      (OPCODE_BASE_AEXT_AMNEGS_1+4)
#define OPCODE_BASE_AEXT_AMAWBS    (OPCODE_BASE_AEXT_AADDL+2)
#define OPCODE_BASE_AEXT_AMAWTS    (OPCODE_BASE_AEXT_AMAWBS+8)
#define OPCODE_BASE_AEXT_AMWBS     (OPCODE_BASE_AEXT_AMAWTS+8)
#define OPCODE_BASE_AEXT_AMWTS     (OPCODE_BASE_AEXT_AMWBS+8)
#define OPCODE_BASE_AEXT_AMABBS    (OPCODE_BASE_AEXT_AMWTS+8)
#define OPCODE_BASE_AEXT_AMABTS    (OPCODE_BASE_AEXT_AMABBS+8)
#define OPCODE_BASE_AEXT_AMATBS    (OPCODE_BASE_AEXT_AMABTS+8)
#define OPCODE_BASE_AEXT_AMATTS    (OPCODE_BASE_AEXT_AMATBS+8)
#define OPCODE_BASE_AEXT_AMBBS     (OPCODE_BASE_AEXT_AMATTS+8)
#define OPCODE_BASE_AEXT_AMBTS     (OPCODE_BASE_AEXT_AMBBS+8)
#define OPCODE_BASE_AEXT_AMTBS     (OPCODE_BASE_AEXT_AMBTS+8)
#define OPCODE_BASE_AEXT_AMTTS     (OPCODE_BASE_AEXT_AMTBS+8)

/* Macro for instruction attribute.  */
#define ATTR(attr)		NASM_ATTR_ ## attr
#define ATTR_NONE		0
#define ATTR_PCREL		(ATTR (PCREL) | ATTR (BRANCH))

#define ATTR_ALL		(ATTR (ISA_ALL))
#define ATTR_V2UP		(ATTR_ALL & ~(ATTR (ISA_V1)))
#define ATTR_V3MUP		(ATTR (ISA_V3) | ATTR (ISA_V3M))
#define ATTR_V3			(ATTR (ISA_V3))
#define ATTR_V3MEX_V1		(ATTR_ALL & ~(ATTR (ISA_V3M)))
#define ATTR_V3MEX_V2		(ATTR_V2UP & ~(ATTR (ISA_V3M)))

/* Lexical element in parsed syntax.  */
typedef int lex_t;

/* Common header for hash entries.  */
struct nds32_hash_entry
{
  const char *name;
};

typedef struct nds32_keyword
{
  const char *name;
  int value;
  uint64_t attr;
} keyword_t;

typedef struct nds32_opcode
{
  /* Opcode for the instruction.  */
  const char *opcode;
  /* Human readable string of this instruction.  */
  const char *instruction;
  /* Base value of this instruction.  */
  uint32_t value;
  /* The byte-size of the instruction.  */
  int isize;
  /* Attributes of this instruction.  */
  uint64_t attr;
  /* Implicit define/use.  */
  uint64_t defuse;
  /* Parsed string for assembling.  */
  lex_t *syntax;
  /* Number of variant.  */
  int variant;
  /* Next form of the same mnemonic.  */
  struct nds32_opcode *next;
  /* Next opcode-id for disassembler.  */
  unsigned long disnext;

  /* TODO: Extra constrains and verification.
	   For example, `mov55 $sp, $sp' is not allowed in v3.  */
} opcode_t;

typedef struct nds32_asm_insn
{
  /* Assembled instruction bytes.  */
  uint32_t insn;
  /* The opcode structure for this instruction.  */
  struct nds32_opcode *opcode;
  /* The field need special fix-up, used for relocation.  */
  const struct nds32_field *field;
  /* Attributes for relocation.  */
  uint64_t attr;
  /* Application-dependent data, e.g., expression.  */
  void *info;
  /* Input/output registers.  */
  uint64_t defuse;
} nds32_asm_insn_t;

typedef struct nds32_asm_desc
{
  /* The callback provided by assembler user for parse an operand,
     e.g., parse integer.  */
  int (*parse_operand) (struct nds32_asm_desc *desc,
			struct nds32_asm_insn *insn,
			char **str, int64_t *value);

  /* Result of assembling.  */
  int result;

  /* The mach for this assembling.  */
  int mach;

  int flags;
} nds32_asm_desc_t;

/* The field information for an operand.  */
typedef struct nds32_field
{
  /* Name of the field.  */
  const char *name;

  int bitpos;
  int bitsize;
  int shift;
  int hw_res;

  int (*parse) (struct nds32_asm_desc *desc,
		struct nds32_asm_insn *insn,
		char **str, int64_t *value);
} field_t;

void nds32_assemble (nds32_asm_desc_t *pdesc, nds32_asm_insn_t *pinsn,
		     char *str);
void nds32_asm_init (nds32_asm_desc_t *pdesc, int flags);

#endif
