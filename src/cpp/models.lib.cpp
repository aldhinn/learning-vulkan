#include "vk_tut/application.h"
#include "vk_tut/logging.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace vk::tut {
    void Application::load_initial_mesh() {
        // For now, simply create colour wheel circle.

        // Define the number of triangles to be drawn.
        const uint32_t triangle_count = 1000;
        const uint32_t vertices_count = triangle_count + 1;

        m_vertices.reserve(vertices_count);
        // Define the origin.
        m_vertices.emplace_back(
            Vertex({0.00f, 0.00f, -0.40f}, {0.00f, 0.00f, 0.00f})
        );
        
        for (uint32_t i = 1; i < vertices_count; i++) {
            m_vertices.emplace_back(
                Vertex(
                    {
                        0.50f * ::glm::cos(i * ::glm::radians(360.0f) / vertices_count),
                        0.50f * ::glm::sin(i * ::glm::radians(360.0f) / vertices_count),
                        -0.40f
                    },
                    {
                        1.00f * ::glm::cos(
                            (i * ::glm::radians(360.0f) / vertices_count) +
                            ::glm::radians(0.0f)
                        ) + 1.00f,
                        1.00f * ::glm::cos(
                            (i * ::glm::radians(360.0f) / vertices_count) +
                            ::glm::radians(120.0f)
                        ) + 1.00f,
                        1.00f * ::glm::cos(
                            (i * ::glm::radians(360.0f) / vertices_count) +
                            ::glm::radians(240.0f)
                        ) + 1.00f
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

    void Application::load_square_mesh() {
        const uint32_t new_vertices_capacity =
            m_vertices.size() + 4 < m_vertices.capacity() ?
            m_vertices.capacity() : m_vertices.size() + 4;
        const uint32_t new_indices_capacity =
            m_indices.size() + 6 < m_indices.capacity() ?
            m_indices.capacity() : m_indices.size() + 6;

        // Resize the verices accordingly as we're adding 4 new vertices.
        m_vertices.reserve(new_vertices_capacity);
        m_indices.reserve(new_indices_capacity);

        ::glm::vec3 square_colour = { 0.98f, 0.32f, 0.93f };

        m_vertices.emplace_back(
            Vertex(
                { -0.50f, -0.50f, 0.50f },
                square_colour
            )
        );
        m_vertices.emplace_back(
            Vertex(
                { -0.50f, 0.50f, 0.50f },
                square_colour
            )
        );
        m_vertices.emplace_back(
            Vertex(
                { 0.50f, -0.50f, 0.50f },
                square_colour
            )
        );
        m_vertices.emplace_back(
            Vertex(
                { 0.50f, 0.50f, 0.50f },
                square_colour
            )
        );

        m_indices.emplace_back(new_vertices_capacity - 4);
        m_indices.emplace_back(new_vertices_capacity - 3);
        m_indices.emplace_back(new_vertices_capacity - 2);
        m_indices.emplace_back(new_vertices_capacity - 3);
        m_indices.emplace_back(new_vertices_capacity - 1);
        m_indices.emplace_back(new_vertices_capacity - 2);

        // Reload the mesh buffers.
        destroy_mesh_buffer();
        create_mesh_buffer();
    }
}