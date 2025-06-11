#pragma once

#include "Constants.hpp"

#include <array>
#include <cmath>
#include <ranges>
#include <stdexcept>

namespace hls {

class Sampler {
  public:
    template <std::size_t N>
        requires(N >= 1 && N <= constants::SOBOL_MAX_DIM)
    static std::conditional_t<N == 1, float, std::array<float, N>> sample() {
        if (dimension + N > constants::SOBOL_MAX_DIM) {
            throw std::out_of_range("out of sobol dimensions");
        }

        std::array<float, N> result;
        for (auto &sample : result) {
            uint32_t bits = 0;
            for (auto i : std::views::iota(0u, constants::SOBOL_MAX_BITS)) {
                if ((index >> i) & 1) {
                    bits ^= constants::SOBOL_DIRECTIONS[dimension][i];
                }
            }
            sample = static_cast<float>(bits) / std::pow(2.0f, 32.0f);
            ++dimension;
        }

        if constexpr (N == 1) {
            return result[0];
        } else {
            return result;
        }
    }

    static void step() {
        ++index;
        dimension = 0;
    }

  private:
    static inline std::size_t index     = 0;
    static inline std::size_t dimension = 0;
};

} // namespace hls