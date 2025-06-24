#pragma once

#include "Constants.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <ranges>
#include <stdexcept>

namespace hls {

/**
 * @brief Scrambled Sobol sequence sampler for randomized quasi-Monte Carlo.
 *
 * `Sampler` implements a Sobol sequence with fast Owen scrambling as described
 * in https://jcgt.org/published/0009/04/01/paper.pdf. It is static for
 * consistent Sobol dimension mapping and thread-local for concurrency.
 */
class Sampler {
  public:
    template <std::size_t N>
        requires(N >= 1 && N <= constants::SOBOL_MAX_DIM)
    inline auto sample() {
        if (dimension + N > constants::SOBOL_MAX_DIM) [[unlikely]] {
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
            bits   = scramble(bits);
            sample = static_cast<float>(bits) / std::pow(2.0f, 32.0f);
            ++dimension;
        }

        if constexpr (N == 1) {
            return result[0];
        } else {
            return result;
        }
    }

    inline void step() {
        ++index;
        dimension = 0;
    }

  private:
    static inline uint32_t lk_hash(uint32_t x) {
        x += seed;
        x ^= x * 0x6c50b47cu;
        x ^= x * 0xb82f1e52u;
        x ^= x * 0xc7afe638u;
        x ^= x * 0x8d22f6e6u;
        return x;
    }

    static inline uint32_t reverse_bits(uint32_t x) {
        x = ((x >> 1) & 0x55555555) | ((x << 1) & 0xAAAAAAAA);
        x = ((x >> 2) & 0x33333333) | ((x << 2) & 0xCCCCCCCC);
        x = ((x >> 4) & 0x0F0F0F0F) | ((x << 4) & 0xF0F0F0F0);
        x = ((x >> 8) & 0x00FF00FF) | ((x << 8) & 0xFF00FF00);
        x = (x >> 16) | (x << 16);
        return x;
    }

    static inline uint32_t scramble(uint32_t x) {
        x = reverse_bits(x);
        x = lk_hash(x);
        x = reverse_bits(x);
        return x;
    }

    static constexpr uint32_t seed      = 42;
    std::size_t               index     = 0;
    std::size_t               dimension = 0;
};

} // namespace hls