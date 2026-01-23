// SPDX-License-Identifier: MIT
// Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.

#ifndef AIEBU_ELF_AIE4_ELF_WRITER_H_
#define AIEBU_ELF_AIE4_ELF_WRITER_H_

#include <aie2ps_elfwriter.h>

namespace aiebu {

// OS/ABI values for AIE architectures
// Values chosen with high Hamming distance for robustness against bit flips
constexpr unsigned char OSABI_AIE2PS_GROUP = 0x46;  // 70 - Legacy group ELF (for backward compatibility)
constexpr unsigned char OSABI_AIE2PS       = 0x40;  // 64 - AIE2PS specific (when .target aie2ps is present)
constexpr unsigned char OSABI_AIE4         = 0x4B;  // 75 - AIE4
constexpr unsigned char OSABI_AIE4A        = 0x56;  // 86 - AIE4A
constexpr unsigned char OSABI_AIE4Z        = 0x69;  // 105 - aie4z

// ELF ABI versions:
// Legacy (no .target in ASM): 0x02 (non-config), 0x03 (config) - uses OSABI_AIE2PS_GROUP
// New (with .target in ASM): 0x04 (non-config), 0x05 (config) - uses specific OSABI
constexpr unsigned char ELF_ABI_VERSION_LEGACY        = 0x02;
constexpr unsigned char ELF_ABI_VERSION_LEGACY_CONFIG = 0x03;
constexpr unsigned char ELF_ABI_VERSION_NEW           = 0x04;
constexpr unsigned char ELF_ABI_VERSION_NEW_CONFIG    = 0x05;

/**
 * @brief AIE4 family ELF writer
 *
 * Reuses aie2ps_elf_writer since both default to same legacy values
 * (OSABI = aie2ps_group = 0x46, version = 0x02).
 * When .target directive is present in ASM, assembler will call
 * set_os_abi() and set_abi_version() to set the new values.
 */
using aie4_elf_writer = aie2ps_elf_writer;

/**
 * @brief AIE4 family config ELF writer
 *
 * Reuses aie2ps_config_elf_writer since both default to same legacy values
 * (OSABI = aie2ps_group = 0x46, version = 0x03) and share the same process() method.
 * When .target directive is present in ASM, assembler will call
 * set_os_abi() and set_abi_version() to set the new values.
 */
using aie4_config_elf_writer = aie2ps_config_elf_writer;

}
#endif //AIEBU_ELF_AIE4_ELF_WRITER_H_
