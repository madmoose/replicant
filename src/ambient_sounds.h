#ifndef AMBIENT_SOUNDS_H
#define AMBIENT_SOUNDS_H

#include "name.h"

#include <cstddef>
#include <cstdint>

class engine_t;

struct non_looping_sound_t
{
	bool     is_active;
	name_t   name;
	uint32_t hash;
	uint32_t time1;
	uint32_t time2;
	uint32_t next_play_time;
	uint32_t volume1;
	uint32_t volume2;
	uint32_t volume;
	uint32_t mixer_id;
};

struct looping_sound_t
{
	bool     is_active;
	name_t   name;
	uint32_t hash;
	uint32_t mixer_id;
	uint32_t volume;
};

class ambient_sounds_t
{
	engine_t *engine;

	non_looping_sound_t *non_looping_sounds;
	looping_sound_t     *looping_sounds;

public:
	ambient_sounds_t(engine_t *engine);

	void add_sound(int id, int time1, int time2, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11);
	void add_sound_by_name(const char *sound_name, int time1, int time2, int volume1, int volume2);

	void add_looping_sound(int id, int volume, int a3, int a4);

	void tick();

	int  find_available_non_looping_track();
	int  find_non_looping_track_by_hash(uint32_t hash);

	int  find_available_looping_track();
	int  find_looping_track_by_hash(uint32_t hash);
};

#endif
