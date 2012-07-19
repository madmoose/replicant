#ifndef SCENE_H
#define SCENE_H

#include "vec.h"

#include <cstdint>

class engine_t;
class set_t;
class vqa_decoder_t;

class scene_t
{
	engine_t      *engine;

	set_t         *set;
	int            set_id;
	int            scene_id;
	vqa_decoder_t *vqa_decoder;
	vec_t          actor_start;
	int            actor_facing;

	uint16_t      *scene_frame;

public:
	scene_t(engine_t *engine);
	bool open_scene(int a_set_id, int a_scene_id);

	void set_loop_special(int loop, bool wait);
	void set_loop_default(int loop);

	int  advance_frame(uint16_t *frame);
};

#endif
