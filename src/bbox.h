#ifndef BBOX_H
#define BBOX_H

#include "reader.h"
#include "vec.h"

class bbox_t
{
public:
	float x1;
	float y1;
	float z1;
	float x2;
	float y2;
	float z2;

	void read(reader_t *r);
};

#endif
