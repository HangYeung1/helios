#pragma once

#include "../Ray.hpp"
#include "../Sampler.hpp"
#include "BounceType.hpp"

namespace hls {

/**
 * @brief A pure emissive material that emits light uniformly.
 */
class PureEmissive {
  public:
    /**
     * @brief Construct a naive diffuse material with the given albedo.
     */
    explicit PureEmissive(glm::vec3 &&luminance) noexcept
        : luminance(std::move(luminance)) {};

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
    inline BounceType scatter([[maybe_unused]] Ray       &ray,
                              [[maybe_unused]] const Ray &normal,
                              glm::vec3                  &radiance,
                              glm::vec3                  &throughput,
                              [[maybe_unused]] Sampler   &sampler) const {
        radiance += luminance * throughput;
        return bounce_type();
    }

    /**
     * @brief Get the bounce type of the material.
     * @return `BounceType::Emissive`.
     */
    constexpr inline BounceType bounce_type() const {
        return BounceType::Emissive;
    }

  private:
    glm::vec3 luminance;
};

} // namespace hls
