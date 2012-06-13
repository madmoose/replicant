#include "engine.h"
#include "resources.h"

#include "platform/win32/platform.h"

#include <cstdio>

int main()
{
	platform_t *platform = new platform_t;
	engine_t *engine     = new engine_t;

	if (!platform->init(engine))
	{
		puts("Failed to initialize platform");
		return 1;
	}

	/* hackety hack hack */
	/* These are the resource files needed for the intro */
	resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\OUTTAKE1.MIX");
	resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\SPCHSFX.TLK");
	resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\STARTUP.MIX");
	resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\VQA1.MIX");

	/*
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\1.TLK");
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\2.TLK");
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\3.TLK");
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\A.TLK");
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\MODE.MIX");
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\OUTTAKE2.MIX");
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\OUTTAKE3.MIX");
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\VQA2.MIX");
		resource_manager.open_resource_file("C:\\WESTWOOD\\BLADE\\VQA3.MIX");
	*/

	engine->init(platform);
	if (!engine->run())
		return 1;

	delete engine;

	platform->shutdown();
	delete platform;
}
