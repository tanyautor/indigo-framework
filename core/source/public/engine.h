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
	std::shared_ptr<Camera> get_active_camera() const { return active_camera; }
	std::shared_ptr<Window> get_window() const { return window; }
	std::shared_ptr<Renderer> get_renderer() const { return renderer; }

	// register module to be called on in tick, fixed_tick, etc...
	template<typename Derived, typename ...Args>
	void register_module(const Args&... _args)
	{
		static_assert(!std::is_class_v<Module> || std::is_base_of_v<Module, Derived>);
		modules.push_back(std::make_shared<Derived>(_args...));
	}

private:
	void tick(float _delta);
	void fixed_tick();

	std::shared_ptr<Camera> active_camera;
	std::shared_ptr<Window> window{ nullptr };
	std::shared_ptr<Renderer> renderer{ nullptr };

	std::vector<std::shared_ptr<Module>> modules;

	float delta_time_world{ 0.f };
	float fixed_timestep{ 1.f / 60.f };
};

extern Engine engine;