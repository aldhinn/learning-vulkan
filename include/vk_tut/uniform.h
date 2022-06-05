#if !defined(_VK_TUT_UNIFORM_HEADER_)
#define _VK_TUT_UNIFORM_HEADER_

// C++ only region.
#if defined(__cplusplus)

#include <glm/glm.hpp>

namespace vk::tut {
    // Encapsulates the uniform data.
    class Uniform final {
    public:
        // Default constructor.
        inline Uniform() {}
        // Copy initializer list constructor.
        Uniform(
            const ::glm::mat4& model,
            const ::glm::mat4& view,
            const ::glm::mat4& projection
        );
        // Move initializer list constructor.
        Uniform(
            ::glm::mat4&& model,
            ::glm::mat4&& view,
            ::glm::mat4&& projection
        );

        // Copy constructor.
        Uniform(const Uniform&);
        // Move constuctor.
        Uniform(Uniform&&);
        // Copy re-assignment.
        Uniform& operator= (const Uniform&);
        // Move re-assignment.
        Uniform& operator= (Uniform&&);

        // Getter for m_model.
        ::glm::mat4 get_model() const;
        // Copy setter for m_model.
        void set_model(const ::glm::mat4&);
        // Move setter for m_model.
        void set_model(::glm::mat4&&);

        // Getter for m_view.
        ::glm::mat4 get_view() const;
        // Copy setter for m_view.
        void set_view(const ::glm::mat4&);
        // Move setter for m_view.
        void set_view(::glm::mat4&&);

        // Getter for m_projection.
        ::glm::mat4 get_projection() const;
        // Copy setter for m_projection.
        void set_projection(const ::glm::mat4&);
        // Move setter for m_projection.
        void set_projection(::glm::mat4&&);

    private:
        ::glm::mat4 m_model;
        ::glm::mat4 m_view;
        ::glm::mat4 m_projection;
    };
}

#endif
// End C++ only region.

#endif
// End of file.
// Do NOT write beyond here.