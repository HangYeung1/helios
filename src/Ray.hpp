#pragma once

#include <glm/vec3.hpp>

#include <utility>

namespace hls {

struct Ray {
    Ray(const glm::vec3 &origin, const glm::vec3 &direction) noexcept
        : origin(origin), direction(direction) {};

    Ray(glm::vec3 &&origin, glm::vec3 &&direction) noexcept
        : origin(std::move(origin)), direction(std::move(direction)) {};

    glm::vec3 at(float t) const {
        return origin + t * direction;
    }

    glm::vec3 origin;
    glm::vec3 direction;
};

} // namespace hls
