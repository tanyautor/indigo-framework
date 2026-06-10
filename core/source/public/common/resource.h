#pragma once


class Resource
{
	friend class ResourceManager;

public:
	Resource(const Resource&) = delete;

	enum class ResourceType : int32
	{
		Shader,
		Light,
		Image,
		Sampler,
		Texture,
		Material,
		Mesh,
		Model,
	} type;


	const std::string& get_path() const { return path; }

	uint32 id{ 0 };
	std::string path{""};

protected:
	Resource(ResourceType _type) : type(_type) {}
	virtual ~Resource() = default;

	std::string get_path(const std::string& _file) { return path; }
	std::string get_path(const FileStream::Directory& _dir, const std::string& _file) { return path; }

};