#pragma once


class Mesh
{
public:
	Mesh();
	~Mesh();

    void render();

    void create_mesh(const unsigned int* _indices,
        unsigned int _index_count,
        const float* _positions,
        const float* _normals,
        const float* _texture_coordinates,
        const float* _colors,
        unsigned int _vertex_count);
    const int32& get_num_vertices() const { return num_vertices; }
    
    Transform transform;

    // GLTF2.0 attribs
    // Material
    int32 material_id = -1;
    
    // VBOs
    uint32 vbo_vert = -1, 
        vbo_norm = -1, 
        vbo_texcoord = -1, 
        vbo_clrs = -1;

    // Indexed Rendering
    uint32 ebo = -1;
    uint32 vao = -1;

    // Index Data
    int32 num_indices = -1;
    int32 num_vertices = -1;
};