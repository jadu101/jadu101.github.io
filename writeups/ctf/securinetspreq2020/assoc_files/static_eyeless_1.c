
/* WARNING: Removing unreachable block (ram,0x00100a44) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_00100820(void)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  ulong uVar5;
  size_t sVar6;
  long lVar7;
  undefined8 *puVar8;
  long in_FS_OFFSET;
  double dVar9;
  int local_238;
  int local_234;
  long local_228;
  uint auStack504 [52];
  undefined8 local_128;
  undefined4 local_120;
  undefined4 local_11c;
  uint local_118;
  undefined4 local_114;
  undefined4 local_110;
  undefined4 local_10c;
  undefined4 local_108;
  undefined4 local_104;
  undefined4 local_100;
  undefined4 local_fc;
  undefined4 local_f8;
  undefined4 local_f4;
  undefined4 local_f0;
  undefined4 local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  undefined4 local_e0;
  undefined4 local_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  undefined4 local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  undefined4 local_b4;
  undefined4 local_b0;
  char local_58 [56];
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  lVar7 = 0x19;
  puVar8 = &local_128;
  while (lVar7 != 0) {
    lVar7 = lVar7 + -1;
    *puVar8 = 0;
    puVar8 = puVar8 + 1;
  }
  local_128._0_4_ = 0xd1;
  local_128._4_4_ = 0x1e;
  local_120 = 0xdb;
  local_11c = 0xfb;
  local_118 = 0x74;
  local_114 = 0xcb;
  local_110 = 0x15;
  local_10c = 0xdd;
  local_108 = 0xfa;
  local_104 = 0x75;
  local_100 = 0xd9;
  local_fc = 0x4b;
  local_f8 = 0xda;
  local_f4 = 0xe8;
  local_f0 = 0x73;
  local_ec = 0xd1;
  local_e8 = 0x4f;
  local_e4 = 0xcc;
  local_e0 = 0xe7;
  local_dc = 0x36;
  local_d8 = 0xcc;
  local_d4 = 0x4e;
  local_d0 = 0xe7;
  local_cc = 0xfc;
  local_c8 = 0x36;
  local_c4 = 0xc1;
  local_c0 = 0x10;
  local_bc = 0x8d;
  local_b8 = 0xaf;
  local_b4 = 0x7b;
  local_b0 = 0xa8;
  iVar2 = FUN_0010080a();
  iVar3 = FUN_00100815();
  uVar5 = (ulong)local_118;
  dVar9 = (double)(int)local_128;
  iVar4 = local_128._4_4_ + -0x14;
  cVar1 = (char)local_128._4_4_;
  puts("Hello REVERSER!");
  lVar7 = ptrace(PTRACE_TRACEME,0,0,0);
  lVar7 = (long)((int)lVar7 + 1) *
          ((long)iVar4 *
           (long)((dVar9 * (((double)iVar3 * 29.00000000 + 58.00000000) * (double)uVar5 +
                           110.00000000) + 141.00000000) * (double)iVar2 + 20.00000000) >>
          (cVar1 - 0x16U & 0x3f)) * 0xc0fe;
  printf("Give me the passcode:");
  if (lVar7 < 0) {
    lVar7 = lVar7 + 0xff;
  }
  fgets(local_58,0x31,_DAT_00302010);
  local_234 = 0;
  local_238 = 0;
  local_228 = lVar7 >> 8;
  while (sVar6 = strlen(local_58), (ulong)(long)local_238 < sVar6) {
    if (local_228 == 0) {
      local_228 = lVar7 >> 8;
    }
    auStack504[local_238] = (int)local_58[local_238] ^ (uint)local_228 & 0xff;
    local_228 = local_228 >> 8;
    local_238 = local_238 + 1;
  }
  local_238 = 0;
  while (local_238 < 0x1e) {
    local_234 = local_234 +
                (*(uint *)((long)&local_128 + (long)local_238 * 4) ^ auStack504[local_238] | 1);
    local_238 = local_238 + 1;
  }
  lVar7 = ptrace(PTRACE_TRACEME,0,0);
  if (local_234 ==
      -((((((0x123456 << ((char)local_128._4_4_ - 0x1aU & 0x1f)) >>
           ((char)(int)local_128 + 0x48U & 0x1f)) + local_128._4_4_) -
         ((0x654321 << ((char)local_128._4_4_ - 0x1aU & 0x1f)) >>
         ((char)(int)local_128 + 0x48U & 0x1f))) + 3) * (int)lVar7)) {
    puts("Good job!");
  }
  else {
    printf("NOOOOOOOO");
  }
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return 0;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}

undefined8 FUN_0010080a(void)

{
  return 0x15;
}

undefined8 FUN_00100815(void)

{
  return 0xfb;
}


