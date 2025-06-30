#pragma once

#include "../Ray.hpp"
#include "../Sampler.hpp"
#include "BounceType.hpp"

namespace hls {

class PureEmissive {
  public:
    explicit PureEmissive(glm::vec3 &&luminance) noexcept
        : luminance(std::move(luminance)) {};

    inline BounceType scatter([[maybe_unused]] Ray       &incident,
                              [[maybe_unused]] const Ray &normal,
                              glm::vec3                  &radiance,
                              glm::vec3                  &throughput,
                              [[maybe_unused]] Sampler   &sampler) const {
        radiance = luminance * throughput;
        return bounce_type();
    }

    constexpr inline BounceType bounce_type() const {
        return BounceType::Emissive;
    }

  private:
    glm::vec3 luminance;
};

} // namespace hls
