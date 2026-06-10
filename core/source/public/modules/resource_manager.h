#pragma once


class ResourceManager : public Module
{
public:
	ResourceManager() : Module() { name = "ResourceManager"; }

	virtual void init() override;

	virtual void tick(float _delta) override;
	virtual void fixed_tick() override;

	virtual void shutdown() override;

	virtual void interface_window() override;

	template<typename T, typename... Args>
	std::shared_ptr<T> load_resource(Args&&... _args);
	template<typename T, typename... Args>
	std::shared_ptr<T> create_resource(Args&&... _args);
	template<typename T, typename... Args>
	std::shared_ptr<T> find_resource(uint32 _id);

	void clean_up();
protected:
	// id from hashed string (file path)
	std::unordered_map<uint32, std::shared_ptr<Resource>> resources;

};


template<typename T, typename... Args>
std::shared_ptr<T> ResourceManager::load_resource(Args&&... _args)
{
	// try to find resource from string
	const std::string path = T::get_path(_args...);
	const uint32 id = static_cast<uint32>(std::hash<std::string>()(path));

	auto resource = find_resource<T>(id);
	if (resource) return resource;

	// cant find resource, create it    
	resources[id] = std::make_shared<T>(std::forward<Args>(_args)...);
	resources[id]->id = id;
	resources[id]->path = path;

	return std::dynamic_pointer_cast<T>(resources[id]);
}
template<typename T, typename... Args>
std::shared_ptr<T> ResourceManager::create_resource(Args&&... _args)
{
	auto res = std::make_shared<T>(std::forward<Args>(_args)...);
	const std::string& path = res->path;
	assert(!path.empty());  // Generated resources must have a path set in the constructor
	const uint32 id = static_cast<uint32>(std::hash<std::string>()(path));
	assert(!find_resource<T>(id));  // Resource with this id already exists (shouldn't happen)

	resources[id] = res;
	return std::dynamic_pointer_cast<T>(resources[id]);
}
template<typename T, typename... Args>
std::shared_ptr<T> ResourceManager::find_resource(uint32 _id)
{
	auto it = resources.find(_id);
	if (it != resources.end()) return std::dynamic_pointer_cast<T>(it->second);
	return std::shared_ptr<T>();
}