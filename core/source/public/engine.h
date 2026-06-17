#pragma once


class Engine
{
public:
	// Games ;)

	void init();
	void run();
	void shutdown();

	// Timers & Time Manipulation

	const float get_engine_delta_time() const { return delta_time_world; }
	const float get_world_delta_time() const { return delta_time_world * fixed_timestep; }
	const float get_fixed_timestep() const { return fixed_timestep; }
	const float get_global_time_dilation() const { return fixed_timestep; }

	void set_fixed_timestep(const float _timestep) { fixed_timestep = _timestep; }
	void set_global_time_dilation(const float _time_dilation) { global_time_dilation = _time_dilation; }

	// Engine Components

	std::shared_ptr<Camera> get_active_camera() const { return active_camera; }
	std::shared_ptr<Window> get_window() const { return window; }
	std::shared_ptr<FileIO> get_file_io() const { return file_io; }
	std::shared_ptr<Renderer> get_renderer() const { return renderer; }
	std::shared_ptr<ResourceManager> get_resource_manager() const { return resource_manager; }

	// register module to be called on in tick, fixed_tick, etc...
	template<typename Derived, typename ...Args>
	void register_module(const Args&... _args)
	{
		static_assert(!std::is_class_v<Module> || std::is_base_of_v<Module, Derived>);
		modules.push_back(std::make_shared<Derived>(_args...));
	}

	template<typename Derived>
	std::shared_ptr<Derived> get_module()
	{
		static_assert(!std::is_class_v<Module> || std::is_base_of_v<Module, Derived>);

		for (const auto& module : modules)
		{
			auto derived_module = std::dynamic_pointer_cast<Derived>(module);
			if(derived_module)
			{
				return derived_module;
			}
		}
		log(Severity::Warning, "Failed to find module, dunno which one tho... please rewrite this function :(");
		return nullptr;
	}
	std::shared_ptr<Module> get_module(Module* _module = nullptr)
	{
		assert(_module);

		for (const auto& module : modules)
		{
			if(module && module.get() == _module)
			{
				return module;
			}
		}
		log(Severity::Warning, "Failed to find module, dunno which one tho... please rewrite this function :(");
		return nullptr;
	}


private:
	void tick(float _delta);
	void fixed_tick();

	std::shared_ptr<Camera> active_camera{ nullptr };
	std::shared_ptr<Window> window{ nullptr };
	std::shared_ptr<FileIO> file_io{ nullptr };
	std::shared_ptr<Renderer> renderer{ nullptr };
	std::shared_ptr<ResourceManager> resource_manager{ nullptr };

	std::vector<std::shared_ptr<Module>> modules;

	float delta_time_engine{ 0.f };
	float delta_time_world{ 0.f };
	float fixed_timestep{ 1.f / 60.f };

	float global_time_dilation{ 1.f };
};

extern Engine engine;