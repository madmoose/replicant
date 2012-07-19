#include "script.h"

#include "ambient_sounds.h"
#include "engine.h"
#include "scripts\rc01.h"

#include <cstring>

class do_enter_script_t
{
	engine_t *engine;
public:
	do_enter_script_t(engine_t *engine)
		: engine(engine)
	{
		engine->enter_script();
	}
	~do_enter_script_t()
	{
		engine->leave_script();
	}
};

void script_t::set_loop_special(int loop, bool wait)
{
	engine->set_loop_special(loop, wait);
}

void script_t::set_loop_default(int loop)
{
	engine->set_loop_default(loop);
}

void script_t::outtake_play(int id, int a2, int a3)
{
	engine->outtake_play(id, a2, a3);
}

void script_t::actor_voice_over(int sentence_id, int actor_id)
{
	engine->actor_voice_over(sentence_id, actor_id);
}

void script_t::sound_play(int id, int volume, int a3, int a4, int a5)
{
	engine->sound_play(id, volume, a3, a4, a5);
}

void script_t::ambient_sounds_add_sound(int id, int time1, int time2, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11)
{
	engine->get_ambient_sounds()->add_sound(id, time1, time2, a4, a5, a6, a7, a8, a9, a10, a11);
}

void script_t::ambient_sounds_add_looping_sound(int id, int volume, int a3, int a4)
{
	engine->get_ambient_sounds()->add_looping_sound(id, volume, a3, a4);
}

#define SCRIPT(script_name, type) \
	if (strcmp(name, script_name) == 0) return new type(engine)

script_t *get_script_by_name(engine_t *engine, const char *name)
{
	SCRIPT("RC01", script_rc01_t);

	return 0;
}
