#include "ambient_sounds.h"

#include "aud_player.h"
#include "engine.h"
#include "gameinfo.h"
#include "resources.h"
#include "utils.h"

#define NON_LOOPING_SOUNDS 25
#define LOOPING_SOUNDS      3

ambient_sounds_t::ambient_sounds_t(engine_t *engine)
	: engine(engine)
{
	non_looping_sounds = new non_looping_sound_t[NON_LOOPING_SOUNDS];

	for (size_t i = 0; i != NON_LOOPING_SOUNDS; ++i)
	{
		non_looping_sounds[i].is_active = false;
		non_looping_sounds[i].mixer_id  = -1;
		// non_looping_sounds[i].field_45 = 0;
	}

	looping_sounds     = new looping_sound_t[LOOPING_SOUNDS];

	for (size_t i = 0; i != LOOPING_SOUNDS; ++i)
	{
		looping_sounds[i].is_active = false;
		looping_sounds[i].mixer_id  = -1;
	}
}

void ambient_sounds_t::add_sound(int id, int time1, int time2, int volume1, int volume2, int a6, int a7, int a8, int a9, int a10, int a11)
{
	const char *sound_name = engine->gameinfo()->get_sfx_track_name(id);
	add_sound_by_name(sound_name, time1, time2, volume1, volume2);
}

void ambient_sounds_t::add_looping_sound(int id, int volume, int a3, int a4)
{
	name_t   name = engine->gameinfo()->get_sfx_track_name(id);
	uint32_t hash = name.hash();

	if (find_looping_track_by_hash(hash) >= 0)
		return;

	int track_id = find_available_looping_track();

	looping_sound_t &track = looping_sounds[track_id];

	track.is_active = true;
	track.name      = name;
	track.hash      = track.name.hash();

	engine->get_aud_player()->play_aud(track.name, volume, AUD_OVERRIDE_VOLUME | AUD_LOOP);
}

void ambient_sounds_t::tick()
{
	uint32_t cur_time = engine->get_time();

	for (int i = 0; i != NON_LOOPING_SOUNDS; ++i)
	{
		non_looping_sound_t &track = non_looping_sounds[i];

		if (!track.is_active || cur_time < track.next_play_time)
			continue;

		int volume = rand_in_range(track.volume1, track.volume2);

		engine->get_aud_player()->play_aud(track.name, volume, AUD_OVERRIDE_VOLUME);

		track.next_play_time = cur_time + rand_in_range(track.time1, track.time2);
	}
}

void ambient_sounds_t::add_sound_by_name(const char *sound_name, int time1, int time2, int volume1, int volume2)
{
	int track_id = find_available_non_looping_track();
	if (track_id < 0)
		return;

	non_looping_sound_t &track = non_looping_sounds[track_id];

	track.is_active = true;
	track.name      = sound_name;
	track.hash      = track.name.hash();

	track.time1     = 1000 * time1;
	track.time2     = 1000 * time2;

	track.volume1   = volume1;
	track.volume2   = volume2;

	track.next_play_time = engine->get_time() + rand_in_range(track.time1, track.time2);
}

int ambient_sounds_t::find_available_non_looping_track()
{
	for (size_t i = 0; i != NON_LOOPING_SOUNDS; ++i)
		if (!non_looping_sounds[i].is_active)
			return i;

	return -1;
}

int ambient_sounds_t::find_non_looping_track_by_hash(uint32_t hash)
{
	for (size_t i = 0; i != NON_LOOPING_SOUNDS; ++i)
		if (non_looping_sounds[i].is_active && non_looping_sounds[i].hash == hash)
			return i;

	return -1;
}

int ambient_sounds_t::find_available_looping_track()
{
	for (size_t i = 0; i != LOOPING_SOUNDS; ++i)
		if (!looping_sounds[i].is_active)
			return i;

	return -1;
}

int ambient_sounds_t::find_looping_track_by_hash(uint32_t hash)
{
	for (size_t i = 0; i != LOOPING_SOUNDS; ++i)
		if (looping_sounds[i].is_active && looping_sounds[i].hash == hash)
			return i;

	return -1;
}
