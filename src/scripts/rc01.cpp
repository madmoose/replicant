#include "scripts\rc01.h"

#include "engine.h"

#include <cstdio>

void script_rc01_t::initialize_scene()
{
	outtake_play(28, -1, -1);
	outtake_play(41, -1, -1);
	outtake_play( 0,  0, -1);
	outtake_play(33, -1, -1);

	set_loop_special(0, false);
	set_loop_default(1);

	ambient_sounds_add_looping_sound(54, 30, 0, 1);

	ambient_sounds_add_sound(181, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(182, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(183, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(184, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(185, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(186, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(188, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(189, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(190, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(191, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(192, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(193, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(194, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(195, 5, 70, 12, 12, -100, 100, -101, -101, 0, 0);

	ambient_sounds_add_looping_sound(81, 60, 100, 1);

	ambient_sounds_add_sound(82,  5, 30, 30, 50, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(83,  5, 30, 30, 55, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(84,  5, 30, 30, 50, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(67, 10, 50, 30, 50, -100, 100, -101, -101, 0, 0);
	ambient_sounds_add_sound(87, 20, 80, 20, 40, -100, 100, -101, -101, 0, 0);

	actor_voice_over(1830, 99);
	actor_voice_over(1850, 99);
	actor_voice_over(1860, 99);
}

void script_rc01_t::scene_frame_advanced(int frame)
{
	if (frame == 1)
		sound_play(118, 40, 0, 0, 50);

	if (frame == 61 || frame == 362)
		sound_play(116, 100, 80, 80, 50);

	if (frame == 108 || frame == 409)
		sound_play(119, 100, 80, 80, 50);

	if (frame == 183 || frame == 484)
		sound_play(116, 100, 80, 80, 50);

	if (frame == 228 || frame == 523)
		sound_play(119, 100, 80, 80, 50);

	if (frame == 243 || frame == 545)
		sound_play(117, 40, 80, 80, 50);

	if (frame == 315)
		sound_play(118, 40, 80, 80, 50);
}
