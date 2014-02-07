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

#include "sysdep.h"
#include <stdio.h>
#include "ansidecl.h"
#include "dis-asm.h"
#include "bfd.h"
#include "symcat.h"
#include "libiberty.h"
#include "opintl.h"
#include "bfd_stdint.h"
#include "nds32-asm.h"
#include "opcode/nds32.h"

/* Get fields macro define.  */
#define OP6(insn)        ((insn >> 25) & 0x3F)
#define RT5(insn)        ((insn >> 20) & 0x1F)
#define RA5(insn)        ((insn >> 15) & 0x1F)
#define RB5(insn)        ((insn >> 10) & 0x1F)
#define RD5(insn)        ((insn >> 5) & 0x1F)
#define SUB5(insn)       ((insn >> 0) & 0x1F)
#define SUB10(insn)      ((insn >> 0) & 0x3FF)
#define IMMU(insn, bs)   (insn & ((1 << bs) - 1))
#define IMMS(insn, bs)   __SEXT((insn & ((1 << bs) - 1)), bs)
#define IMM1U(insn)      IMMU((insn >> 10), 5)
#define IMM1S(insn)      IMMS((insn >> 10), 5)
#define IMM2U(insn)      IMMU((insn >> 5), 5)
#define IMM2S(insn)      IMMS((insn >> 5), 5)

/* Default text to print if an instruction isn't recognized.  */
#define UNKNOWN_INSN_MSG _("*unknown*")

#define NDS32_PARSE_INSN16      0x01
#define NDS32_PARSE_INSN32      0x02
#define NDS32_PARSE_EX9IT       0x04
#define NDS32_PARSE_EX9TAB      0x08

extern struct nds32_opcode nds32_opcodes[];
extern const field_t operand_fields[];
extern const keyword_t *keywords[];
extern const int keywords_maxidx[];
extern const keyword_t keyword_gpr[];
static void print_insn16 (bfd_vma pc, disassemble_info *info,
			  uint32_t insn, uint32_t parse_mode);
static void print_insn32 (bfd_vma pc, disassemble_info *info,
			  uint32_t insn, uint32_t parse_mode);

/* define in objdump.c.  */
struct objdump_disasm_info
{
  bfd *              abfd;
  asection *         sec;
  bfd_boolean        require_sec;
  arelent **         dynrelbuf;
  long               dynrelcount;
  disassembler_ftype disassemble_fn;
  arelent *          reloc;
};

/* file_ptr    ex9_filepos=NULL;.  */
bfd_byte *ex9_data = NULL;
int ex9_ready = 0, ex9_base_offset = 0;

static void
nds32_get_itb_base (struct disassemble_info *info)
{
  asymbol *psym_tab;
  int i;

  for (i = 0; i < info->symtab_size; i++)
    {
      psym_tab = info->symtab[i];
      if (psym_tab->name)
	{
	  if (strstr (psym_tab->name, "_ITB_BASE_") != NULL)
	    ex9_base_offset = psym_tab->value;
	}
    }
}

static void
nds32_get_ex9_contents (disassemble_info *info, unsigned int *max_size)
{
  struct objdump_disasm_info *paux;
  bfd *abfd;
  asection *sect;
  bfd_size_type datasize = 0;

  paux = (struct objdump_disasm_info *) info->application_data;
  abfd = paux->abfd;
  sect = bfd_get_section_by_name (abfd, ".ex9.itable");
  if (sect == NULL)
    {
      /* check EX9-info ERROR.  */
      ex9_ready = -1;
      return;
    }
  datasize = bfd_get_section_size (sect);
  *max_size = datasize - 4;
  ex9_data = (bfd_byte *) xmalloc (datasize);
  bfd_get_section_contents (abfd, sect, ex9_data, 0, datasize);
  /* free (ex9_data), maybe free this buffer.  */
  ex9_ready = 1;
  nds32_get_itb_base (info);
}

