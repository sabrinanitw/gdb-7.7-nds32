/* Simulator for NDS32 processors.

   Copyright (C) 2011-2013 Free Software Foundation, Inc.
   Contributed by Andes Technology Corporation.

   This file is part of simulators.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include "bfd.h"
#include "gdb/callback.h"
#include "gdb/signals.h"
#include "libiberty.h"
#include "gdb/remote-sim.h"
#include "dis-asm.h"
#include "sim-main.h"
#include "nds32-sim.h"
#include "sim-utils.h"
#include "sim-fpu.h"

#include "opcode/nds32.h"
#include "nds32-sim.h"
#include "nds32-libc.h"

#ifdef __linux__
/* FIXME */
#include <sys/time.h>
#include <sys/times.h>
#endif

/* Debug flag to display instructions and registers.  */
static int tracing = 0;
static int lock_step = 0;
static int verbose;

/* We update a cycle counter.  */
static unsigned int cycles = 0;

static struct bfd *cur_bfd;

static SIM_OPEN_KIND sim_kind;
static char *myname;
static host_callback *callback;

static ulongest_t
extract_unsigned_integer (unsigned char *addr, int len, int byte_order)
{
  ulongest_t retval;
  const unsigned char *p;
  const unsigned char *startaddr = addr;
  const unsigned char *endaddr = startaddr + len;

  retval = 0;
  if (byte_order == BIG_ENDIAN)
    {
      for (p = startaddr; p < endaddr; ++p)
	retval = (retval << 8) | *p;
    }
  else
    {
      for (p = endaddr - 1; p >= startaddr; --p)
	retval = (retval << 8) | *p;
    }
  return retval;
}

static ulongest_t
extract_unsigned_integer_by_psw (sim_cpu *cpu, unsigned char *addr, int len)
{
  int order = CCPU_PSW_TEST (PSW_BE) ? BIG_ENDIAN : LITTLE_ENDIAN;

  return extract_unsigned_integer (addr, len, order);
}

static void
store_unsigned_integer (unsigned char *addr, int len,
			int byte_order, ulongest_t val)
{
  unsigned char *p;
  unsigned char *startaddr = addr;
  unsigned char *endaddr = startaddr + len;

  /* Start at the least significant end of the integer,
     and work towards the most significant.  */
  if (byte_order == BIG_ENDIAN)
    {
      for (p = endaddr - 1; p >= startaddr; --p)
	{
	  *p = val & 0xff;
	  val >>= 8;
	}
    }
  else
    {
      for (p = startaddr; p < endaddr; ++p)
	{
	  *p = val & 0xff;
	  val >>= 8;
	}
    }
}

static void
store_unsigned_integer_by_psw (sim_cpu *cpu, unsigned char *addr, int len, ulongest_t val)
{
  int order = CCPU_PSW_TEST (PSW_BE) ? BIG_ENDIAN : LITTLE_ENDIAN;

  store_unsigned_integer (addr, len, order, val);
}

/* Read a null terminated string from memory, return in a buffer */

static char *
fetch_str (SIM_DESC sd, address_word addr)
{
  char *buf;
  int nr = 0;
  char null;

  while (sim_read (sd, addr + nr, &null, 1) == 1 && null != 0)
    nr++;
  buf = NZALLOC (char, nr + 1);
  sim_read (sd, addr, buf, nr);

  return buf;
}

enum nds32_exceptions
{
  EXP_RESET = 0,
  EXP_TLB_FILL = 1,
  EXP_NO_PTE = 2,
  EXP_TLB_MISC = 3,
  EXP_TLB_VLPT_MISS = 4,
  EXP_MACHINE_ERROR = 5,
  EXP_DEBUG = 6,
  EXP_GENERAL = 7,
  EXP_SYSCALL = 8,

  EXP_BADOP,
};

void
nds32_bad_op (sim_cpu *cpu, uint32_t cia, uint32_t insn, char *tag)
{
  if (tag == NULL)
    tag = "";

  sim_io_error (CPU_STATE (cpu),
		"Unhandled %s instruction at 0x%x, code=0x%08x\n",
		tag, cia, insn);

  sim_engine_halt (CPU_STATE (cpu), cpu, NULL, cia, sim_signalled, SIM_SIGILL);
}

static sim_cia
nds32_syscall (sim_cpu *cpu, int swid, sim_cia cia)
{
  SIM_DESC sd = CPU_STATE (cpu);
  int r = -1;

  switch (swid)
    {
    case SYS_exit:
      sim_engine_halt (CPU_STATE (cpu), cpu, NULL, cia, sim_exited, CCPU_GPR[0].s);
      break;
    case SYS_open:
      {
	char *path = fetch_str (sd, CCPU_GPR[0].u);

	r = sim_io_open (sd, path, CCPU_GPR[1].u);
	free (path);
      }
      break;
    case SYS_close:
      r = sim_io_close (sd, CCPU_GPR[0].u);
      break;
    case SYS_read:
      {
	int fd = CCPU_GPR[0].s;
	SIM_ADDR addr = CCPU_GPR[1].s;
	int nr = CCPU_GPR[2].s;
	char *buf = zalloc (nr);

	r = sim_io_read (sd, fd, buf, nr);
	if (r > 0)
	  sim_write (sd, addr, buf, r);
	/* SIM_IO_DPRINTF (sd, "sys_read () = %s\n", buf); */
      }
      break;
    case SYS_write:
      {
	int fd = CCPU_GPR[0].s;
	SIM_ADDR addr = CCPU_GPR[1].s;
	int nr = CCPU_GPR[2].s;
	char *buf = zalloc (nr);

	sim_read (sd, addr, buf, nr);
#if 0
	if (fd == 1)
	  r = sim_io_write_stdout (sd, buf, nr);
	else if (fd == 2)
	  r = sim_io_write_stderr (sd, buf, nr);
	else
#endif
	/* SIM_IO_DPRINTF (sd, "sys_write (%s)\n", buf); */
	  r = sim_io_write (sd, fd, buf, nr);

	break;
      }
    case SYS_lseek:
      r = sim_io_lseek (sd, CCPU_GPR[0].s, CCPU_GPR[1].s, CCPU_GPR[2].s);
      break;
    case SYS_fstat:
      {
	SIM_ADDR addr = CCPU_GPR[1].s;
	struct stat stat;
	struct nds32_stat nstat;

	SIM_ASSERT (sizeof (struct nds32_stat) == 60);
	r = sim_io_fstat (sd, CCPU_GPR[0].s, &stat);
	if (r >= 0)
	  {
	    memset (&nstat, 0, sizeof (nstat));
	    nstat.st_dev = stat.st_dev;
	    nstat.st_ino = stat.st_ino;
	    nstat.st_mode = stat.st_mode;
	    nstat.st_nlink = stat.st_nlink;
	    nstat.st_uid = stat.st_uid;
	    nstat.st_gid = stat.st_gid;
	    nstat.st_rdev = stat.st_rdev;
	    nstat.st_size = stat.st_size;
	    nstat.st_atime_ = stat.st_atime;
	    nstat.st_mtime_ = stat.st_mtime;
	    nstat.st_ctime_ = stat.st_ctime;
	    sim_write (sd, addr, (unsigned char *) &nstat,
		       sizeof (struct nds32_stat));
	  }
      }
      break;
    case SYS_isatty:
      r = sim_io_isatty (sd, CCPU_GPR[0].s);
      break;
    case SYS_getcmdline:
      r = CCPU_GPR[0].u;
      sim_write (sd, CCPU_GPR[0].u, sd->cmdline, strlen (sd->cmdline) + 1);
      break;
#ifdef __linux__
    case SYS_gettimeofday:
      {
	struct timeval t;
	struct timezone tz;

	r = gettimeofday (&t, &tz);
	if (CCPU_GPR[0].u)
	  sim_write (sd, CCPU_GPR[0].u, (const unsigned char *) &t,
		     sizeof (t));
	if (CCPU_GPR[1].u)
	  sim_write (sd, CCPU_GPR[1].u, (const unsigned char *) &t,
		     sizeof (tz));
      }
      break;
#endif
    case SYS_unlink:
      {
	char *path;

	path = fetch_str (sd, CCPU_GPR[0].u);
	r = sim_io_unlink (sd, path);
	free (path);
      }
      break;
#ifdef __linux__
    case SYS_times:
      {
	struct tms tms;

	r = times (&tms);
	if (CCPU_GPR[0].u)
	  sim_write (sd, CCPU_GPR[0].u, (const unsigned char *) &tms,
		     sizeof (tms));
      }
      break;
#endif
    case SYS_rename:
    case SYS_errno:
    case SYS_time:
    default:
      nds32_bad_op (cpu, cia, swid, "syscall");
      return cia;
    }

  CCPU_GPR[0].s = r;
  return cia + 4;
}


