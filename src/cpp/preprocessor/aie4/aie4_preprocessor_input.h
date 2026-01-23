// SPDX-License-Identifier: MIT
// Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.

#ifndef AIEBU_PREPROCESSOR_AIE4_PREPROCESSOR_INPUT_H_
#define AIEBU_PREPROCESSOR_AIE4_PREPROCESSOR_INPUT_H_

#include "aie2ps_preprocessor_input.h"
namespace aiebu {

class aie4_preprocessor_input : public asm_preprocessor_input
{
public:
  aie4_preprocessor_input() {
    control_packet_patching = symbol::patch_schema::control_packet_57_aie4;
    m_control_packet_offset_correction = 4;
  }

  std::string get_expected_target() const override { return "aie4"; }
};

}
#endif //AIEBU_PREPROCESSOR_AIE4_PREPROCESSOR_INPUT_H_
