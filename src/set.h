#ifndef SET_H
#define SET_H

#include "bbox.h"
#include "vec.h"

#include <vector>

struct walkbox_t
{
	char     name[20];
	float    y;
	uint32_t corner_count;
	vec_t    corners[8];
};

struct object_t
{
	char    name[20];
	bbox_t  bbox;
	uint8_t is_obstacle;
	uint8_t is_clickable;
	uint8_t is_hot_mouse;
	uint8_t is_field_1b8;
	uint8_t is_combat_target;
};

class view_t;

class set_t
{
	uint32_t               object_count;
	uint32_t               walkbox_count;
	std::vector<object_t>  objects;
	std::vector<walkbox_t> walkboxes;
	int                    walkbox_step_sound[85];
	int                    footstep_sound_override;
	float                  unk[10];
public:
	bool read(const char *name);
	void dump();

	size_t     get_walkbox_count();
	walkbox_t &get_walkbox(size_t i);

	size_t    get_object_count();
	object_t &get_object(size_t i);

	void draw(uint16_t *frame, view_t *view);
};

#endif
