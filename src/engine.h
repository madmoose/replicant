#ifndef ENGINE_H
#define ENGINE_H

#include <cstdint>

class ambient_sounds_t;
class aud_player_t;
class gameinfo_t;
class platform_t;
class scene_t;
class script_t;
class tre_t;
class view_t;
class vqa_decoder_t;

/* engine_t
 *
 * Blade Runner has a LOT of global variables.
 * I'm trying to gather them into engine_t.
 *
 */

class engine_t
{
private:
	platform_t *platform;
	uint16_t   *frame;

	gameinfo_t    *_gameinfo;
	scene_t       *scene;
	vqa_decoder_t *vqa_decoder;
	view_t        *view;
	script_t      *script;

	uint32_t       start_frame_time;
	uint32_t       next_frame_time;

	bool skip;

	int chapter_id;
	int set_id;
	int scene_id;

	tre_t *tre_options;

	aud_player_t *aud_speech_player;
	aud_player_t *_aud_player;

	ambient_sounds_t *_ambient_sounds;

	int in_script_counter;

public:
	bool init(platform_t *platform);
	bool run();

	gameinfo_t *gameinfo() { return _gameinfo; }

	platform_t *get_platform() { return platform; }

	aud_player_t *get_aud_player() { return _aud_player; }

	ambient_sounds_t *get_ambient_sounds() { return _ambient_sounds; }

	void     handle_input();
	void     process_game();
	uint32_t get_time();

	bool load_script(const char *script_name);
	void enter_script();
	void leave_script();

	void script_initialize_scene();
	void script_scene_frame_advanced(int frame);

	int get_chapter_id();
	const char *get_room_name(int scene_id);

	void render_frame(uint16_t *a_frame);

	void set_skip();
	void clear_skip();
	bool get_skip();

	void set_loop_special(int loop, bool wait);
	void set_loop_default(int loop);

	void outtake_play(int id, int a2, int a3);
	void sound_play(int id, int volume, int a3, int a4, int a5);
	void actor_voice_over(int id, int actor_id);
};

#endif
