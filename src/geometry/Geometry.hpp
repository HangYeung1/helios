#pragma once

#include "../Sampler.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"

#include <concepts>
#include <optional>
#include <variant>

namespace hls {

/**
 * @brief A variant representing all geometric types in a scene.
 */
using Geometry = std::variant<Sphere, Triangle>;

/**
 * @brief The interface for geometric objects in a scene.
 * @details See the requirements below for details.
 */
template <typename T>
concept is_geometry =
    /**
     * @brief Determine the intersection normal `Ray`.
     * @param ray to test for intersection.
     * @return the surface normal `Ray` or `std::nullopt`.
     */
    requires(const T &obj, const Ray &ray) {
        { obj.hit(ray) } -> std::same_as<std::optional<Ray>>;
    } &&
    /**
     * @brief Sample a point on the surface of the object.
     * @param sampler the rng to use.
     * @return the sampled point `glm::vec3`.
     */
    requires(const T &obj, Sampler &sampler) {
        { obj.sample_point(sampler) } -> std::same_as<glm::vec3>;
    };

/**
 * @brief A utility to verify all types in a variant satisfies `is_geometry`.
 */
template <typename T>
constexpr inline bool geometry_variant = false;

template <typename... Ts>
constexpr inline bool geometry_variant<std::variant<Ts...>> =
    (is_geometry<Ts> && ...);

static_assert(geometry_variant<Geometry>);

} // namespace hls