#include <unicorn/unicorn.h>
#include "util.h"

void hook_mem_rw(uc_engine* uc, uc_mem_type type, uint64_t address, int size, int64_t value, void* user_data) {
  const char* access = (type == UC_MEM_WRITE) ? "WRITE" : "READ";
  printf("Memory %s @ 0x%" PRIx64 ",\t[0x%llX]\n", access, address, value);
  if (address == 0x5000000) {
    printf("%c", (char)value);
  }
}

int main(int argc, char** argv, char** envp) {
  long boot0_size;
  char* boot0_buffer;

  if (!aw_emu::util::read_binary("F:\\Git\\aw_emu\\out\\build\\x64-Debug\\boot0_aw.bin", &boot0_size, &boot0_buffer)) {
    printf("loading boot0 failed.");
    return -1;
  }

  uc_engine* uc;
  uc_hook memrw;
  uc_err err;

  err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &uc);
  if (err != UC_ERR_OK) {
    printf("Failed on uc_open() with error returned: %u\n", err);
    return -1;
  }

  uc_mem_map(uc, 0, 0xffff + 1, UC_PROT_ALL);
  uc_mem_map(uc, 0x20000, 0x3fff + 1, UC_PROT_ALL);
  uc_mem_map(uc, 0x24000, 0x20fff + 1, UC_PROT_ALL);
  uc_mem_map(uc, 0x100000, 0x13fff + 1, UC_PROT_ALL);

  uc_mem_map(uc, 0x3000000, 0xfff + 1, UC_PROT_ALL);
  uc_mem_map(uc, 0x3001000, 0xfff + 1, UC_PROT_ALL);
  uc_mem_map(uc, 0x300b000, 0x3ff + 1, UC_PROT_ALL);
  uc_mem_map(uc, 0x30f0000, 0xffff + 1, UC_PROT_ALL);

  uc_mem_map(uc, 0x300d000, 0xfff + 1, UC_PROT_ALL);

  uc_mem_map(uc, 0x5000000, 0x3ff + 1, UC_PROT_ALL);
  uc_mem_write(uc, 0x5000014, "\x40", 1);

  uc_mem_map(uc, 0x7000000, 0x3ff + 1, UC_PROT_ALL);
  uc_mem_map(uc, 0x7000400, 0x7ff + 1, UC_PROT_ALL);
  uc_mem_map(uc, 0x7010000, 0x3ff + 1, UC_PROT_ALL);

  if (uc_mem_write(uc, 0x20000, boot0_buffer, boot0_size - 1)) {
    printf("Failed to write emulation code to memory, quit!\n");
    return -1;
  }

  int r_sp = 0x551c4; // Is this even accurate for A133? Copied from H616.
  uc_reg_write(uc, UC_ARM_REG_SP, &r_sp);

  uc_hook_add(uc, &memrw, UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, hook_mem_rw, NULL, 1, 0);

  err = uc_emu_start(uc, 0x20000, 0x20000 + boot0_size - 1, 0, 0);
  if (err) {
    printf("Failed on uc_emu_start() with error returned %u: %s\n", err, uc_strerror(err));
  }

  // cpu resets here, but we dont handle it yet
 // err = uc_emu_start(uc, 0x20000, 0x20000 + boot0_size - 1, 0, 0);
  if (err) {
    printf("Failed on uc_emu_start() with error returned %u: %s\n", err, uc_strerror(err));
  }

  uc_close(uc);

  return 0;
}
