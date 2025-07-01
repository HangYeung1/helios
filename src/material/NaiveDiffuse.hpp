#pragma once

#include "../Ray.hpp"
#include "../Sampler.hpp"
#include "BounceType.hpp"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <numbers>

namespace hls {

class NaiveDiffuse {
  public:
    explicit NaiveDiffuse(glm::vec3 &&albedo) noexcept
        : albedo(std::move(albedo)) {};

    inline BounceType scatter(Ray                        &incident,
                              const Ray                  &normal,
                              [[maybe_unused]] glm::vec3 &radiance,
                              glm::vec3                  &throughput,
                              Sampler                    &sampler) const {
        // Naive diffuse
        auto [r1, r2]   = sampler.sample<2>();
        float     phi   = r1 * 2.0f * std::numbers::pi_v<float>;
        float     theta = r2 * std::numbers::pi_v<float>;
        glm::vec3 direction(std::sin(theta) * std::cos(phi),
                            std::sin(theta) * std::sin(phi),
                            std::cos(theta));

        if (glm::dot(direction, normal.direction) <= 0.0f) {
            direction = -direction;
        }

        incident = Ray(normal.origin, direction);

        // Surface Albedo
        float           cos_theta = glm::dot(direction, normal.direction);
        constexpr float pdf       = 1.0f / (2.0f * std::numbers::pi_v<float>);
        throughput *= cos_theta * albedo / pdf;

        return bounce_type();
    }

    constexpr inline BounceType bounce_type() const {
        return BounceType::Diffuse;
    }

  private:
    glm::vec3 albedo;
};

} // namespace hls
