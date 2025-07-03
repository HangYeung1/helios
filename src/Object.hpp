#pragma once

#include "Sampler.hpp"
#include "geometry/Geometry.hpp"
#include "material/Material.hpp"

#include <memory>
#include <variant>

namespace hls {

/**
 * @brief A type erased object in a scene with some geometry and material.
 */
class Object {
  public:
    /**
     * @brief Construct an `Object` with the given geometry and material.
     */
    Object(Geometry                       &&geometry,
           const std::shared_ptr<Material> &material) noexcept
        : geometry(std::move(geometry)), material(material) {};

    /**
     * @brief Determine the intersection normal `Ray`.
     * @details See the `Geometry` interface.
     */
    inline std::optional<Ray> hit(const Ray &ray) const {
        return std::visit(
            [&](const auto &g) {
                return g.hit(ray);
            },
            geometry);
    }

    /**
     * @brief Sample a point on the surface of the object.
     * @details See the `Geometry` interface.
     */
    inline glm::vec3 sample_point(Sampler &sampler) const {
        return std::visit(
            [&](const auto &g) {
                return g.sample_point(sampler);
            },
            geometry);
    }

    /**
     * @brief Scatter a ray at a surface normal.
     * @details See the `Material` interface.
     */
    inline BounceType scatter(Ray       &incident,
                              const Ray &normal,
                              glm::vec3 &radiance,
                              glm::vec3 &throughput,
                              Sampler   &sampler) const {
        return std::visit(
            [&](const auto &m) {
                return m.scatter(
                    incident, normal, radiance, throughput, sampler);
            },
            *material);
    }

    /**
     * @brief Get the bounce type of the material.
     * @details See the `Material` interface.
     */
    inline BounceType bounce_type() const {
        return std::visit(
            [&](const auto &m) {
                return m.bounce_type();
            },
            *material);
    }

  private:
    Geometry                  geometry;
    std::shared_ptr<Material> material;
};

} // namespace hls
