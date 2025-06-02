#pragma once

#include "Ray.hpp"
#include "Scene.hpp"

#include <generator>
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

    Camera(Config &&config)                        //
        : max_bounces(config.max_bounces),         //
          pixel_samples(config.pixel_samples),     //
          image_width(config.image_width),         //
          image_height(config.image_height),       //
          viewport_width(config.viewport_width),   //
          viewport_height(config.viewport_height), //
          focal_length(config.focal_length),       //
          position(config.position),               //
          target(config.target),                   //
          up(config.up) {};                        //

    void render(const Scene &scene) const;

  private:
    std::generator<std::generator<Ray>> pixel_rays() const;

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
