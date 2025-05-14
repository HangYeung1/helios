#pragma once

#include "Ray.hpp"
#include "Sphere.hpp"
#include <vector>

namespace hls {

class Scene {
  public:
    void add(const Sphere &object) {
        objects.push_back(object);
    }

    bool hit(const Ray &ray) const {
        for (const auto &object : objects) {
            if (object.hit(ray)) {
                return true;
            }
        }
        return false;
    }

  private:
    std::vector<Sphere> objects;
};

} // namespace hls
