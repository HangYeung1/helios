#pragma once

#include "../Ray.hpp"

#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <optional>

namespace hls {

class Triangle {
  public:
    Triangle(glm::vec3 &&v1, glm::vec3 &&v2, glm::vec3 &&v3) noexcept
        : vertices({std::move(v1), std::move(v2), std::move(v3)}),
          normal(glm::normalize(glm::cross(v2 - v1, v3 - v1))) {};

    // https://jcgt.org/published/0002/01/05/paper.pdf
    std::optional<Ray> hit(const Ray &ray) const {
        // Transform vertices
        unsigned int kz      = 0;
        float        max_dir = 0;
        for (unsigned int i = 0; i < 3; ++i) {
            float abs_val = std::abs(ray.direction[i]);
            if (abs_val > max_dir) {
                max_dir = abs_val;
                kz      = i;
            }
        }

        unsigned int kx = (kz + 1) % 3;
        unsigned int ky = (kx + 1) % 3;
        if (ray.direction[kz] < 0.0f) {
            std::swap(kx, ky);
        }

        glm::vec3 A = vertices[0] - ray.origin;
        glm::vec3 B = vertices[1] - ray.origin;
        glm::vec3 C = vertices[2] - ray.origin;

        // Calculate barycentric coordinates
        float Sx = ray.direction[kx] / ray.direction[kz];
        float Sy = ray.direction[ky] / ray.direction[kz];
        float Sz = 1.0f / ray.direction[kz];

        float Ax = A[kx] - Sx * A[kz];
        float Ay = A[ky] - Sy * A[kz];
        float Bx = B[kx] - Sx * B[kz];
        float By = B[ky] - Sy * B[kz];
        float Cx = C[kx] - Sx * C[kz];
        float Cy = C[ky] - Sy * C[kz];

        float U = Cx * By - Cy * Bx;
        float V = Ax * Cy - Ay * Cx;
        float W = Bx * Ay - By * Ax;

        // Double precision fall back
        if (U == 0.0f || V == 0.0f || W == 0.0f) {
            double CxBy = static_cast<double>(Cx) * static_cast<double>(By);
            double CyBx = static_cast<double>(Cy) * static_cast<double>(Bx);
            U           = static_cast<float>(CxBy - CyBx);

            double AxCy = static_cast<double>(Ax) * static_cast<double>(Cy);
            double AyCx = static_cast<double>(Ay) * static_cast<double>(Cx);
            V           = static_cast<float>(AxCy - AyCx);

            double BxAy = static_cast<double>(Bx) * static_cast<double>(Ay);
            double ByAx = static_cast<double>(By) * static_cast<double>(Ax);
            W           = static_cast<float>(BxAy - ByAx);
        }

        if ((U < 0.0f || V < 0.0f || W < 0.0f)
            && (U > 0.0f || V > 0.0f || W > 0.0f)) {
            return std::nullopt;
        }

        // Caculate intersection parameter
        float det = U + V + W;
        if (det == 0.0f) {
            return std::nullopt;
        }

        float Az = Sz * A[kz];
        float Bz = Sz * B[kz];
        float Cz = Sz * C[kz];
        float T  = U * Az + V * Bz + W * Cz;

        if ((det > 0.0f && T < 0.0f) || (det < 0.0f && T > 0.0f)) {
            return std::nullopt;
        }
        return Ray(ray.at(T / det),
                   glm::dot(ray.direction, normal) >= 0.0f ? -normal : normal);
    }

  private:
    std::array<glm::vec3, 3> vertices;
    glm::vec3                normal;
};

} // namespace hls