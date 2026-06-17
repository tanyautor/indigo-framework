#pragma once


struct Light : public EditorInterface, public Resource
{
	Light() : Resource(ResourceType::Light) {}

	// Light attenuation stuff
	vec3 color;
	float intensity;

	// can draw position/direction from this. Ignore if needed
	Transform transform;

	enum class Type : int32
	{
		PointLight,
		DirectionalLight,
	};

	virtual const std::string& get_name() override { return path; }
	virtual void interface_component() override;
};

//IMGUI_REFLECT(Light, color, intensity, transform);
//
//void Light::interface_component()
//{
//	ImReflect::Input("Directional Light", this);
//}