static void
nds32_ex9_info (bfd_vma pc ATTRIBUTE_UNUSED,
		disassemble_info *info, uint32_t ex9_index)
{
  uint32_t insn;
  static unsigned int max_size = 0;

  if (ex9_ready == 0)
    nds32_get_ex9_contents (info, &max_size);
  if (ex9_ready == -1)
    return;

  if (max_size < (ex9_index << 2) + ex9_base_offset)
    return;

  insn = bfd_getb32 (&ex9_data[(ex9_index << 2) + ex9_base_offset]);
  /* 16-bit instructions in ex9 table.  */
  if (insn & 0x80000000)
    print_insn16 (pc, info, (insn & 0x0000FFFF),
		  NDS32_PARSE_INSN16 | NDS32_PARSE_EX9IT);
  /* 32-bit instructions in ex9 table.  */
  else
    print_insn32 (pc, info, insn, NDS32_PARSE_INSN32 | NDS32_PARSE_EX9IT);
}

static void
nds32_parse_audio_ext (const field_t *pfd,
		       disassemble_info *info, uint32_t insn)
{
  fprintf_ftype func = info->fprintf_func;
  void *stream = info->stream;
  keyword_t *psys_reg;
  int int_value, new_value;

  if (pfd->hw_res == HW_INT)
    {
      int_value =
	IMMS ((insn >> pfd->bitpos), pfd->bitsize) << pfd->shift;
      if (int_value < 0)
	func (stream, "#%d", int_value);
      else
	func (stream, "#0x%x", int_value);
      return;
    }
  int_value =
    __GF (insn, pfd->bitpos, pfd->bitsize) << pfd->shift;
  new_value = int_value;
  psys_reg = (keyword_t*)keywords[pfd->hw_res];

  /* p = bit[4].bit[1:0], r = bit[4].bit[3:2].  */
  if (strcmp (pfd->name, "im5_i") == 0)
    {
      new_value = int_value & 0x03;
      new_value |= ((int_value & 0x10) >> 2);
    }
  else if (strcmp (pfd->name, "im5_m") == 0)
    {
      new_value = ((int_value & 0x1C) >> 2);
    }
  /* p = 0.bit[1:0], r = 0.bit[3:2].  */
  /* q = 1.bit[1:0], s = 1.bit[5:4].  */
  else if (strcmp (pfd->name, "im6_iq") == 0)
    {
      new_value |= 0x04;
    }
  else if (strcmp (pfd->name, "im6_ms") == 0)
    {
      new_value |= 0x04;
    }
  else
    {
      /*  Rt CONCAT(c, t21, t0).  */
      if (strcmp (pfd->name, "a_rt21") == 0)
	{
	  new_value = (insn & 0x00000020) >> 5;
	  new_value |= (insn & 0x00000C00) >> 9;
	  new_value |= (insn & 0x00008000) >> 12;
	}
      else if (strcmp (pfd->name, "a_rte") == 0)
	{
	  new_value = (insn & 0x00000C00) >> 9;
	  new_value |= (insn & 0x00008000) >> 12;
	}
      else if (strcmp (pfd->name, "a_rte1") == 0)
	{
	  new_value = (insn & 0x00000C00) >> 9;
	  new_value |= (insn & 0x00008000) >> 12;
	  new_value |= 0x01;
	}
      else if (strcmp (pfd->name, "a_rte69") == 0)
	{
	  new_value = int_value << 1;
	}
      else if (strcmp (pfd->name, "a_rte69_1") == 0)
	{
	  new_value = int_value << 1;
	  new_value |= 0x01;
	}
    }
  psys_reg += new_value;
  func (stream, "$%s", psys_reg->name);
}

