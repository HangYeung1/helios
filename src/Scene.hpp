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

    [[nodiscard]] bool interact(Ray       &incident,
                                glm::vec3 &radiance,
                                glm::vec3 &throughput) const {
        std::optional<std::pair<const Object *, Ray>> closest;
        float closest_dist = std::numeric_limits<float>::max();

        for (const auto &object : objects) {
            std::optional<Ray> normal = object.hit(incident);
            if (normal) {
                float dist = glm::length(normal->origin - incident.origin);
                if (dist < closest_dist) {
                    closest      = std::make_pair(&object, std::move(*normal));
                    closest_dist = dist;
                }
            }
        }

        if (!closest) {
            return false;
        }

        const auto &[object, normal] = *closest;
        object->scatter(incident, normal, radiance, throughput);
        return true;
    }

  private:
    std::vector<Object> objects;
};

} // namespace hls
