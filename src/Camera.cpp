#include "Camera.hpp"

#include "Ray.hpp"
#include "Scene.hpp"

#include <cmath>
#include <cstdlib>
#include <generator>
#include <glm/vec3.hpp>
#include <print>
#include <vector>

void hls::Camera::render(const Scene &scene) const {
    std::vector<glm::vec3> image;
    image.reserve(image_width * image_height);

    // Rendering equation
    for (const Ray ray : rays()) {
        glm::vec3 color(0, 0, 0);

        for (unsigned int i = 0; i < subsamples; ++i) {
            Ray       sample_ray = ray;
            glm::vec3 sample_color(0, 0, 0);
            float     throughput = 1;

            for (unsigned int j = 0; j < max_bounces; ++j) {
                std::optional<Sphere::Hit> hit = scene.hit(sample_ray);

                // Ambiance
                if (!hit) {
                    float     t = 0.5f * (ray.direction.y + 1.0f);
                    glm::vec3 sky =
                        (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) // white
                        + t * glm::vec3(0.5f, 0.7f, 1.0f);       // blue
                    sample_color += throughput * sky;
                    break;
                }

                // Attentuation/contribution
                const glm::vec3 surface_color(1, 0, 0);
                sample_color += throughput * (hit->normal * 0.5f + 0.5f);

                const float attentuation = 0.3;
                throughput *= attentuation;

                // Naive diffuse
                float r1 = static_cast<float>(std::rand()) / RAND_MAX;
                float r2 = static_cast<float>(std::rand()) / RAND_MAX;

                float     phi   = r1 * 2 * 3.14;
                float     theta = r2 * 3.14;
                glm::vec3 direction(std::sin(theta) * std::cos(phi), //
                                    std::sin(theta) * std::sin(phi), //
                                    std::cos(theta));

                if (glm::dot(direction, hit->normal) < 0) {
                    direction = -direction;
                }

                glm::vec3 eps_point = hit->point + 0.001f * hit->normal;
                sample_ray          = Ray(eps_point, direction);
            }

            color += sample_color;
        }

        color /= subsamples;
        image.push_back(color);
    }

    std::print("P3 {} {} 255 ", image_height, image_width);
    for (const glm::vec3 &pixel : image) {
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
