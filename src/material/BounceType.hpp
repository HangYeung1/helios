#pragma once

namespace hls {

/**
 * @brief The type of bounce that occurs when a ray hits a surface.
 * @details This broadly categorizes surface interactions for rendering.
 */
enum class BounceType {
    Diffuse,
    Refactive,
    Specular,
    Emissive,
    None,
};

} // namespace hls