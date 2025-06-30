#pragma once

#include "../Ray.hpp"
#include "../Sampler.hpp"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <numbers>
#include <optional>

namespace hls {

class Sphere {
  public:
    Sphere(glm::vec3 &&center, float &&radius) noexcept
        : center(std::move(center)), radius(std::move(radius)) {};

    std::optional<Ray> hit(const Ray &ray) const {
        // Calculate quadratic parameters
        glm::vec3 oc = ray.origin - center;

        float a = glm::dot(ray.direction, ray.direction);
        float b = 2.0f * glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - radius * radius;

        float discrim = b * b - 4.0f * a * c;
        if (discrim < 0.0f) {
            return std::nullopt;
        }

        float sqrt_discrim = std::sqrt(discrim);
        float t1           = (-b - sqrt_discrim) / (2.0f * a);
        float t2           = (-b + sqrt_discrim) / (2.0f * a);

        if (t1 < 0.0f && t2 < 0.0f) {
            return std::nullopt;
        }
        float t = (t1 >= 0.0f) ? t1 : t2;

        glm::vec3 point  = ray.at(t);
        glm::vec3 normal = glm::normalize(point - center);
        if (glm::dot(ray.direction, normal) >= 0.0f) {
            normal = -normal;
        }

        return Ray(point + EPSILON * normal, std::move(normal));
    }

    glm::vec3 sample_point(Sampler &sampler) const {
        auto [r1, r2] = sampler.sample<2>();
        float theta   = r1 * 2.0f * std::numbers::pi_v<float>;
        float phi     = r2 * std::numbers::pi_v<float>;

        float x = radius * std::sin(phi) * std::cos(theta);
        float y = radius * std::sin(phi) * std::sin(theta);
        float z = radius * std::cos(phi);

        return center + glm::vec3(x, y, z);
    }

  private:
    static constexpr float EPSILON = 1e-2f;

    glm::vec3 center;
    float     radius;
};

} // namespace hls
