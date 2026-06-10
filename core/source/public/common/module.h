#pragma once

class Module : public EditorWindow
{
public:
	Module() = default;
	virtual ~Module() {}

	virtual void init() { register_self(); }

	virtual void tick(float _delta) {}
	virtual void fixed_tick() {}

	virtual void shutdown() {}

	const std::string& get_name() const { return name; }
	virtual const std::string& get_title() override { return get_name(); };

protected:
	std::string name{ "Module" };

	void register_self();
};
