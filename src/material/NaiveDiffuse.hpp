#pragma once

#include "../Ray.hpp"
#include "../Sampler.hpp"
#include "BounceType.hpp"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <numbers>

namespace hls {

/**
 * @brief A naive diffuse material that scatters light uniformly.
 */
class NaiveDiffuse {
  public:
    /**
     * @brief Construct a naive diffuse material with the given albedo.
     */
    explicit NaiveDiffuse(glm::vec3 &&albedo) noexcept
        : albedo(std::move(albedo)) {};

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
    inline BounceType scatter(Ray                        &ray,
                              const Ray                  &normal,
                              [[maybe_unused]] glm::vec3 &radiance,
                              glm::vec3                  &throughput,
                              Sampler                    &sampler) const {
        // Sample a random direction on the normal hemisphere
        auto [r1, r2] = sampler.sample<2>();
        float phi     = r1 * 2.0f * std::numbers::pi_v<float>;
        float theta   = r2 * std::numbers::pi_v<float>;

        glm::vec3 direction(std::sin(theta) * std::cos(phi),
                            std::sin(theta) * std::sin(phi),
                            std::cos(theta));
        if (glm::dot(direction, normal.direction) <= 0.0f) {
            direction = -direction;
        }

        ray = Ray(normal.origin, direction);

        // Attenuate throughput
        float           cos_theta = glm::dot(direction, normal.direction);
        constexpr float pdf       = 1.0f / (2.0f * std::numbers::pi_v<float>);
        throughput *= cos_theta * albedo / pdf;

        return bounce_type();
    }

    /**
     * @brief Get the bounce type of the material.
     * @return `BounceType::Diffuse`.
     */
    constexpr inline BounceType bounce_type() const {
        return BounceType::Diffuse;
    }

  private:
    glm::vec3 albedo;
};

} // namespace hls
