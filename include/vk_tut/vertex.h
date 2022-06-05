#if !defined(_VK_TUT_VERTEX_HEADER_)
#define _VK_TUT_VERTEX_HEADER_

// C++ only region.
#if defined(__cplusplus)

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>

namespace vk::tut {
    // Encapsulate the vertex data of a point.
    class Vertex final {
    public:
        // Default constructor.
        inline Vertex() {}

        // Copy initializer list.
        Vertex(const ::glm::vec2&, const ::glm::vec3&);
        // Move initializer list.
        Vertex(::glm::vec2&&, ::glm::vec3&&);

        // Copy constructor.
        Vertex(const Vertex&);
        // Move constructor.
        Vertex(Vertex&&);
        // Copy re-assignment.
        Vertex& operator= (const Vertex&);
        // Move re-assignment.
        Vertex& operator= (Vertex&&);

        // Getter for m_2D_position.
        inline ::glm::vec2 get_2D_position() const { return m_2D_position; }
        // Copy setter for m_2D_position.
        void set_2D_position(const ::glm::vec2&);
        // Move setter for m_2D_position.
        void set_2D_position(::glm::vec2&&);

        // Getter for m_colour.
        inline ::glm::vec3 get_colour() const { return m_colour; }
        // Copy setter for m_colour.
        void set_colour(const ::glm::vec3&);
        // Move setter for m_colour.
        void set_colour(::glm::vec3&&);

        static VkVertexInputBindingDescription
        get_binding_description();
        static std::array<VkVertexInputAttributeDescription, 2>
        get_attribute_descriptions();

    private:
        // The 2 dimensional coordinate of the position in the screen.
        ::glm::vec2 m_2D_position;
        // The normalized RGB colour value.
        ::glm::vec3 m_colour;
    };
}

#endif

#endif
// End of file.
// Do NOT write beyond here.