longest_t
nds32_ld_sext (sim_cpu *cpu, SIM_ADDR addr, int size)
{
  int r;
  longest_t val = 0;
  int order;
  SIM_DESC sd = CPU_STATE (cpu);
  SIM_ASSERT (size <= sizeof (longest_t));

  r = sim_read (sd, addr, (unsigned char *) &val, size);
  order = CCPU_PSW_TEST (PSW_BE) ? BIG_ENDIAN : LITTLE_ENDIAN;
  val = extract_unsigned_integer ((unsigned char *) &val, size, order);
  val = __SEXT (val, size * 8);

  if (r != size)
    {
      uint32_t cia = CCPU_USR[NC_PC].u; /* FIXME */
      sim_io_eprintf (sd, "Access violation at 0x%08x. Read of address 0x%08x\n",
		      cia, addr);
      sim_engine_halt (CPU_STATE (cpu), cpu, NULL, cia, sim_signalled, SIM_SIGSEGV);
    }

  return val;
}

ulongest_t
nds32_ld (sim_cpu *cpu, SIM_ADDR addr, int size)
{
  int r;
  ulongest_t val = 0;
  int order;
  SIM_DESC sd = CPU_STATE (cpu);
  SIM_ASSERT (size <= sizeof (ulongest_t));

  r = sim_read (sd, addr, (unsigned char *) &val, size);
  order = CCPU_PSW_TEST (PSW_BE) ? BIG_ENDIAN : LITTLE_ENDIAN;
  val = extract_unsigned_integer ((unsigned char *) &val, size, order);

  if (r != size)
    {
      uint32_t cia = CCPU_USR[NC_PC].u; /* FIXME */
      sim_io_eprintf (sd, "Access violation at 0x%08x. Read of address 0x%08x\n",
		      cia, addr);
      sim_engine_halt (CPU_STATE (cpu), cpu, NULL, cia, sim_signalled, SIM_SIGSEGV);
    }

  return val;
}

void
nds32_st (sim_cpu *cpu, SIM_ADDR addr, int size, ulongest_t val)
{
  int r;
  int order;
  SIM_DESC sd = CPU_STATE (cpu);
  SIM_ASSERT (size <= sizeof (ulongest_t));

  order = CCPU_PSW_TEST (PSW_BE) ? BIG_ENDIAN : LITTLE_ENDIAN;
  store_unsigned_integer ((unsigned char *) &val, size, order, val);
  r = sim_write (sd, addr, (unsigned char *) &val, size);

  if (r != size)
    {
      uint32_t cia = CCPU_USR[NC_PC].u; /* FIXME */
      sim_io_eprintf (sd, "Access violation at %p. Write of address %p\n", (void*)cia, (void*)addr);
      sim_engine_halt (CPU_STATE (cpu), cpu, NULL, cia, sim_signalled, SIM_SIGSEGV);
    }

  return;
}

static void
nds32_free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}

void
sim_size (int s)
{
}

static sim_cia
nds32_decode32_mem (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  const int rt = N32_RT5 (insn);
  const int ra = N32_RA5 (insn);
  const int rb = N32_RB5 (insn);
  const int sv = __GF (insn, 8, 2);
  const int op = insn & 0xFF;

  switch (op)
    {
    case 0x0:			/* lb */
    case 0x1:			/* lh */
    case 0x2:			/* lw */
    case 0x3:			/* ld */
      CCPU_GPR[rt].u =
	nds32_ld (cpu, CCPU_GPR[ra].u + (CCPU_GPR[rb].u << sv), (1 << (op)));
      break;
    case 0x4:			/* lb.bi */
    case 0x5:			/* lh.bi */
    case 0x6:			/* lw.bi */
    case 0x7:			/* ld.bi */
      CCPU_GPR[rt].u = nds32_ld (cpu, CCPU_GPR[ra].u, (1 << (op & 0x3)));
      CCPU_GPR[ra].u += (CCPU_GPR[rb].u << sv);
      break;
    case 0x8:			/* sb */
    case 0x9:			/* sh */
    case 0xa:			/* sw */
    case 0xb:			/* sd */
      nds32_st (cpu, CCPU_GPR[ra].u + (CCPU_GPR[rb].u << sv), (1 << (op & 0x3)),
		CCPU_GPR[rt].u);
      break;
    case 0xc:			/* sb.bi */
    case 0xd:			/* sh.bi */
    case 0xe:			/* sw.bi */
    case 0xf:			/* sd.bi */
      nds32_st (cpu, CCPU_GPR[ra].u, (1 << (op & 0x3)), CCPU_GPR[rt].u);
      CCPU_GPR[ra].u += (CCPU_GPR[rb].u << sv);
      break;
    case 0x10:			/* lbs */
    case 0x11:			/* lhs */
    case 0x12:			/* lws */
      CCPU_GPR[rt].u =
	nds32_ld_sext (cpu, CCPU_GPR[ra].u + (CCPU_GPR[rb].u << sv), (1 << (op & 0x3)));
      break;
    case 0x13:			/* dpref */
      /* do nothing */
      break;
    case 0x14:			/* lbs.bi */
    case 0x15:			/* lhs.bi */
    case 0x16:			/* lws.bi */
      CCPU_GPR[rt].u = nds32_ld_sext (cpu, CCPU_GPR[ra].u, (1 << (op & 0x3)));
      CCPU_GPR[ra].u += (CCPU_GPR[rb].u << sv);
      break;
    case 0x18:			/* llw */
    case 0x19:			/* scw */
    case 0x20:			/* lbup */
    case 0x22:			/* lwup */
    case 0x28:			/* sbup */
    case 0x2a:			/* swup */
    default:
      nds32_bad_op (cpu, cia, insn, "MEM");
      return cia;
    }

  return cia + 4;
}

static sim_cia
nds32_decode32_lsmw (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  /* smwa?.(a|b)(d|i)m? rb,[ra],re,enable4 */
  SIM_DESC sd = CPU_STATE (cpu);
  int rb, re, ra, enable4, i;
  int aligned;
  int m = 0;
  int di;			/* dec=-1 or inc=1 */
  unsigned char reg[4];
  char enb4map[2][4] = { /*smw */ {0, 1, 2, 3}, /*smwa */ {3, 1, 2, 0} };
  ulongest_t base = ~1 + 1;

  rb = N32_RT5 (insn);
  ra = N32_RA5 (insn);
  re = N32_RB5 (insn);
  enable4 = (insn >> 6) & 0x0F;
  aligned = (insn & 3) ? 1 : 0;
  di = (insn & (1 << 3)) ? -1 : 1;

  /* m = TNReg * 4; */
  m += (enable4 & 0x1) ? 1 : 0;
  m += (enable4 & 0x2) ? 1 : 0;
  m += (enable4 & 0x4) ? 1 : 0;
  m += (enable4 & 0x8) ? 1 : 0;
  if (rb < NG_FP && re < NG_FP)
    {
      /* Reg-list should not include fp, gp, lp and sp,
	 i.e., the rb == re == sp case, anyway... */
      m += (re - rb) + 1;
    }
  m *= 4;			/* 4*TNReg */

  base = CCPU_GPR[ra].u;

  if (insn & (1 << 0x4))	/* a:b, a for +-4 */
    base += 4 * di;

  if (di == 1)
    base += (m - 4);

  switch (insn & 0x23)
    {
    case 33:			/* smwa */
      if (base & 3)
	{
	  sim_io_eprintf (sd, "SMWA: unaligned access at 0x%x. "
			      "Write of address 0x%llx.\n",
			  cia, base);
	  return cia;
	}
    case 32:			/* smw */
      /* TODO: alignment exception check for SMWA */
      for (i = 0; i < 4; i++)
	{
	  if (enable4 & (1 << enb4map[aligned][i]))
	    {
	      sim_write (sd, base, reg, 4);
	      nds32_st (cpu, base, 4,
			CCPU_GPR[NG_SP - (enb4map[aligned][i])].u);
	      base -= 4;
	    }
	}

      /* Skip if re == rb == sp > fp.  */
      for (i = re; i >= rb && rb < NG_FP; i--)
	{
	  nds32_st (cpu, base, 4, CCPU_GPR[i].u);
	  base -= 4;
	}

      if (insn & (1 << 2))
	CCPU_GPR[ra].u += m * di;
      break;
    case 1:			/* lmwa */
      if (base & 3)
	{
	  sim_io_eprintf (sd, "LMWA: unaligned access at 0x%x. "
			      "Read of address 0x%llx.\n",
			  cia, base);
	  return cia;
	}
    case 0:			/* lmw */
      /* TODO: alignment exception check for SMWA */
      for (i = 0; i < 4; i++)
	{
	  if (enable4 & (1 << enb4map[aligned][i]))
	    {
	      CCPU_GPR[NG_SP - (enb4map[aligned][i])].u
		= nds32_ld (cpu, base, 4);
	      base -= 4;
	    }
	}

      /* Skip if re == rb == sp > fp.  */
      for (i = re; i >= rb && rb < NG_FP; i--)
	{
	  CCPU_GPR[i].u = nds32_ld (cpu, base, 4);
	  base -= 4;
	}

      if (insn & (1 << 2))
	CCPU_GPR[ra].u += m * di;
      break;
    case 2:			/* lmwzb */
    case 34:			/* smwzb */
    default:
      nds32_bad_op (cpu, cia, insn, "LSMW");
      return cia;
    }

  return cia + 4;
}

