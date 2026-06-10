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
protected:


};

int main()
{
	// Register GameModule & Init
	engine.register_module<GameModule>();
	auto game = engine.get_module<GameModule>();
	if(game)
	{
		log(Severity::INFO, "GameModule found!!");
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
	log(Severity::INFO, "GameModule init.");
	register_self();

	engine.get_renderer()->register_subsystem<SkydomeTrace>();
	engine.get_renderer()->register_subsystem<MeshRenderer>();


	mrvoyage = engine.get_resource_manager()->create_resource<Model>(FileStream::Directory::Shared, "models/Bomb Voyage/boss_bombvoyage_BV_bind.obj");
	leg = engine.get_resource_manager()->create_resource<Model>(FileStream::Directory::Shared, "models/LEG/LEG.obj");
	engine.get_renderer()->get_subsystem<MeshRenderer>()->models.push_back(mrvoyage);
	engine.get_renderer()->get_subsystem<MeshRenderer>()->models.push_back(leg);

	leg->transform.SetTranslation({ -10,10,0 });
	leg->transform.SetScale({ 5.f,5.f,5.f });
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
	log(Severity::INFO, "GameModule shutdown.");
}

void GameModule::interface_window()
{
#ifdef INDIGO_EDITOR


	ImGui::Text("Hello! I aam Game;");
#endif // INDIGO_EDITOR
}