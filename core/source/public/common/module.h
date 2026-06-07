#pragma once

class Module
{
public:
	Module() = default;
	virtual ~Module() {}

	virtual void init() {}

	virtual void tick(float _delta) {}
	virtual void fixed_tick() {}

	virtual void shutdown() {}

	const std::string& get_name() const { return name; }

protected:
	std::string name{ "Module" };

};