static sim_cia
nds32_decode32_alu1 (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  int rt = N32_RT5 (insn);
  int ra = N32_RA5 (insn);
  int rb = N32_RB5 (insn);
  const int rd = N32_RD5 (insn);
  const int imm5u = rb;
  const int sh5 = N32_SH5 (insn);

  switch (insn & 0x1f)
    {
    case 0x0:			/* add, add_slli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u + (CCPU_GPR[rb].u << sh5);
      break;
    case 0x1:			/* sub, sub_slli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u - (CCPU_GPR[rb].u << sh5);
      break;
    case 0x2:			/* and, add_slli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u & (CCPU_GPR[rb].u << sh5);
      break;
    case 0x3:			/* xor, xor_slli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u ^ (CCPU_GPR[rb].u << sh5);
      break;
    case 0x4:			/* or, or_slli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u | (CCPU_GPR[rb].u << sh5);
      break;
    case 0x5:			/* nor */
      CCPU_GPR[rt].u = ~(CCPU_GPR[ra].u | CCPU_GPR[rb].u);
      break;
    case 0x6:			/* slt */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u < CCPU_GPR[rb].u ? 1 : 0;
      break;
    case 0x7:			/* slts */
      CCPU_GPR[rt].u = CCPU_GPR[ra].s < CCPU_GPR[rb].s ? 1 : 0;
      break;

    case 0x8:			/* slli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u << imm5u;
      break;
    case 0x9:			/* srli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u >> imm5u;
      break;
    case 0xa:			/* srai */
      CCPU_GPR[rt].s = CCPU_GPR[ra].s >> imm5u;
      break;
    case 0xc:			/* sll */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u << (CCPU_GPR[rb].u & 0x1f);
      break;
    case 0xd:			/* srl */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u >> CCPU_GPR[rb].u;
      break;
    case 0xe:			/* sra */
      CCPU_GPR[rt].s = CCPU_GPR[ra].s >> CCPU_GPR[rb].u;
      break;
    case 0xb:			/* rotri */
    case 0xf:			/* rotr */
      {
	uint32_t shift = ((insn & 0x1f) == 0xb) ? imm5u : CCPU_GPR[rb].u;
	uint32_t m = CCPU_GPR[ra].u & ((1 << shift) - 1);
	CCPU_GPR[rt].u = CCPU_GPR[ra].u >> shift;
	CCPU_GPR[rt].u |= m << (32 - shift);
      }
      break;

    case 0x10:			/* seb */
      CCPU_GPR[rt].s = __SEXT (CCPU_GPR[ra].s, 8);
      break;
    case 0x11:			/* seh */
      CCPU_GPR[rt].s = __SEXT (CCPU_GPR[ra].s, 16);
      break;
    case 0x12:			/* bitc */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u & ~(CCPU_GPR[rb].u);
      break;
    case 0x13:			/* zeh */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u & 0xffff;
      break;
    case 0x14:			/* wsbh */
      CCPU_GPR[rt].u = ((CCPU_GPR[ra].u & 0xFF00FF00) >> 8)
		       | ((CCPU_GPR[ra].u & 0x00FF00FF) << 8);
      break;
    case 0x15:			/* or_srli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u | (CCPU_GPR[rb].u >> sh5);
      break;
    case 0x16:			/* divsr */
      {
	/* FIXME: Positive qoutient exception.  */
	int64_t q;
	int64_t r;

	q = CCPU_GPR[ra].s / CCPU_GPR[rb].s;
	r = CCPU_GPR[ra].s % CCPU_GPR[rb].s;
	CCPU_GPR[rt].s = q;
	if (rt != rd)
	  CCPU_GPR[rd].s = r;
      }
      break;
    case 0x17:			/* divr */
      {
	uint64_t q;
	uint64_t r;

	q = CCPU_GPR[ra].u / CCPU_GPR[rb].u;
	r = CCPU_GPR[ra].u % CCPU_GPR[rb].u;
	CCPU_GPR[rt].u = q;
	if (rt != rd)
	  CCPU_GPR[rd].u = r;
      }
      break;
    case 0x18:			/* sva */
      {
	uint64_t s = (uint64_t) CCPU_GPR[ra].u + (uint64_t) CCPU_GPR[rb].u;
	s = (s >> 31) & 0x3;
	CCPU_GPR[rt].u = (s == 0 || s == 3);
      }
      break;
    case 0x19:			/* svs */
      break;
    case 0x1a:			/* comvz */
      if (CCPU_GPR[rb].u == 0)
	CCPU_GPR[rt].u = CCPU_GPR[ra].u;
      break;
    case 0x1b:			/* comvn */
      if (CCPU_GPR[rb].u != 0)
	CCPU_GPR[rt].u = CCPU_GPR[ra].u;
      break;
    case 0x1c:			/* add_srli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u + (CCPU_GPR[rb].u >> sh5);
      break;
    case 0x1d:			/* sub_srli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u - (CCPU_GPR[rb].u >> sh5);
      break;
    case 0x1e:			/* and_srli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u & (CCPU_GPR[rb].u >> sh5);
      break;
    case 0x1f:			/* xor_srli */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u ^ (CCPU_GPR[rb].u >> sh5);
      break;
    default:
      nds32_bad_op (cpu, cia, insn, "ALU1");
      return cia;
    }

  return cia + 4;
}

