#pragma once

#include "Ray.hpp"
#include "Scene.hpp"

#include <generator>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace hls {

class Camera {
  public:
    struct Config {
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
        : image_width(config.image_width),         //
          image_height(config.image_height),       //
          viewport_width(config.viewport_width),   //
          viewport_height(config.viewport_height), //
          focal_length(config.focal_length),       //
          position(config.position),               //
          target(config.target),                   //
          up(config.up) {};                        //

    void render(const Scene &scene) const;

  private:
    std::generator<Ray> rays() const;

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
