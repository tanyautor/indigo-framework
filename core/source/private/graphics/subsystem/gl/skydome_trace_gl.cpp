#include "precomp.h"

SkydomeTrace::SkydomeTrace(Camera* _camera) : camera(_camera)
{
	skydome_trace_shader = Shader("shared/shaders/skydome_trace.vert", "shared/shaders/skydome_trace.frag");

	// Framebuffer 
	framebuffer = new Framebuffer("SkydomeTrace FBO");

	// testing everything, probs only need color and render in the end
	framebuffer->init_color_buffer(GL_TEXTURE_2D);
	framebuffer->init_depth_stencil(GL_RENDERBUFFER);

	// CameraData
	camera_data = new CameraDataUBO();	
	glm::mat4 view = glm::lookAt(camera->transform.GetTranslation(), camera->transform.GetTranslation() + glm::eulerAngles(camera->transform.GetRotation()), glm::vec3(0, 1, 0));
	camera_data->view = view;
	camera_data->projection = camera->projection;
	camera_data->vp = camera->projection * view;

	glGenBuffers(1, &camera_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraDataUBO), camera_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_UBO, camera_ubo);
	label_gl(GL_BUFFER, camera_ubo, "SkydomeTrace CameraDataUBO");

	// texture size
	std::string filename{ "shared/textures/sky_00.hdr" };
	int x,y,bpp; // bytes per pixel
	float* src = stbi_loadf(filename.c_str(), &x, &y, &bpp, 0);
	if (!src)
	{
		tanlog::log(tanlog::ERROR, "failed to load skydome file {}", filename);
	}
	skydome_width = x;
	skydome_height = y;

	glGenTextures(1, &skydome);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skydome);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(bpp == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, skydome_width, skydome_height, 0, GL_RGBA,
			GL_FLOAT, src);
	}
	else if(bpp == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, skydome_width, skydome_height, 0, GL_RGB,
			GL_FLOAT, src);
	}
	stbi_image_free(src);

	label_gl(GL_TEXTURE, skydome, "SkydomeTrace Skydome");

	glCheckError();
}

SkydomeTrace::~SkydomeTrace()
{
}

void SkydomeTrace::base_pass()
{
	//Draw to Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	skydome_trace_shader.use_shader();

	// update camera
	glm::mat4 view = glm::lookAt(camera->transform.GetTranslation(), camera->transform.GetTranslation() + glm::eulerAngles(camera->transform.GetRotation()), glm::vec3(0, 1, 0));
	camera_data->view = view;
	camera_data->projection = camera->projection;
	camera_data->vp = camera->projection * view;
	glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraDataUBO), camera_data, GL_DYNAMIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skydome);
	screen_pass_trace();


	// Screen pass to default framebuffer
	engine.get_renderer().bind_default_framebuffer();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer->color_buffer.value());
	draw_quad();
}

void SkydomeTrace::post_process_pass()
{
}

void SkydomeTrace::screen_pass_trace()
{
	static float quadVertices[] = {
		// positions // texCoords
		-1.f,  -1.f,  0.0f, 0.0f,
		 1.f,  -1.f,  1.0f, 0.0f,
		-1.f,   1.f,  0.0f, 1.0f,

		-1.f,   1.f,  0.0f, 1.0f,
		 1.f,  -1.f,  1.0f, 0.0f,
		 1.f,   1.f,  1.0f, 1.0f,
	};

	static unsigned int quadVAO{ 0 }, quadVBO{ 0 };
	if (!quadVAO)
	{
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}

	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glCheckError();
}
