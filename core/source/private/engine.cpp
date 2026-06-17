#include "precomp.h"


Engine engine;

void Engine::init()
{
	window = std::make_shared<Window>(1280, 768, "bomb voyage stole me wallet >:(", false);
	file_io = std::make_shared<FileIO>();
	register_module<Renderer>();
	renderer = get_module<Renderer>();
	active_camera = std::make_shared<Camera>();

	register_module<ResourceManager>();
	resource_manager = get_module<ResourceManager>();

#ifdef INDIGO_EDITOR
	register_module<Editor>();
	register_module<EditorCameraController>();

#endif // INDIGO_EDITOR


	for (const auto& module : modules) { module->init(); }
}

void Engine::run()
{
	Timer delta_timer;
	float dt_total = 0.f;
	float dt_acc = 0.f;

	while(window->is_running())
	{
		window->begin_frame();

		// rendering
		renderer->render();

		// logic
		// giving delta_world to modules, since delta_engine should be reserved for edge cases
		tick(delta_time_world);
		while(dt_acc > fixed_timestep)
		{
			fixed_tick();
			dt_acc -= fixed_timestep;
		}

		window->end_frame();

		delta_time_engine = delta_timer.elapsed() > fixed_timestep ? fixed_timestep : delta_timer.elapsed();
		delta_time_world = delta_time_engine * global_time_dilation;

		dt_total += delta_time_engine;
		dt_acc += delta_time_engine;
		delta_timer.reset();
	}
}

void Engine::shutdown()
{
	for (size_t i = 0; i < modules.size(); i++)
	{
		modules[i]->shutdown();
	}

	bool empty = modules.empty();

}

void Engine::tick(float _delta)
{
	input.update();
	
	for (const auto& module : modules)
	{		
		module->tick(_delta);
	}

	renderer->tick(_delta);
}

void Engine::fixed_tick()
{
	for (const auto& module : modules)
	{
		module->fixed_tick();
	}
}