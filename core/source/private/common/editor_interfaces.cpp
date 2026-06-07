#include "precomp.h"

void EditorWindow::register_interface(std::shared_ptr<EditorInterface> _interface)
{
	registered_interfaces.push_back(_interface);
}
void EditorWindow::interface_components()
{
	for (auto interface : registered_interfaces)
	{
		interface->interface_component();
	}
	clean_up();
}
void EditorWindow::clean_up()
{
	// clean up expired pointers
	auto it = registered_interfaces.begin();
	for (auto interface : registered_interfaces)
	{
		if (interface.use_count() <= 1)
		{
			registered_interfaces.erase(it);
		}
		it++;
	}
}