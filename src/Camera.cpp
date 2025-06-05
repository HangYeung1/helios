#include "Camera.hpp"

#include "Ray.hpp"
#include "Sampler.hpp"
#include "Scene.hpp"

#include <glm/vec3.hpp>
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

    glm::vec3 delta_u    = right * pixel_width;
    glm::vec3 delta_v    = -true_up * pixel_height;
    glm::vec3 base_pixel = position + forward * focal_length
                           - right * (viewport_width - pixel_width) * 0.5f
                           + true_up * (viewport_height - pixel_height) * 0.5f;

    // Get lazy range of pixel centers
    auto generate_product = [this](unsigned int y) {
        return std::views::zip(std::views::iota(0u, image_width),
                               std::views::repeat(y));
    };
    auto cartesian = std::views::iota(0u, image_height)
                     | std::views::transform(generate_product)
                     | std::views::join;

    auto generate_center =
        [&](const std::pair<unsigned int, unsigned int> &point) {
            auto &[x, y] = point;
            return base_pixel //
                   + static_cast<float>(x) * delta_u
                   + static_cast<float>(y) * delta_v;
        };
    auto centers = cartesian | std::views::transform(generate_center);

    // Render
    std::vector<glm::vec3> pixels(image_width * image_height);
    for (auto [pixel, center] : std::views::zip(pixels, centers)) {
        for (auto _ : std::views::iota(0u, pixel_samples)) {
            auto [r1, r2]      = Sampler<2>::rand();
            glm::vec3 jittered = center //
                                 + (r1 - 0.5f) * delta_u
                                 + (r2 - 0.5f) * delta_v;

            glm::vec3 direction = glm::normalize(jittered - position);
            Ray       ray(position, direction);

            pixel += trace(ray, scene) / static_cast<float>(pixel_samples);
        }
    }

    // Output PPM
    std::print("P3 {} {} 255 ", image_width, image_height);
    for (const glm::vec3 &pixel : pixels) {
        glm::vec<3, unsigned int> rgb = glm::clamp(pixel, 0.0f, 1.0f) * 255.99f;
        std::print("{} {} {} ", rgb[0], rgb[1], rgb[2]);
    }
}

glm::vec3 hls::Camera::trace(Ray &ray, const Scene &scene) const {
    glm::vec3 radiance(0.0f, 0.0f, 0.0f);
    glm::vec3 throughput(1.0f, 1.0f, 1.0f);

    for (auto _ : std::views::iota(0u, max_bounces)) {
        // Object scatter
        bool success = scene.interact(ray, radiance, throughput);

        // Ambient Light
        if (!success) {
            float     t   = 0.5f * (ray.direction.y + 1.0f);
            glm::vec3 sky = (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f)
                            + t * glm::vec3(0.5f, 0.7f, 1.0f);
            radiance += throughput * sky;
            break;
        }
    }

    return radiance;
}