#include "set.h"

#include "lite.h"
#include "reader.h"
#include "resources.h"
#include "utils.h"
#include "view.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>

#define kSet0 0x53657430

#define DUMP_SET 1

lite_t lite;

bool set_t::read(const char *name)
{
	reader_t *r = resource_manager.get_resource_by_name(name);
	if (!r)
		return false;

	uint32_t sig;

	r->read_be32(&sig);
	if (sig != kSet0)
		return false;

	uint32_t v20;
	r->read_le32(&v20);

	r->read_le32(&object_count);

	objects.resize(object_count);

	for (uint32_t i = 0; i != object_count; ++i)
	{
		r->read_string(objects[i].name, 20);
		objects[i].bbox.read(r);
		r->read_byte(&objects[i].is_obstacle);
		r->read_byte(&objects[i].is_clickable);
		r->seek_cur(4);
	}

	r->read_le32(&walkbox_count);
	walkboxes.resize(walkbox_count);

	for (uint32_t i = 0; i != walkbox_count; ++i)
	{
		float x, y, z;

		r->read_string(walkboxes[i].name, 20);
		r->read_float(&y);
		r->read_le32(&walkboxes[i].corner_count);

		assert(walkboxes[i].corner_count <= 8);

		walkboxes[i].y = y;

		for (uint32_t j = 0; j != walkboxes[i].corner_count; ++j)
		{
			r->read_float(&x);
			r->read_float(&z);

			walkboxes[i].corners[j] = vec_t(x, y, z);
		}
	}

	lite.read_from_set(r, v20);

	size_t remain = r->remain();
	printf("\tset remain: %d\n", remain);
	if (0)
	{
		uint8_t *p = new uint8_t[remain];
		r->read_bytes(p, remain);
		hexdump(p, remain);
		delete[] p;
	}

	return true;
}

void set_t::dump()
{
	printf("\nobject_count: %d\n\n", object_count);

	for (uint32_t i = 0; i != object_count; ++i)
		printf("object %-20s [(%8.2f %8.2f %8.2f) (%8.2f %8.2f %8.2f)] %d %d\n",
			objects[i].name,
			objects[i].bbox.x1, objects[i].bbox.y1, objects[i].bbox.z1,
			objects[i].bbox.x2, objects[i].bbox.y2, objects[i].bbox.z2,
			objects[i].is_obstacle, objects[i].is_clickable
			);

	printf("\nwalkbox_count: %d\n\n", walkbox_count);

	for (uint32_t i = 0; i != walkbox_count; ++i)
	{
		printf("walkbox %-20s %6.2f %d: ",
			walkboxes[i].name,
			walkboxes[i].y, walkboxes[i].corner_count
			);

		printf("[");
		for (uint32_t j = 0; j != walkboxes[i].corner_count; ++j)
			printf("%s(%8.2f %8.2f)", j == 0 ? "" : " ", walkboxes[i].corners[j].x, walkboxes[i].corners[j].z);
		printf("]\n");
	}

	lite.dump();

	puts("\n\n");
}

