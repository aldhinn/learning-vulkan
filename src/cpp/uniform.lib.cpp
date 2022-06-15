#include "vk_tut/uniform.h"

#include <utility>

namespace vk::tut {
    // Copy initializer list constructor.
    Uniform::Uniform(
        const ::glm::mat4& model,
        const ::glm::mat4& view,
        const ::glm::mat4& projection
    ) : m_model(model), m_view(view), m_projection(projection) {
        m_projection[1][1] *= -1;
    }
        
    // Move initializer list constructor.
    Uniform::Uniform(
        ::glm::mat4&& model,
        ::glm::mat4&& view,
        ::glm::mat4&& projection
    ) : m_model(::std::move(model)), m_view(::std::move(view)),
    m_projection(::std::move(projection)) {
        m_projection[1][1] *= -1;
    }

    // Copy constructor.
    Uniform::Uniform(const Uniform& from) :
    m_model(from.m_model), m_view(from.m_view),
    m_projection(from.m_projection) {
        m_projection[1][1] *= -1;
    }

    // Move constuctor.
    Uniform::Uniform(Uniform&& from) :
    m_model(::std::move(from.m_model)),
    m_view(::std::move(from.m_view)),
    m_projection(::std::move(from.m_projection)) {
        m_projection[1][1] *= -1;
    }

    // Copy re-assignment.
    Uniform& Uniform::operator= (const Uniform& from) {
        m_model = from.m_model;
        m_view = from.m_view;
        m_projection = from.m_projection;

        return *this;
    }
    
    // Move re-assignment.
    Uniform& Uniform::operator= (Uniform&& from) {
        m_model = ::std::move(from.m_model);
        m_view = ::std::move(from.m_view);
        m_projection = ::std::move(from.m_projection);

        return *this;
    }

    // Getter for m_model.
    ::glm::mat4 Uniform::get_model() const {
        return m_model;
    }

    // Copy setter for m_model.
    void Uniform::set_model(const ::glm::mat4& model) {
        m_model = model;
    }

    // Move setter for m_model.
    void Uniform::set_model(::glm::mat4&& model) {
        m_model = ::std::move(model);
    }

    // Getter for m_view.
    ::glm::mat4 Uniform::get_view() const {
        return m_view;
    }

    // Copy setter for m_view.
    void Uniform::set_view(const ::glm::mat4& view) {
        m_view = view;
    }

    // Move setter for m_view.
    void Uniform::set_view(::glm::mat4&& view) {
        m_view = ::std::move(view);
    }

    // Getter for m_projection.
    ::glm::mat4 Uniform::get_projection() const {
        return m_projection;
    }

    // Copy setter for m_projection.
    void Uniform::set_projection(const ::glm::mat4& projection) {
        m_projection = projection;
        m_projection[1][1] *= -1;
    }

    // Move setter for m_projection.
    void Uniform::set_projection(::glm::mat4&& projection) {
        m_projection = ::std::move(projection);
        m_projection[1][1] *= -1;
    }
}