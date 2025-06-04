#pragma once

#include "../Ray.hpp"

#include <cmath>
#include <cstdlib>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <numbers>

namespace hls {

class NaiveDiffuse {
  public:
    explicit NaiveDiffuse(glm::vec3 &&albedo) noexcept
        : albedo(std::move(albedo)) {};

    Ray scatter(const Ray                  &normal,
                [[maybe_unused]] glm::vec3 &radiance,
                glm::vec3                  &throughput) const {
        // Surface Albedo
        throughput *= albedo;

        // Naive diffuse
        float r1 = static_cast<float>(std::rand()) / RAND_MAX;
        float r2 = static_cast<float>(std::rand()) / RAND_MAX;

        float     phi   = r1 * 2.0f * std::numbers::pi_v<float>;
        float     theta = r2 * std::numbers::pi_v<float>;
        glm::vec3 direction(std::sin(theta) * std::cos(phi),
                            std::sin(theta) * std::sin(phi),
                            std::cos(theta));

        if (glm::dot(direction, normal.direction) <= 0.0f) {
            direction = -direction;
        }

        return Ray(normal.origin, direction);
    }

  private:
    glm::vec3 albedo;
};

} // namespace hls
