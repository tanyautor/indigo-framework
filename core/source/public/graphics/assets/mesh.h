#pragma once


class Mesh
{
public:
	Mesh();
	~Mesh();

    virtual void render();

    void create_mesh(const unsigned int* _indices,
        unsigned int _index_count,
        const float* _positions,
        const float* _normals,
        const float* _texture_coordinates,
        const float* _colors,
        unsigned int _vertex_count);
    const int32& get_num_vertices() const { return num_vertices; }
    
    int32 material_id = -1;

protected:
    uint32 vbo_vert = -1, 
        vbo_norm = -1, 
        vbo_texcoord = -1, 
        vbo_clrs = -1;
    uint32 ebo = -1;
    uint32 vao = -1;

    int32 num_indices = -1;
    int32 num_vertices = -1;

    friend class AnimatedMesh;
};