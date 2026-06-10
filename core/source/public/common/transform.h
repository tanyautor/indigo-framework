#pragma once

struct Transform : public EditorInterface
{
    std::string name = {};

    Transform() {}


    Transform(const glm::vec3& translation, const glm::vec3& scale, const glm::quat& rotation)
        : m_translation(translation), m_scale(scale), m_rotation(rotation)
    {
    }

    inline const glm::vec3& GetTranslation() const { return m_translation; }
    inline const glm::vec3& GetScale() const { return m_scale; }
    inline const glm::quat& GetRotation() const { return m_rotation; }

    const glm::mat4& World();

    void SetTranslation(const glm::vec3& translation)
    {
        m_translation = translation;
        SetMatrixDirty();
    }

    void SetScale(const glm::vec3& scale)
    {
        m_scale = scale;
        SetMatrixDirty();
    }

    void SetRotation(const glm::quat& rotation)
    {
        m_rotation = rotation;
        SetMatrixDirty();
    }

    void SetFromMatrix(const glm::mat4& transform);

    // Editor
    virtual const std::string& get_name() override { return name; }
    virtual void interface_component() override;

    // Hierarchy
    void set_parent(std::shared_ptr<Transform> _transform);
    const std::unordered_map<std::string, std::shared_ptr<Transform>> get_children() const { return children; }

private:
    std::shared_ptr<Transform> parent;
    std::unordered_map<std::string, std::shared_ptr<Transform>> children;

    glm::vec3 m_translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::quat m_rotation = glm::identity<glm::quat>();

    glm::mat4 m_worldMatrix = glm::identity<glm::mat4>();
    bool m_worldMatrixDirty = true;

    void SetMatrixDirty();

};

//IMGUI_REFLECT(Transform, m_translation, m_scale, m_rotation);
