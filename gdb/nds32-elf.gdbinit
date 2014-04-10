# gdbinit for nds32 systems.
#
# Copyright (C) 2006-2013 Free Software Foundation, Inc.
# Contributed by Andes Technology Corporation.
#
# This file is part of GDB.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This commands are used for nds32 OpenOCD/SID.

echo [info] Loading .Andesgdbinit.\n

# Reduce remote memory access.
set trust-readonly-sections 1

# set standard library path for remote debugging
# set sysroot /path/to/nds32le-linux/nds32le-linux/libc
# set solib-search-path /path/to/nds32le-lib

# Set limit to workaround issues about backtrace in ISR or borken debug information.
set backtrace limit 100

# Set Timeout limit to wait for target to respond to 60 (defualt=2)
# Reason: 'monitor reset target' may take over 2s and the comunication
#	  would be a mess because the next command will be responsed
#	  with previous reply
set remotetimeout 60

# Handle elf-check and cache control only if the target (sid/iceman) requires.
# In other words, do not handle elf-check and cache control for sim.
# $_nds32_target_type is built-in convenience variable for target type:
#  0 - unknown (linux gdbserver or sim)
#  1 - SID
#  2 - ICE
#  3 - OpenOCD
define target hookpost-remote
  nds32 query target
end

define target hookpost-extended-remote
  nds32 query target
end

define target hookpost-sim
  nds32 query target
end

# Force to go through ifc common block if $psw.IFCON is ON
# Distinguish stop caused by breakpoint from others -- step, stepi, next, nexti
set $nds32_force_ifc_return = 1

define hook-continue
  set $_nds32_flag_continue = 1
end
define hook-stop
  if $_nds32_target_type
    if ((int) $ir0 & 0x8000)
      # Take care of display issue
      set logging on
      set logging file .nds32_temp.log
      echo ifc.fcon is on\n
      set logging redirect on

      # If stop caused by breakpoint, resume by continue
      # Otherwise, single stepi until $psw.IFCON bit is OFF
      if ($_nds32_flag_continue && $nds32_force_ifc_return)
	continue
      else
	while (((int) $ir0 & 0x8000) && $nds32_force_ifc_return )
	  stepi
	end
      end
    end
    set logging redirect off
    set $_nds32_flag_continue = 0
  end
end

# Disable cache when loading code
define hook-load
  if $_nds32_target_type
    nds32 elf-check
    set $old_mr8 = $mr8
    set $mr8 = 0
    if $_nds32_target_type != 3
      maintenance packet qPart:nds32:request:InvalidateCache
      maintenance packet qPart:nds32:request:MemAccBus
    else
      monitor nds cache invalidate
      monitor nds mem_access bus
    end
  end
end

define hookpost-load
  if $_nds32_target_type
    if $_nds32_target_type != 3
      maintenance packet qPart:nds32:request:MemAccCPU
    else
      monitor nds mem_access cpu
    end
    set $mr8 = $old_mr8
    nds32 set-gloss
  end
end

# Workaround for bug6907 (avoid cache issue)
# Always use CPU mode for 'restore'.
define hook-restore
  if $_nds32_target_type
    if $_nds32_target_type != 3
      maintenance packet qPart:nds32:request:InvalidateCache
    else
      monitor nds cache invalidate
    end
    set $old_mr8 = $mr8
    set $mr8 = 0
  end
end

define hookpost-restore
  if $_nds32_target_type
    set $mr8 = $old_mr8
  end
end

# set args if target is SID
# this should be implement with vRun
define set hookpost-args
  nds32 set-gloss
end

# reset and hold
define reset-and-hold
  if $argc == 0
    if $_nds32_target_type != 3
      monitor reset hold
    else
      monitor reset halt
    end
  else
    echo "targetreset has no parameter"
  end
  # Clear $lp and $sp when reset-and-hold in order to avoid backtrace. (bug8032)
  set $lp = 0
  set $sp = 0
  flushregs
end

# Deprecated. Use reset-and-hold.
define targetreset
  if $argc == 0
    if $_nds32_target_type != 3
      monitor reset hold
    else
      monitor reset halt
    end
  else
    echo "targetreset has no parameter"
  end
  # Clear $lp and $sp when reset-and-hold in order to avoid backtrace. (bug8032)
  set $lp = 0
  set $sp = 0
  flushregs
end

# default reset address:0x00000000
# reset and run
define reset-and-run
  if $_nds32_target_type != 3
    if $argc == 0
      monitor reset target 0x00000000
    else
      monitor reset target $arg0
    end
  else
    monitor reset run
  end
  flushregs
end

# Decprecated. Use reset-and-run.
define targetresetrun
  if $_nds32_target_type != 3
    if $argc == 0
      monitor reset target 0x00000000
    else
      monitor reset target $arg0
    end
  else
    monitor reset run
  end
  flushregs
end

# mode: bus/cpu
define change_memory_access_mode
  if $argc == 0
    echo "Missing access MODE. (bus or cpu)"
  else
    if $_nds32_target_type != 3
      monitor change memory access mode $arg0
    else
      monitor nds mem_mode $arg0
    end
  end
end

# arg0 : burner program
# arg1 : address for write buffer
# arg2 : size of write buffer
# arg3 : Flash address
# arg4 : Flash I/O address
# arg5 : binary file
# arg6 : size of binary file
define program_flash_with_burner
  if ( $argc != 7 )
    echo "Parameter Error!"
  else
    set remote ignore_message_in_user_cmd 1
    exec-file $arg0
    info files
    load
    set $ir0=0x40a
    set $ir1=0x8
    set $ir2=0x0
    p/x $pc
    set $ir9=$
    set $ir11=$
    set $r0=$arg1
    set $r1=$arg2
    set $r2=$arg3
    set $r3=$arg4
    set $r4=$arg6
    b _buffer_done
    set $binary_file_begin=(0-$arg2)
    set $binary_file_end=0
    set $remain=$arg6
    set $burned=0
    if ( $remain < $arg2 )
      set $binary_file_begin=(0-$remain)
    end
    while ( $remain )
      if ( $remain >= $arg2 )
        set $binary_file_begin+=$arg2
        set $binary_file_end=$binary_file_begin+$arg2
        set $remain-=$arg2
      else
        if ( $burned == 0)
          set $binary_file_begin+=$remain
        else
          set $binary_file_begin+=$arg2
        end
        set $binary_file_end=$binary_file_begin+$remain
        set $remain-=$remain
      end
      set $burned=1
      change_memory_access_mode bus
      restore $arg5 binary ($arg1-$binary_file_begin) $binary_file_begin ($binary_file_end)
      change_memory_access_mode cpu
      c
      p $r0
      if ( $ != 0 )
        echo "Flash programming error!"
        loop_break
      end
    end
    if ( $remain == 0)
      echo "Flash program finished!"
    end
    d 1
    set remote ignore_message_in_user_cmd 0
  end
end

# arg0 : burner program
define flash_block_size
  if ( $argc != 1 )
    echo "Parameter Error!"
  else
    set remote ignore_message_in_user_cmd 1
    exec-file $arg0
    symbol-file $arg0
    load
    p/x flash_block_size
    set remote ignore_message_in_user_cmd 0
  end
end

echo [info] .Andesgdbinit loaded.\n
