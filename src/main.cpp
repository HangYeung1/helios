#include "Camera.hpp"
#include "Scene.hpp"

#include <glm/vec3.hpp>
#include <memory>

int main() {
    auto light_gray = std::make_shared<hls::Material>(
        hls::NaiveDiffuse(glm::vec3(0.9f, 0.9f, 0.9f)));
    auto dark_gray = std::make_shared<hls::Material>(
        hls::NaiveDiffuse(glm::vec3(0.6f, 0.6f, 0.6f)));

    hls::Scene scene;
    scene.add(hls::Sphere(glm::vec3(0.0f, 16.0f, 0.0f), 16.0f), light_gray);
    scene.add(hls::Sphere(glm::vec3(0.0f, -1e4f, 0.0f), -1e4f), dark_gray);

    hls::Camera camera({.max_bounces     = 16u,
                        .pixel_samples   = 32u,
                        .image_width     = 256u,
                        .image_height    = 256u,
                        .viewport_width  = 4.0f,
                        .viewport_height = 4.0f,
                        .focal_length    = 4.0f,
                        .position        = glm::vec3(0.0f, 16.0f, -64.0f),
                        .target          = glm::vec3(0.0f, 16.0f, 0.0f),
                        .up              = glm::vec3(0.0f, 1.0f, 0.0f)});
    camera.render(scene);
}
