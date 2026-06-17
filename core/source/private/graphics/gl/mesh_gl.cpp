#include "precomp.h"

Mesh::Mesh(const Model& _model, uint32 _index) : Mesh()
{

    // Vertex Array Object
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    label_gl(GL_VERTEX_ARRAY, vao, "VAO:" + Resource::get_path());

    // wavefront only atm TODO:
    // Vertex Buffer

    // Load Attribs:
    // position
    set_attribute(Attribute::Position, 
        (uint32)(_model.get_wavefront_buffer_pos().size() * sizeof(_model.get_wavefront_buffer_pos()[0])),
        _model.get_wavefront_buffer_pos().data());

    // color
    set_attribute(Attribute::Color, 
        (uint32)(_model.get_wavefront_buffer_color().size() * sizeof(_model.get_wavefront_buffer_color()[0])),
        _model.get_wavefront_buffer_color().data());

    // texcoords
    set_attribute(Attribute::Texture,
        (uint32)(_model.get_wavefront_buffer_texcoord().size() * sizeof(_model.get_wavefront_buffer_texcoord()[0])),
        _model.get_wavefront_buffer_texcoord().data());

    // normals
    set_attribute(Attribute::Normal,
        (uint32)(_model.get_wavefront_buffer_norm().size() * sizeof(_model.get_wavefront_buffer_norm()[0])),
        _model.get_wavefront_buffer_norm().data());

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

    path = get_path(_model, _index);
    transform.name = get_path(_model, _index);
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
    label_gl(GL_BUFFER, vbos[location], "vbos:"/*TODO: want to add resource name to this*/ + locationName);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[location]);
    glBufferData(GL_ARRAY_BUFFER, _size, _data, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, (GLsizei)0, nullptr);

    glCheckError();
}
