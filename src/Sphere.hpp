#pragma once

#include "Ray.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

namespace hls {

class Sphere {
  public:
    Sphere(glm::vec3 center, float radius) : center(center), radius(radius) {};

    bool hit(const Ray &ray) const {
        glm::vec3 oc = ray.origin - center;

        float a = glm::dot(ray.direction, ray.direction);
        float b = 2.0f * glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - radius * radius;

        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return false;
        }

        float sqrt_disriminant = std::sqrt(discriminant);

        float t1 = (-b - sqrt_disriminant) / (2.0f * a);
        float t2 = (-b + sqrt_disriminant) / (2.0f * a);

        return t1 >= 0 || t2 >= 0;
    }

  private:
    glm::vec3 center;
    float     radius;
};

} // namespace hls
