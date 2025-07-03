#pragma once

#include "Object.hpp"
#include "Ray.hpp"

#include <glm/vec3.hpp>
#include <optional>
#include <utility>
#include <vector>

namespace hls {

/**
 * @brief A scene containing objects that can be rendered.
 */
class Scene {
  public:
    /**
     * @brief Add an object to the scene.
     * @param args the arguments to construct the object.
     */
    template <typename... Args>
    inline void add(Args &&...args) {
        objects.emplace_back(std::forward<Args>(args)...);
        if (objects.back().bounce_type() == BounceType::Emissive) {
            lights.push_back(objects.size() - 1);
        }
    }

    /**
     * @brief Bounce a ray in the scene.
     * @param ray the ray to bounce.
     * @param radiance the radiance of the ray.
     * @param throughput the throughput of the ray.
     * @param sampler the sampler to use for random sampling.
     * @return the type of bounce that occurred.
     */
    BounceType bounce(Ray       &ray,
                      glm::vec3 &radiance,
                      glm::vec3 &throughput,
                      Sampler   &sampler) const {
        std::optional<Intersection> intersection = intersect(ray);
        if (!intersection) {
            return BounceType::None;
        }

        const auto &[object, normal] = *intersection;
        return object->scatter(ray, normal, radiance, throughput, sampler);
    }

    /**
     * @brief Get next event estimation for indirect light sampling.
     * @param incident the incident ray to estimate from.
     * @param radiance the radiance of the ray.
     * @param throughput the throughput of the ray.
     * @param sampler the sampler to use for random sampling.
     * @return the sampled indirect light or std::nullopt.
     */
    std::optional<glm::vec3> sample_indirect(const Ray &incident,
                                             glm::vec3  radiance,
                                             glm::vec3  throughput,
                                             Sampler   &sampler) const {
        // Get a random point on a random light source
        float       rand  = sampler.sample<1>();
        std::size_t index = std::min(
            static_cast<std::size_t>(rand * lights.size()), lights.size() - 1);
        const Object &light = objects[lights[index]];

        glm::vec3 point = light.sample_point(sampler);
        Ray       ray(incident.origin, glm::normalize(point - incident.origin));
        if (glm::dot(ray.direction, incident.direction) < 0.0f) {
            return std::nullopt;
        }

        // Check if the ray intersects the light source
        auto intersection = intersect(ray);
        if (!intersection || intersection->object != &light) {
            return std::nullopt;
        }

        // Calculate the radiance contribution from the light source
        const auto &[object, normal] = *intersection;
        object->scatter(ray, normal, radiance, throughput, sampler);

        glm::vec3 light_dir      = point - incident.origin;
        glm::vec3 light_dir_norm = glm::normalize(light_dir);

        float cos_theta_i   = glm::dot(incident.direction, light_dir_norm);
        float cos_theta_l   = glm::dot(normal.direction, -light_dir_norm);
        float dist_squared  = glm::dot(light_dir, light_dir);
        float geometry_term = (cos_theta_i * cos_theta_l) / dist_squared;

        return radiance * geometry_term;
    }

  private:
    /**
     * @brief A structure representing an intersection of a ray with an object.
     */
    struct Intersection {
        const Object *object;
        Ray           normal;
    };

    /**
     * @brief Find the closest intersection of a ray with the scene objects.
     * @param incident The ray to test for intersection.
     * @return the closest intersection or std::nullopt.
     */
    std::optional<Intersection> intersect(const Ray &incident) const {
        std::optional<Intersection> closest;
        float closest_dist = std::numeric_limits<float>::max();

        for (const auto &object : objects) {
            std::optional<Ray> normal = object.hit(incident);
            if (normal) {
                float dist = glm::length(normal->origin - incident.origin);
                if (dist < closest_dist) {
                    closest      = Intersection{.object = &object,
                                                .normal = std::move(*normal)};
                    closest_dist = dist;
                }
            }
        }

        return closest;
    }

    std::vector<Object>      objects;
    std::vector<std::size_t> lights;
};

} // namespace hls