static void
nds32_parse_opcode (struct nds32_opcode *opc, bfd_vma pc ATTRIBUTE_UNUSED,
		    disassemble_info *info, uint32_t insn,
		    uint32_t parse_mode)
{
  int op = 0;
  fprintf_ftype func = info->fprintf_func;
  void *stream = info->stream;
  const char *pstr_src;
  char *pstr_tmp;
  char tmp_string[16];
  unsigned int push25gpr = 0, lsmwRb, lsmwRe, lsmwEnb4, checkbit, i;
  int int_value, max_index, ifthe1st = 1;
  const field_t *pfd;
  keyword_t *psys_reg;

  if (opc->instruction == NULL)
    {
      func (stream, UNKNOWN_INSN_MSG);
      return;
    }
  pstr_src = opc->instruction;
  if (*pstr_src == 0)
    {
      func (stream, "%s", opc->opcode);
      return;
    }
  /* NDS32_PARSE_INSN16.  */
  if (parse_mode & NDS32_PARSE_INSN16)
    {
      func (stream, "%s ", opc->opcode);
    }
  /* NDS32_PARSE_INSN32.  */
  else
    {
      op = OP6 (insn);
      if (op == N32_OP6_LSMW)
	func (stream, "%s.", opc->opcode);
      else
	func (stream, "%s ", opc->opcode);
    }

  while (*pstr_src)
    {
      switch (*pstr_src)
	{
	case '%':
	case '=':
	case '&':
	  pstr_src++;
	  /* compare with operand_fields[].name.  */
	  pstr_tmp = &tmp_string[0];
	  while (*pstr_src)
	    {
	      if ((*pstr_src == ',') || (*pstr_src == ' ')
		  || (*pstr_src == '{') || (*pstr_src == '}')
		  || (*pstr_src == '[') || (*pstr_src == ']')
		  || (*pstr_src == '(') || (*pstr_src == ')')
		  || (*pstr_src == '+') || (*pstr_src == '<'))
		break;
	      *pstr_tmp++ = *pstr_src++;
	    }
	  *pstr_tmp = 0;

	  pfd = (const field_t *) &operand_fields[0];
	  while (1)
	    {
	      if (pfd->name == NULL)
		return;
	      else if (strcmp (&tmp_string[0], pfd->name) == 0)
		break;
	      pfd++;
	    }

	  /* for insn-16.  */
	  if (parse_mode & NDS32_PARSE_INSN16)
	    {
	      if (pfd->hw_res == HW_GPR)
		{
		  int_value =
		    __GF (insn, pfd->bitpos, pfd->bitsize) << pfd->shift;
		  /* push25/pop25.  */
		  if ((opc->value == 0xfc00) || (opc->value == 0xfc80))
		    {
		      if (int_value == 0)
			int_value = 6;
		      else
			int_value = (6 + (0x01 << int_value));
		      push25gpr = int_value;
		    }
		  else if (strcmp (pfd->name, "rt4") == 0)
		    {
		      int_value = nds32_r45map[int_value];
		    }
		  func (stream, "$%s", keyword_gpr[int_value].name);
		}
	      else if ((pfd->hw_res == HW_INT) || (pfd->hw_res == HW_UINT))
		{
		  if (pfd->hw_res == HW_INT)
		    int_value =
		      IMMS ((insn >> pfd->bitpos),
			    pfd->bitsize) << pfd->shift;
		  else
		    int_value =
		      __GF (insn, pfd->bitpos, pfd->bitsize) << pfd->shift;

		  /* movpi45.  */
		  if (opc->value == 0xfa00)
		    {
		      int_value += 16;
		      func (stream, "#0x%x", int_value);
		    }
		  /* lwi45.fe.  */
		  else if (opc->value == 0xb200)
		    {
		      int_value = 0 - (128 - int_value);
		      func (stream, "#%d", int_value);
		    }
		  /* beqz38/bnez38/beqs38/bnes38/j8/beqzs8/bnezs8/ifcall9.  */
		  else if ((opc->value == 0xc000) || (opc->value == 0xc800)
			   || (opc->value == 0xd000) || (opc->value == 0xd800)
			   || (opc->value == 0xd500) || (opc->value == 0xe800)
			   || (opc->value == 0xe900)
			   || (opc->value == 0xf800))
		    {
		      info->print_address_func (int_value + pc, info);
		    }
		  /* push25/pop25.  */
		  else if ((opc->value == 0xfc00) || (opc->value == 0xfc80))
		    {
		      func (stream, "#%d    ! {$r6", int_value);
		      if (push25gpr != 6)
			func (stream, "~$%s", keyword_gpr[push25gpr].name);
		      func (stream, ", $fp, $gp, $lp}");
		    }
		  /* ex9.it.  */
		  else if ((opc->value == 0xdd40) || (opc->value == 0xea00))
		    {
		      func (stream, "#%d          ! ", int_value);
		      nds32_ex9_info (pc, info, int_value);
		    }
		  else if (pfd->hw_res == HW_INT)
		    {
		      if (int_value < 0)
			func (stream, "#%d", int_value);
		      else
			func (stream, "#0x%x", int_value);
		    }
		  else		/* if(pfd->hw_res == HW_UINT).  */
		    func (stream, "#0x%x", int_value);
		}

	    }
	  /* for audio-ext.  */
	  else if (op == N32_OP6_AEXT)
	    {
	      nds32_parse_audio_ext (pfd, info, insn);
	    }
	  /* for insn-32.  */
	  else if (pfd->hw_res < _HW_LAST)
	    {
	      int_value =
		__GF (insn, pfd->bitpos, pfd->bitsize) << pfd->shift;

	      psys_reg = (keyword_t*)keywords[pfd->hw_res];
	      max_index = keywords_maxidx[pfd->hw_res];
	      if ((pfd->hw_res == HW_SR) || (pfd->hw_res == HW_USR))
		{
		  while (1)
		    {
		      if (psys_reg->name == NULL)
			{
			  return;
			}
		      else if (psys_reg->value == int_value)
			{
			  if (pfd->hw_res == HW_SR)
			    {
			      psys_reg++;
			      if (psys_reg->value != int_value)
				psys_reg--;
			    }
			  func (stream, "$%s", psys_reg->name);
			  break;
			}
		      psys_reg++;
		    }
		}
	      else
		{
		  if (int_value >= max_index)
		    {
		      func (stream, UNKNOWN_INSN_MSG);
		    }
		  else
		    {
		      psys_reg += int_value;
		      if ((pfd->hw_res == HW_GPR) || (pfd->hw_res == HW_CPR) 
			  || (pfd->hw_res == HW_FDR) || (pfd->hw_res == HW_FSR)
			  || (pfd->hw_res == HW_DXR))
			func (stream, "$%s", psys_reg->name);
		      else
			func (stream, "%s", psys_reg->name);
		    }
		}
	    }
	  else if ((pfd->hw_res == HW_INT) || (pfd->hw_res == HW_UINT))
	    {
	      if (pfd->hw_res == HW_INT)
		int_value =
		  IMMS ((insn >> pfd->bitpos), pfd->bitsize) << pfd->shift;
	      else
		int_value =
		  __GF (insn, pfd->bitpos, pfd->bitsize) << pfd->shift;

	      if ((op == N32_OP6_BR1) || (op == N32_OP6_BR2))
		{
		  info->print_address_func (int_value + pc, info);
		}
	      else if ((op == N32_OP6_BR3) && (pfd->bitpos == 0))
		{
		  info->print_address_func (int_value + pc, info);
		}
	      else if (op == N32_OP6_JI)
		{
		  /* FIXME: Handle relocation.  */
		  if (info->flags & INSN_HAS_RELOC)
		    pc = 0;
		  /* check if insn32 in ex9 table.  */
		  if (parse_mode & NDS32_PARSE_EX9IT)
		    info->print_address_func ((pc & 0xFE000000) | int_value,
					      info);
		  /* check if decode ex9 table,  PC(31,25)|Inst(23,0)<<1.  */
		  else if (parse_mode & NDS32_PARSE_EX9TAB)
		    func (stream, "PC(31,25)|#0x%x", int_value);
		  else
		    info->print_address_func (int_value + pc, info);
		}
	      else if (op == N32_OP6_LSMW)
		{
		  /* lmw.adm/smw.adm.  */
		  func (stream, "#0x%x    ! {", int_value);
		  lsmwEnb4 = int_value;
		  lsmwRb = ((insn >> 20) & 0x1F);
		  lsmwRe = ((insn >> 10) & 0x1F);

		  /* If [Rb, Re] specifies at least one register,
		     Rb(4,0) <= Re(4,0) and 0 <= Rb(4,0), Re(4,0) < 28.  */
		  if ((lsmwRb <= lsmwRe) && (lsmwRe < 28))
		    {
		      func (stream, "$%s", keyword_gpr[lsmwRb].name);
		      if (lsmwRb != lsmwRe)
			func (stream, "~$%s", keyword_gpr[lsmwRe].name);
		      ifthe1st = 0;
		    }
		  if (lsmwEnb4 != 0)
		    {
		      /* $fp, $gp, $lp, $sp.  */
		      checkbit = 0x08;
		      for (i = 0; i < 4; i++)
			{
			  if (lsmwEnb4 & checkbit)
			    {
			      if (ifthe1st == 1)
				{
				  ifthe1st = 0;
				  func (stream, "$%s", keyword_gpr[28 + i].name);
				}
			      else
				func (stream, ", $%s", keyword_gpr[28 + i].name);
			    }
			  checkbit >>= 1;
			}
		    }
		  func (stream, "}");
		}
	      else if (pfd->hw_res == HW_INT)
		{
		  if (int_value < 0)
		    func (stream, "#%d", int_value);
		  else
		    func (stream, "#0x%x", int_value);
		}
	      else		/* if(pfd->hw_res == HW_UINT).  */
		{
		  func (stream, "#0x%x", int_value);
		}
	    }
	  break;

	case '{':
	case '}':
	  pstr_src++;
	  break;

	default:
	  func (stream, "%c", *pstr_src++);
	  break;
	}			/* switch (*pstr_src).  */

    }				/* while (*pstr_src).  */
}

