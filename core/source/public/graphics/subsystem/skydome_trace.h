#pragma once

struct CameraDataUBO;
struct TransformUBO;
struct Framebuffer;

class SkydomeTrace : public SubsystemBase
{
public:
	SkydomeTrace(Camera* _camera);
	virtual ~SkydomeTrace();

	virtual void base_pass() override;
	virtual void post_process_pass() override;

	virtual void tick(float _delta_time) override {}

	void screen_pass_trace();

private:
	Camera* camera{ nullptr };
	Shader skydome_trace_shader;

	uint32 skydome;
	uint32 skydome_width;
	uint32 skydome_height;

	// uniforms
	uint32  camera_ubo{ 0 };
	CameraDataUBO* camera_data{ nullptr };
};