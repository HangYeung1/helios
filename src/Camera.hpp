#pragma once

#include "Scene.hpp"

#include <glm/vec3.hpp>

namespace hls {

class Camera {
  public:
    struct Config {
        unsigned int max_bounces;
        unsigned int pixel_samples;
        unsigned int image_width;
        unsigned int image_height;
        float        viewport_width;
        float        viewport_height;
        float        focal_length;
        glm::vec3    position;
        glm::vec3    target;
        glm::vec3    up;
    };

    Camera(Config &&config) noexcept
        : max_bounces(std::move(config.max_bounces)),
          pixel_samples(std::move(config.pixel_samples)),
          image_width(std::move(config.image_width)),
          image_height(std::move(config.image_height)),
          viewport_width(std::move(config.viewport_width)),
          viewport_height(std::move(config.viewport_height)),
          focal_length(std::move(config.focal_length)),
          position(std::move(config.position)),
          target(std::move(config.target)),
          up(std::move(config.up)) {};

    void render(const Scene &scene) const;

  private:
    void output(const std::vector<glm::u8vec3> &pixels) const;

    unsigned int max_bounces;
    unsigned int pixel_samples;
    unsigned int image_width;
    unsigned int image_height;
    float        viewport_width;
    float        viewport_height;
    float        focal_length;
    glm::vec3    position;
    glm::vec3    target;
    glm::vec3    up;
};

} // namespace hls
