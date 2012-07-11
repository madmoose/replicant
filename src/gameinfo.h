#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <cstdint>

class gameinfo_t
{
	uint32_t actor_count;
	uint32_t flag_count;
	uint32_t global_var_count;
	uint32_t set_names_count;
	uint32_t initial_scene_id;
	uint32_t initial_set_id;
	uint32_t waypoint_count;
	uint32_t sfx_track_count;
	uint32_t music_track_count;
	uint32_t outtake_count;
	uint32_t cover_waypoint_count;
	uint32_t flee_waypoint_count;

	char (*set_names)[5];
	char (*sfx_tracks)[13];
	char (*music_tracks)[13];
	char (*outtakes)[13];
public:
	bool read_gameinfo();

	uint32_t get_initial_scene_id() { return initial_scene_id; }
	uint32_t get_initial_set_id()   { return initial_set_id; }

	const char *get_set_name(uint32_t idx);
	const char *get_sfx_track_name(uint32_t idx);
	const char *get_music_track_name(uint32_t idx);
	const char *get_outtake_name(uint32_t idx);
};

#endif
