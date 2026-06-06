#pragma once

class ModuleBase
{
public:
	ModuleBase() = default;
	virtual ~ModuleBase() {}

	virtual void init() {}

	virtual void tick(float _delta) {}
	virtual void fixed_tick() {}

	virtual void shutdown() {}

private:
};