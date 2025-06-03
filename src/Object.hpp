#pragma once

#include "geometry/Geometry.hpp"
#include "material/Material.hpp"

#include <memory>
#include <variant>

namespace hls {

class Object {
  public:
    template <typename T, typename U>
    Object(T &&geometry, const U &material)
        : geometry(geometry), material(material){};

    std::optional<Ray> hit(const Ray &ray) const {
        return std::visit([&](const auto &g) { return g.hit(ray); }, geometry);
    }

    Ray scatter(const Ray &normal, glm::vec3 &radiance,
                glm::vec3 &throughput) const {
        return std::visit(
            [&](const auto &m) {
                return m.scatter(normal, radiance, throughput);
            },
            *material);
    }

  private:
    Geometry                  geometry;
    std::shared_ptr<Material> material;
};
} // namespace hls
