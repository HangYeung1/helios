#pragma once

#include "BounceType.hpp"
#include "NaiveDiffuse.hpp"
#include "PureEmissive.hpp"

#include <concepts>
#include <variant>

namespace hls {

/**
 * @brief Variant representing all material types in a scene.
 */
using Material = std::variant<NaiveDiffuse, PureEmissive>;

/**
 * @brief Interface for materials in a scene.
 *
 * A type `T` satisfies `is_material` if it provides a `scatter` function that
 * determines the scattering behavior of a ray at a surface normal. It may
 * contribute side effects on `radiance` and `throughput` and must return `true`
 * if scattering continues, and `false` otherwise.
 */
template <typename T>
concept is_material = requires(const T   &obj,
                               Ray       &incident,
                               const Ray &normal,
                               glm::vec3 &radiance,
                               glm::vec3 &throughput,
                               Sampler   &sampler) {
    {
        obj.scatter(incident, normal, radiance, throughput, sampler)
    } -> std::same_as<BounceType>;
    { obj.bounce_type() } -> std::same_as<BounceType>;
};

/**
 * @brief Utility to verify all types in a variant satisfies `is_material`.
 */
template <typename T>
constexpr inline bool material_variant = false;

template <typename... Ts>
constexpr inline bool material_variant<std::variant<Ts...>> =
    (is_material<Ts> && ...);

static_assert(material_variant<Material>);

} // namespace hls