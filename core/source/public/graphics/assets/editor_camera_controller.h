#pragma once


class EditorCameraController
{
public:
	EditorCameraController();
	~EditorCameraController();

	void init(Camera* _to_control);
	void update(float _delta);

private:
	Camera* camera{ nullptr };

	glm::vec2 prev_mouse_pos{ 0,0 };
	float pitch{ 0.f };
	float yaw{ 0.f };
	float prev_wheel{ 0.f };

	float speed{ 1.f };
};