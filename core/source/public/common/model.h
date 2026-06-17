#pragma once

struct Image;
struct Sampler;
struct Texture;
struct Material;
class Mesh;

class Model : public EditorInterface, public Resource
{
public:
	Model(FileStream::Directory _dir, const std::string& _file);

	void create_from_obj(const FileStream& _file);
	void create_from_gltf(const FileStream& _file);


	const std::string& get_path() const { return path; }
	static std::string get_path(FileStream::Directory _dir, const std::string& _file)
	{
		return _file;
	}

	// everything to render a mesh
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Image>> images;
	std::vector<std::shared_ptr<Sampler>> samplers;
	std::vector<std::shared_ptr<Texture>> textures;

	Transform transform;
	std::unique_ptr<FileStream> original_file;

	//gltf specific
	const tinygltf::Model& get_gltf_model() const { return model; }

	// wavefront specific
	const tinyobj::material_t& get_wavefront_material(int32 _index) const { return wavefront_materials[_index]; }
	const tinyobj::shape_t& get_wavefront_shapes(int32 _index) const { return wavefront_shapes[_index]; }
	const std::vector<uint32>& get_wavefront_indices() const { return wavefront_indices; }
	const std::vector<float>& get_wavefront_buffer_pos() const { return wavefront_buffer_pos; }
	const std::vector<float>& get_wavefront_buffer_norm() const { return wavefront_buffer_norm; }
	const std::vector<float>& get_wavefront_buffer_color() const { return wavefront_buffer_color; }
	const std::vector<float>& get_wavefront_buffer_texcoord() const { return wavefront_buffer_texcoord; }


	std::shared_ptr<Image> get_image(int32 _index) const { return images[_index]; }
	std::shared_ptr<Sampler> get_sampler(int32 _index) const { return samplers[_index]; }
	std::shared_ptr<Mesh> get_mesh(int32 _index) const { return meshes[_index]; }

	virtual const std::string& get_name() { return path; }
	virtual void interface_component() {}

private:
	tinygltf::Model model;

	std::vector<uint32> wavefront_indices;  // pos(3float)
	std::vector<float> wavefront_buffer_pos;  // pos(3float)
	std::vector<float> wavefront_buffer_norm;  // normal(3float)
	std::vector<float> wavefront_buffer_color;  // color(3float)
	std::vector<float> wavefront_buffer_texcoord;  // texcoord(2float)
	std::vector<tinyobj::material_t> wavefront_materials;
	std::vector<tinyobj::shape_t> wavefront_shapes;



};