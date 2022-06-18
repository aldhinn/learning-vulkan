#include "vk_tut/vertex.h"

#include <utility>

namespace vk::tut {
    // Copy initializer list.
    Vertex::Vertex(
        const ::glm::vec3& _3D_position,
        const ::glm::vec3& colour,
        const ::glm::vec2& texture_coordinate
    ) : m_3D_position(_3D_position), m_colour(colour),
    m_texture_coordinate(texture_coordinate) {}

    // Move initializer list.
    Vertex::Vertex(
        ::glm::vec3&& _3D_position,
        ::glm::vec3&& colour,
        ::glm::vec2&& texture_coordinate
    ) :
    m_3D_position(::std::move(_3D_position)),
    m_colour(::std::move(colour)),
    m_texture_coordinate(::std::move(texture_coordinate)) {}

    // Copy constructor.
    Vertex::Vertex(const Vertex& from) :
    m_3D_position(from.m_3D_position),
    m_colour(from.m_colour),
    m_texture_coordinate(from.m_texture_coordinate) {}

    // Move constructor.
    Vertex::Vertex(Vertex&& from) :
    m_3D_position(::std::move(from.m_3D_position)),
    m_colour(::std::move(from.m_colour)),
    m_texture_coordinate(::std::move(from.m_texture_coordinate)) {}

    // Copy re-assignment.
    Vertex& Vertex::operator= (const Vertex& from) {
        m_3D_position = from.m_3D_position;
        m_colour = from.m_colour;
        m_texture_coordinate = from.m_texture_coordinate;

        return *this;
    }

    // Move re-assignment.
    Vertex& Vertex::operator= (Vertex&& from) {
        m_3D_position = ::std::move(from.m_3D_position);
        m_colour = ::std::move(from.m_colour);
        m_texture_coordinate = ::std::move(from.m_texture_coordinate);

        return *this;
    }

    // Copy setter for m_2D_position.
    void Vertex::set_3D_position(const ::glm::vec3& _3D_position) {
        m_3D_position = _3D_position;
    }

    // Move setter for m_2D_position.
    void Vertex::set_3D_position(::glm::vec3&& _3D_position) {
        m_3D_position = ::std::move(_3D_position);
    }

    // Copy setter for m_colour.
    void Vertex::set_colour(const ::glm::vec3& colour) {
        m_colour = colour;
    }

    // Move setter for m_colour.
    void Vertex::set_colour(::glm::vec3&& colour) {
        m_colour = ::std::move(colour);
    }

    // Copy setter for m_colour.
    void Vertex::set_texture_coordinate(
        const ::glm::vec2& texture_coordinate
    ) {
        m_texture_coordinate = texture_coordinate;
    }

    // Move setter for m_colour.
    void Vertex::set_texture_coordinate(
        ::glm::vec2&& texture_coordinate
    ) {
        m_texture_coordinate = ::std::move(texture_coordinate);
    }

    VkVertexInputBindingDescription
    Vertex::get_binding_description() {
        VkVertexInputBindingDescription description{};
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VkVertexInputRate
            ::VK_VERTEX_INPUT_RATE_VERTEX;

        return description;
    }

    std::array<VkVertexInputAttributeDescription, 3>
    Vertex::get_attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 3>
        attribute_descriptions{};

        // Tells vulkan how to find the values to be
        // assigned to the 3 coordinates of in_3D_position.
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VkFormat
            ::VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, m_3D_position);

        // Tells vulkan how to find the values to be
        // assigned to the 3 coordinates of in_colour.
        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VkFormat
            ::VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, m_colour);

        // Tells vulkan how to find the texture coordinate.
        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].format = VkFormat
            ::VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[2].offset = offsetof(
            Vertex, m_texture_coordinate
        );

        return attribute_descriptions;
    }
}