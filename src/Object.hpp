#pragma once

#include "geometry/Geometry.hpp"
#include "material/Material.hpp"

#include <memory>
#include <variant>

namespace hls {

/**
 * @brief Object in a scene with some geometry and material.
 *
 * `Object` is a type agonistic wrapper describing a geometric shape and its
 * associated material. It provides an interface for fast polymorphism.
 */
class Object {
  public:
    Object(Geometry                       &&geometry,
           const std::shared_ptr<Material> &material) noexcept
        : geometry(std::move(geometry)), material(material) {};

    inline std::optional<Ray> hit(const Ray &ray) const {
        return std::visit(
            [&](const auto &g) {
                return g.hit(ray);
            },
            geometry);
    }

    [[nodiscard]] inline bool scatter(Ray       &incident,
                                      const Ray &normal,
                                      glm::vec3 &radiance,
                                      glm::vec3 &throughput) const {
        return std::visit(
            [&](const auto &m) {
                return m.scatter(incident, normal, radiance, throughput);
            },
            *material);
    }

  private:
    Geometry                  geometry;
    std::shared_ptr<Material> material;
};

} // namespace hls
