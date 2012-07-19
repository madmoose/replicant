#include "scene.h"

#include "engine.h"
#include "platform/win32/platform.h"
#include "resources.h"
#include "set.h"
#include "view.h"
#include "vqa_decoder.h"

#include <cstdio>
#include <cstring>

scene_t::scene_t(engine_t *a_engine)
	: engine(a_engine)
{
	set = new set_t;
	scene_frame = new uint16_t[640*480];
}

bool scene_t::open_scene(int a_set_id, int a_scene_id)
{
	set_id = a_set_id;
	scene_id = a_scene_id;

	const char *scene_name = engine->get_room_name(scene_id);
	int chapter_id = engine->get_chapter_id();

	char vqa_name[13];

	if (chapter_id == 1)
		sprintf(vqa_name, "%s.VQA", scene_name);
	else
		sprintf(vqa_name, "%s_%d.VQA", scene_name, chapter_id);

	reader_t *r = resource_manager.get_resource_by_name(vqa_name);
	if (!r)
		return false;

	vqa_decoder = new vqa_decoder_t(r);
	if (!vqa_decoder)
		return false;

	if (!vqa_decoder->read_header())
		return false;

	char set_name[13];
	sprintf(set_name, "%s-MIN.SET", scene_name);
	if (!set->read(set_name))
		return false;

#if 0
	set->dump();
#endif

	if (!engine->load_script(scene_name))
		return false;
	engine->script_initialize_scene();

	return true;
}

void scene_t::set_loop_special(int loop, bool wait)
{
	vqa_decoder->set_loop_special(loop, wait);
}

void scene_t::set_loop_default(int loop)
{
	vqa_decoder->set_loop_default(loop);
}

int scene_t::advance_frame(uint16_t *frame)
{
	int frame_no = vqa_decoder->read_frame();
	if (frame_no < 0)
		return -1;

	// int16_t *audio_frame = vqa_decoder->get_audio_frame();
	// if (audio_frame)
	// 	engine->get_platform()->mix_in_audio_frame(audio_frame, 100);

	if (!vqa_decoder->decode_frame(scene_frame))
		return -1;

	memcpy(frame, scene_frame, 2*640*480);

#if 0
	view_t view;
	if (vqa_decoder->get_view(&view))
		set->draw(frame, &view);
#endif

	return frame_no;
}
