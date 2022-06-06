#include "vk_tut/application.h"
#include "vk_tut/logging.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace vk::tut {
    void Application::load_mesh() {
        // For now, simply create colour wheel circle.

        // Define the number of triangles to be drawn.
        const uint32_t triangle_count = 1000;
        const uint32_t vertices_count = triangle_count + 1;

        m_vertices.reserve(vertices_count);
        // Define the origin.
        m_vertices.emplace_back(
            Vertex({0.00f, 0.00f, 0.00f}, {0.50f, 0.50f, 0.50f})
        );
        
        for (uint32_t i = 1; i < vertices_count; i++) {
            m_vertices.emplace_back(
                Vertex(
                    {
                        0.75f * ::glm::cos(i * ::glm::radians(360.0f) / vertices_count),
                        0.75f * ::glm::sin(i * ::glm::radians(360.0f) / vertices_count),
                        0.00f
                    },
                    {
                        0.5f * ::glm::cos(
                            (i * ::glm::radians(360.0f) / vertices_count) +
                            ::glm::radians(0.0f)
                        ) + 0.5f,
                        0.5f * ::glm::cos(
                            (i * ::glm::radians(360.0f) / vertices_count) +
                            ::glm::radians(120.0f)
                        ) + 0.5f,
                        0.5f * ::glm::cos(
                            (i * ::glm::radians(360.0f) / vertices_count) +
                            ::glm::radians(240.0f)
                        ) + 0.5f
                    }
                )
            );
        }

        m_indices.reserve(triangle_count * 3);
        for (int i = 0; i < m_indices.capacity() - 1; i++) {
            if (i % 3 == 0) {
                m_indices.emplace_back(0U);
                continue;
            }
            
            m_indices.emplace_back(
                static_cast<uint32_t>((i + 1) / 3)
            );
        }

        m_indices.emplace_back(1);
    }
}