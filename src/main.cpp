#include "Camera.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"

#include <glm/vec3.hpp>

int main() {
    hls::Scene scene;
    scene.add(hls::Sphere(glm::vec3(0, 10, 0), 10));
    scene.add(hls::Sphere(glm::vec3(0, -10000, 0), 10000));

    hls::Camera camera({.max_bounces     = 8,
                        .pixel_samples   = 16,
                        .image_width     = 256,
                        .image_height    = 256,
                        .viewport_width  = 4,
                        .viewport_height = 4,
                        .focal_length    = 1,
                        .position        = glm::vec3(0, 10, -15),
                        .target          = glm::vec3(0, 10, 0),
                        .up              = glm::vec3(0, 1, 0)});
    camera.render(scene);
}
