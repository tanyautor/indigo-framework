#include "precomp.h"


Engine engine;

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::init()
{
	window = std::make_shared<Window>(1280, 768, "bomb voyage stole me wallet >:(", false);
	renderer = std::make_shared<Renderer>();

	active_camera = std::make_shared<Camera>();
	if (!FileHandler::load_bin("editor_camera", active_camera.get(), sizeof(Camera)))
	{
		active_camera->projection = glm::perspective(glm::radians(45.f), (float)window->get_window_size().x / (float)window->get_window_size().y, 0.1f, 100.f);
		active_camera->transform.SetTranslation(glm::vec3(0, 10, 20));
		active_camera->transform.SetRotation(glm::vec3(0, 0, -1));
	}
	active_camera->transform.name = "Editor Camera";

	for (const auto& module : modules) { module->init(); }
}

void Engine::run()
{
	Timer delta_timer;
	float dt_total = 0.f;
	float dt_acc = 0.f;
	float dt_prev = 0.f;

	while(window->is_running())
	{
		window->begin_frame();

		// logic
		tick(dt_prev);
		while(dt_acc > fixed_timestep)
		{
			fixed_tick();
			dt_acc -= fixed_timestep;
		}

		// rendering
		renderer->render();

		window->end_frame();

		dt_prev = delta_timer.elapsed() > fixed_timestep ? fixed_timestep : delta_timer.elapsed();
		dt_total += dt_prev;
		dt_acc += dt_prev;
		delta_time_world = dt_prev;
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

	FileHandler::save_bin("editor_camera", active_camera.get(), sizeof(Camera));
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