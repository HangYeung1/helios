#pragma once

#include <glm/vec3.hpp>
#include <utility>

namespace hls {

/**
 * @brief A ray in 3D space defined by an origin and a direction.
 */
struct Ray {
    /**
     * @brief Construct a ray with the given origin and direction.
     */
    Ray(const glm::vec3 &origin, const glm::vec3 &direction) noexcept
        : origin(origin), direction(direction) {};

    /**
     * @brief Construct a ray with the given origin and direction.
     */
    Ray(glm::vec3 &&origin, glm::vec3 &&direction) noexcept
        : origin(std::move(origin)), direction(std::move(direction)) {};

    /**
     * @brief Get a point on the ray at parameter `t`.
     * @param t the parameter along the ray.
     * @return the point on the ray at parameter `t`.
     */
    inline glm::vec3 at(float t) const {
        return origin + t * direction;
    }

    glm::vec3 origin;
    glm::vec3 direction;
};

} // namespace hls