static void
print_insn32 (bfd_vma pc, disassemble_info *info, uint32_t insn,
	      uint32_t parse_mode)
{
  int op = OP6 (insn);
  struct nds32_opcode *opc;
  unsigned int opc_index = 0, opc_next, bitstart, bitnums;
  unsigned int rt, ra, rb, rd;

  /* The 1st stage get nds32_opcodes.  */
  opc = &nds32_opcodes[op];
  while (1)
    {
      if (opc->instruction == NULL)
	{
	  /* The 2nd/3rd stage get nds32_opcodes.  */
	  if (opc->disnext == 0)
	    break;
	  bitstart = (opc->disnext >> 24) & 0xff;
	  bitnums = (opc->disnext >> 16) & 0xff;
	  opc_next = (opc->disnext & 0xffff);
	  opc_index = __GF (insn, bitstart, bitnums);

	  opc = &nds32_opcodes[opc_next];
	  if (opc_next == OPCODE_BASE_COP)
	    {
	      /* check if cp0 => FPU.  */
	      if (((insn >> 4) & 0x03) == 0)
		opc = &nds32_opcodes[OPCODE_BASE_FPU];
	    }
	  opc += opc_index;
	}
      /* check if special case.  */
      else if (opc->disnext != 0)
	{
	  opc_next = (opc->disnext & 0xffff);
	  if ((op == N32_OP6_LWC) || (op == N32_OP6_SWC)
	      || (op == N32_OP6_LDC) || (op == N32_OP6_SDC))
	    {
	      /* check if cp0 => FPU.  */
	      if (((insn >> 13) & 0x03) == 0)
		opc = &nds32_opcodes[opc_next];
	    }
	  else if (op == N32_OP6_ALU1)
	    {
	      /* check if (add/add_slli) (sub/sub_slli) (and/and_slli).  */
	      if (opc_index <= N32_ALU1_OR)
		{
		  rd = RD5 (insn);
		  if (rd != 0)
		    opc = &nds32_opcodes[opc_next];
		}
	      /* check if nop.  */
	      else if (opc_index == N32_ALU1_SRLI)
		{
		  rt = RT5 (insn);
		  ra = RA5 (insn);
		  rb = RB5 (insn);
		  if (ra == 0 && rt == 0 && rb == 0)
		    opc = &nds32_opcodes[opc_next];
		}
	    }
	  else if (op == N32_OP6_JREG)
	    {
	      rt = RT5 (insn);
	      rb = RB5 (insn);

	      /* JREG_RET        (1 << 5).  */
	      if (insn & (0x01 << 5))
		{
		  opc = &nds32_opcodes[opc_next];
		  if ((rb == 30) && (opc->disnext != 0))
		    opc = &nds32_opcodes[opc->disnext & 0xffff];
		}
	      else if (rt == 30)
		opc = &nds32_opcodes[opc_next];
	      else if (insn & (0x01 << 6))
		opc = &nds32_opcodes[OPCODE_BASE_JREG_IFRET];
	    }
	  else if (op == N32_OP6_LSMW)
	    {
	      /* check if store.  */
	      if (insn & (0x01 << 5))
		opc = &nds32_opcodes[opc_next];
	    }
	  break;
	}
      else
	break;
    }

  /* get the final correct opcode, parsing....  */
  nds32_parse_opcode (opc, pc, info, insn, parse_mode);
}

