#pragma once

#include "BounceType.hpp"
#include "NaiveDiffuse.hpp"
#include "PureEmissive.hpp"

#include <concepts>
#include <variant>

namespace hls {

/**
 * @brief A variant representing all material types in a scene.
 */
using Material = std::variant<NaiveDiffuse, PureEmissive>;

/**
 * @brief The interface for materials in a scene.
 * @details See the requirements below for details.
 */
template <typename T>
concept is_material =
    /**
     * @brief Scatter a ray at a surface normal.
     * @param ray the ray to scatter.
     * @param normal the surface normal ray.
     * @param radiance the radiance to contribute to.
     * @param throughput the throughput to contribute to.
     * @param sampler the sampler to use for sampling.
     * @return the type of bounce that occurs.
     * @note This function has side effects on `radiance` and `throughput`.
     */
    requires(const T   &obj,
             Ray       &ray,
             const Ray &normal,
             glm::vec3 &radiance,
             glm::vec3 &throughput,
             Sampler   &sampler) {
        {
            obj.scatter(ray, normal, radiance, throughput, sampler)
        } -> std::same_as<BounceType>;
    }
    &&
    /**
     * @brief Get the bounce type of the material.
     * @return the bounce type of the material.
     */
    requires(const T &obj) {
        { obj.bounce_type() } -> std::same_as<BounceType>;
    };

/**
 * @brief A utility to verify all types in a variant satisfies `is_material`.
 */
template <typename T>
constexpr inline bool material_variant = false;

template <typename... Ts>
constexpr inline bool material_variant<std::variant<Ts...>> =
    (is_material<Ts> && ...);

static_assert(material_variant<Material>);

} // namespace hls