static sim_cia
nds32_decode32_alu2 (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  int rt = N32_RT5 (insn);
  int ra = N32_RA5 (insn);
  int rb = N32_RB5 (insn);
  const int imm5u = rb;
  const int dt = (insn & (1 << 21)) ? NC_D1LO : NC_D0LO;

  switch (insn & 0x3ff)
    {
    case 0x0:			/* max */
      CCPU_GPR[rt].s = (CCPU_GPR[ra].s > CCPU_GPR[rb].s)
		       ? CCPU_GPR[ra].s : CCPU_GPR[rb].s;
      break;
    case 0x1:			/* min */
      CCPU_GPR[rt].s = (CCPU_GPR[ra].s < CCPU_GPR[rb].s)
		       ? CCPU_GPR[ra].s : CCPU_GPR[rb].s;
      break;
    case 0x2:			/* ave */
      {
	int64_t r = ((int64_t) CCPU_GPR[ra].s << 1)
		    + ((int64_t) CCPU_GPR[rb].s << 1) + 1;
	CCPU_GPR[rt].u = (r >> 1) & 0xFFFFFFFF;
      }
      break;
    case 0x3:			/* abs */
      if (CCPU_GPR[ra].s >= 0)
	CCPU_GPR[rt].s = CCPU_GPR[ra].s;
      else if (CCPU_GPR[ra].u == 0x80000000)
	CCPU_GPR[rt].u = 0x7fffffff;
      else
	CCPU_GPR[rt].s = -CCPU_GPR[ra].s;
      break;
    case 0x6:			/* clo */
      {
	int i, cnt = 0;

	for (i = 31; i >= 0; i--)
	  {
	    if (CCPU_GPR[ra].u & (1 << i))
	      cnt++;
	    else
	      break;
	  }
	CCPU_GPR[rt].u = cnt;
      }
      break;
    case 0x7:			/* clz */
      {
	int i, cnt = 0;

	for (i = 31; i >= 0; i--)
	  {
	    if ((CCPU_GPR[ra].u & (1 << i)) == 0)
	      cnt++;
	    else
	      break;
	  }
	CCPU_GPR[rt].u = cnt;
      }
      break;
    case 0x8:			/* bset */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u | (1 << imm5u);
      break;
    case 0x9:			/* bclr */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u & ~(1 << imm5u);
      break;
    case 0xa:			/* btgl */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u ^ (1 << imm5u);
      break;
    case 0xb:			/* btst */
      CCPU_GPR[rt].u = (CCPU_GPR[ra].u & (1 << imm5u)) != 0;
      break;
    case 0x24:			/* mul */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u * CCPU_GPR[rb].u;
      break;
    case 0x20:			/* mfusr */
      CCPU_GPR[rt].u = CCPU_USR[rb << 5 | ra].u;
      if (((rb << 5) | ra) == 31)	/* PC */
	CCPU_GPR[rt].u = cia;
      break;
    case 0x21:			/* mtusr */
      CCPU_USR[(rb << 5) | ra].u = CCPU_GPR[rt].u;
      break;
    case 0x28:			/* mults64 */
      {
	int64_t d = (int64_t) CCPU_GPR[ra].s * (int64_t) CCPU_GPR[rb].s;

	CCPU_USR[dt].s = d & 0xFFFFFFFF;
	CCPU_USR[dt + 1].s = (d >> 32) & 0xFFFFFFFF;
      }
      break;
    case 0x29:			/* mult64 */
      {
	uint64_t d = (uint64_t) CCPU_GPR[ra].u * (uint64_t) CCPU_GPR[rb].u;

	CCPU_USR[dt].u = d & 0xFFFFFFFF;
	CCPU_USR[dt + 1].u = (d >> 32) & 0xFFFFFFFF;
      }
      break;
    case 0x2a:			/* madds64 */
      {
	int64_t mr = (int64_t) CCPU_GPR[ra].s * (int64_t) CCPU_GPR[rb].s;
	int64_t d = ((int64_t) CCPU_USR[dt + 1].s << 32)
		    | ((int64_t) CCPU_USR[dt].  s & 0xFFFFFFFF);

	d += mr;
	CCPU_USR[dt].u = d & 0xFFFFFFFF;
	CCPU_USR[dt + 1].u = (d >> 32) & 0xFFFFFFFF;
      }
      break;
    case 0x2b:			/* madd64 */
      {
	uint64_t mr = (uint64_t) CCPU_GPR[ra].u * (uint64_t) CCPU_GPR[rb].u;
	uint64_t d = ((uint64_t) CCPU_USR[dt + 1].u << 32)
		     | ((uint64_t) CCPU_USR[dt].u & 0xFFFFFFFF);

	d += mr;
	CCPU_USR[dt].u = d & 0xFFFFFFFF;
	CCPU_USR[dt + 1].u = (d >> 32) & 0xFFFFFFFF;
      }
      break;
    case 0x2c:			/* msubs64 */
      {
	int64_t mr = (int64_t) CCPU_GPR[ra].s * (int64_t) CCPU_GPR[rb].s;
	int64_t d = ((int64_t) CCPU_USR[dt + 1].s << 32)
		    | ((int64_t) CCPU_USR[dt].s & 0xFFFFFFFF);

	d -= mr;
	CCPU_USR[dt].u = d & 0xFFFFFFFF;
	CCPU_USR[dt + 1].u = (d >> 32) & 0xFFFFFFFF;
      }
      break;
    case 0x2d:			/* msub64 */
      {
	uint64_t mr = (uint64_t) CCPU_GPR[ra].u * (uint64_t) CCPU_GPR[rb].u;
	uint64_t d = ((uint64_t) CCPU_USR[dt + 1].u << 32)
		     | ((uint64_t) CCPU_USR[dt].u & 0xFFFFFFFF);

	d -= mr;
	CCPU_USR[dt].u = d & 0xFFFFFFFF;
	CCPU_USR[dt + 1].u = (d >> 32) & 0xFFFFFFFF;
      }
      break;
    case 0x2e:			/* divs */
      {
	int32_t q;
	int32_t r;

	q = CCPU_GPR[ra].s / CCPU_GPR[rb].s;
	r = CCPU_GPR[ra].s % CCPU_GPR[rb].s;
	CCPU_USR[dt].s = q;
	CCPU_USR[dt + 1].s = r;
      }
      break;
    case 0x2f:			/* div */
      {
	uint32_t q;
	uint32_t r;

	q = CCPU_GPR[ra].u / CCPU_GPR[rb].u;
	r = CCPU_GPR[ra].u % CCPU_GPR[rb].u;
	CCPU_USR[dt].u = q;
	CCPU_USR[dt + 1].u = r;
      }
      break;
    case 0x31:			/* mult32 */
      CCPU_USR[dt].s = CCPU_GPR[ra].s * CCPU_GPR[rb].s;
      break;
    case 0x33:			/* madd32 */
      CCPU_USR[dt].s += CCPU_GPR[ra].s * CCPU_GPR[rb].s;
      break;
    case 0x35:			/* msub32 */
      CCPU_USR[dt].s -= CCPU_GPR[ra].s * CCPU_GPR[rb].s;
      break;
    case 0x68:			/* mulsr64 */
      {
	int64_t r = (int64_t) CCPU_GPR[ra].s * (int64_t) CCPU_GPR[rb].s;
	int d = rt & ~1;

	if (CCPU_PSW_TEST (PSW_BE))
	  {
	    CCPU_GPR[d].u = (r >> 32) & 0xFFFFFFFF;
	    CCPU_GPR[d + 1].u = r & 0xFFFFFFFF;
	  }
	else
	  {
	    CCPU_GPR[d + 1].u = (r >> 32) & 0xFFFFFFFF;
	    CCPU_GPR[d].u = r & 0xFFFFFFFF;
	  }
      }
      break;
    case 0x69:			/* mulr64 */
      {
	uint64_t r = (uint64_t) CCPU_GPR[ra].u * (uint64_t) CCPU_GPR[rb].u;
	int d = rt & ~1;

	if (CCPU_PSW_TEST (PSW_BE))
	  {
	    CCPU_GPR[d].u = (r >> 32) & 0xFFFFFFFF;
	    CCPU_GPR[d + 1].u = r & 0xFFFFFFFF;
	  }
	else
	  {
	    CCPU_GPR[d + 1].u = (r >> 32) & 0xFFFFFFFF;
	    CCPU_GPR[d].u = r & 0xFFFFFFFF;
	  }
      }
      break;
    case 0x73:			/* maddr32 */
      CCPU_GPR[rt].u += (CCPU_GPR[ra].u * CCPU_GPR[rb].u) & 0xFFFFFFFF;
      break;
    case 0x75:			/* msubr32 */
      CCPU_GPR[rt].u -= (CCPU_GPR[ra].u * CCPU_GPR[rb].u) & 0xFFFFFFFF;
      break;
    default:
      nds32_bad_op (cpu, cia, insn, "ALU2");
      return cia;
    }

  return cia + 4;
}

static sim_cia
nds32_decode32_jreg (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  SIM_DESC sd = CPU_STATE (cpu);
  int rt = N32_RT5 (insn);
  int ra = N32_RA5 (insn);
  int rb = N32_RB5 (insn);

  if (ra != 0)
    sim_io_error (sd, "JREG RA == %d at pc=0x%x, code=0x%08x\n",
		  ra, cia, insn);

  if (__GF (insn, 8, 2) != 0)
    sim_io_error (sd, "JREG DT/IT not supported at pc=0x%x, code=0x%08x\n",
		  cia, insn);

  switch (insn & 0x1f)
    {
    case 0:			/* jr, ifret, ret */
      if (__GF (insn, 5, 2) == 0x3)
	{
	  /* ifret. IFC + RET */
	  if (CCPU_PSW_TEST (PSW_IFCON))
	    cia = CCPU_USR[NC_IFCLP].u;
	  else
	    cia += 4;	/* Do nothing */
	}
      else
	/* jr or ret */
	cia = CCPU_GPR[rb].u;

      CCPU_PSW_CLEAR (PSW_IFCON);
      /* SIM_IO_DPRINTF (sd, "set $pc to 0x%x\n", CCPU_USR[NC_PC].u); */
      cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
      return cia;

    case 1:			/* jral */
      if (cpu->iflags & NIF_EX9)
	CCPU_GPR[rt].u = cia + 2;
      else
	CCPU_GPR[rt].u = cia + 4;

      cia = CCPU_GPR[rb].u;
      /* SIM_IO_DPRINTF (sd, "set $pc to 0x%x, save ra to $r%d\n", CCPU_USR[NC_PC].u, rb); */
      if (CCPU_PSW_TEST (PSW_IFCON))
	{
	  CCPU_GPR[rt] = CCPU_USR[NC_IFCLP];
	  CCPU_PSW_CLEAR (PSW_IFCON);
	}

      cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
      return cia;

    case 2:			/* jrnez */
      if (CCPU_GPR[rb].u != 0)	/* taken */
	{
	  CCPU_PSW_CLEAR (PSW_IFCON);
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return CCPU_GPR[rb].u;
	}

      break; /* NOT taken */

    case 3:			/* jralnez */
      if (cpu->iflags & NIF_EX9)
	CCPU_GPR[rt].u = cia + 2;
      else
	CCPU_GPR[rt].u = cia + 4;

      if (CCPU_GPR[rb].u != 0)	/* taken */
	{
	  if (CCPU_PSW_TEST (PSW_IFCON))
	    {
	      CCPU_GPR[rt] = CCPU_USR[NC_IFCLP];
	      CCPU_PSW_CLEAR (PSW_IFCON);
	    }
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return CCPU_GPR[rb].u;
	}

      break; /* NOT taken */

    default:
      nds32_bad_op (cpu, cia, insn, "JREG");
      return cia;
    }

  return cia + 4;
}

static sim_cia
nds32_decode32_br1 (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  int rt = N32_RT5 (insn);
  int ra = N32_RA5 (insn);
  int imm14s = N32_IMM14S (insn);

  switch ((insn >> 14) & 1)
    {
    case 0:			/* beq */
      if (CCPU_GPR[rt].u == CCPU_GPR[ra].u)
	{
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm14s << 1);
	}
      break;
    case 1:			/* bne */
      if (CCPU_GPR[rt].u != CCPU_GPR[ra].u)
	{
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm14s << 1);
	}
      break;
    }

  return cia + 4;
}

