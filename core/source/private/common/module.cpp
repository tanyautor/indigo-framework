#include "precomp.h"

void Module::register_self()
{
#ifdef INDIGO_EDITOR
	engine.get_module<Editor>()->register_window(engine.get_module(this));
#endif
}
