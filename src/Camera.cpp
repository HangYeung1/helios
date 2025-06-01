#include "Camera.hpp"

#include "Ray.hpp"
#include "Scene.hpp"

#include <cstdlib>
#include <generator>
#include <glm/vec3.hpp>
#include <print>
#include <vector>

void hls::Camera::render(const Scene &scene) const {
    std::vector<glm::vec3> image;
    image.reserve(image_width * image_height);

    // Rendering equation
    for (auto ray : rays()) {
        glm::vec3 color(0, 0, 0);

        for (unsigned int i = 0; i < subsamples; ++i) {
            glm::vec3 sample_color(0, 0, 0);
            float     throughput = 1;

            for (unsigned int j = 0; j < max_bounces; ++j) {
                std::optional<Sphere::Intersection> hit_point = scene.hit(ray);

                // Ambiance
                if (!hit_point) {
                    const glm::vec3 background_color(0.8, 0.8, 0.8);
                    sample_color += throughput * background_color;
                    break;
                }

                // Attentuation/contribution
                const glm::vec3 surface_color(1, 0, 0);
                sample_color += throughput * surface_color;

                const float attentuation = 0.8;
                throughput *= attentuation;

                // Naive diffuse
                float r1 = static_cast<float>(rand()) / RAND_MAX;
                float r2 = static_cast<float>(rand()) / RAND_MAX;

                float     phi   = r1 * 2 * 3.14;
                float     theta = r2 * 3.14;
                glm::vec3 direction{sin(theta) * cos(phi),
                                    sin(theta) * sin(phi), cos(theta)};

                if (glm::dot(direction, hit_point->normal) < 0) {
                    direction = -direction;
                }

                glm::vec3 eps_point =
                    hit_point->point + (float)0.01 * hit_point->normal;
                ray = Ray{eps_point, direction};
            }

            color += sample_color;
        }

        color /= subsamples;
        image.push_back(color);
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
    glm::vec3 true_up = glm::cross(right, forward);

    float pixel_width  = viewport_width / image_width;
    float pixel_height = viewport_height / image_height;

    glm::vec3 delta_u = right * pixel_width;
    glm::vec3 delta_v = -true_up * pixel_height;

    glm::vec3 base_pixel =
        position + forward * focal_length                    // center
        - right * (viewport_width - pixel_width) * 0.5f      // justify left
        + true_up * (viewport_height - pixel_height) * 0.5f; // justify top

    for (unsigned int y = 0; y < image_height; ++y) {
        for (unsigned int x = 0; x < image_width; ++x) {
            glm::vec3 pixel = base_pixel                         //
                               + static_cast<float>(x) * delta_u  //
                               + static_cast<float>(y) * delta_v; //
            glm::vec3 direction = glm::normalize(pixel - position);
            co_yield Ray(position, direction);
        }
    }
}
