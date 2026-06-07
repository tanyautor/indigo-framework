
#include "precomp.h"

class GameModule : public Module
{
public:
	GameModule() { log(Severity::INFO, "GameModule registered."); };
	~GameModule() {};

	virtual void init() override;
	virtual void tick(float _delta) override;
	virtual void fixed_tick() override;
	virtual void shutdown() override;

private:


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
}

void GameModule::tick(float _delta)
{
}

void GameModule::fixed_tick()
{
}
void GameModule::shutdown()
{
	log(Severity::INFO, "GameModule shutdown.");
}