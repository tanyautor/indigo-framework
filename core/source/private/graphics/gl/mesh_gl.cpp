#include "precomp.h"

static uint32 tinygltf_to_typesize(tinygltf::Accessor const& accessor) noexcept
{
    uint32 elementSize = 0;
    switch (accessor.componentType)
    {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: 
    case TINYGLTF_COMPONENT_TYPE_BYTE:
        elementSize = 1;
        break;
    case TINYGLTF_COMPONENT_TYPE_SHORT:
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        elementSize = 2;
        break;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
    case TINYGLTF_COMPONENT_TYPE_INT:
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        elementSize = 4;
        break;
    default:
        assert(false);
    }

    switch (accessor.type)
    {
    case TINYGLTF_TYPE_MAT2:
        return 4 * elementSize;
    case TINYGLTF_TYPE_MAT3:
        return 9 * elementSize;
    case TINYGLTF_TYPE_MAT4:
        return 16 * elementSize;
    case TINYGLTF_TYPE_SCALAR:
        return elementSize;
    case TINYGLTF_TYPE_VEC2:
        return 2 * elementSize;
    case TINYGLTF_TYPE_VEC3:
        return 3 * elementSize;
    case TINYGLTF_TYPE_VEC4:
        return 4 * elementSize;
    default:
        assert(false);
    }

    return 0;
}

