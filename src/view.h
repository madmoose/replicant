#ifndef VIEW_H
#define VIEW_H

#include "matrix.h"
#include "vec.h"

#include <cstdint>

class reader_t;

class view_t
{
public:
	uint32_t frame;
	float    fovx;
	matrix_t view_matrix;
	vec_t    camera_position;

	void read(reader_t *r);
	void dump();
};

#endif