static void
print_insn16 (bfd_vma pc, disassemble_info *info,
	      uint32_t insn, uint32_t parse_mode)
{
  struct nds32_opcode *opc;
  unsigned int opc_index, opc_next, bitstart, bitnums;
  unsigned int rt3, ra5, rt5;

  /* The 1st stage get nds32_opcodes.  */
  opc = &nds32_opcodes[BIT16_BASE_START];
  opc_index = __GF (insn, 11, 4);
  opc += opc_index;

  while (1)
    {
      if (opc->instruction == NULL)
	{
	  /* The 2nd/3rd stage get nds32_opcodes.  */
	  if (opc->disnext == 0)
	    break;
	  bitstart = (opc->disnext >> 24) & 0xff;
	  bitnums = (opc->disnext >> 16) & 0xff;
	  opc_next = (opc->disnext & 0xffff);
	  opc_index = __GF (insn, bitstart, bitnums);

	  opc = &nds32_opcodes[opc_next];
	  opc += opc_index;
	}
      /* check if special case.  */
      else if (opc->disnext != 0)
	{
	  opc_next = (opc->disnext & 0xffff);
	  if (opc->value == 0x8000)
	    {
	      /* check if ifret16, rt5=0x1f && ra5=0x1f.  */
	      ra5 = __GF (insn, 0, 5);
	      rt5 = __GF (insn, 5, 5);
	      if ((rt5 == 0x1f) && (ra5 == 0x1f))
		opc = &nds32_opcodes[opc_next];
	      break;
	    }
	  else if (opc->value == 0x9200)
	    {
	      /* check if nop16, rt4=0,%i5u=0.  */
	      if (__GF (insn, 0, 8) == 0)
		opc = &nds32_opcodes[opc_next];
	      break;
	    }
	  else if ((opc->value == 0xd000) || (opc->value == 0xd800))
	    {
	      /* check if Rt3=5.  */
	      rt3 = __GF (insn, 8, 3);
	      if (rt3 == 5)
		{
		  if (opc->value == 0xd000)
		    {
		      opc = &nds32_opcodes[opc_next];
		      break;
		    }
		  /* case 0xd800, do not break.  */
		  else
		    opc = &nds32_opcodes[opc_next];
		}
	      else
		break;
	    }
	  else if (opc->value == 0xdd80)
	    {
	      /* check if Ra5=30.  */
	      ra5 = __GF (insn, 0, 5);
	      if (ra5 == 30)
		opc = &nds32_opcodes[opc_next];
	      break;
	    }
	  else if (opc->value == 0xea00)
	    {
	      /* check if imm4u!=0.  */
	      if (__GF (insn, 5, 4) != 0)
		opc = &nds32_opcodes[opc_next];
	      break;
	    }
	  else
	    return;
	}
      else
	break;
    }

  /* get the final correct opcode, parsing....  */
  nds32_parse_opcode (opc, pc, info, insn, parse_mode);
}