static sim_cia
nds32_decode32_br2 (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  int rt = N32_RT5 (insn);
  int imm16s = N32_IMM16S (insn);

  switch (__GF (insn, 16, 4))
    {
    case 0x0:			/* ifcall */
      if (!CCPU_PSW_TEST (PSW_IFCON))
	{
	  if (cpu->iflags & NIF_EX9)
	    CCPU_USR[NC_IFCLP].u = cia + 2;
	  else
	    CCPU_USR[NC_IFCLP].u = cia + 4;

	  CCPU_PSW_SET (PSW_IFCON);
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (N32_IMMS (insn, 16) << 1);
	}
      else
	{
	  sim_io_error (CPU_STATE (cpu), "Nested IFCALL.\n");
	  sim_engine_halt (CPU_STATE (cpu), cpu, NULL, cia, sim_signalled, SIM_SIGABRT);
	}
      break;
    case 0x2:			/* beqz */
      if (CCPU_GPR[rt].s == 0)
	{
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm16s << 1);
	}
      break;
    case 0x3:			/* bnez */
      if (CCPU_GPR[rt].s != 0)
	{
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm16s << 1);
	}
      break;
    case 0x4:			/* bgez */
      if (CCPU_GPR[rt].s >= 0)
	{
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm16s << 1);
	}
      break;
    case 0x5:			/* bltz */
      if (CCPU_GPR[rt].s < 0)
	{
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm16s << 1);
	}
      break;
    case 0x6:			/* bgtz */
      if (CCPU_GPR[rt].s > 0)
	{
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm16s << 1);
	}
      break;
    case 0x7:			/* blez */
      if (CCPU_GPR[rt].s <= 0)
	{
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm16s << 1);
	}
      break;
    case 0x1c:			/* bgezal */
      if (CCPU_GPR[rt].s >= 0)
	{
	  CCPU_GPR[NG_LP].u = cia + 4;
	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm16s << 1);
	}
      break;
    case 0x1d:			/* bltzal */
      if (CCPU_GPR[rt].s < 0)
	{
	  if (cpu->iflags & NIF_EX9)
	    CCPU_USR[NC_IFCLP].u = cia + 2;
	  else
	    CCPU_USR[NC_IFCLP].u = cia + 4;

	  cpu->iflags &= ~NIF_EX9;	/* Check ex9.it for details. */
	  return cia + (imm16s << 1);
	}
      break;
    default:
      goto bad_op;
    }

  return cia + 4;

bad_op:
  nds32_bad_op (cpu, cia, insn, "BR2");
  return cia;
}

static sim_cia
nds32_decode32_misc (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  int rt = N32_RT5 (insn);

  switch (insn & 0x1F)
    {
    case 0x0:			/* standby */
    case 0x1:			/* cctl */
    case 0x8:			/* dsb */
    case 0x9:			/* isb */
    case 0xd:			/* isync */
    case 0xc:			/* msync */
      break;
    case 0x5:			/* trap */
    case 0xa:			/* break */
      sim_engine_halt (CPU_STATE (cpu), cpu, NULL, cia, sim_stopped, SIM_SIGTRAP);
      return cia; /* FIXME dispatch exception? */
    case 0x2:			/* mfsr */
      CCPU_GPR[rt] = CCPU_SR[__GF (insn, 10, 10)];
      break;
    case 0x3:			/* mtsr */
      CCPU_SR[__GF (insn, 10, 10)] = CCPU_GPR[rt];
      break;
    case 0xb:			/* syscall */
      return nds32_syscall (cpu, __GF (insn, 5, 15), cia);
    case 0x4:			/* iret */
      nds32_bad_op (cpu, cia, insn, "iret (MISC)");
      return cia;
    case 0x6:			/* teqz */
      nds32_bad_op (cpu, cia, insn, "teqz (MISC)");
      return cia;
    case 0x7:			/* tnez */
      nds32_bad_op (cpu, cia, insn, "tnez (MISC)");
      return cia;
    case 0xe:			/* tlbop */
      nds32_bad_op (cpu, cia, insn, "tlbop (MISC)");
      return cia;
    default:
      nds32_bad_op (cpu, cia, insn, "MISC");
      return cia;
    }

  return cia + 4;
}

static sim_cia
nds32_decode32 (sim_cpu *cpu, const uint32_t insn, sim_cia cia)
{
  int op = N32_OP6 (insn);
  int rt = N32_RT5 (insn);
  int ra = N32_RA5 (insn);
  int rb = N32_RB5 (insn);
  int imm15s = N32_IMM15S (insn);
  int imm15u = N32_IMM15U (insn);

  switch (op)
    {
    case 0x0:			/* lbi */
    case 0x1:			/* lhi */
    case 0x2:			/* lwi */
    case 0x3:			/* ldi */
      {
	int shift = (op - 0x0);

	CCPU_GPR[rt].u =
	  nds32_ld (cpu, CCPU_GPR[ra].u + (imm15s << shift), 1 << shift);
      }
      break;

    case 0x4:			/* lbi.bi */
    case 0x5:			/* lhi.bi */
    case 0x6:			/* lwi.bi */
    case 0x7:			/* ldi.bi */
      {
	int shift = (op - 0x4);

	CCPU_GPR[rt].u = nds32_ld (cpu, CCPU_GPR[ra].u, 1 << shift);
	CCPU_GPR[ra].u += (imm15s << shift);
      }
      break;

    case 0x8:			/* sbi */
    case 0x9:			/* shi */
    case 0xa:			/* swi */
    case 0xb:			/* sdi */
      {
	int shift = (op - 0x8);

	nds32_st (cpu, CCPU_GPR[ra].u + (imm15s << shift), 1 << shift,
		  CCPU_GPR[rt].u);
      }
      break;

    case 0xc:			/* sbi.bi */
    case 0xd:			/* shi.bi */
    case 0xe:			/* swi.bi */
    case 0xf:			/* sdi.bi */
      {
	int shift = (op - 0xc);

	nds32_st (cpu, CCPU_GPR[ra].u, 1 << shift, CCPU_GPR[rt].u);
	CCPU_GPR[ra].u += (imm15s << shift);
      }
      break;

    case 0x10:			/* lbsi */
    case 0x11:			/* lhsi */
    case 0x12:			/* lwsi */
      {
	int shift = (op - 0x10);

	CCPU_GPR[rt].s =
	  nds32_ld_sext (cpu, CCPU_GPR[ra].u + (imm15s << shift), 1 << shift);
      }
      break;
    case 0x13:			/* dprefi */
      /* do nothing */
      break;
    case 0x14:			/* lbsi.bi */
    case 0x15:			/* lhsi.bi */
    case 0x16:			/* lwsi.bi */
      {
	int shift = (op - 0x14);

	CCPU_GPR[rt].s = nds32_ld_sext (cpu, CCPU_GPR[ra].u, 1 << shift);
	CCPU_GPR[ra].u += (imm15s << shift);
      }
      break;
    case 0x17:			/* LBGP */
      if (insn & (1 << 19))	/* lbsi.gp */
	CCPU_GPR[rt].s = nds32_ld_sext (cpu, CCPU_GPR[NG_GP].u + N32_IMMS (insn, 19), 1);
      else			/* lbi.gp */
	CCPU_GPR[rt].u = nds32_ld (cpu, CCPU_GPR[NG_GP].u + N32_IMMS (insn, 19), 1);
      break;
    case 0x18:			/* LWC */
      return nds32_decode32_lwc (cpu, insn, cia);
    case 0x19:			/* SWC */
      return nds32_decode32_swc (cpu, insn, cia);
    case 0x1a:			/* LDC */
      return nds32_decode32_ldc (cpu, insn, cia);
    case 0x1b:			/* SDC */
      return nds32_decode32_sdc (cpu, insn, cia);
    case 0x1c:			/* MEM */
      return nds32_decode32_mem (cpu, insn, cia);
    case 0x1d:			/* LSMW */
      return nds32_decode32_lsmw (cpu, insn, cia);
    case 0x1e:			/* HWGP */
      switch (__GF (insn, 17, 3))
	{
	case 0: case 1:		/* lhi.gp */
	  CCPU_GPR[rt].u =
	    nds32_ld (cpu, CCPU_GPR[NG_GP].u + (N32_IMMS (insn, 18) << 1), 2);
	  break;
	case 2: case 3:		/* lhsi.gp */
	  CCPU_GPR[rt].u =
	    nds32_ld_sext (cpu, CCPU_GPR[NG_GP].u + (N32_IMMS (insn, 18) << 1), 2);
	  break;
	case 4: case 5:		/* shi.gp */
	  nds32_st (cpu, CCPU_GPR[NG_GP].u + (N32_IMMS (insn, 18) << 1), 2, CCPU_GPR[rt].u);
	  break;
	case 6:			/* lwi.gp */
	  CCPU_GPR[rt].u =
	    nds32_ld (cpu, CCPU_GPR[NG_GP].u + (N32_IMMS (insn, 17) << 2), 4);
	  break;
	case 7:			/* swi.gp */
	  nds32_st (cpu, CCPU_GPR[NG_GP].u + (N32_IMMS (insn, 17) << 2), 4, CCPU_GPR[rt].u);
	  break;
	}
      break;
    case 0x1f:			/* SBGP */
      if (insn & (1 << 19))	/* addi.gp */
	CCPU_GPR[rt].s = CCPU_GPR[NG_GP].u + N32_IMMS (insn, 19);
      else			/* sbi.gp */
	nds32_st (cpu, CCPU_GPR[NG_GP].u + N32_IMMS (insn, 19), 1, CCPU_GPR[rt].u & 0xFF);
      break;
    case 0x20:			/* ALU_1 */
      return nds32_decode32_alu1 (cpu, insn, cia);
    case 0x21:			/* ALU_2 */
      return nds32_decode32_alu2 (cpu, insn, cia);
    case 0x22:			/* movi */
      CCPU_GPR[rt].s = N32_IMM20S (insn);
      break;
    case 0x23:			/* sethi */
      CCPU_GPR[rt].u = N32_IMM20U (insn) << 12;
      break;
    case 0x24:			/* ji, jal */
      if (cpu->iflags & NIF_EX9)
	{
	  if (insn & (1 << 24))	/* jal in ex9 */
	    CCPU_GPR[NG_LP].u = cia + 2;
	  cia = (cia & 0xff000000) | (N32_IMMU (insn, 24) << 1);
	}
      else
	{
	  if (insn & (1 << 24))	/* jal */
	    CCPU_GPR[NG_LP].u = cia + 4;
	  cia = cia + (N32_IMMS (insn, 24) << 1);
	}

      if (CCPU_PSW_TEST (PSW_IFCON))
	{
	  CCPU_GPR[NG_LP] = CCPU_USR[NC_IFCLP];
	  CCPU_PSW_CLEAR (PSW_IFCON);
	}

      cpu->iflags &= ~NIF_EX9;
      return cia;
    case 0x25:			/* jreg */
      cia = nds32_decode32_jreg (cpu, insn, cia);
      return cia;
    case 0x26:			/* br1 */
      cia = nds32_decode32_br1 (cpu, insn, cia);
      return cia;
    case 0x27:			/* br2 */
      cia = nds32_decode32_br2 (cpu, insn, cia);
      return cia;
    case 0x28:			/* addi rt, ra, imm15s */
      CCPU_GPR[rt].s = CCPU_GPR[ra].s + imm15s;
      break;
    case 0x29:			/* subri */
      CCPU_GPR[rt].s = imm15s - CCPU_GPR[ra].s;
      break;
    case 0x2a:			/* andi */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u & imm15u;
      break;
    case 0x2b:			/* xori */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u ^ imm15u;
      break;
    case 0x2c:			/* ori */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u | imm15u;
      break;
    case 0x2d:			/* br3, beqc, bnec */
      {
	int imm11s = __SEXT (__GF (insn, 8, 11), 11);

	if (((insn & (1 << 19)) == 0) ^ (CCPU_GPR[rt].s != imm11s))
	  {
	    cpu->iflags &= ~NIF_EX9;
	    return cia + (N32_IMMS (insn, 8) << 1);
	  }
      }
      break;
    case 0x2e:			/* slti */
      CCPU_GPR[rt].u = (CCPU_GPR[ra].u < (uint32_t) imm15s) ? 1 : 0;
      break;
    case 0x2f:			/* sltsi */
      CCPU_GPR[rt].u = (CCPU_GPR[ra].s < imm15s) ? 1 : 0;
      break;
    case 0x32:			/* misc */
      return nds32_decode32_misc (cpu, insn, cia);
    case 0x33:			/* bitci */
      CCPU_GPR[rt].u = CCPU_GPR[ra].u & ~imm15u;
      break;
    case 0x35:			/* COP */
      return nds32_decode32_cop (cpu, insn, cia);
    default:
      goto bad_op;
    }

  return cia + 4;

bad_op:
  nds32_bad_op (cpu, cia, insn, "32-bit");
  return cia;
}

