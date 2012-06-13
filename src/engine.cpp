#include "engine.h"

#include "aud_player.h"
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

	aud_speech_player = new aud_player_t(platform);
	if (!aud_speech_player)
		return false;

	script = 0;
	in_script_counter = 0;

	chapter_id = 1;
	scene = new scene_t(this);
	if (!scene)
		return false;

	skip = false;

	read_gameinfo();

	return true;
}

bool engine_t::run()
{
	next_frame_time = 0;

	platform->start_audio();

	if (!scene->open_scene(initial_set_id, initial_scene_id))
		return false;

	for (;;)
		process_game();

	return true;
}

void engine_t::process_game()
{
	handle_input();

	uint32_t cur_time = 60 * get_time();

	if (next_frame_time <= cur_time)
	{
		if (!scene->advance_frame(frame))
			return;

		aud_speech_player->output_frame();

		render_frame(frame);
		platform->output_audio_frame();

		if (!next_frame_time)
			next_frame_time = 60 * get_time();
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

void engine_t::read_gameinfo()
{
	reader_t *r = resource_manager.get_resource_by_name("GAMEINFO.DAT");
	if (!r)
		return;

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
		//printf("%s\n", sfx_tracks[i]);
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
}

int engine_t::get_chapter_id()
{
	return chapter_id;
}

const char *engine_t::get_room_name(int scene_id)
{
	return set_names[scene_id];
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
	char filename[13];
	if (no_localization)
		sprintf(filename, "%s.VQA", outtakes[id]);
	else
		sprintf(filename, "%s_E.VQA", outtakes[id]);

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
			if (!b)
				break;

			int16_t *audio_frame = vqa_decoder.get_audio_frame();
			if (audio_frame)
			{
				platform->mix_in_audio_frame(audio_frame, 100);
				platform->output_audio_frame();
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

void engine_t::actor_voice_over(int sentence_id, int actor_id)
{
	reader_t *r = resource_manager.get_speech_resource(actor_id, sentence_id);
	if (!r) return;

	aud_speech_player->play_aud(r);

	while (!aud_speech_player->is_done())
	{
		process_game();
	}
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
