#include "precomp.h"

EditorCameraController::EditorCameraController()
{
}

EditorCameraController::~EditorCameraController()
{
}

void EditorCameraController::init()
{ 
	auto camera = engine.get_active_camera();
	
	// set yaw and pitch
	pitch = glm::pitch(camera->transform.GetRotation());
	yaw = glm::yaw(camera->transform.GetRotation()) + glm::pi<float>();

	const float correction_pitch{ pitch / glm::half_pi<float>() * glm::pi<float>() };
	pitch += pitch > 0.f ? -correction_pitch : correction_pitch;
}

void EditorCameraController::update(float _delta)
{
	auto camera = engine.get_active_camera();

	ImGui::Begin("Camera");
	ImGui::SliderFloat("speed", &speed, 0.1f, 5.f);
	ImGui::SliderFloat("pitch", &pitch, -glm::pi<float>(), glm::pi<float>());
	ImGui::SliderFloat("yaw", &yaw, -glm::pi<float>(), glm::pi<float>());


	glm::vec3 position = camera->transform.GetTranslation();
	glm::quat rotation = camera->transform.GetRotation();

	// need to update these every frame
	const glm::vec2 mouse_pos{ input.get_mouse_position() };
	const glm::vec2 delta_mouse{ mouse_pos - prev_mouse_pos };
	prev_mouse_pos = mouse_pos;

	const float wheel{ input.get_mouse_wheel() };
	const float delta_wheel{ wheel - prev_wheel };
	prev_wheel = wheel;

	if (input.get_mouse_button(MouseButton::Right))
	{

		glm::vec3 forward{ glm::eulerAngles(camera->transform.GetRotation()) };
		glm::vec3 right{ glm::cross(forward, glm::vec3(0, 1, 0)) };
		glm::vec3 up{ glm::cross(forward, right) };

		// position
		const float base_speed{ 25.f };
		position += base_speed * (forward * speed) * (float)input.get_keyboard_key(KeyboardKey::W) * _delta;
		position -= base_speed * (forward * speed) * (float)input.get_keyboard_key(KeyboardKey::S) * _delta;
		position -= base_speed * (right * speed) * (float)input.get_keyboard_key(KeyboardKey::A) * _delta;
		position += base_speed * (right * speed) * (float)input.get_keyboard_key(KeyboardKey::D) * _delta;
		position -= base_speed * (up * speed) * (float)input.get_keyboard_key(KeyboardKey::R) * _delta;
		position += base_speed * (up * speed) * (float)input.get_keyboard_key(KeyboardKey::F) * _delta;

		// rotation
		pitch -=	(delta_mouse.y * 0.5f) * _delta;
		yaw -=		(delta_mouse.x * 0.5f) * _delta;

		//speed control
		speed += delta_wheel * 0.1f;
	}

	pitch = glm::clamp(pitch, -glm::radians(89.f), glm::radians(89.f));

	rotation = glm::vec3{
		glm::cos(pitch) * glm::sin(yaw),
		glm::sin(pitch),
		glm::cos(pitch) * glm::cos(yaw) };

	ImGui::InputFloat3("moved to", glm::value_ptr(position));
	ImGui::InputFloat4("rotated", glm::value_ptr(rotation));

	ImGui::End();

	camera->transform.SetTranslation(position);
	camera->transform.SetRotation(rotation);
}
