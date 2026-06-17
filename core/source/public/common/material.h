#pragma once


// Metallic Roughness thingy, from gltf2.0 standard
struct Material : public EditorInterface, public Resource
{
	Material() : Resource(ResourceType::Material) {}
	Material(const Model& _model, int32 _index);

	static std::string get_path(const Model& _model, const int32& _index)
	{
		return _model.get_path() + " | Material-" + std::to_string(_index);
	}

	virtual const std::string& get_name() { return path; }
	virtual void interface_component() {}

	// Base Color
	std::shared_ptr<Texture> base_color;
	vec4 base_color_factor{0.f};

	// Metallic Roughness
	std::shared_ptr<Texture> metallic_roughness;
	float metallic_factor{ 0.f };
	float roughness_factor{ 0.f };

	// Normal
	std::shared_ptr<Texture> normal_map;
	float normal_scale{ 1.f };

	// Occlusion
	std::shared_ptr<Texture> occlusion;
	float occlusion_strength{ 0.f };

	// Emissive
	std::shared_ptr<Texture> emissive;
	vec3 emissive_factor{ 0.f };

	uint32 num_registered_textures{ 0 };
};