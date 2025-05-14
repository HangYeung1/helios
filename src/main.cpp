#include "Camera.hpp"
#include "Scene.hpp"
#include "Sphere.hpp"

#include <glm/vec3.hpp>

int main() {
    hls::Scene scene;
    scene.add(hls::Sphere(glm::vec3(0, 0, 10), 10));

    hls::Camera camera({.image_width     = 100,
                        .image_height    = 100,
                        .viewport_width  = 10,
                        .viewport_height = 10,
                        .focal_length    = 1.2,
                        .position        = glm::vec3(0, 0, 0),
                        .target          = glm::vec3(0, 0, 10),
                        .up              = glm::vec3(0, 1, 0)});
    camera.render(scene);
}
