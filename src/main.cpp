#include "Camera.hpp"
#include "Scene.hpp"

#include <glm/vec3.hpp>
#include <memory>

int main() {
    auto light_gray = std::make_shared<hls::Material>(
        hls::NaiveDiffuse(glm::vec3(0.8f, 0.8f, 0.8f)));
    auto dark_gray = std::make_shared<hls::Material>(
        hls::NaiveDiffuse(glm::vec3(0.3f, 0.3f, 0.3f)));
    auto white_light = std::make_shared<hls::Material>(
        hls::PureEmissive(glm::vec3(0.1f, 0.1f, 0.1f)));
    auto red_light = std::make_shared<hls::Material>(
        hls::PureEmissive(glm::vec3(2.0f, 0.0f, 0.0f)));
    auto blue_light = std::make_shared<hls::Material>(
        hls::PureEmissive(glm::vec3(0.0f, 0.0f, 2.0f)));

    hls::Scene scene;
    scene.add(hls::Sphere(glm::vec3(0.0f, 16.0f, 0.0f), 16.0f), light_gray);
    scene.add(hls::Sphere(glm::vec3(0.0f, -1e4f, 0.0f), 1e4f), dark_gray);
    scene.add(hls::Sphere(glm::vec3(0.0f, 250.0f, 0.0f), 50.0f), white_light);
    scene.add(hls::Sphere(glm::vec3(-20.0f, 4.0f, 3.0f), 4.0f), red_light);
    scene.add(hls::Sphere(glm::vec3(20.0f, 4.0f, 3.0f), 4.0f), blue_light);

    hls::Camera camera({.max_bounces     = 16u,
                        .pixel_samples   = 256u,
                        .image_width     = 1024u,
                        .image_height    = 1024u,
                        .viewport_width  = 4.0f,
                        .viewport_height = 4.0f,
                        .focal_length    = 4.0f,
                        .position        = glm::vec3(0.0f, 16.0f, 64.0f),
                        .target          = glm::vec3(0.0f, 16.0f, 0.0f),
                        .up              = glm::vec3(0.0f, 1.0f, 0.0f)});
    camera.render(scene);
}
