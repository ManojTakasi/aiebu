// SPDX-License-Identifier: MIT
// Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.

#ifndef AIEBU_ELF_AIE4_ELF_WRITER_H_
#define AIEBU_ELF_AIE4_ELF_WRITER_H_

#include <aie2ps_elfwriter.h>

namespace aiebu {

// OS/ABI values for AIE4 family architectures
// Values chosen with high Hamming distance for robustness against bit flips
// AIE4:   0x4B = 0100 1011
// AIE4A:  0x56 = 0101 0110 (5 bits from 0x4B)
// AIEZ:   0x69 = 0110 1001 (4 bits from 0x56)
constexpr unsigned char OSABI_AIE4   = 0x4B;  // 75
constexpr unsigned char OSABI_AIE4A  = 0x56;  // 86
constexpr unsigned char OSABI_AIEZ   = 0x69;  // 105

// ELF ABI versions for AIE4 family - new values distinct from aie2ps (which uses 0x02/0x03)
// Version 0x04 (non-config) and 0x05 (config) indicate OS ABI contains accurate target info
constexpr unsigned char ELF_ABI_VERSION_AIE4       = 0x04;
constexpr unsigned char ELF_ABI_VERSION_AIE4_CONFIG = 0x05;

class aie4_elf_writer: public elf_writer
{
public:
  aie4_elf_writer(): elf_writer(OSABI_AIE4, ELF_ABI_VERSION_AIE4)
  { }
};

// Config writers inherit from aie2ps_config_elf_writer for the process() method,
// then override OS ABI and version in constructor
class aie4_config_elf_writer: public aie2ps_config_elf_writer
{
public:
  aie4_config_elf_writer(): aie2ps_config_elf_writer()
  {
    m_elfio.set_os_abi(OSABI_AIE4);
    m_elfio.set_abi_version(ELF_ABI_VERSION_AIE4_CONFIG);
  }
};

class aie4a_elf_writer: public elf_writer
{
public:
  aie4a_elf_writer(): elf_writer(OSABI_AIE4A, ELF_ABI_VERSION_AIE4)
  { }
};

class aie4a_config_elf_writer: public aie2ps_config_elf_writer
{
public:
  aie4a_config_elf_writer(): aie2ps_config_elf_writer()
  {
    m_elfio.set_os_abi(OSABI_AIE4A);
    m_elfio.set_abi_version(ELF_ABI_VERSION_AIE4_CONFIG);
  }
};

class aiez_elf_writer: public elf_writer
{
public:
  aiez_elf_writer(): elf_writer(OSABI_AIEZ, ELF_ABI_VERSION_AIE4)
  { }
};

class aiez_config_elf_writer: public aie2ps_config_elf_writer
{
public:
  aiez_config_elf_writer(): aie2ps_config_elf_writer()
  {
    m_elfio.set_os_abi(OSABI_AIEZ);
    m_elfio.set_abi_version(ELF_ABI_VERSION_AIE4_CONFIG);
  }
};

}
#endif //AIEBU_ELF_AIE4_ELF_WRITER_H_
