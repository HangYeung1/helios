#pragma once

#include "Object.hpp"
#include "Ray.hpp"

#include <glm/vec3.hpp>
#include <optional>
#include <utility>
#include <vector>

namespace hls {

class Scene {
  public:
    inline void add(Object &&object) {
        objects.push_back(std::move(object));
    }

    template <typename... Args>
    inline void add(Args &&...args) {
        objects.emplace_back(std::forward<Args>(args)...);
    }

    std::optional<Ray> interact(const Ray &ray, glm::vec3 &radiance,
                                glm::vec3 &throughput) const {
        std::optional<std::pair<const Object *, Ray>> closest;
        float closest_dist = std::numeric_limits<float>::max();

        for (const auto &object : objects) {
            std::optional<Ray> normal = object.hit(ray);
            if (normal) {
                float dist = glm::length(normal->origin - ray.origin);
                if (dist < closest_dist) {
                    closest      = std::make_pair(&object, std::move(*normal));
                    closest_dist = dist;
                }
            }
        }

        if (!closest) {
            return std::nullopt;
        }

        const auto &[object, normal] = *closest;
        return object->scatter(normal, radiance, throughput);
    }

  private:
    std::vector<Object> objects;
};

} // namespace hls
