#pragma once

#include "Sampler.hpp"
#include "Scene.hpp"

#include <glm/vec3.hpp>

namespace hls {

/**
 * @brief A camera that renders a scene.
 */
class Camera {
  public:
    /**
     * @brief Configuration POD for the camera.
     */
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

    /**
     * @brief Construct a camera with the given configuration.
     */
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

    /**
     * @brief Render the scene from the camera's perspective.
     * @param scene the scene to render.
     */
    void render(const Scene &scene) const;

  private:
    /**
     * @brief Trace a ray through the scene.
     * @param scene the scene to trace the ray in
     * @param ray the ray to trace.
     * @param sampler the sampler to use for random sampling.
     * @return the radiance of the pixel.
     */
    inline glm::vec3 trace_ray(const hls::Scene &scene,
                               Ray              &ray,
                               Sampler          &sampler) const;

    /**
     * @brief Write the rendered image to a PPM P6 file.
     * @param pixels the pixel data.
     */
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
