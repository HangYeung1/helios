#include "Camera.hpp"

#include "Ray.hpp"
#include "Sampler.hpp"
#include "Scene.hpp"

#include <execution>
#include <glm/vec3.hpp>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

void hls::Camera::render(const Scene &scene) const {
    // Calculate camera plane
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right   = glm::normalize(glm::cross(forward, up));
    glm::vec3 true_up = glm::cross(right, forward);

    float pixel_width  = viewport_width / static_cast<float>(image_width);
    float pixel_height = viewport_height / static_cast<float>(image_height);

    glm::vec3 delta_u       = right * pixel_width;
    glm::vec3 delta_v       = -true_up * pixel_height;
    glm::vec3 matrix_origin = position + forward * focal_length
                              - right * viewport_width * 0.5f
                              + true_up * viewport_height * 0.5f;

    // Render
    auto pixel_indicies = std::views::iota(0u, image_width * image_height);
    std::vector<glm::vec3> pixels(image_width * image_height);
    std::transform(
        std::execution::par_unseq,
        pixel_indicies.begin(),
        pixel_indicies.end(),
        pixels.begin(),
        [&, this](std::size_t pixel_index) {
            std::size_t x = pixel_index % image_width;
            std::size_t y = pixel_index / image_width;

            glm::vec3 basis = matrix_origin + static_cast<float>(x) * delta_u
                              + static_cast<float>(y) * delta_v;

            auto      sample_indicies = std::views::iota(0u, pixel_samples);
            glm::vec3 accumulated     = std::transform_reduce(
                std::execution::par_unseq,
                sample_indicies.begin(),
                sample_indicies.end(),
                glm::vec3(0.0f),
                std::plus<glm::vec3>(),
                [&, this](std::size_t sample_index) {
                    Sampler sampler(pixel_index * pixel_samples + sample_index);
                    auto [r1, r2]  = sampler.sample<2>();
                    glm::vec3 cast = basis + r1 * delta_u + r2 * delta_v;
                    Ray       ray(position, glm::normalize(cast - position));

                    glm::vec3 radiance(0.0f);
                    glm::vec3 throughput(1.0f);

                    for (std::size_t _ = 0; _ < max_bounces; ++_) {
                        bool success =
                            scene.interact(ray, radiance, throughput, sampler);
                        if (!success) {
                            break;
                        }
                    }

                    return radiance;
                });

            accumulated /= static_cast<float>(pixel_samples);
            return accumulated;
        });

    output(pixels);
}

void hls::Camera::output(const std::vector<glm::vec3> &pixels) const {
    std::print("P3 {} {} 255 ", image_width, image_height);
    for (const glm::vec3 &pixel : pixels) {
        // Reinhard tonemapping
        glm::vec3 tone_mapped = pixel / (pixel + 1.0f);

        // Gamma 2.2 correction
        glm::vec3 gamma_corrected =
            glm::pow(tone_mapped, glm::vec3(1.0f / 2.2f));

        // Quantize
        glm::uvec3 rgb = glm::round(gamma_corrected * 255.00f);
        std::print("{} {} {} ", rgb[0], rgb[1], rgb[2]);
    }
}
