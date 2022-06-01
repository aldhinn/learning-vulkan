#include "vk_tut/vertex.h"

#include <utility>

namespace vk::tut {
    // Copy initializer list.
    Vertex::Vertex(const ::glm::vec2& _2D_position, const ::glm::vec3& colour) :
    m_2D_position(_2D_position),
    m_colour(colour) {}

    // Move initializer list.
    Vertex::Vertex(::glm::vec2&& _2D_position, ::glm::vec3&& colour) :
    m_2D_position(::std::move(_2D_position)),
    m_colour(::std::move(colour)) {}

    // Copy constructor.
    Vertex::Vertex(const Vertex& from) :
    m_2D_position(from.m_2D_position),
    m_colour(from.m_colour) {}

    // Move constructor.
    Vertex::Vertex(Vertex&& from) :
    m_2D_position(::std::move(from.m_2D_position)),
    m_colour(::std::move(from.m_colour)) {}

    // Copy re-assignment.
    Vertex& Vertex::operator= (const Vertex& from) {
        m_2D_position = from.m_2D_position;
        m_colour = from.m_colour;
        return *this;
    }

    // Move re-assignment.
    Vertex& Vertex::operator= (Vertex&& from) {
        m_2D_position = ::std::move(from.m_2D_position);
        m_colour = ::std::move(from.m_colour);
        return *this;
    }

    // Copy setter for m_2D_position.
    void Vertex::set_2D_position(const ::glm::vec2& _2D_position) {
        m_2D_position = _2D_position;
    }

    // Move setter for m_2D_position.
    void Vertex::set_2D_position(::glm::vec2&& _2D_position) {
        m_2D_position = ::std::move(_2D_position);
    }

    // Copy setter for m_colour.
    void Vertex::set_colour(const ::glm::vec3& colour) {
        m_colour = colour;
    }

    // Move setter for m_colour.
    void Vertex::set_colour(::glm::vec3&& colour) {
        m_colour = ::std::move(colour);
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

    std::array<VkVertexInputAttributeDescription, 2>
    Vertex::get_attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 2>
        attribute_descriptions{};

        // Tells vulkan how to find the values to be
        // assigned to the 2 coordinates of inPosition.
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, m_2D_position);

        // Tells vulkan how to find the values to be
        // assigned to the 3 coordinates of inColor.
        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, m_colour);

        return attribute_descriptions;
    }
}