#pragma once


class Editor : public Module 
{
public:
	Editor() { name = "Editor"; }

	virtual void init() override;
	virtual void tick(float _delta) override;
	virtual void shutdown() override;

	template<typename T>
	void register_window(std::shared_ptr<T> _window)
	{
		static_assert(std::is_base_of_v<EditorWindow, T>);
		register_window(std::dynamic_pointer_cast<EditorWindow>(_window));
	}
	void register_window(std::shared_ptr<EditorWindow> _window);

	const std::string& get_name() const { return name; }
	virtual const std::string& get_title() override { return get_name(); };

	virtual void interface_window() override;

protected:
	void clean_up();

	std::vector<std::shared_ptr<EditorWindow>> registered_windows;
	std::unordered_map<std::string, bool> opened_windows;

};