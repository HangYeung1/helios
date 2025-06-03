#pragma once

#include "../Ray.hpp"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <optional>

static constexpr float EPSILON = 1e-3f;

namespace hls {

class Sphere {
  public:
    Sphere(const glm::vec3 &center, float radius) noexcept
        : center(center), radius(radius) {};

    Sphere(glm::vec3 &&center, float radius) noexcept
        : center(std::move(center)), radius(radius) {};

    std::optional<Ray> hit(const Ray &ray) const {
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

  private:
    glm::vec3 center;
    float     radius;
};

} // namespace hls
