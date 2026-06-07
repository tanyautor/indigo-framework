#pragma once


class EditorCameraController : public Module, public EditorWindow
{
public:
	EditorCameraController() { name = "EditorCameraController"; }

	virtual void init() override;
	virtual void tick(float _delta) override;
	virtual void shutdown() override;

	virtual void interface_window() override;
	virtual const std::string get_title() override { return name; }

protected:
	glm::vec2 prev_mouse_pos{ 0,0 };
	float pitch{ 0.f };
	float yaw{ 0.f };
	float prev_wheel{ 0.f };

	float speed{ 1.f };
};