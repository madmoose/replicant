#ifndef RC01_H
#define RC01_H

#include "../script.h"

struct script_rc01_t : script_t
{
	script_rc01_t(engine_t *engine)
		: script_t(engine)
	{}

	void initialize_scene();
};

#endif
