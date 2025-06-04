#pragma once

#include "NaiveDiffuse.hpp"

#include <concepts>
#include <variant>

namespace hls {

using Material = std::variant<NaiveDiffuse>;

template <typename T>
concept Scatterable = requires(
    const T &obj, const Ray &ray, glm::vec3 &radiance, glm::vec3 &throughput) {
    { obj.scatter(ray, radiance, throughput) } -> std::same_as<Ray>;
};

template <typename Variant>
inline constexpr bool scatterable_variant = false;

template <typename... Ts>
inline constexpr bool scatterable_variant<std::variant<Ts...>> =
    (Scatterable<Ts> && ...);

static_assert(scatterable_variant<Material>);

} // namespace hls