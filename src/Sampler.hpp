#pragma once

#include <algorithm>
#include <array>
#include <random>

static constexpr std::size_t MAX_DIM = 3;

namespace hls {

template <std::size_t N>
    requires(N >= 1 && N <= MAX_DIM)
class Sampler {
  public:
    using return_type = std::conditional_t<N == 1, float, std::array<float, N>>;
    static return_type rand() {
        std::array<float, N> result;
        std::generate(result.begin(), result.end(), []() {
            return uniform(engine);
        });

        if constexpr (N == 1) {
            return result[0];
        }
        return result;
    }

  private:
    static inline std::mt19937 engine{std::random_device{}()};
    static inline std::uniform_real_distribution<float> uniform{0.0f, 1.0f};
};

} // namespace hls