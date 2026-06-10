#pragma once

struct Camera : public EditorInterface
{
	glm::mat4 projection;
	Transform transform;

	virtual const std::string& get_name()
	{
		static std::string name{ "temporary camera name" };
		return name;
	};
	virtual void interface_component() {};
};