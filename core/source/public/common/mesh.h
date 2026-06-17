#pragma once


class Mesh : public EditorInterface, public Resource
{
public:
	Mesh() : Resource(Resource::ResourceType::Mesh) {}
	Mesh(const Model& _model, uint32 _index);
	~Mesh();

	static std::string get_path(const Model& _model, const int32& _index)
	{
		return _model.get_path() + " | Mesh-" + std::to_string(_index);
	}

	virtual const std::string& get_name() { return path; }
	virtual void interface_component() {}

	void render();

	enum class Attribute
	{
		Position,
		Normal,
		Tangent,
		Color,
		Texture,
		Texture1,
	};

	void set_attribute(Attribute _attribute, uint32 _size, void* data);
	template <typename T>
	inline void set_attribute(Attribute attribute, std::vector<T>& data)
	{
		set_attribute(attribute, data.size() * sizeof(data[0]), &data[0]);
	}


	Transform transform;

	// GLTF2.0 attribs
	// Material
	std::shared_ptr<Material> material_slot;
	std::vector<vec3> vertices;

	// VBOs
	uint32 vbos[6] = { 0 };

	// Indexed Rendering
	uint32 ebo = -1;
	uint32 vao = -1;

	// Index Data
	int32 num_indices = -1;
	int32 index_type = -1;
};