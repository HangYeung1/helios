#include "Camera.hpp"

#include "Ray.hpp"
#include "Sampler.hpp"
#include "Scene.hpp"

#include <execution>
#include <glm/vec3.hpp>
#include <numeric>
#include <print>
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

    std::vector<std::size_t> indicies(image_width * image_height);
    std::iota(indicies.begin(), indicies.end(), 0);

    std::vector<glm::vec3> pixels(image_width * image_height);
    std::transform(std::execution::par_unseq,
                   indicies.begin(),
                   indicies.end(),
                   pixels.begin(),
                   [&, this](std::size_t index) {
                       Sampler sampler(index * pixel_samples);

                       std::size_t x = index % image_width;
                       std::size_t y = index / image_width;

                       glm::vec3 accumulated(0);
                       glm::vec3 basis = matrix_origin
                                         + static_cast<float>(x) * delta_u
                                         + static_cast<float>(y) * delta_v;

                       for (std::size_t _ = 0; _ < pixel_samples; ++_) {
                           auto [r1, r2]  = sampler.sample<2>();
                           glm::vec3 cast = basis + r1 * delta_u + r2 * delta_v;
                           Ray ray(position, glm::normalize(cast - position));

                           glm::vec3 radiance(0.0f, 0.0f, 0.0f);
                           glm::vec3 throughput(1.0f, 1.0f, 1.0f);

                           for (std::size_t _ = 0; _ < max_bounces; ++_) {
                               bool success = scene.interact(
                                   ray, radiance, throughput, sampler);
                               if (!success) {
                                   break;
                               }
                           }

                           accumulated += radiance;
                           sampler.step();
                       }

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

        // Gamma 2.2 corerction
        glm::vec3 gamma_corrected =
            glm::pow(tone_mapped, glm::vec3(1.0f / 2.2f));

        // Quantize
        glm::uvec3 rgb = glm::round(gamma_corrected * 255.00f);
        std::print("{} {} {} ", rgb[0], rgb[1], rgb[2]);
    }
}
