// SPDX-License-Identifier: MIT
// Copyright (C) 2026, Advanced Micro Devices, Inc. All rights reserved.

#ifndef AIEBU_ELF_AIE4_ELF_WRITER_H_
#define AIEBU_ELF_AIE4_ELF_WRITER_H_

#include <aie2ps_elfwriter.h>

namespace aiebu {

// OS/ABI values for AIE architectures (high Hamming distance for robustness)
constexpr unsigned char osabi_aie2ps_group = 0x46;  // 70 - Legacy group ELF
constexpr unsigned char osabi_aie2ps       = 0x40;  // 64 - AIE2PS specific
constexpr unsigned char osabi_aie4         = 0x4B;  // 75 - AIE4
constexpr unsigned char osabi_aie4a        = 0x56;  // 86 - AIE4A
constexpr unsigned char osabi_aie4z        = 0x69;  // 105 - aie4z

// ELF versions
constexpr unsigned char elf_version_legacy        = 0x02;  // Legacy non-config
constexpr unsigned char elf_version_legacy_config = 0x03;  // Legacy config
constexpr unsigned char elf_version_new           = 0x04;  // New non-config
constexpr unsigned char elf_version_new_config    = 0x05;  // New config

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