static sim_cia
nds32_decode16_ex9 (sim_cpu *cpu, uint32_t insn, sim_cia cia)
{
  sim_cia ex9_cia;

  /* For jump and taken branch, it should clear the NIF_EX9 bit
     to indicate the ex9_cia should be used; otherwsie,
     the next cia, cia + 2, should be used.  */

  cpu->iflags |= NIF_EX9;
  ex9_cia = nds32_decode32 (cpu, insn, cia);

  if ((cpu->iflags & NIF_EX9) == 0)
    return ex9_cia;

  cpu->iflags &= ~NIF_EX9;
  return cia + 2;
}

static sim_cia
nds32_decode16 (sim_cpu *cpu, uint32_t insn, sim_cia cia)
{
  SIM_DESC sd = CPU_STATE (cpu);
  const int rt5 = N16_RT5 (insn);
  const int ra5 = N16_RA5 (insn);
  const int rt4 = N16_RT4 (insn);
  const int imm5u = N16_IMM5U (insn);
  const int imm5s = N16_IMM5S (insn);
  const int imm9u = N16_IMM9U (insn);
  const int rt3 = N16_RT3 (insn);
  const int ra3 = N16_RA3 (insn);
  const int rb3 = N16_RB3 (insn);
  const int rt38 = N16_RT38 (insn);
  const int imm3u = rb3;

  if (__GF (insn, 5, 10) == 0x2ea) /* ex9.it imm5 */
    {
      sim_read (sd, CCPU_USR[NC_ITB].u + (imm5u << 2), (unsigned char *) &insn, 4);
      insn = extract_unsigned_integer ((unsigned char *) &insn, 4, BIG_ENDIAN);
      return nds32_decode16_ex9 (cpu, insn, cia);
    }

  switch (__GF (insn, 7, 8))
    {
    case 0xf8:			/* push25 */
      {
	uint32_t smw_adm = 0x3A6F83BC;
	uint32_t res[] = { 6, 8, 10, 14 };
	uint32_t re = __GF (insn, 5, 2);

	smw_adm |= res[re] << 10;
	nds32_decode32_lsmw (cpu, smw_adm, cia);
	CCPU_GPR[NG_SP].u -= (imm5u << 3);
	if (re >= 1)
	  CCPU_GPR[8].u = cia & 0xFFFFFFFC;
      }
      goto done;
    case 0xf9:			/* pop25 */
      {
	uint32_t lmw_bim = 0x3A6F8384;
	uint32_t res[] = { 6, 8, 10, 14 };
	uint32_t re = __GF (insn, 5, 2);

	lmw_bim |= res[re] << 10;
	CCPU_GPR[NG_SP].u += (imm5u << 3);
	nds32_decode32_lsmw (cpu, lmw_bim, cia);
	cia = CCPU_GPR[NG_LP].u;
	CCPU_PSW_CLEAR (PSW_IFCON);
	return cia;
      }
    }

  if (__GF (insn, 8, 7) == 0x7d)
    {
      int rt5e = __GF (insn, 4, 4) << 1;
      int ra5e = __GF (insn, 0, 4) << 1;

      CCPU_GPR[rt5e] = CCPU_GPR[ra5e];
      CCPU_GPR[rt5e + 1] = CCPU_GPR[ra5e + 1];
      goto done;
    }

  switch (__GF (insn, 9, 6))
    {
    case 0x4:			/* add45 */
      CCPU_GPR[rt4].u += CCPU_GPR[ra5].u;
      goto done;
    case 0x5:			/* sub45 */
      CCPU_GPR[rt4].u -= CCPU_GPR[ra5].u;
      goto done;
    case 0x6:			/* addi45 */
      CCPU_GPR[rt4].u += imm5u;
      goto done;
    case 0x7:			/* subi45 */
      CCPU_GPR[rt4].u -= imm5u;
      goto done;
    case 0x8:			/* srai45 */
      CCPU_GPR[rt4].u = CCPU_GPR[rt4].s >> imm5u;
      goto done;
    case 0x9:			/* srli45 */
      CCPU_GPR[rt4].u = CCPU_GPR[rt4].u >> imm5u;
      goto done;
    case 0xa:			/* slli333 */
      CCPU_GPR[rt3].u = CCPU_GPR[ra3].u << imm3u;
      goto done;
    case 0xc:			/* add333 */
      CCPU_GPR[rt3].u = CCPU_GPR[ra3].u + CCPU_GPR[rb3].u;
      goto done;
    case 0xd:			/* sub333 */
      CCPU_GPR[rt3].u = CCPU_GPR[ra3].u - CCPU_GPR[rb3].u;
      goto done;
    case 0xe:			/* addi333 */
      CCPU_GPR[rt3].u = CCPU_GPR[ra3].u + imm3u;
      goto done;
    case 0xf:			/* subi333 */
      CCPU_GPR[rt3].u = CCPU_GPR[ra3].u - imm3u;
      goto done;
    case 0x10:			/* lwi333 */
    case 0x12:			/* lhi333 */
    case 0x13:			/* lbi333 */
      {
	int shtbl[] = { 2, -1, 1, 0 };
	int shift = shtbl[(__GF (insn, 9, 6) - 0x10)];

	CCPU_GPR[rt3].u =
	  nds32_ld (cpu, CCPU_GPR[ra3].u + (imm3u << shift), 1 << shift);
      }
      goto done;
    case 0x11:			/* lwi333.bi */
      CCPU_GPR[rt3].u = nds32_ld (cpu, CCPU_GPR[ra3].u, 4);
      CCPU_GPR[ra3].u += imm3u << 2;
      goto done;
    case 0x14:			/* swi333 */
    case 0x16:			/* shi333 */
    case 0x17:			/* sbi333 */
      {
	int shtbl[] = { 2, -1, 1, 0 };
	int shift = shtbl[(__GF (insn, 9, 6) - 0x14)];

	nds32_st (cpu, CCPU_GPR[ra3].u + (imm3u << shift), 1 << shift,
		  CCPU_GPR[rt3].u);
      }
      goto done;
    case 0x15:			/* swi333.bi */
      nds32_st (cpu, CCPU_GPR[ra3].u, 4, CCPU_GPR[rt3].u);
      CCPU_GPR[ra3].u += imm3u << 2;
      goto done;
    case 0x18:			/* addri36.sp */
      CCPU_GPR[rt3].u = CCPU_GPR[NG_SP].u + (N16_IMM6U (insn) << 2);
      goto done;
    case 0x19:			/* lwi45.fe */
      {
	/* Not tested yet */
	int imm7n = -((32 - imm5u) << 2);

	CCPU_GPR[rt4].u = nds32_ld (cpu, CCPU_GPR[8].u + imm7n, 4);
      }
      goto done;
    case 0x1a:			/* lwi450 */
      CCPU_GPR[rt4].u = nds32_ld (cpu, CCPU_GPR[ra5].u, 4);
      goto done;
    case 0x1b:			/* swi450 */
      nds32_st (cpu, CCPU_GPR[ra5].u, 4, CCPU_GPR[rt4].u);
      goto done;
    case 0x30:			/* slts45 */
      CCPU_GPR[NG_TA].u = (CCPU_GPR[rt4].s < CCPU_GPR[ra5].s) ? 1 : 0;
      goto done;
    case 0x31:			/* slt45 */
      CCPU_GPR[NG_TA].u = (CCPU_GPR[rt4].u < CCPU_GPR[ra5].u) ? 1 : 0;
      goto done;
    case 0x32:			/* sltsi45 */
      CCPU_GPR[NG_TA].u = (CCPU_GPR[rt4].s < imm5u) ? 1 : 0;
      goto done;
    case 0x33:			/* slti45 */
      CCPU_GPR[NG_TA].u = (CCPU_GPR[rt4].u < imm5u) ? 1 : 0;
      goto done;

    case 0x34:			/* beqzs8, bnezs8 */
      if (((insn & (1 << 8)) == 0) ^ (CCPU_GPR[NG_TA].u != 0))
	return cia + (N16_IMM8S (insn) << 1);
      goto done;
    case 0x35:			/* break16, ex9.it */
      if (imm9u < 32)		/* break16 */
	{
	  sim_engine_halt (CPU_STATE (cpu), cpu, NULL, cia, sim_stopped, SIM_SIGTRAP);
	  return cia;
	}

      /* ex9.it */
      sim_read (sd, CCPU_USR[NC_ITB].u + (imm9u << 2),
		(unsigned char *) &insn, 4);
      insn = extract_unsigned_integer ((unsigned char *) &insn, 4, BIG_ENDIAN);
      return nds32_decode16_ex9 (cpu, insn, cia);
    case 0x3c:			/* ifcall9 */
      if (!CCPU_PSW_TEST (PSW_IFCON))
	{
	  CCPU_USR[NC_IFCLP].u = cia + 2;
	  CCPU_PSW_SET (PSW_IFCON);
	  return cia + (N16_IMM9U (insn) << 1);
	}
      else
	{
	  /* FIXME: Raise Exception */
	}
      goto done;
    case 0x3d:			/* movpi45 */
      CCPU_GPR[rt4].u = imm5u + 16;
      goto done;
    case 0x3f:			/* MISC33 */
      switch (insn & 0x7)
	{
	case 2:			/* neg33 */
	  CCPU_GPR[rt3].s = -CCPU_GPR[ra3].u;
	  goto done;
	case 3:			/* not33 */
	  CCPU_GPR[rt3].u = ~CCPU_GPR[ra3].u;
	  goto done;
	case 4:			/* mul33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[rt3].u * CCPU_GPR[ra3].u;
	  goto done;
	case 5:			/* xor33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[rt3].u ^ CCPU_GPR[ra3].u;
	  goto done;
	case 6:			/* and33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[rt3].u & CCPU_GPR[ra3].u;
	  goto done;
	case 7:			/* or33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[rt3].u | CCPU_GPR[ra3].u;
	  goto done;
	default:
	  goto bad_op;
	}
      goto done;
    case 0xb:			/* ... */
      switch (insn & 0x7)
	{
	case 0:			/* zeb33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[ra3].u & 0xff;
	  break;
	case 1:			/* zeh33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[ra3].u & 0xffff;
	  break;
	case 2:			/* seb33 */
	  CCPU_GPR[rt3].s = __SEXT (CCPU_GPR[ra3].s, 8);
	  break;
	case 3:			/* seh33 */
	  CCPU_GPR[rt3].s = __SEXT (CCPU_GPR[ra3].s, 16);
	  break;
	case 4:			/* xlsb33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[ra3].u & 0x1;
	  break;
	case 5:			/* x11b33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[ra3].u & 0x7FF;
	  break;
	case 6:			/* bmski33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[rt3].u & (1 << __GF (insn, 3, 3));
	  break;
	case 7:			/* fexti33 */
	  CCPU_GPR[rt3].u = CCPU_GPR[rt3].u & ((1 << (__GF (insn, 3, 3) + 1)) - 1);
	  break;
	}
      goto done;
    }

  switch (__GF (insn, 10, 5))
    {
    case 0x0:			/* mov55 or ifret16 */
      if (CCPU_PSW_TEST (PSW_IFCON) && rt5 == ra5 && rt5 == 31)
	{
	  CCPU_PSW_CLEAR (PSW_IFCON);
	  return CCPU_USR[NC_IFCLP].u;
	}
      else
	CCPU_GPR[rt5].u = CCPU_GPR[ra5].u;
      goto done;
    case 0x1:			/* movi55 */
      CCPU_GPR[rt5].s = imm5s;
      goto done;
    case 0x1b:			/* addi10s (V2) */
      CCPU_GPR[NG_SP].u += N16_IMM10S (insn);
      goto done;
    }

  switch (__GF (insn, 11, 4))
    {
    case 0x7:			/* lwi37.fp/swi37.fp */
      if (insn & (1 << 7))	/* swi37.fp */
	nds32_st (cpu, CCPU_GPR[NG_FP].u + (N16_IMM7U (insn) << 2), 4, CCPU_GPR[rt38].u);
      else			/* lwi37.fp */
	CCPU_GPR[rt38].u = nds32_ld (cpu, CCPU_GPR[NG_FP].u + (N16_IMM7U (insn) << 2), 4);
      goto done;
    case 0x8:			/* beqz38 */
      if (CCPU_GPR[rt38].u == 0)
	return cia + (N16_IMM8S (insn) << 1);
      goto done;
    case 0x9:			/* bnez38 */
      if (CCPU_GPR[rt38].u != 0)
	return cia + (N16_IMM8S (insn) << 1);
      goto done;
    case 0xa:			/* beqs38/j8, implied r5 */
      if (CCPU_GPR[rt38].u == CCPU_GPR[5].u)	/* rt38 == 5 means j8 */
	{
	  CCPU_PSW_CLEAR (PSW_IFCON);
	  return cia + (N16_IMM8S (insn) << 1);
	}
      goto done;
    case 0xb:			/* bnes38 and others */
      if (rt38 == 5)
	{
	  switch (__GF (insn, 5, 3))
	    {
	    case 0:		/* jr5 */
	    case 4:		/* ret5 */
	      CCPU_PSW_CLEAR (PSW_IFCON);
	      return CCPU_GPR[ra5].u;
	    case 1:		/* jral5 */
	      CCPU_GPR[NG_LP].u = cia + 2;
	      if (CCPU_PSW_TEST (PSW_IFCON))
		{
		  CCPU_GPR[NG_LP] = CCPU_USR[NC_IFCLP];
		  CCPU_PSW_CLEAR (PSW_IFCON);
		}
	      return CCPU_GPR[ra5].u;
	      goto done;
	    default:
	      goto bad_op;
	    }
	}
      else if (CCPU_GPR[rt38].u != CCPU_GPR[5].u)
	return cia + (N16_IMM8S (insn) << 1);
      goto done;
    case 0xe:			/* lwi37/swi37 */
      if (insn & (1 << 7))	/* swi37.sp */
	nds32_st (cpu, CCPU_GPR[NG_SP].u + (N16_IMM7U (insn) << 2), 4, CCPU_GPR[rt38].u);
      else			/* lwi37.sp */
	CCPU_GPR[rt38].u = nds32_ld (cpu, CCPU_GPR[NG_SP].u + (N16_IMM7U (insn) << 2), 4);
      goto done;
    }

bad_op:
  nds32_bad_op (cpu, cia, insn, "16-bit");
  return cia;

done:
  return cia + 2;
}

void
sim_engine_run (SIM_DESC sd, int next_cpu_nr, int nr_cpus, int siggnal)
{
  sim_cia cia;
  sim_cpu *cpu;
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);
  cpu = STATE_CPU (sd, 0);
  cia = CIA_GET (cpu);

  while (1)
    {
      uint32_t insn;

      sim_read (sd, cia, (unsigned char *) &insn, 4);
      insn = extract_unsigned_integer ((unsigned char *) &insn, 4,
				       BIG_ENDIAN);

      if ((insn & 0x80000000) == 0)
	cia = nds32_decode32 (cpu, insn, cia);
      else
	cia = nds32_decode16 (cpu, insn >> 16, cia);

      /* process any events */
      if (sim_events_tick (sd))
	{
	  CIA_SET (cpu, cia);
	  sim_events_process (sd);
	}
    }
}

