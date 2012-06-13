#include "scripts\rc01.h"

#include "engine.h"

void script_rc01_t::initialize_scene()
{
	outtake_play(28, -1, -1);
	outtake_play(41, -1, -1);
	outtake_play( 0,  0, -1);
	outtake_play(33, -1, -1);

	set_loop_special(0, false);
	set_loop_default(1);

	actor_voice_over(1830, 99);
	actor_voice_over(1850, 99);
	actor_voice_over(1860, 99);
}
