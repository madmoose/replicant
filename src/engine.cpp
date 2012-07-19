#include "engine.h"

#include "ambient_sounds.h"
#include "aud_player.h"
#include "gameinfo.h"
#include "platform.h"
#include "resources.h"
#include "scene.h"
#include "script.h"
#include "utils.h"
#include "vqa_decoder.h"

#include <cstdio>
#include <cstring>

bool engine_t::init(platform_t *a_platform)
{
	platform = a_platform;

	frame = new uint16_t[640 * 480];
	if (!frame)
		return false;

	_gameinfo = new gameinfo_t();
	if (!_gameinfo)
		return false;

	_aud_player = new aud_player_t(platform);
	if (!_aud_player)
		return false;

	_ambient_sounds = new ambient_sounds_t(this);
	if (!_ambient_sounds)
		return false;

	script = 0;
	in_script_counter = 0;

	chapter_id = 1;
	scene = new scene_t(this);
	if (!scene)
		return false;

	skip = false;

	_gameinfo->read_gameinfo();

	return true;
}

bool engine_t::run()
{
	next_frame_time = 0;

	platform->start_audio();

	if (!scene->open_scene(gameinfo()->get_initial_set_id(),
		                   gameinfo()->get_initial_scene_id()))
		return false;

	for (;;)
		process_game();

	return true;
}

void engine_t::process_game()
{
	handle_input();

	uint32_t cur_time = 60 * get_time();

	_ambient_sounds->tick();

	if (next_frame_time <= cur_time)
	{
		int frame_no = scene->advance_frame(frame);
		if (frame_no < 0)
			return;

		script_scene_frame_advanced(frame_no);

		render_frame(frame);

		if (!next_frame_time)
			next_frame_time = cur_time;
		next_frame_time += (60 * 1000) / 15;
	}
}

void engine_t::handle_input()
{
	platform->handle_input();
}

uint32_t engine_t::get_time()
{
	return platform->get_time();
}

bool engine_t::load_script(const char *script_name)
{
	script = get_script_by_name(this, script_name);
	return !!script;
}

void engine_t::enter_script()
{
	++in_script_counter;
}

void engine_t::leave_script()
{
	if (in_script_counter <= 0)
	{
		puts("Unbalanced enter/leave script!");
	}
	else
		--in_script_counter;
}

void engine_t::script_initialize_scene()
{
	enter_script();
	if (script)
		script->initialize_scene();
	leave_script();
}

void engine_t::script_scene_frame_advanced(int frame_no)
{
	enter_script();
	if (script)
		script->scene_frame_advanced(frame_no);
	leave_script();
}

void engine_t::set_skip()
{
	skip = true;
}

void engine_t::clear_skip()
{
	skip = false;
}

bool engine_t::get_skip()
{
	bool r = skip;
	skip = false;
	return r;
}

void engine_t::render_frame(uint16_t *a_frame)
{
	platform->render_frame(a_frame);
}

int engine_t::get_chapter_id()
{
	return chapter_id;
}

const char *engine_t::get_room_name(int scene_id)
{
	return gameinfo()->get_set_name(scene_id);
}

void engine_t::set_loop_special(int loop, bool wait)
{
	scene->set_loop_special(loop, wait);
}

void engine_t::set_loop_default(int loop)
{
	scene->set_loop_default(loop);
}

void engine_t::outtake_play(int id, int no_localization, int a3)
{
	const char *outtake_name = gameinfo()->get_outtake_name(id);

	char filename[13];
	if (no_localization)
		sprintf(filename, "%s.VQA", outtake_name);
	else
		sprintf(filename, "%s_E.VQA", outtake_name);

	reader_t *r = resource_manager.get_resource_by_name(filename);
	if (!r) return;

	vqa_decoder_t vqa_decoder(r);

	bool b = vqa_decoder.read_header();
	if (!b) return;

	memset(frame, 0, 2*640*480);

	uint32_t frame_count = 0;
	uint32_t start_time = get_time();

	uint32_t next_frame_time = 0;

	clear_skip();
	for (;;)
	{
		handle_input();
		if (get_skip())
			break;

		uint32_t cur_time = 60 * get_time();

		if (next_frame_time <= cur_time)
		{
			b = vqa_decoder.read_frame();
			if (b < 0)
				break;

			int16_t *audio_frame = vqa_decoder.get_audio_frame();
			if (audio_frame)
			{
				// platform->mix_in_audio_frame(audio_frame, 100);
				// platform->output_audio_frame();
			}

			b = vqa_decoder.decode_frame(frame);
			render_frame(frame);
			++frame_count;

			if (!next_frame_time)
				next_frame_time = cur_time;
			next_frame_time = next_frame_time + (60 * 1000) / 15;
		}
	}

	uint32_t end_time = get_time();

	float fps = 1000.0 * frame_count / (end_time - start_time);
	printf("FPS: %d/%d = %f\n", frame_count, end_time - start_time, fps);
}

void engine_t::sound_play(int id, int volume, int a3, int a4, int a5)
{
	const char *name = _gameinfo->get_sfx_track_name(id);
	_aud_player->play_aud(name, volume, 0);
}

void engine_t::actor_voice_over(int sentence_id, int actor_id)
{
}

/*
	//uint32_t t = timeGetTime();
	//printf("%8d: %8d\n", t, 3 * (t / 60));

	char filename[64];
	sprintf(filename, "png\\frame_%08d.raw", frame_number++);
	save_raw(filename, frame);
	printf("%s\n", filename);
	save_rgb555_as_png(filename, 640, 480, frame);
*/
