#include "precomp.h"

Mesh::Mesh()
{
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

void Mesh::create_mesh(const unsigned int* _indices,
	unsigned int _index_count,
	const float* _positions,
	const float* _normals,
	const float* _texture_coordinates,
	const float* _colors,
	unsigned int _vertex_count)
{
    num_vertices = _vertex_count; // for later use in skeletal mesh

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the index buffer
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _index_count * sizeof(int), _indices, GL_STATIC_DRAW);
    num_indices = _index_count;

    // Create the vertex buffers
    glGenBuffers((GLsizei)1, &vbo_vert);
    glGenBuffers((GLsizei)1, &vbo_norm);
    glGenBuffers((GLsizei)1, &vbo_texcoord);
    glGenBuffers((GLsizei)1, &vbo_clrs);

    // Create the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vert);
    glBufferData(GL_ARRAY_BUFFER, _vertex_count * 3 * sizeof(float), _positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_POSITION);
    glVertexAttribPointer(MESH_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Create the normal buffer
    if (_normals)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
        glBufferData(GL_ARRAY_BUFFER, _vertex_count * 3 * sizeof(float), _normals, GL_STATIC_DRAW);
        glEnableVertexAttribArray(MESH_NORMAL);
        glVertexAttribPointer(MESH_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    // Create the texture coordinate buffer
    if (_texture_coordinates)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
        glBufferData(GL_ARRAY_BUFFER, _vertex_count * 2 * sizeof(float), _texture_coordinates, GL_STATIC_DRAW);
        glEnableVertexAttribArray(MESH_TEXCOORD);
        glVertexAttribPointer(MESH_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    // Create the color buffer
    if (_colors)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_clrs);
        glBufferData(GL_ARRAY_BUFFER, _vertex_count * 3 * sizeof(float), _colors, GL_STATIC_DRAW);
        glEnableVertexAttribArray(MESH_COLOR);
        glVertexAttribPointer(MESH_COLOR, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    // Unbind the vertex array
    glBindVertexArray(0);
}