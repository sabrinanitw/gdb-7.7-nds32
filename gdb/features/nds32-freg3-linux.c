/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: nds32-freg3-linux.xml */

#include "defs.h"
#include "osabi.h"
#include "target-descriptions.h"

struct target_desc *tdesc_nds32_freg3_linux;
static void
initialize_tdesc_nds32_freg3_linux (void)
{
  struct target_desc *result = allocate_target_description ();
  struct tdesc_feature *feature;

  set_tdesc_architecture (result, bfd_scan_arch ("n1h"));

  set_tdesc_osabi (result, osabi_from_tdesc_string ("GNU/Linux"));

  feature = tdesc_create_feature (result, "org.gnu.gdb.nds32.core");
  tdesc_create_reg (feature, "r0", 0, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r1", 1, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r2", 2, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r3", 3, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r4", 4, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r5", 5, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r6", 6, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r7", 7, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r8", 8, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r9", 9, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r10", 10, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r11", 11, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r12", 12, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r13", 13, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r14", 14, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r15", 15, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r16", 16, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r17", 17, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r18", 18, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r19", 19, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r20", 20, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r21", 21, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r22", 22, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r23", 23, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r24", 24, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r25", 25, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "", 26, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "", 27, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "fp", 28, 1, NULL, 32, "data_ptr");
  tdesc_create_reg (feature, "gp", 29, 1, NULL, 32, "data_ptr");
  tdesc_create_reg (feature, "lp", 30, 1, NULL, 32, "code_ptr");
  tdesc_create_reg (feature, "sp", 31, 1, NULL, 32, "data_ptr");
  tdesc_create_reg (feature, "pc", 32, 1, NULL, 32, "code_ptr");
  tdesc_create_reg (feature, "d0lo", 33, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "d0hi", 34, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "d1lo", 35, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "d1hi", 36, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "orig_r0", 37, 1, "system", 32, "int");
  tdesc_create_reg (feature, "fucpr", 38, 1, "system", 32, "int");

  feature = tdesc_create_feature (result, "org.gnu.gdb.nds32.fpu");
  tdesc_create_reg (feature, "fd0", 39, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd1", 40, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd2", 41, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd3", 42, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd4", 43, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd5", 44, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd6", 45, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd7", 46, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd8", 47, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd9", 48, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd10", 49, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd11", 50, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd12", 51, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd13", 52, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd14", 53, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd15", 54, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd16", 55, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd17", 56, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd18", 57, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd19", 58, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd20", 59, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd21", 60, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd22", 61, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd23", 62, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd24", 63, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd25", 64, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd26", 65, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd27", 66, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd28", 67, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd29", 68, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd30", 69, 1, NULL, 64, "ieee_single");
  tdesc_create_reg (feature, "fd31", 70, 1, NULL, 64, "ieee_single");

  tdesc_nds32_freg3_linux = result;
}
