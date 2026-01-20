// SPDX-License-Identifier: MIT
// Copyright (C) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
#ifndef AIEBU_UTILITIES_TARGET_H_
#define AIEBU_UTILITIES_TARGET_H_

#include "aiebu/aiebu_assembler.h"
#include "aiebu/aiebu_error.h"
#include "logger.h"

#include <filesystem>
#include <fstream>


namespace aiebu::utilities {

class target;

using target_collection = std::vector<std::shared_ptr<target>>;

class target
{
  protected:
  const std::string m_executable;
  const std::string m_sub_target_name;
  const std::string m_description;

  inline bool file_exists(const std::string& name) const {
    return std::filesystem::exists(name);
  }

  inline void readfile(const std::string& filename, std::vector<char>& buffer)
  {
    if (!file_exists(filename))
      throw std::runtime_error("file:" + filename + " not found\n");

    std::ifstream input(filename, std::ios::in | std::ios::binary);
    auto file_size = std::filesystem::file_size(filename);
    buffer.resize(file_size);
    input.read(buffer.data(), file_size);
  }

  inline void write_elf(const aiebu::aiebu_assembler& as, const std::string& outfile)
  {
    auto e = as.get_elf();
    log_info() << "elf size:" << e.size();
    std::ofstream output_file(outfile, std::ios_base::binary);
    output_file.write(e.data(), e.size());
  }

  public:
  using sub_cmd_options = std::vector<std::string>;
  virtual void assemble(const sub_cmd_options &_options) = 0;
  const std::string &get_name() const { return m_sub_target_name; }
  const std::string &get_nescription() const { return m_description; }

  target(const std::string& exename, const std::string& name, const std::string& description)
    : m_executable(exename),
      m_sub_target_name(name),
      m_description(description)
  {}
  virtual ~target() = default;

};

class target_aie2ps: public target
{
  public:
  void assemble(const sub_cmd_options &_options) override;

  target_aie2ps(const std::string& name): target(name, "aie2ps", "aie2ps asm assembler") {}
};

class target_aie2blob: public target
{
  protected:
  std::string m_transaction_file;
  std::string m_controlpkt_file;
  std::string m_external_buffers_file;
  std::vector<char> m_transaction_buffer;
  std::vector<char> m_control_packet_buffer;
  std::vector<char> m_patch_data_buffer;
  std::vector<std::string> m_libs;
  std::vector<std::string> m_libpaths;
  std::map<uint32_t, std::vector<char> > m_ctrlpkt;
  std::string m_output_elffile;
  bool m_print_report = false;
  target_aie2blob(const std::string& exename, const std::string& name, const std::string& description)
    : target(exename, name, description) {}
  bool parseOption(const sub_cmd_options &_options);

  std::map<uint32_t, std::vector<char> >
  parse_pmctrlpkt(std::vector<std::string> pm_key_value_pairs);
};

class target_aie2blob_transaction: public target_aie2blob
{
  public:
  target_aie2blob_transaction(const std::string& exename, const std::string& name = "aie2txn",
                              const std::string& description = "aie2 txn blob assembler")
    : target_aie2blob(exename, name, description) {}
  void assemble(const sub_cmd_options &_options) override;
};

class target_aie2: public target_aie2blob_transaction
{
  public:
  void assemble(const sub_cmd_options &_options) override;
  target_aie2(const std::string& exename): target_aie2blob_transaction(exename, "aie2asm", "aie2 asm assembler") {}
};

class target_aie2blob_dpu: public target_aie2blob
{
  public:
  target_aie2blob_dpu(const std::string& exename)
    : target_aie2blob(exename, "aie2dpu", "aie2 dpu blob assembler") {}
  void assemble(const sub_cmd_options &_options) override;
};

class target_aie2_config: public target
{
  public:
  void assemble(const sub_cmd_options &_options) override;
  explicit target_aie2_config(const std::string& name): target(name, "aie2_config", "generate aie2 config elf") {}
};

// Base class for AIE4 family asm targets (aie4, aie4a, aie4z)
class target_aie4_base: public target
{
  protected:
  aiebu::aiebu_assembler::buffer_type m_buffer_type;
  void assemble_common(const sub_cmd_options &_options);

  public:
  target_aie4_base(const std::string& exename, const std::string& name,
                   const std::string& description, aiebu::aiebu_assembler::buffer_type btype)
    : target(exename, name, description), m_buffer_type(btype) {}
  void assemble(const sub_cmd_options &_options) override { assemble_common(_options); }
};

class target_aie4: public target_aie4_base
{
  public:
  explicit target_aie4(const std::string& name)
    : target_aie4_base(name, "aie4", "aie4 asm assembler", aiebu::aiebu_assembler::buffer_type::asm_aie4) {}
};

class target_aie4a: public target_aie4_base
{
  public:
  explicit target_aie4a(const std::string& name)
    : target_aie4_base(name, "aie4a", "aie4a asm assembler", aiebu::aiebu_assembler::buffer_type::asm_aie4a) {}
};

class target_aie4z: public target_aie4_base
{
  public:
  explicit target_aie4z(const std::string& name)
    : target_aie4_base(name, "aie4z", "aie4z asm assembler", aiebu::aiebu_assembler::buffer_type::asm_aiez) {}
};

class asm_config_parser: public target
{
  protected: // NOLINT
  std::string output_elffile;
  std::vector<char> json_buffer;
  std::vector<std::string> libpaths;
  std::vector<std::string> flags;
  bool parser(const sub_cmd_options &options);
  public:
  asm_config_parser(const std::string& exename, const std::string& name, const std::string& description)
    : target(exename, name, description) {}
};

class target_aie2ps_config: public asm_config_parser
{
  public:
  void assemble(const sub_cmd_options &_options) override;
  explicit target_aie2ps_config(const std::string& name): asm_config_parser(name, "aie2ps_config", "generate aie2ps config elf") {}
};

// Base class for AIE4 family config targets
class target_aie4_config_base: public asm_config_parser
{
  protected:
  aiebu::aiebu_assembler::buffer_type m_buffer_type;

  public:
  target_aie4_config_base(const std::string& exename, const std::string& name,
                          const std::string& description, aiebu::aiebu_assembler::buffer_type btype)
    : asm_config_parser(exename, name, description), m_buffer_type(btype) {}
  void assemble(const sub_cmd_options &_options) override;
};

class target_aie4_config: public target_aie4_config_base
{
  public:
  explicit target_aie4_config(const std::string& name)
    : target_aie4_config_base(name, "aie4_config", "generate aie4 config elf", aiebu::aiebu_assembler::buffer_type::aie4_config) {}
};

class target_aie4a_config: public target_aie4_config_base
{
  public:
  explicit target_aie4a_config(const std::string& name)
    : target_aie4_config_base(name, "aie4a_config", "generate aie4a config elf", aiebu::aiebu_assembler::buffer_type::aie4a_config) {}
};

class target_aie4z_config: public target_aie4_config_base
{
  public:
  explicit target_aie4z_config(const std::string& name)
    : target_aie4_config_base(name, "aie4z_config", "generate aie4z config elf", aiebu::aiebu_assembler::buffer_type::aiez_config) {}
};
} //namespace aiebu::utilities

#endif // AIEBU_UTILITIES_TARGET_H_