Mesh::Mesh(const Model& _model, uint32 _index) : Mesh()
{
    path = get_path(_model, _index);
    transform.name = get_path(_model, _index);

    // Vertex Array Object
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    label_gl(GL_VERTEX_ARRAY, vao, "VAO:" + Resource::get_path());

    if (_model.original_file->type == FileStream::Type::Wavefront)
    {
        // Vertex Buffer
        {
            // for consistency sake it's better to skip this lint
            glCreateBuffers(1, &vbos[MESH_POSITION]);  // NOLINT(readability-container-data-pointer)
            glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_POSITION]);
            glBufferData(GL_ARRAY_BUFFER,
                (uint32)(_model.get_wavefront_buffer_pos().size() * sizeof(_model.get_wavefront_buffer_pos()[0])),
                _model.get_wavefront_buffer_pos().data(),
                GL_STATIC_DRAW);
            glEnableVertexAttribArray(MESH_POSITION);
            glVertexAttribPointer(MESH_POSITION, 3, GL_FLOAT, GL_FALSE, (GLsizei)0, nullptr);
            label_gl(GL_BUFFER, vbos[MESH_POSITION], "VBO |" + path + " | POSITION");
            vertices.resize((uint32)_model.get_wavefront_buffer_pos().capacity());
            memcpy(vertices.data(), _model.get_wavefront_buffer_pos().data(), (uint32)_model.get_wavefront_buffer_pos().capacity());
        }
        {
            glCreateBuffers(1, &vbos[MESH_COLOR]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_COLOR]);
            glBufferData(GL_ARRAY_BUFFER,
                (uint32)(_model.get_wavefront_buffer_color().size() * sizeof(_model.get_wavefront_buffer_color()[0])),
                _model.get_wavefront_buffer_color().data(),
                GL_STATIC_DRAW);
            glEnableVertexAttribArray(MESH_COLOR);
            glVertexAttribPointer(MESH_COLOR, 3, GL_FLOAT, GL_FALSE, (GLsizei)0, nullptr);
            label_gl(GL_BUFFER, vbos[MESH_COLOR], "VBO |" + path + " | COLOR");
        }
        {
            glCreateBuffers(1, &vbos[MESH_NORMAL]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_NORMAL]);
            glBufferData(GL_ARRAY_BUFFER,
                (uint32)(_model.get_wavefront_buffer_norm().size() * sizeof(_model.get_wavefront_buffer_norm()[0])),
                _model.get_wavefront_buffer_norm().data(),
                GL_STATIC_DRAW);
            glEnableVertexAttribArray(MESH_NORMAL);
            glVertexAttribPointer(MESH_NORMAL, 3, GL_FLOAT, GL_FALSE, (GLsizei)0, nullptr);
            label_gl(GL_BUFFER, vbos[MESH_NORMAL], "VBO |" + path + " | NORMAL");
        }
        {
            glCreateBuffers(1, &vbos[MESH_TEXTURE0]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_TEXTURE0]);
            glBufferData(GL_ARRAY_BUFFER,
                (uint32)(_model.get_wavefront_buffer_texcoord().size() * sizeof(_model.get_wavefront_buffer_texcoord()[0])),
                _model.get_wavefront_buffer_texcoord().data(),
                GL_STATIC_DRAW);
            glEnableVertexAttribArray(MESH_TEXTURE0);
            glVertexAttribPointer(MESH_TEXTURE0, 2, GL_FLOAT, GL_FALSE, (GLsizei)0, nullptr);
            label_gl(GL_BUFFER, vbos[MESH_TEXTURE0], "VBO |" + path + " | TEXCOORD_0");
        }


        // Element Buffer
        // ebo
        auto indices = _model.get_wavefront_indices();
        num_indices = (uint32)indices.size();
        glBindVertexArray(vao);
        glCreateBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            num_indices * sizeof(indices[0]),
            indices.data(),
            GL_DYNAMIC_DRAW);
        label_gl(GL_BUFFER, ebo, "EBO |" + Resource::get_path());

        glCheckError();

        material_slot = _model.materials[_model.get_wavefront_shapes(_index).mesh.material_ids[_index]];
    }
    else if (_model.original_file->type == FileStream::Type::GLTF)
    {
        auto model = _model.get_gltf_model();
        auto mesh = model.meshes[_index];

        // Vertex Array Object
        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);
        label_gl(GL_VERTEX_ARRAY, vao, "VAO:" + path);

        // Only one primitive per mesh used in bee
        assert(!mesh.primitives.empty());
        auto primitive = mesh.primitives[0];

        // Loop over all attributes
        for (auto& attribute : primitive.attributes)
        {
            // Get attribute data from GLTF
            const auto& accessor = model.accessors[attribute.second];
            const auto& view = model.bufferViews[accessor.bufferView];
            const auto& buffer = model.buffers[view.buffer];

            // Note: accessor.max / accessor.min are the AABB of the mesh. You can use this for culling if you want.

            if (attribute.first == "POSITION")
            {
                // for consistency sake it's better to skip this lint
                glCreateBuffers(1, &vbos[MESH_POSITION]);  // NOLINT(readability-container-data-pointer)
                glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_POSITION]);
                glBufferData(GL_ARRAY_BUFFER,
                    accessor.count * 12,
                    &buffer.data.at(view.byteOffset + accessor.byteOffset),
                    GL_STATIC_DRAW);
                glEnableVertexAttribArray(MESH_POSITION);
                glVertexAttribPointer(MESH_POSITION, 3, GL_FLOAT, GL_FALSE, (GLsizei)view.byteStride, nullptr);
                label_gl(GL_BUFFER, vbos[MESH_POSITION], "VBO |" + path + " | POSITION");
                vertices.resize(accessor.count);
                memcpy(vertices.data(), &buffer.data.at(view.byteOffset + accessor.byteOffset), accessor.count * 12);
            }
            else if (attribute.first == "COLOR")
            {
                glCreateBuffers(1, &vbos[MESH_COLOR]);
                glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_COLOR]);
                glBufferData(GL_ARRAY_BUFFER,
                    accessor.count * 12,
                    &buffer.data.at(view.byteOffset + accessor.byteOffset),
                    GL_STATIC_DRAW);
                glEnableVertexAttribArray(MESH_COLOR);
                glVertexAttribPointer(MESH_COLOR, 3, GL_FLOAT, GL_FALSE, (GLsizei)view.byteStride, nullptr);
                label_gl(GL_BUFFER, vbos[MESH_COLOR], "VBO |" + path + " | COLOR");
            }
            else if (attribute.first == "NORMAL")
            {
                glCreateBuffers(1, &vbos[MESH_NORMAL]);
                glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_NORMAL]);
                glBufferData(GL_ARRAY_BUFFER,
                    accessor.count * 12,
                    &buffer.data.at(view.byteOffset + accessor.byteOffset),
                    GL_STATIC_DRAW);
                glEnableVertexAttribArray(MESH_NORMAL);
                glVertexAttribPointer(MESH_NORMAL, 3, GL_FLOAT, GL_FALSE, (GLsizei)view.byteStride, nullptr);
                label_gl(GL_BUFFER, vbos[MESH_NORMAL], "VBO |" + path + " | NORMAL");
            }
            else if (attribute.first == "TANGENT")
            {
                glCreateBuffers(1, &vbos[MESH_TANGENT]);
                glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_TANGENT]);
                glBufferData(GL_ARRAY_BUFFER,
                    accessor.count * 16,
                    &buffer.data.at(view.byteOffset + accessor.byteOffset),
                    GL_STATIC_DRAW);
                glEnableVertexAttribArray(MESH_TANGENT);
                glVertexAttribPointer(MESH_TANGENT, 4, GL_FLOAT, GL_FALSE, (GLsizei)view.byteStride, nullptr);
            }
            else if (attribute.first == "TEXCOORD_0")
            {
                glCreateBuffers(1, &vbos[MESH_TEXTURE0]);
                glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_TEXTURE0]);
                glBufferData(GL_ARRAY_BUFFER,
                    accessor.count * 8,
                    &buffer.data.at(view.byteOffset + accessor.byteOffset),
                    GL_STATIC_DRAW);
                glEnableVertexAttribArray(MESH_TEXTURE0);
                glVertexAttribPointer(MESH_TEXTURE0, 2, GL_FLOAT, GL_FALSE, (GLsizei)view.byteStride, nullptr);
                label_gl(GL_BUFFER, vbos[MESH_TEXTURE0], "VBO |" + path + " | TEXCOORD_0");
            }
            else if (attribute.first == "TEXCOORD_1")
            {
                glCreateBuffers(1, &vbos[MESH_TEXTURE1]);
                glBindBuffer(GL_ARRAY_BUFFER, vbos[MESH_TEXTURE1]);
                glBufferData(GL_ARRAY_BUFFER,
                    accessor.count * 8,
                    &buffer.data.at(view.byteOffset + accessor.byteOffset),
                    GL_STATIC_DRAW);
                glEnableVertexAttribArray(MESH_TEXTURE1);
                glVertexAttribPointer(MESH_TEXTURE1, 2, GL_FLOAT, GL_FALSE, (GLsizei)view.byteStride, nullptr);
                label_gl(GL_BUFFER, vbos[MESH_TEXTURE1], "VBO |" + path + " | TEXCOORD_1");
            }
        }

        // Index buffer
        // Get index data from GLTF
        const auto& accessor = model.accessors[primitive.indices];
        const auto& view = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[view.buffer];

        // This many to render
        num_indices = static_cast<uint32>(accessor.count);
        index_type = static_cast<uint32>(accessor.componentType);
        uint32 typeSize = tinygltf_to_typesize(accessor);

        glCreateBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            accessor.count * typeSize,
            &buffer.data.at(accessor.byteOffset + view.byteOffset),
            GL_DYNAMIC_DRAW);
        label_gl(GL_BUFFER, ebo, "EBO |" + path);


        int32 mat = mesh.primitives[0].material;
        if (mat != -1)
        {
            material_slot = _model.materials[mat];
        }
    }

}
Mesh::~Mesh()
{
}

