#pragma once


class Renderer
{
public:
	Renderer();
	~Renderer();

	void render()
	{

		begin_frame();

		for (const auto& subsystem : subsystems)
		{
			subsystem->base_pass();
		}
		for (const auto& subsystem : subsystems)
		{
			subsystem->post_process_pass();
		}
		

		end_frame();
	}

	void tick(float _delta)
	{
		for (const auto& subsystem : subsystems)
		{
			subsystem->tick(_delta);
		}
	}

	std::shared_ptr<Framebuffer> get_default_framebuffer();
	void bind_default_framebuffer();
	void bind_backbuffer();

	// registered Subsystems are sorted by priority, Lowest -> Highest priority
	template<typename Derived, typename ...Args>
	void register_subsystem(const Args&... _args)
	{
		static_assert(!std::is_class_v<Subsystem> || std::is_base_of_v<Subsystem, Derived>);
		
		subsystems.push_back(new Derived(_args...));
		std::sort(subsystems.begin(), subsystems.end(), [](Subsystem* _a, Subsystem* _b) { return _a->priority < _b->priority; });
	}

#pragma warning(push)
#pragma warning(disable:4715)
	template<typename Derived>
	Derived* get_subsystem()
	{
		static_assert(!std::is_class_v<Subsystem> || std::is_base_of_v<Subsystem, Derived>);
		
		for(Subsystem*& system : subsystems)
		{
			Derived* ret = dynamic_cast<Derived*>(system);
			if(ret != nullptr)
			{
				return ret;
			}
		}
		log(Fatal, "could not find subsystem of type {}", typeid(Derived).name());
		// this will likely crash :3
	}
#pragma warning( pop )

private:
	void begin_frame();
	void end_frame();

	struct Impl;
	std::unique_ptr<Impl> pImpl;

	std::vector<Subsystem*> subsystems;

	std::shared_ptr<Framebuffer> default_framebuffer;
};