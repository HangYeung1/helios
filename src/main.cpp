#include "Camera.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"

#include <glm/vec3.hpp>

int main() {
    hls::Scene scene;
    scene.add(hls::Sphere(glm::vec3(0, 10, 0), 10));
    scene.add(hls::Sphere(glm::vec3(0, -10000, 0), 10000));

    hls::Camera camera({.max_bounces     = 2,
                        .pixel_samples   = 8,
                        .image_width     = 250,
                        .image_height    = 250,
                        .viewport_width  = 5,
                        .viewport_height = 5,
                        .focal_length    = 1,
                        .position        = glm::vec3(0, 10, -15),
                        .target          = glm::vec3(0, 10, 0),
                        .up              = glm::vec3(0, 1, 0)});
    camera.render(scene);
}
