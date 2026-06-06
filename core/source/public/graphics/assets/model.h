#pragma once


class Model
{
public:
	Model();
	virtual ~Model() = default;

	virtual void load_from_file_obj(std::string _path);
	virtual void load_from_file_gltf(std::string _path);

	virtual void render();
	virtual void tick(float _delta);

	Transform transform;
protected:
	std::vector<Mesh> meshes;

	std::vector<tinyobj::material_t> materials;
	std::map<std::string, uint32> textures;

};