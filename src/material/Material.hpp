#pragma once

#include "NaiveDiffuse.hpp"

#include <variant>

namespace hls {

using Material = std::variant<NaiveDiffuse>;

} // namespace hls