/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

#include <cstdint>
#include <immintrin.h>

namespace tetris_bi {
  inline uint32_t generate_randui32() {
    uint32_t rd;
    while (!_rdrand32_step(&rd));
    return rd;
  };
}

