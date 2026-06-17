#define INDIGO_EDITOR

#include "precomp.h"

class GameModule : public Module
{
public:
	GameModule() { name = "GameModule"; };

	virtual void init() override;
	virtual void tick(float _delta) override;
	virtual void fixed_tick() override;
	virtual void shutdown() override;

	virtual void interface_window() override;

	std::shared_ptr<Model> mrvoyage;
	std::shared_ptr<Model> leg;
	std::shared_ptr<Model> cube;
protected:


};

int main()
{
	// Register GameModule & Init
	engine.register_module<GameModule>();
	auto game = engine.get_module<GameModule>();
	if(game)
	{
		log(Severity::Info, "GameModule found!!");
	}

	engine.init();

	// Main Loop
	engine.run();

	// Shutdown
	engine.shutdown();

	return 0;
}

void GameModule::init()
{
	log(Severity::Info, "GameModule init.");
	register_self();

	engine.get_renderer()->register_subsystem<SkydomeTrace>();
	engine.get_renderer()->register_subsystem<MeshRenderer>();


	mrvoyage = engine.get_resource_manager()->create_resource<Model>(FileStream::Directory::Shared, "models/Bomb Voyage/boss_bombvoyage_BV_bind.obj");
	leg = engine.get_resource_manager()->create_resource<Model>(FileStream::Directory::Shared, "models/LEG/LEG.obj");
	cube = engine.get_resource_manager()->create_resource<Model>(FileStream::Directory::Shared, "models/Cube/Cube.gltf");

	engine.get_renderer()->get_subsystem<MeshRenderer>()->models.push_back(mrvoyage);
	engine.get_renderer()->get_subsystem<MeshRenderer>()->models.push_back(leg);
	engine.get_renderer()->get_subsystem<MeshRenderer>()->models.push_back(cube);

	leg->transform.SetTranslation({ -10,10,0 });
	leg->transform.SetScale({ 5.f,5.f,5.f });
	cube->transform.SetTranslation({ 10,0,0 });
}

void GameModule::tick(float _delta)
{
	//for(int i = 0; i < 10; i++)
	//{
	//	auto tmp_voyage = engine.get_resource_manager()->load_resource<Model>(FileStream::Directory::Shared, "models/Bomb Voyage/boss_bombvoyage_BV_bind.obj");
	//	engine.get_renderer()->get_subsystem<MeshRenderer>()->models.push_back(tmp_voyage);
	//}
}

void GameModule::fixed_tick()
{
}
void GameModule::shutdown()
{
	log(Severity::Info, "GameModule shutdown.");
}

void GameModule::interface_window()
{
#ifdef INDIGO_EDITOR


	ImGui::Text("Hello! I aam Game;");
#endif // INDIGO_EDITOR
}