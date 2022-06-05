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
        Vertex(
            const ::glm::vec3& _3D_position,
            const ::glm::vec3& colour
        );
        // Move initializer list.
        Vertex(
            ::glm::vec3&& _3D_position,
            ::glm::vec3&& colour
        );

        // Copy constructor.
        Vertex(const Vertex&);
        // Move constructor.
        Vertex(Vertex&&);
        // Copy re-assignment.
        Vertex& operator= (const Vertex&);
        // Move re-assignment.
        Vertex& operator= (Vertex&&);

        // Getter for m_3D_position.
        inline ::glm::vec3 get_3D_position() const { return m_3D_position; }
        // Copy setter for m_3D_position.
        void set_3D_position(const ::glm::vec3&);
        // Move setter for m_3D_position.
        void set_3D_position(::glm::vec3&&);

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
        // The 3 dimensional coordinate of the position in the screen.
        ::glm::vec3 m_3D_position;
        // The normalized RGB colour value.
        ::glm::vec3 m_colour;
    };
}

#endif

#endif
// End of file.
// Do NOT write beyond here.