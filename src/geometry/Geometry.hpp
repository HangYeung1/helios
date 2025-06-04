#pragma once

#include "Sphere.hpp"

#include <concepts>
#include <optional>
#include <variant>

namespace hls {

using Geometry = std::variant<Sphere>;

template <typename T>
concept Hittable = requires(const T &obj, const Ray &ray) {
    { obj.hit(ray) } -> std::same_as<std::optional<Ray>>;
};

template <typename Variant>
inline constexpr bool hittable_variant = false;

template <typename... Ts>
inline constexpr bool hittable_variant<std::variant<Ts...>> =
    (Hittable<Ts> && ...);

static_assert(hittable_variant<Geometry>);

} // namespace hls