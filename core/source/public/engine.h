#pragma once


class Engine
{
public:
	Engine();
	~Engine();

	void init();
	void run();
	void shutdown();

	const float get_delta_time() const { return delta_time_world; }
	Camera& get_editor_camera() { return editor_camera; }
	Window& get_window() const { return *window; }
	Renderer& get_renderer() const { return *renderer; }

	// register module to be called on in tick, fixed_tick, etc...
	template<typename Derived, typename ...Args>
	void register_module(const Args&... _args)
	{
		static_assert(!std::is_class_v<ModuleBase> || std::is_base_of_v<ModuleBase, Derived>);

		modules.push_back(new Derived(_args...));
	}

private:
	void tick(float _delta);
	void fixed_tick();

	Camera editor_camera;
	Window* window{ nullptr };
	Renderer* renderer{ nullptr };

	std::vector<ModuleBase*> modules;

	float delta_time_world{ 0.f };
	float fixed_timestep{ 1.f / 60.f };
};

extern Engine engine;