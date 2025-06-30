#pragma once

#include "../Sampler.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"

#include <concepts>
#include <optional>
#include <variant>

namespace hls {

/**
 * @brief Variant representing all geometric types in a scene.
 */
using Geometry = std::variant<Sphere, Triangle>;

/**
 * @brief Interface for geometric objects in a scene.
 *
 * A type `T` satisfies `is_geometry` if it provides a `hit` function that
 * determines the intersection normal of a ray. It must return the surface
 * normal `Ray` if an intersection occurs, and `std::nullopt` otherwise.
 */
template <typename T>
concept is_geometry = requires(const T &obj, const Ray &ray, Sampler &sampler) {
    { obj.hit(ray) } -> std::same_as<std::optional<Ray>>;
    { obj.sample_point(sampler) } -> std::same_as<glm::vec3>;
};

/**
 * @brief Utility to verify all types in a variant satisfies `is_geometry`.
 */
template <typename T>
constexpr inline bool geometry_variant = false;

template <typename... Ts>
constexpr inline bool geometry_variant<std::variant<Ts...>> =
    (is_geometry<Ts> && ...);

static_assert(geometry_variant<Geometry>);

} // namespace hls