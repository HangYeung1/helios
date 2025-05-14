#include "Camera.hpp"

#include "Ray.hpp"
#include "Scene.hpp"

#include <generator>
#include <print>
#include <vector>

void hls::Camera::render(const Scene &scene) const {
    std::vector<glm::vec3> image;
    image.reserve(image_width * image_height);
    for (const auto &ray : rays()) {
        image.emplace_back(static_cast<float>(scene.hit(ray)), 0, 0);
    }

    std::print("P3 {} {} 255 ", image_height, image_width);
    for (const auto &pixel : image) {
        std::print("{} {} {} ", //
                   static_cast<unsigned int>(pixel[0] * 255),
                   static_cast<unsigned int>(pixel[1] * 255),
                   static_cast<unsigned int>(pixel[2] * 255));
    }
}

std::generator<hls::Ray> hls::Camera::rays() const {
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right   = glm::normalize(glm::cross(forward, up));
    glm::vec3 up      = glm::cross(right, forward);

    float pixel_width  = viewport_width / image_width;
    float pixel_height = viewport_height / image_height;

    glm::vec3 delta_u = right * pixel_width;
    glm::vec3 delta_v = -up * pixel_height;

    glm::vec3 base_pixel =
        position + forward * focal_length                // center
        + right * (-viewport_width + pixel_width) * 0.5f // justify left
        + up * (viewport_height + pixel_height) * 0.5f;  // justify top

    for (unsigned int y = 0; y < image_height; ++y) {
        for (unsigned int x = 0; x < image_width; ++x) {
            glm::vec3 origin = base_pixel                         //
                               + static_cast<float>(x) * delta_u  //
                               + static_cast<float>(y) * delta_v; //
            glm::vec3 direction = glm::normalize(origin - position);
            co_yield Ray(origin, direction);
        }
    }
}