void Mesh::render() 
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, num_indices);

    glBindVertexArray(0);
}

void Mesh::set_attribute(Attribute _attribute, uint32 _size, void* _data)
{
    glBindVertexArray(vao);

    uint32 location = 0;
    uint32 size = 0;
    std::string locationName;
    switch (_attribute)
    {
    case Attribute::Position:
        location = MESH_POSITION;
        locationName = "POSITION";
        size = 3;
        break;
    case Attribute::Normal:
        location = MESH_NORMAL;
        locationName = "NORMAL";
        size = 3;
        break;
    case Attribute::Tangent:
        location = MESH_TANGENT;
        locationName = "TANGENT";
        size = 4;
        break;
    case Attribute::Color:
        location = MESH_COLOR;
        locationName = "COLOR";
        size = 3;
        break;
    case Attribute::Texture:
        location = MESH_TEXTURE0;
        locationName = "TEXTURE0";
        size = 2;
        break;
    case Attribute::Texture1:
        location = MESH_TEXTURE1;
        locationName = "TEXTURE1";
        size = 2;
        break;
    default:
        assert(false);
        break;
    }

    if (vbos[location] != 0)
    {
        glDeleteBuffers(1, &vbos[location]);
        vbos[location] = 0;
    }
    glCreateBuffers(1, &vbos[location]);
    label_gl(GL_BUFFER, vbos[location], "VBO | " + path + " | " + locationName);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[location]);
    glBufferData(GL_ARRAY_BUFFER, _size, _data, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, (GLsizei)0, nullptr);

    glCheckError();
}
