#pragma once

class EditorInterface
{
public:
	// return std::string("name"), no need to make this a member
	virtual const std::string get_name() = 0;
	virtual void interface_component() = 0;
};

class EditorWindow
{
public:
	// return std::string("name"), no need to make this a member
	virtual const std::string get_title() = 0;
	virtual void interface_window() = 0;

	void register_interface(std::shared_ptr<EditorInterface> _interface);
	void interface_components();

protected:
	void clean_up();
	std::vector<std::shared_ptr<EditorInterface>> registered_interfaces;
};