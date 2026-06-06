#include "precomp.h"

MeshRenderer::MeshRenderer()
{
	priority = 1 << 2;

	forward_pass = Shader("shared/shaders/static_mesh.vert", "shared/shaders/static_mesh.frag");

	forward_pass.use_shader();

	trans_data = new TransformUBO();
	camera_data = new CameraDataUBO();
	directional_light_data = new DirectionalLightUBO();
	point_light_data = new PointLightUBO();

	float tmp[7];
	if (FileHandler::load_bin("model_renderer_directional_light", &tmp, sizeof(tmp)))
	{
		directional_light_data->directional_light.color[0] = tmp[0];
		directional_light_data->directional_light.color[1] = tmp[1];
		directional_light_data->directional_light.color[2] = tmp[2];
		directional_light_data->directional_light.direction[0] = tmp[3];
		directional_light_data->directional_light.direction[1] = tmp[4];
		directional_light_data->directional_light.direction[2] = tmp[5];
		directional_light_data->directional_light.intensity = tmp[6];
	}
	if (FileHandler::load_bin("model_renderer_point_light", &tmp, sizeof(tmp)))
	{
		point_light_data->point_light.color[0] = tmp[0];
		point_light_data->point_light.color[1] = tmp[1];
		point_light_data->point_light.color[2] = tmp[2];
		point_light_data->point_light.position[0] = tmp[3];
		point_light_data->point_light.position[1] = tmp[4];
		point_light_data->point_light.position[2] = tmp[5];
		point_light_data->point_light.intensity = tmp[6];
	}

	glGenBuffers(1, &trans_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, trans_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(TransformUBO), trans_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORM_UBO, trans_ubo);
	label_gl(GL_BUFFER, trans_ubo, "MeshRenderer TransformUBO");
	glCheckError();

	glGenBuffers(1, &camera_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraDataUBO), camera_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_UBO, camera_ubo);
	label_gl(GL_BUFFER, camera_ubo, "MeshRenderer CameraUBO");
	glCheckError();

	glGenBuffers(1, &directional_light_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, directional_light_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLightUBO), directional_light_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, DIRECTIONAL_LIGHT_UBO, directional_light_ubo);
	label_gl(GL_BUFFER, directional_light_ubo, "MeshRenderer DirectionalLightUBO");
	glCheckError();

	glGenBuffers(1, &point_light_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, point_light_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightUBO), point_light_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, POINT_LIGHT_UBO, point_light_ubo);
	label_gl(GL_BUFFER, point_light_ubo, "MeshRenderer PointLightUBO");
	glCheckError();

	// Framebuffer 
	framebuffer = new Framebuffer("MeshRenderer FBO");

	// testing everything, probs only need color and render in the end
	framebuffer->init_color_buffer(GL_TEXTURE_2D);
	framebuffer->init_depth_stencil(GL_RENDERBUFFER);

	if (!framebuffer->check_complete())
	{
		log(ERROR, "framebuffer failed to init, deleting it now :3");
		delete framebuffer;
		framebuffer = nullptr;
	}
}

MeshRenderer::~MeshRenderer()
{
	glDeleteBuffers(1, &trans_ubo);
	glDeleteBuffers(1, &camera_ubo);
	glDeleteBuffers(1, &directional_light_ubo);

	float tmp_dir[]{ directional_light_data->directional_light.color[0] ,directional_light_data->directional_light.color[1] ,directional_light_data->directional_light.color[2], directional_light_data->directional_light.direction[0] ,directional_light_data->directional_light.direction[1] ,directional_light_data->directional_light.direction[2],directional_light_data->directional_light.intensity };
	FileHandler::save_bin("model_renderer_directional_light", &tmp_dir, sizeof(tmp_dir));
	float tmp_point[]{ point_light_data->point_light.color[0] ,point_light_data->point_light.color[1] ,point_light_data->point_light.color[2], point_light_data->point_light.position[0] ,point_light_data->point_light.position[1] ,point_light_data->point_light.position[2],point_light_data->point_light.intensity };
	FileHandler::save_bin("model_renderer_point_light", &tmp_point, sizeof(tmp_point));

	delete framebuffer;
	delete trans_data;
	delete camera_data;
	delete directional_light_data;
}

void MeshRenderer::base_pass()
{
	if (!framebuffer) return;

	auto camera = engine.get_active_camera();
	
	ImGui::Begin("Lights");
	ImReflect::Input("Directional Light", directional_light_data->directional_light);
	ImReflect::Input("Point Light", point_light_data->point_light);
	ImGui::End();

	//Draw to Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	forward_pass.use_shader();

	// update camera
	mat4 view = lookAt(camera->transform.GetTranslation(), camera->transform.GetTranslation() + eulerAngles(camera->transform.GetRotation()), vec3(0, 1, 0));
	camera_data->view = view;
	camera_data->projection = camera->projection;
	camera_data->vp = camera->projection * view;
	glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraDataUBO), camera_data, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, directional_light_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLightUBO), directional_light_data, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, point_light_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightUBO), point_light_data, GL_DYNAMIC_DRAW);

	for (auto& mesh : meshes)
	{
		// update mesh and wvp matrices... look into instancing later pls
		trans_data->mesh.world = mesh.transform.World();
		trans_data->mesh.wvp = camera_data->vp * mesh.transform.World();
		glBindBuffer(GL_UNIFORM_BUFFER, trans_ubo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(TransformUBO), trans_data, GL_DYNAMIC_DRAW);
		mesh.render();
	}

	// Screen pass to default framebuffer
	engine.get_renderer()->bind_default_framebuffer();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer->color_buffer.value());
	draw_quad();
}

//void MeshRenderer::post_process_pass()
//{
//}


