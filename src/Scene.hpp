#pragma once

#include "Ray.hpp"
#include "Sphere.hpp"
#include <glm/vec3.hpp>
#include <optional>
#include <vector>

namespace hls {

class Scene {
  public:
    void add(const Sphere &object) {
        objects.push_back(object);
    }

    std::optional<Sphere::Intersection> hit(const Ray &ray) const {
        for (const auto &object : objects) {
            std::optional<Sphere::Intersection> hit_point = object.hit(ray);
            if (hit_point) {
                return hit_point;
            }
        }
        return std::nullopt;
    }

  private:
    std::vector<Sphere> objects;
};

} // namespace hls
