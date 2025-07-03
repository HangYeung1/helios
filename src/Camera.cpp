#include "Camera.hpp"

#include "Ray.hpp"
#include "Sampler.hpp"
#include "Scene.hpp"

#include <execution>
#include <fstream>
#include <glm/vec3.hpp>
#include <numeric>
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

    // Render each pixel
    auto pixel_indicies = std::views::iota(0u, image_width * image_height);
    std::vector<glm::u8vec3> pixels(image_width * image_height);
    std::transform(
        std::execution::par_unseq,
        pixel_indicies.begin(),
        pixel_indicies.end(),
        pixels.begin(),
        [&, this](unsigned int pixel_index) {
            unsigned int x     = pixel_index % image_width;
            unsigned int y     = pixel_index / image_width;
            glm::vec3    basis = matrix_origin + static_cast<float>(x) * delta_u
                              + static_cast<float>(y) * delta_v;

            // Sub-sample the pixel
            auto      sample_indicies = std::views::iota(0u, pixel_samples);
            glm::vec3 accumulated     = std::transform_reduce(
                std::execution::par_unseq,
                sample_indicies.begin(),
                sample_indicies.end(),
                glm::vec3(0.0f),
                std::plus<glm::vec3>(),
                [&, this](unsigned int sample_index) {
                    Sampler sampler(pixel_index * pixel_samples + sample_index);
                    auto [r1, r2]  = sampler.sample<2>();
                    glm::vec3 cast = basis + r1 * delta_u + r2 * delta_v;
                    Ray       ray(position, glm::normalize(cast - position));

                    glm::vec3 radiance(0.0f);
                    glm::vec3 throughput(1.0f);

                    glm::vec3    total_radiance(0.0f);
                    unsigned int radiance_samples = 1;

                    for (unsigned int _ = 0; _ < max_bounces; ++_) {
                        BounceType bounce_type =
                            scene.bounce(ray, radiance, throughput, sampler);

                        switch (bounce_type) {
                        case BounceType::Diffuse:
                        case BounceType::Refactive: {
                            std::optional<glm::vec3> indirect =
                                scene.sample_indirect(
                                    ray, radiance, throughput, sampler);
                            if (indirect) {
                                total_radiance += *indirect;
                                ++radiance_samples;
                            }
                            break;
                        }
                        case BounceType::Specular:
                            break;
                        case BounceType::Emissive:
                            total_radiance += radiance;
                            return total_radiance
                                   / static_cast<float>(radiance_samples);
                        }
                    }

                    total_radiance += radiance;
                    return total_radiance
                           / static_cast<float>(radiance_samples);
                });

            glm::vec3   norm  = accumulated / static_cast<float>(pixel_samples);
            glm::vec3   map   = norm / (norm + 1.0f);
            glm::vec3   gamma = glm::pow(map, glm::vec3(1.0f / 2.2f));
            glm::u8vec3 quant = glm::round(gamma * 255.00f);

            return quant;
        });

    output(pixels);
}

void hls::Camera::output(const std::vector<glm::u8vec3> &pixels) const {
    std::ofstream file("image.ppm", std::ios::binary);
    file << "P6 " << image_width << " " << image_height << " 255\n";
    file.write(reinterpret_cast<const char *>(pixels.data()),
               pixels.size() * sizeof(glm::u8vec3));
}
