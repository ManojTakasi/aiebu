// SPDX-License-Identifier: MIT
// Copyright (C) 2024-2025, Advanced Micro Devices, Inc. All rights reserved.
#include "assembler.h"

#include "aie2ps_preprocessed_output.h"
#include "aie2_asm_preprocessor.h"
#include "aie2_blob_elfwriter.h"
#include "aie2_blob_encoder.h"
#include "aie2_blob_preprocessor.h"
#include "aie2ps_elfwriter.h"
#include "aie2ps_encoder.h"
#include "aie2ps_preprocessor.h"
#include "encoder/aie4/aie4_encoder.h"
#include "preprocessor/aie4/aie4_preprocessor.h"
#include "preprocessor/aie4/aie4_preprocessor_input.h"
#include "elf/aie4/aie4_elfwriter.h"
#include "elfwriter.h"
#include "encoder.h"
#include "preprocessor.h"

#include "aiebu/aiebu_error.h"

namespace aiebu {

// Normalize target string format only (dash removal)
// "aie4-a" -> "aie4a", "aie4-z" -> "aie4z"
// Each target (aie4, aie4a, aie4z) remains unique and distinct
static std::string
normalize_target_format(const std::string& target)
{
  std::string normalized = target;
  // Remove dash if present (e.g., "aie4-a" -> "aie4a")
  size_t dash_pos = normalized.find('-');
  if (dash_pos != std::string::npos)
    normalized.erase(dash_pos, 1);
  return normalized;
}

std::string
assembler::
elf_type_to_target(elf_type type)
{
  switch (type) {
    case elf_type::aie2ps_asm:
    case elf_type::aie2ps_config:
      return "aie2ps";
    case elf_type::aie4_asm:
    case elf_type::aie4_config:
      return "aie4";
    case elf_type::aie4a_asm:
    case elf_type::aie4a_config:
      return "aie4a";
    case elf_type::aiez_asm:
    case elf_type::aiez_config:
      return "aie4z";
    default:
      return "";  // No target validation for other types
  }
}

void
assembler::
validate_target(const std::string& parsed_target) const
{
  std::string expected = elf_type_to_target(m_elf_type);

  // If no target validation needed for this elf_type, skip
  if (expected.empty())
    return;

  // Normalize format only (e.g., "aie4-a" -> "aie4a")
  // Each target remains unique: aie4, aie4a, aie4z are all different
  std::string normalized_parsed = normalize_target_format(parsed_target);

  // Exact match required - aie4, aie4a, aie4z are distinct targets
  if (normalized_parsed != expected) {
    throw error(error::error_code::invalid_asm,
                "Target mismatch: ASM is for architecture '" + parsed_target +
                "' while the assembler target (-t option) is '" + expected + "'");
  }
}

assembler::
assembler(const elf_type type) : m_elf_type(type)
{

  if (type == elf_type::aie2_dpu_blob)  {
    m_preprocessor = std::make_unique<aie2_blob_preprocessor>();
    m_enoder = std::make_unique<aie2_blob_encoder>();
    m_elfwriter = std::make_unique<aie2_blob_elf_writer>();
    m_ppi = std::make_shared<aie2_blob_dpu_preprocessor_input>();
  }
  else if (type == elf_type::aie2_transaction_blob)  {
    m_preprocessor = std::make_unique<aie2_blob_preprocessor>();
    m_enoder = std::make_unique<aie2_blob_encoder>();
    m_elfwriter = std::make_unique<aie2_blob_elf_writer>();
    m_ppi = std::make_shared<aie2_blob_transaction_preprocessor_input>();
  }
  else if (type == elf_type::aie2_asm)  {
    m_preprocessor = std::make_unique<aie2_asm_preprocessor>();
    // Reuse encoder and elfwriter flow from the aie2 blob as they do not
    // see the ASM but instead see the same binary aie2 blob.
    m_enoder = std::make_unique<aie2_blob_encoder>();
    m_elfwriter = std::make_unique<aie2_blob_elf_writer>();
    m_ppi = std::make_shared<aie2_asm_preprocessor_input>();
  }
  else if (type == elf_type::aie2ps_asm)
  {
    m_preprocessor = std::make_unique<aie2ps_preprocessor>();
    m_enoder = std::make_unique<aie2ps_encoder>();
    m_elfwriter = std::make_unique<aie2ps_elf_writer>();
    m_ppi = std::make_shared<aie2ps_preprocessor_input>();
  }
  else if (type == elf_type::aie2_config)  {
    m_preprocessor = std::make_unique<aie2_config_preprocessor>();
    m_enoder = std::make_unique<aie2_config_encoder>();
    m_elfwriter = std::make_unique<aie2_config_elf_writer>();
    m_ppi = std::make_shared<aie2_config_preprocessor_input>();
  }
  else if (type == elf_type::aie4_asm)
  {
    m_preprocessor = std::make_unique<aie4_preprocessor>();
    m_enoder = std::make_unique<aie4_encoder>();
    m_elfwriter = std::make_unique<aie4_elf_writer>();
    m_ppi = std::make_shared<aie4_preprocessor_input>();
  }
  else if (type == elf_type::aie2ps_config)
  {
    m_preprocessor = std::make_unique<asm_config_preprocessor<aie2ps_preprocessor, aie2ps_preprocessor_input, aie2ps_preprocessed_output>>();
    m_enoder = std::make_unique<asm_config_encoder<aie2ps_encoder, aie2ps_preprocessed_output>>();
    m_elfwriter = std::make_unique<aie2ps_config_elf_writer>();
    m_ppi = std::make_shared<controlcode_config_preprocessor_input<aie2ps_preprocessor_input>>();
  }
  else if (type == elf_type::aie4_config)
  {
    m_preprocessor = std::make_unique<asm_config_preprocessor<aie4_preprocessor, aie4_preprocessor_input, aie2ps_preprocessed_output>>();
    m_enoder = std::make_unique<asm_config_encoder<aie4_encoder, aie2ps_preprocessed_output>>();
    m_elfwriter = std::make_unique<aie4_config_elf_writer>();
    m_ppi = std::make_shared<controlcode_config_preprocessor_input<aie4_preprocessor_input>>();
  }
  else if (type == elf_type::aie4a_asm)
  {
    m_preprocessor = std::make_unique<aie4_preprocessor>();
    m_enoder = std::make_unique<aie4_encoder>();
    m_elfwriter = std::make_unique<aie4a_elf_writer>();
    m_ppi = std::make_shared<aie4_preprocessor_input>();
  }
  else if (type == elf_type::aie4a_config)
  {
    m_preprocessor = std::make_unique<asm_config_preprocessor<aie4_preprocessor, aie4_preprocessor_input, aie2ps_preprocessed_output>>();
    m_enoder = std::make_unique<asm_config_encoder<aie4_encoder, aie2ps_preprocessed_output>>();
    m_elfwriter = std::make_unique<aie4a_config_elf_writer>();
    m_ppi = std::make_shared<controlcode_config_preprocessor_input<aie4_preprocessor_input>>();
  }
  else if (type == elf_type::aiez_asm)
  {
    m_preprocessor = std::make_unique<aie4_preprocessor>();
    m_enoder = std::make_unique<aie4_encoder>();
    m_elfwriter = std::make_unique<aiez_elf_writer>();
    m_ppi = std::make_shared<aie4_preprocessor_input>();
  }
  else if (type == elf_type::aiez_config)
  {
    m_preprocessor = std::make_unique<asm_config_preprocessor<aie4_preprocessor, aie4_preprocessor_input, aie2ps_preprocessed_output>>();
    m_enoder = std::make_unique<asm_config_encoder<aie4_encoder, aie2ps_preprocessed_output>>();
    m_elfwriter = std::make_unique<aiez_config_elf_writer>();
    m_ppi = std::make_shared<controlcode_config_preprocessor_input<aie4_preprocessor_input>>();
  }
  else {
    throw error(error::error_code::invalid_buffer_type ,"Invalid elf type!!!");
  }
}

std::vector<char>
assembler::
process(const std::vector<char>& buffer1,
        const std::vector<std::string>& libs,
        const std::vector<std::string>& libpaths,
        const std::vector<char>& patch_json,
        const std::vector<char>& buffer2,
        const std::map<uint32_t, std::vector<char> >& ctrlpkt,
        const file_artifact* artifacts)
{
  m_ppi->set_args(buffer1, patch_json, buffer2, libs, libpaths, ctrlpkt, artifacts);
  auto ppo = m_preprocessor->process(m_ppi);

  // Validate .target directive if present
  auto aie2ps_output = std::dynamic_pointer_cast<aie2ps_preprocessed_output>(ppo);
  if (aie2ps_output && aie2ps_output->has_target()) {
    validate_target(aie2ps_output->get_target());
  }

  auto w = m_enoder->process(ppo);
  auto u = m_elfwriter->process(w);
  return u;
}

}
