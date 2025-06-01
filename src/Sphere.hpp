#pragma once

#include "Ray.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <optional>

namespace hls {

class Sphere {
  public:
    struct Hit {
        glm::vec3 point;
        glm::vec3 normal;
    };

    Sphere(glm::vec3 center, float radius) : center(center), radius(radius) {};

    std::optional<Hit> hit(const Ray &ray) const {
        glm::vec3 oc = ray.origin - center;

        float a = glm::dot(ray.direction, ray.direction);
        float b = 2.0f * glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - radius * radius;

        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return std::nullopt;
        }

        float sqrt_discriminant = std::sqrt(discriminant);

        float t1 = (-b - sqrt_discriminant) / (2.0f * a);
        float t2 = (-b + sqrt_discriminant) / (2.0f * a);

        if (t1 >= 0) {
            glm::vec3 point = ray.at(t1);
            return Hit{.point  = point,
                       .normal = glm::normalize(point - center)};
        } else if (t2 >= 0) {
            glm::vec3 point = ray.at(t2);
            return Hit{.point  = point,
                       .normal = glm::normalize(point - center)};
        }
        return std::nullopt;
    }

  private:
    glm::vec3 center;
    float     radius;
};

} // namespace hls