int
print_insn_nds32 (bfd_vma pc, disassemble_info *info)
{
  int status;
  bfd_byte buf[4];
  uint32_t insn;

  status = info->read_memory_func (pc, (bfd_byte *) buf, 4, info);
  if (status)
    {
      /* for the last 16-bit instruction.  */
      status = info->read_memory_func (pc, (bfd_byte *) buf, 2, info);
      if (status)
	return -1;
    }
  insn = bfd_getb32 (buf);
  /* 16-bit instruction.  */
  if (insn & 0x80000000)
    {
      if (info->section && strstr (info->section->name, ".ex9.itable") != NULL)
	{
	  print_insn16 (pc, info, (insn & 0x0000FFFF),
			NDS32_PARSE_INSN16 | NDS32_PARSE_EX9TAB);
	  return 4;
	}
      print_insn16 (pc, info, (insn >> 16), NDS32_PARSE_INSN16);
      return 2;
    }

  /* 32-bit instructions.  */
  else
    {
      if (info->section && strstr (info->section->name, ".ex9.itable") != NULL)
	print_insn32 (pc, info, insn,
		      NDS32_PARSE_INSN32 | NDS32_PARSE_EX9TAB);
      else
	print_insn32 (pc, info, insn, NDS32_PARSE_INSN32);
      return 4;
    }
}

/* Ignore disassembling ifc common block name.  */

bfd_boolean
nds32_symbol_is_valid (asymbol *sym,
		       struct disassemble_info *info ATTRIBUTE_UNUSED)
{
  const char *name;

  if (sym == NULL)
    return FALSE;

  name = bfd_asymbol_name (sym);

  if (name[0] == '$' && (strstr (name, "$nds32ifc_") != NULL
			 || strcmp (name, "$_ITB_BASE_") == 0))
    return FALSE;
  return TRUE;
}
