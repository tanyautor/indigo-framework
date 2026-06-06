#pragma once

struct CameraDataUBO;
struct TransformUBO;
struct Framebuffer;

class ModelRenderer : public SubsystemBase
{
public:
	ModelRenderer(Camera* _camera);
	virtual ~ModelRenderer();

	virtual void base_pass() override;
	virtual void post_process_pass() override;

	virtual void tick(float _delta_time) override
	{
		for (auto& model : models)
		{
			model.tick(_delta_time);
		}
	}

	std::vector<Model> models;

private:
	Camera* camera{ nullptr };
	Shader base_shader;

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