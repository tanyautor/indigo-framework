#include "precomp.h"

using namespace glm;



PixelSorter::PixelSorter(Framebuffer* to_manipulate) : input(to_manipulate)
{
	priority = 1 << 3;
	compute_sorter = Shader("shared/shaders/pixel_sort.comp");
	effect_mask_init = Shader("shared/shaders/pixel_sort_mask.comp");

	compute_sorter.use_shader();

	// Environment UBO
	environment_data = new PixelSortEnvUBO();

	environment_data->bitonic_elements = 32;
	environment_data->compare_value = 0;
	environment_data->inverse_swap = false;
	environment_data->origin_offset = 0;
	environment_data->sort_direction = { 1,0 };
	environment_data->sort_size = { input->width, input->height };

	glGenBuffers(1, &environment_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, environment_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PixelSortEnvUBO), environment_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, PIXEL_ENV_UBO, environment_ubo);
	label_gl(GL_BUFFER, environment_ubo, "PixelSorter EnvironmentUBO");

	effect_mask_data = new PixelSortMaskUBO();
	effect_mask_data->high_threshold = 0.6f;
	effect_mask_data->low_threshold = 0.37f;
	effect_mask_data->delta_time = 0.16f;
	effect_mask_data->horizontal = false;
	effect_mask_data->invert_mask = false;
	effect_mask_data->speed = 1.f;
	effect_mask_data->seed_offset = 1.f;

	glGenBuffers(1, &effect_mask_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, effect_mask_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PixelSortMaskUBO), effect_mask_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, PIXEL_MASK_UBO, effect_mask_ubo);
	label_gl(GL_BUFFER, effect_mask_ubo, "PixelSorter EffectMaskUBO");

	// texture size
	output_width = engine.get_window().get_window_size().x;
	output_height = engine.get_window().get_window_size().y;

	glGenTextures(1, &output);
	glActiveTexture(GL_TEXTURE0 + PIXELSORTER_BINDING);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, output_width, output_height, 0, GL_RGBA,
		GL_FLOAT, NULL);

	label_gl(GL_TEXTURE, output, "PixelSorter SortOutput");

	glGenTextures(1, &effect_mask);
	glActiveTexture(GL_TEXTURE0 + OFFSETMASK_BINDING);
	glBindTexture(GL_TEXTURE_2D, effect_mask);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, output_width, output_height, 0, GL_RED_INTEGER,
		GL_UNSIGNED_BYTE, NULL);
	
	label_gl(GL_TEXTURE, effect_mask, "PixelSorter EffectMask");

	// Bind image slots for compute shader
	glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_BINDING);
	glBindTexture(GL_TEXTURE_2D, input->color_buffer.value());
	glBindImageTexture(FRAMEBUFFER_BINDING, input->color_buffer.value(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0 + PIXELSORTER_BINDING);
	glBindTexture(GL_TEXTURE_2D, output);
	glBindImageTexture(PIXELSORTER_BINDING, output, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0 + OFFSETMASK_BINDING);
	glBindTexture(GL_TEXTURE_2D, effect_mask);
	glBindImageTexture(OFFSETMASK_BINDING, effect_mask, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R8UI);
	glBindTexture(GL_TEXTURE_2D, 0);

	accumulator = new Framebuffer("PixelSorter Accumulator");
	accumulator->init_color_buffer(GL_TEXTURE_2D);

	glCheckError();
}

PixelSorter::~PixelSorter()
{
	delete environment_data;
	delete accumulator;
}

