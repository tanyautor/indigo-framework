#include "precomp.h"

int main()
{
	// game code???? 
	engine.register_module<IndigoPixelSort>();

	engine.init();
	engine.run();
	engine.shutdown();

	return 0;
}