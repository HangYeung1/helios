#pragma once

#include "Sphere.hpp"

#include <variant>

namespace hls {

using Geometry = std::variant<Sphere>;

} // namespace hls