void set_t::draw(uint16_t *frame, view_t *view)
{
	/* Ugh... I need better 3d drawing methods... */

	for (uint32_t i = 0; i != object_count; ++i)
	{
		//if (!objects[i].is_clickable)
		//	continue;

		vec_t p0(objects[i].bbox.x1, objects[i].bbox.y1, objects[i].bbox.z1);
		vec_t p1(objects[i].bbox.x2, objects[i].bbox.y1, objects[i].bbox.z1);
		vec_t p2(objects[i].bbox.x2, objects[i].bbox.y2, objects[i].bbox.z1);
		vec_t p3(objects[i].bbox.x1, objects[i].bbox.y2, objects[i].bbox.z1);

		vec_t p4(objects[i].bbox.x1, objects[i].bbox.y1, objects[i].bbox.z2);
		vec_t p5(objects[i].bbox.x2, objects[i].bbox.y1, objects[i].bbox.z2);
		vec_t p6(objects[i].bbox.x2, objects[i].bbox.y2, objects[i].bbox.z2);
		vec_t p7(objects[i].bbox.x1, objects[i].bbox.y2, objects[i].bbox.z2);

		int screen_x[8], screen_y[8];

		p0.multiply_by_matrix(view->view_matrix);
		p1.multiply_by_matrix(view->view_matrix);
		p2.multiply_by_matrix(view->view_matrix);
		p3.multiply_by_matrix(view->view_matrix);
		p4.multiply_by_matrix(view->view_matrix);
		p5.multiply_by_matrix(view->view_matrix);
		p6.multiply_by_matrix(view->view_matrix);
		p7.multiply_by_matrix(view->view_matrix);

		p0.project(&screen_x[0], &screen_y[0], view->fovx);
		p1.project(&screen_x[1], &screen_y[1], view->fovx);
		p2.project(&screen_x[2], &screen_y[2], view->fovx);
		p3.project(&screen_x[3], &screen_y[3], view->fovx);
		p4.project(&screen_x[4], &screen_y[4], view->fovx);
		p5.project(&screen_x[5], &screen_y[5], view->fovx);
		p6.project(&screen_x[6], &screen_y[6], view->fovx);
		p7.project(&screen_x[7], &screen_y[7], view->fovx);

		//printf("(%d %d) (%d %d)\n", screen_x0, screen_y0, screen_x1, screen_y1);

		draw_line(frame, screen_x[0], screen_y[0], screen_x[1], screen_y[1], 0xfffe);
		draw_line(frame, screen_x[1], screen_y[1], screen_x[2], screen_y[2], 0xfffe);
		draw_line(frame, screen_x[2], screen_y[2], screen_x[3], screen_y[3], 0xfffe);
		draw_line(frame, screen_x[3], screen_y[3], screen_x[0], screen_y[0], 0xfffe);

		draw_line(frame, screen_x[0], screen_y[0], screen_x[4], screen_y[4], 0xfffe);
		draw_line(frame, screen_x[1], screen_y[1], screen_x[5], screen_y[5], 0xfffe);
		draw_line(frame, screen_x[2], screen_y[2], screen_x[6], screen_y[6], 0xfffe);
		draw_line(frame, screen_x[3], screen_y[3], screen_x[7], screen_y[7], 0xfffe);

		draw_line(frame, screen_x[4], screen_y[4], screen_x[5], screen_y[5], 0xfffe);
		draw_line(frame, screen_x[5], screen_y[5], screen_x[6], screen_y[6], 0xfffe);
		draw_line(frame, screen_x[6], screen_y[6], screen_x[7], screen_y[7], 0xfffe);
		draw_line(frame, screen_x[7], screen_y[7], screen_x[4], screen_y[4], 0xfffe);
	}

	for (uint32_t i = 0; i != walkbox_count; ++i)
	{
		for (uint32_t j = 0; j != walkboxes[i].corner_count; ++j)
		{
			uint32_t n = (j + 1) % walkboxes[i].corner_count;

			vec_t p0(walkboxes[i].corners[j].x, walkboxes[i].y, walkboxes[i].corners[j].z);
			vec_t p1(walkboxes[i].corners[n].x, walkboxes[i].y, walkboxes[i].corners[n].z);

			int screen_x0, screen_y0, screen_x1, screen_y1;

			p0.multiply_by_matrix(view->view_matrix);
			p1.multiply_by_matrix(view->view_matrix);

			if (p0.z <= 0.0 || p1.z <= 0.0) // Hah!
				continue;

			p0.project(&screen_x0, &screen_y0, view->fovx);
			p1.project(&screen_x1, &screen_y1, view->fovx);

			//printf("(%d %d) (%d %d)\n", screen_x0, screen_y0, screen_x1, screen_y1);

			draw_line(frame, screen_x0, screen_y0, screen_x1, screen_y1, 0xf800);
		}
	}

	vec_t p0(  0,   0,   0);
	vec_t p1(100,   0,   0);
	vec_t p2(  0, 100,   0);
	vec_t p3(  0,   0, 100);

	int screen_x0, screen_y0, screen_x1, screen_y1;

	p0.multiply_by_matrix(view->view_matrix);
	p1.multiply_by_matrix(view->view_matrix);
	p2.multiply_by_matrix(view->view_matrix);
	p3.multiply_by_matrix(view->view_matrix);

	p0.project(&screen_x0, &screen_y0, view->fovx);
	p1.project(&screen_x1, &screen_y1, view->fovx);
	draw_line(frame, screen_x0, screen_y0, screen_x1, screen_y1, 0xf800);

	p2.project(&screen_x1, &screen_y1, view->fovx);
	draw_line(frame, screen_x0, screen_y0, screen_x1, screen_y1, 0x07c0);

	p3.project(&screen_x1, &screen_y1, view->fovx);
	draw_line(frame, screen_x0, screen_y0, screen_x1, screen_y1, 0x003e);
}

size_t set_t::get_walkbox_count()
{
	return walkbox_count;
}

walkbox_t &set_t::get_walkbox(size_t i)
{
	assert(i < walkbox_count);
	return walkboxes[i];
}

size_t set_t::get_object_count()
{
	return object_count;
}

object_t &set_t::get_object(size_t i)
{
	assert(i < object_count);
	return objects[i];
}
