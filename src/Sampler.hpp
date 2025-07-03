#pragma once

#include "Constants.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace hls {

/**
 * @brief Scrambled Sobol sequence sampler for randomized quasi-Monte Carlo.
 * @details https://jcgt.org/published/0009/04/01/paper.pdf.
 */
class Sampler {
  public:
    /**
     * @brief Construct a sampler with the given Sobol index.
     */
    explicit Sampler(unsigned int index) noexcept
        : index(index), dimension(0) {};

    /**
     * @brief Get the next Sobol sample(s).
     * @tparam N the number of dimensions to sample.
     * @return the sampled value(s).
     */
    template <unsigned int N>
        requires(N >= 1 && N <= constants::SOBOL_MAX_DIM)
    inline auto sample() {
        if (dimension + N > constants::SOBOL_MAX_DIM) [[unlikely]] {
            throw std::out_of_range("out of sobol dimensions");
        }

        std::array<float, N> result;
        for (unsigned int i = 0; i < N; ++i) {
            uint32_t bits = 0;
            for (unsigned int j = 0; j < constants::SOBOL_MAX_BITS; ++j) {
                if ((index >> j) & 1) {
                    bits ^= constants::SOBOL_DIRECTIONS[dimension][j];
                }
            }
            bits      = scramble(bits);
            result[i] = static_cast<float>(bits) / std::pow(2.0f, 32.0f);
            ++dimension;
        }

        if constexpr (N == 1) {
            return result[0];
        } else {
            return result;
        }
    }

  private:
    /**
     * @brief Hash a 32-bit integer with Laine-Karras.
     * @param x the integer to hash.
     * @return the hashed value.
     */
    static inline uint32_t lk_hash(uint32_t x) {
        x += seed;
        x ^= x * 0x6c50b47cu;
        x ^= x * 0xb82f1e52u;
        x ^= x * 0xc7afe638u;
        x ^= x * 0x8d22f6e6u;
        return x;
    }

    /**
     * @brief Reverse the bits of a 32-bit integer.
     * @param x the integer to reverse.
     * @return the integer with its bits reversed.
     */
    static inline uint32_t reverse_bits(uint32_t x) {
        x = ((x >> 1) & 0x55555555) | ((x << 1) & 0xAAAAAAAA);
        x = ((x >> 2) & 0x33333333) | ((x << 2) & 0xCCCCCCCC);
        x = ((x >> 4) & 0x0F0F0F0F) | ((x << 4) & 0xF0F0F0F0);
        x = ((x >> 8) & 0x00FF00FF) | ((x << 8) & 0xFF00FF00);
        x = (x >> 16) | (x << 16);
        return x;
    }

    /**
     * @brief Scramble a 32-bit integer using Laine-Karras and bit reversal.
     * @param x the integer to scramble.
     * @return the scrambled value.
     */
    static inline uint32_t scramble(uint32_t x) {
        x = reverse_bits(x);
        x = lk_hash(x);
        x = reverse_bits(x);
        return x;
    }

    /**
     * @brief The seed used for scrambling Sobol samples.
     */
    static constexpr uint32_t seed = 42;

    unsigned int index;
    unsigned int dimension;
};

} // namespace hls