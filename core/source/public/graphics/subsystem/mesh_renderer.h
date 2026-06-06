#pragma once

struct CameraDataUBO;
struct TransformUBO;
struct Framebuffer;

class MeshRenderer : public Subsystem
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	virtual void base_pass() override;
	//virtual void post_process_pass() override;

	std::vector<Mesh> meshes;

private:
	Shader forward_pass;

	// uniforms
	uint32  camera_ubo{ 0 };
	CameraDataUBO* camera_data{ nullptr };

	uint32 trans_ubo{ 0 };
	TransformUBO* trans_data{ nullptr };

	uint32 directional_light_ubo{ 0 };
	DirectionalLightUBO* directional_light_data{ nullptr };
	
	uint32 point_light_ubo{ 0 };
	PointLightUBO* point_light_data{ nullptr };
};