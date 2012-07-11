#include "gameinfo.h"

#include "reader.h"
#include "resources.h"

#include <cassert>
#include <cstring>

bool gameinfo_t::read_gameinfo()
{
	reader_t *r = resource_manager.get_resource_by_name("GAMEINFO.DAT");
	if (!r || !r->is_open())
		return false;

	r->read_le32(&actor_count);             /* 00 */
	r->seek_cur(4);                         /* 01 */
	r->read_le32(&flag_count);              /* 02 */
	r->seek_cur(4);                         /* 03 */
	r->read_le32(&global_var_count);        /* 04 */
	r->read_le32(&set_names_count);         /* 05 */
	r->read_le32(&initial_scene_id);        /* 06 */
	r->seek_cur(4);                         /* 07 */
	r->read_le32(&initial_set_id);          /* 08 */
	r->seek_cur(4);                         /* 09 */
	r->read_le32(&waypoint_count);          /* 10 */
	r->read_le32(&sfx_track_count);         /* 11 */
	r->read_le32(&music_track_count);       /* 12 */
	r->read_le32(&outtake_count);           /* 13 */
	r->seek_cur(4);                         /* 14 */
	r->seek_cur(4);                         /* 15 */
	r->read_le32(&cover_waypoint_count);    /* 16 */
	r->read_le32(&flee_waypoint_count);     /* 17 */


	set_names = new char[set_names_count][5];
	for (uint32_t i = 0; i != set_names_count; ++i)
	{
		r->read_bytes(set_names[i], 5);
		//printf("%3d: %s\n", i, set_names[i]);
	}

	sfx_tracks = new char[sfx_track_count][13];
	for (uint32_t i = 0; i != sfx_track_count; ++i)
	{
		r->read_bytes(sfx_tracks[i], 9);
		strcat(sfx_tracks[i], ".AUD");
		//printf("%3d: %s\n", i, sfx_tracks[i]);
	}

	music_tracks = new char[music_track_count][13];
	for (uint32_t i = 0; i != music_track_count; ++i)
	{
		r->read_bytes(music_tracks[i], 9);
		strcat(music_tracks[i], ".AUD");
		//printf("%s\n", music_tracks[i]);
	}

	outtakes = new char[outtake_count][13];
	for (uint32_t i = 0; i != outtake_count; ++i)
	{
		r->read_bytes(outtakes[i], 9);
		//printf("%2d: %s\n", i, outtakes[i]);
	}

	return true;
}

const char *gameinfo_t::get_set_name(uint32_t idx)
{
	assert(idx < set_names_count);
	return set_names[idx];
}

const char *gameinfo_t::get_sfx_track_name(uint32_t idx)
{
	assert(idx < sfx_track_count);
	return sfx_tracks[idx];
}

const char *gameinfo_t::get_music_track_name(uint32_t idx)
{
	assert(idx < music_track_count);
	return music_tracks[idx];
}

const char *gameinfo_t::get_outtake_name(uint32_t idx)
{
	assert(idx < outtake_count);
	return outtakes[idx];
}