int
sim_trace (SIM_DESC sd)
{
  tracing = 1;

  sim_resume (sd, 0, 0);

  tracing = 0;

  return 1;
}

/* This function is mainly used for fetch general purpose registers.
   GDB remote-sim calls this too, so it will be used for fetch some
   USR (PC, D0, D1), FLOAT, SR (PSW).  */

static int
nds32_fetch_register (sim_cpu *cpu, int rn, unsigned char *memory, int length)
{
  /* General purpose registers.  */
  if (rn < 32)
    {
      store_unsigned_integer_by_psw (cpu, memory, length, cpu->reg_gpr[rn].u);
      return 4;
    }

  /* Special user registers.  */
  switch (rn)
    {
    case NG_PC:
      store_unsigned_integer_by_psw (cpu, memory, length, cpu->reg_usr[NC_PC].u);
      return 4;
    case NG_D0LO:
      store_unsigned_integer_by_psw (cpu, memory, length, cpu->reg_usr[NC_D0LO].u);
      return 4;
    case NG_D0HI:
      store_unsigned_integer_by_psw (cpu, memory, length, cpu->reg_usr[NC_D0HI].u);
      return 4;
    case NG_D1LO:
      store_unsigned_integer_by_psw (cpu, memory, length, cpu->reg_usr[NC_D1LO].u);
      return 4;
    case NG_D1HI:
      store_unsigned_integer_by_psw (cpu, memory, length, cpu->reg_usr[NC_D1HI].u);
      return 4;
    }

  if (rn >= NG_FS0 && rn < NG_FS0 + 64)
    {
      int fr = rn - NG_FS0;
      if (fr < 32)
	{
	  store_unsigned_integer_by_psw (cpu, memory, length, cpu->reg_fpr[fr].u);
	}
      else
	{
	  uint64_t d;
	  fr = (fr - 32) << 1;
	  d = ((uint64_t)cpu->reg_fpr[fr].u << 32) | (uint64_t)cpu->reg_fpr[fr + 1].u;
	  store_unsigned_integer_by_psw (cpu, memory, length, d);
	}
    }

  /* System registers.  */
  switch (rn)
    {
    case NG_PSW:
      store_unsigned_integer_by_psw (cpu, memory, length, cpu->reg_sr[SRIDX (1, 0, 0)].u);
      break;
    }
  return 0;
}

