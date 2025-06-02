#include "Camera.hpp"

#include "Ray.hpp"
#include "Scene.hpp"

#include <cmath>
#include <cstdlib>
#include <generator>
#include <glm/vec3.hpp>
#include <numbers>
#include <print>
#include <ranges>
#include <vector>

void hls::Camera::render(const Scene &scene) const {
    std::vector<glm::vec3> pixels(image_width * image_height);

    // Rendering equation
    for (auto &&[pixel, rays] : std::views::zip(pixels, pixel_rays())) {
        for (auto &&ray : rays) {
            glm::vec3 radiance(0.0f, 0.0f, 0.0f);
            glm::vec3 throughput(1.0f, 1.0f, 1.0f);

            for (auto _ : std::views::iota(0u, max_bounces)) {
                std::optional<Sphere::Hit> hit = scene.hit(ray);

                // Ambient Light
                if (!hit) {
                    float     t   = 0.5f * (ray.direction.y + 1.0f);
                    glm::vec3 sky =                              //
                        (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) //
                        + t * glm::vec3(0.5f, 0.7f, 1.0f);       //
                    radiance += throughput * sky;
                    break;
                }

                // Surface Albedo
                const glm::vec3 albedo(0.7f, 0.7f, 0.7f);
                throughput *= albedo;

                // Naive diffuse
                float r1 = static_cast<float>(std::rand()) / RAND_MAX;
                float r2 = static_cast<float>(std::rand()) / RAND_MAX;

                float     phi   = r1 * 2.0f * std::numbers::pi_v<float>;
                float     theta = r2 * std::numbers::pi_v<float>;
                glm::vec3 direction(std::sin(theta) * std::cos(phi), //
                                    std::sin(theta) * std::sin(phi), //
                                    std::cos(theta));                //

                if (glm::dot(direction, hit->normal) <= 0.0f) {
                    direction = -direction;
                }

                glm::vec3 eps_point = hit->point + 0.001f * hit->normal;
                ray                 = Ray(eps_point, direction);
            }

            pixel += radiance;
        }

        pixel /= pixel_samples;
    }

    // Output PPM
    std::print("P3 {} {} 255 ", image_width, image_height);
    for (const glm::vec3 &pixel : pixels) {
        std::print("{} {} {} ", //
                   static_cast<unsigned int>(pixel[0] * 255),
                   static_cast<unsigned int>(pixel[1] * 255),
                   static_cast<unsigned int>(pixel[2] * 255));
    }
}

std::generator<std::generator<hls::Ray>> hls::Camera::pixel_rays() const {
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right   = glm::normalize(glm::cross(forward, up));
    glm::vec3 true_up = glm::cross(right, forward);

    float pixel_width  = viewport_width / image_width;
    float pixel_height = viewport_height / image_height;

    glm::vec3 delta_u = right * pixel_width;
    glm::vec3 delta_v = -true_up * pixel_height;

    glm::vec3 base_pixel =                                   //
        position + forward * focal_length                    //
        - right * (viewport_width - pixel_width) * 0.5f      //
        + true_up * (viewport_height - pixel_height) * 0.5f; //

    for (auto y : std::views::iota(0u, image_height)) {
        for (auto x : std::views::iota(0u, image_width)) {
            glm::vec3 pixel = base_pixel                         //
                              + static_cast<float>(x) * delta_u  //
                              + static_cast<float>(y) * delta_v; //

            // Randomized subsampler
            co_yield [&]() -> std::generator<Ray> {
                for (auto _ : std::views::iota(0u, pixel_samples)) {
                    float r1 = static_cast<float>(std::rand()) / RAND_MAX;
                    float r2 = static_cast<float>(std::rand()) / RAND_MAX;

                    glm::vec3 jittered = pixel                    //
                                         + (r1 - 0.5f) * delta_u  //
                                         + (r2 - 0.5f) * delta_v; //
                    glm::vec3 direction = glm::normalize(jittered - position);
                    co_yield Ray(position, direction);
                }
            }();
        }
    }
}