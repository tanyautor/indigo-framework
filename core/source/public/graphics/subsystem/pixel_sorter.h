#pragma once

struct Framebuffer;

class PixelSorter : public SubsystemBase
{
public:
	PixelSorter(Framebuffer* to_manipulate);
	virtual ~PixelSorter();

	virtual void base_pass() override;
	virtual void post_process_pass() override;

	virtual void tick(float _delta) override;

	void environment_animation();
	void update_dispatch_data();

private:
	Shader compute_sorter;
	Shader effect_mask_init;
	Framebuffer* input{ nullptr };


	bool accumulate{ false };
	Framebuffer* accumulator{ nullptr };
	PixelSortEnvUBO* environment_data{ nullptr };
	uint32 environment_ubo{ 0 };

	PixelSortMaskUBO* effect_mask_data{ nullptr };
	uint32 effect_mask_ubo{ 0 };

	uint32 output_width{ 0 };
	uint32 output_height{ 0 };
	uint32 output{ 0 };

	uint32 effect_mask{ 0 };
};