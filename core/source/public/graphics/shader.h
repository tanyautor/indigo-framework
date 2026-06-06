#pragma once

enum ShaderType : uint32
{
	SH_NONE = 0,
	SH_FRAGMENT = 1,
	SH_COMPUTE = 2,

};

class Shader
{
public:
	Shader();
	Shader(const Shader& other);
	Shader(std::string _vertex_path, std::string _fragment_path);
	Shader(std::string _compute_path);
	~Shader();

	Shader& operator=(const Shader& other);
	Shader& operator=( Shader&& other) noexcept;

	bool is_valid() const;
	const uint32 id() const;

	// TODO: maybe register uniform locations on init, store in map for easy access??
	//void register_uniform(std::string _name);
	//const uint32 uniform_location() const;
	void use_shader() const;

private:
	void init_fragment(std::string _vertex_src, std::string _fragment_src);
	void init_compute(std::string _compute_src);

	//std::unordered_map<std::string, uint32> uniform_locations;

	struct Impl;
	std::unique_ptr<Impl> pImpl;
};

namespace indigo_files
{
	std::string read_txt_file(const std::string& _path);
	std::string parse_shader(const std::string& _path);
}