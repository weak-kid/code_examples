#pragma once

#include "array"
#include "bit_reader.h"
#include "consts.h"
#include "iostream"
#include "limits"

namespace counter {
std::array<u_int64_t, consts::CHARS_COUNT> count(std::istream&);
}
