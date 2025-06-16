#include "Camera.hpp"

#include "Ray.hpp"
#include "Sampler.hpp"
#include "Scene.hpp"

#include <glm/vec3.hpp>
#include <print>
#include <ranges>
#include <thread>
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

    // Create render patches
    auto chunk = [](unsigned int max, unsigned int chunk_size) {
        return std::views::iota(0u, (max + chunk_size - 1) / chunk_size)
               | std::views::transform([=](std::size_t i) {
                     unsigned int begin = i * chunk_size;
                     unsigned int end   = std::min(begin + chunk_size, max);
                     return std::views::iota(begin, end);
                 });
    };
    auto x_chunks = chunk(image_width, 256);
    auto y_chunks = chunk(image_height, 256);

    auto cartesian_product = [](const auto &a, const auto &b) {
        return b | std::views::transform([=](const auto &b_item) {
                   return a | std::views::transform([=](const auto &a_item) {
                              return std::make_pair(a_item, b_item);
                          });
               })
               | std::views::join;
    };
    auto patches = cartesian_product(x_chunks, y_chunks)
                   | std::views::transform([=](const auto &pair) {
                         auto [x_chunk, y_chunk] = pair;
                         return cartesian_product(x_chunk, y_chunk);
                     });

    // Render patch per thread
    std::vector<glm::vec3>   pixels(image_width * image_height);
    std::vector<std::thread> threads;
    for (auto patch : patches) {
        threads.emplace_back([&, this, patch = std::move(patch)]() mutable {
            for (auto [x, y] : patch) {
                glm::vec3 accumulated(0);
                glm::vec3 basis = matrix_origin
                                  + static_cast<float>(x) * delta_u
                                  + static_cast<float>(y) * delta_v;

                for (auto _ : std::views::iota(0u, pixel_samples)) {
                    auto [r1, r2]  = Sampler::sample<2>();
                    glm::vec3 cast = basis + r1 * delta_u + r2 * delta_v;
                    Ray       ray(position, glm::normalize(cast - position));

                    accumulated += trace(ray, scene);
                    Sampler::step();
                }

                accumulated /= static_cast<float>(pixel_samples);
                pixels[y * image_width + x] += accumulated;
            }
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    output(pixels);
}

inline glm::vec3 hls::Camera::trace(Ray &ray, const Scene &scene) const {
    glm::vec3 radiance(0.0f, 0.0f, 0.0f);
    glm::vec3 throughput(1.0f, 1.0f, 1.0f);

    for (auto _ : std::views::iota(0u, max_bounces)) {
        bool success = scene.interact(ray, radiance, throughput);
        if (!success) {
            break;
        }
    }

    return radiance;
}

void hls::Camera::output(const std::vector<glm::vec3> &pixels) const {
    std::print("P3 {} {} 255 ", image_width, image_height);
    for (const glm::vec3 &pixel : pixels) {
        // Reinhard tonemapping
        glm::vec3 tone_mapped = pixel / (pixel + 1.0f);

        // Gamma 2.2 corerction
        glm::vec3 gamma_corrected =
            glm::pow(tone_mapped, glm::vec3(1.0f / 2.2f));

        // Quantize
        glm::uvec3 rgb = glm::round(gamma_corrected * 255.00f);
        std::print("{} {} {} ", rgb[0], rgb[1], rgb[2]);
    }
}