void PixelSorter::base_pass()
{
	ImGui::Begin("PixelSorter");
	ImReflect::Input("Sorting Environment", environment_data);
	environment_data->bitonic_elements = glm::max<uint32>(glm::min(environment_data->bitonic_elements + (environment_data->bitonic_elements % 2), uint32(BITONIC_ELEMENTS)), 2);
	
	ImReflect::Input("Sorting Mask", effect_mask_data);
	effect_mask_data->delta_time = engine.get_delta_time();
	environment_animation();

	if (ImGui::Checkbox("Accumulate", &accumulate))
	{
		// on switch
		if (accumulate)
		{
			glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_BINDING);
			glBindTexture(GL_TEXTURE_2D, accumulator->color_buffer.value());
			glBindImageTexture(FRAMEBUFFER_BINDING, accumulator->color_buffer.value(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_BINDING);
			glBindTexture(GL_TEXTURE_2D, input->color_buffer.value());
			glBindImageTexture(FRAMEBUFFER_BINDING, input->color_buffer.value(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		}
	}

	// Generate Mask
	effect_mask_init.use_shader();
	update_dispatch_data();

	glDispatchCompute(
		output_width / PIXEL_SORT_SIZE_X,
		output_height / PIXEL_SORT_SIZE_Y,
		1 / PIXEL_SORT_SIZE_Z);
	
	glCheckError();
	
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Sort Frame
	compute_sorter.use_shader();

	update_dispatch_data();

	// attend to bitonic elements
	glm::ivec2 bitonic_attenuation{ 1,1 };
	bitonic_attenuation.x = (environment_data->sort_direction.x != 0) ? environment_data->bitonic_elements : 1;
	bitonic_attenuation.y = (environment_data->sort_direction.y != 0) ? environment_data->bitonic_elements : 1;

	uint32
		dispatch_x = output_width / PIXEL_SORT_SIZE_X / bitonic_attenuation.x,
		dispatch_y = output_height / PIXEL_SORT_SIZE_Y / bitonic_attenuation.y,
		dispatch_z = 1 / PIXEL_SORT_SIZE_Z;

	glDispatchCompute(
		dispatch_x,
		dispatch_y,
		dispatch_z); // get size of framebuffer

	glCheckError();

}

void PixelSorter::post_process_pass()
{

	static bool overlay{ false };
	ImGui::Checkbox("Overlay", &overlay);

	ImGui::End();

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	if (overlay)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, output);

		// update accumulator
		glBindFramebuffer(GL_FRAMEBUFFER, accumulator->fbo);
		draw_quad();

		// draw post-process to backbuffer
		engine.get_renderer().bind_default_framebuffer();
		draw_quad();
	}
}

void PixelSorter::tick(float _delta)
{
	ImGui::Begin("Save Frame");
	
	static int index{ 0 };
	ImGui::InputInt("Ind", &index);
	if(ImGui::Button("Save Frame"))
	{
		float* img_data = new float[(output_width * output_height) * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, img_data);

		std::string image_save_path{ "saved/output/screen" + std::to_string(index) };
		std::string image_save_path_hdr{ image_save_path + ".hdr" };
		std::string image_save_path_png{ image_save_path + ".png" };
		
		stbi_flip_vertically_on_write(true);
		if(stbi_write_hdr(image_save_path_hdr.c_str(), output_width, output_height, 4, img_data))
		{
			tanlog::log(tanlog::INFO, "wrote screen to {}", image_save_path_hdr);
		}
		
		// conver to uchar rbga values for pngs
		uint8* img_data_uchar = new uint8[(output_width * output_height) * 4];
		for (uint32 i = 0; i < (output_width * output_height) * 4; i++)
		{
			img_data_uchar[i] = static_cast<uint8>(glm::min(glm::max(img_data[i], 0.0f), 1.0f) * 255.0f);
		}
		if (stbi_write_png(image_save_path_png.c_str(), output_width, output_height, 4, img_data_uchar, output_width * 4))
		{
			tanlog::log(tanlog::INFO, "wrote screen to {}", image_save_path_png);
		}

		delete[] img_data;
		delete[] img_data_uchar;
	}
	
	ImGui::End();
}

void PixelSorter::environment_animation()
{
	static bool animation{ false };
	static float counter{ 0.f };
	static float offset{ 0.f };

	ImGui::Checkbox("Animation", &animation);

	if (animation)
	{
		counter += (engine.get_delta_time() * static_cast<float>(std::rand()));
		int anim_offset{ (int)(2.f * glm::sin(counter)) };
		int anim_comp{ (int)(1.6f * (1.f + glm::sin(counter))) };
		
		//environment_data->origin_offset = anim_offset;
		//environment_data->compare_value = anim_comp;

		//effect_mask_data->speed = glm::sin(counter);
		effect_mask_data->seed_offset = glm::sin(counter) * 512.f;
		//effect_mask_data->low_threshold = glm::sin(counter);
		//effect_mask_data->high_threshold = glm::cos(counter);
	}

}

void PixelSorter::update_dispatch_data()
{
	glBindBuffer(GL_UNIFORM_BUFFER, environment_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PixelSortEnvUBO), environment_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, effect_mask_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PixelSortMaskUBO), effect_mask_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	if (accumulate)
	{
		// use accumulator as input
		glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_BINDING);
		glBindTexture(GL_TEXTURE_2D, accumulator->color_buffer.value());
	}
	else
	{
		// use framebuffer as input
		glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_BINDING);
		glBindTexture(GL_TEXTURE_2D, input->color_buffer.value());
	}

	glActiveTexture(GL_TEXTURE0 + OFFSETMASK_BINDING);
	glBindTexture(GL_TEXTURE_2D, effect_mask);

	glActiveTexture(GL_TEXTURE0 + PIXELSORTER_BINDING);
	glBindTexture(GL_TEXTURE_2D, output);
}