static int
nds32_store_register (sim_cpu *cpu, int rn, unsigned char *memory, int length)
{
  /* General purpose registers.  */
  if (rn < 32)
    {
      cpu->reg_gpr[rn].u = extract_unsigned_integer_by_psw (cpu, memory, length);
      return 4;
    }

  /* Special user registers.  */
  switch (rn)
    {
    case NG_PC:
      cpu->reg_usr[NC_PC].u = extract_unsigned_integer_by_psw (cpu, memory, length);
      return 4;
    case NG_D0LO:
      cpu->reg_usr[NC_D0LO].u = extract_unsigned_integer_by_psw (cpu, memory, length);
      return 4;
    case NG_D0HI:
      cpu->reg_usr[NC_D0HI].u = extract_unsigned_integer_by_psw (cpu, memory, length);
      return 4;
    case NG_D1LO:
      cpu->reg_usr[NC_D1LO].u = extract_unsigned_integer_by_psw (cpu, memory, length);
      return 4;
    case NG_D1HI:
      cpu->reg_usr[NC_D1HI].u = extract_unsigned_integer_by_psw (cpu, memory, length);
      return 4;
    }

  /* System registers.  */
  return 0;
}

static sim_cia
nds32_pc_get (sim_cpu *cpu)
{
  return cpu->reg_usr[NC_PC].u;
}

static void
nds32_pc_set (sim_cpu *cpu, sim_cia cia)
{
  cpu->reg_usr[NC_PC].u = cia;
}

static void
nds32_initialize_cpu (SIM_DESC sd, sim_cpu *cpu, struct bfd *abfd)
{
  /* Common operations defined in sim-cpu.h */
  CPU_REG_FETCH (cpu) = nds32_fetch_register;
  CPU_REG_STORE (cpu) = nds32_store_register;
  CPU_PC_FETCH (cpu) = nds32_pc_get;
  CPU_PC_STORE (cpu) = nds32_pc_set;

  /* CPU_VER */
  CCPU_SR[SRIDX (0, 0, 0)].u = (0xc << 24) | 3;
}

SIM_DESC
sim_open (SIM_OPEN_KIND kind, host_callback * callback,
	  struct bfd *abfd, char **argv)
{
  char c;
  int i;
  SIM_DESC sd = sim_state_alloc (kind, callback);

  /* The cpu data is kept in a separately allocated chunk of memory.  */
  if (sim_cpu_alloc_all (sd, 1, 0) != SIM_RC_OK)
    {
      nds32_free_state (sd);
      return 0;
    }

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    {
      nds32_free_state (sd);
      return 0;
    }

  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      nds32_free_state (sd);
      return 0;
    }

  sim_do_command (sd, "memory region 0,0x04000000");

  /* Check for/establish the a reference program image.  */
  if (sim_analyze_program (sd,
			   (STATE_PROG_ARGV (sd) != NULL
			    ? *STATE_PROG_ARGV (sd)
			    : NULL), abfd) != SIM_RC_OK)
    {
      nds32_free_state (sd);
      return 0;
    }

#if 0
  /* Establish any remaining configuration options.  */
  if (sim_config (sd) != SIM_RC_OK)
    {
      nds32_free_state (sd);
      return 0;
    }
#endif

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      nds32_free_state (sd);
      return 0;
    }

  /* CPU specific initialization.  */
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    {
      sim_cpu *cpu = STATE_CPU (sd, i);
      nds32_initialize_cpu (sd, cpu, abfd);
    }

  return sd;
}

void
sim_close (SIM_DESC sd, int quitting)
{
  /* Nothing to do.  */
}

SIM_RC
sim_create_inferior (SIM_DESC sd, struct bfd *prog_bfd, char **argv,
		     char **env)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  int len;
  int mlen;
  int i;
  asection *s;

  memset (sd->cmdline, 0, sizeof (sd->cmdline));
  mlen = sizeof (sd->cmdline) - 1;
  len = 0;
  for (i = 0; argv && argv[i]; i++)
    {
      int l = strlen (argv[i]) + 1;
      if (l + len >= mlen)
	break;

      len += sprintf (sd->cmdline + len, "%s ",
		argv[i]);
    }
  if (len > 0)
    sd->cmdline[len - 1] = '\0'; /* Eat the last space. */


  /* nrun doesn't pass prog_bfd, (but run do).
     so we can only handle bfd here instead of sim_open. */

  /* Set the initial register set.  */
  if (prog_bfd != NULL)
    {
      /* FIXME: Remove this when crt0 is readly. */
      for (s = prog_bfd->sections; s; s = s->next)
	{
	  if (strcmp (bfd_get_section_name (prog_bfd, s), ".ex9.itable") == 0)
	    {
	      CCPU_USR[NC_ITB].u = ((uint32_t) bfd_get_section_vma (prog_bfd, s));
	      break;
	    }
	}

      /* Set PC to entry point address. */
      (* CPU_PC_STORE (cpu)) (cpu, bfd_get_start_address (prog_bfd));
    }

  return SIM_RC_OK;
}

void
sim_kill (SIM_DESC sd)
{
  /* Nothing to do.  */
}

void
sim_set_callbacks (host_callback * ptr)
{
  callback = ptr;
}
