#ifndef LITE_H
#define LITE_H

#include <cstdint>

class reader_t;

class lite_t
{
	float color_r;
	float color_g;
	float color_b;
	uint32_t lite_count;

	uint32_t lite_kind;
	uint32_t lite_size;
	char lite_name[20];
	uint32_t lite_flags;

public:
	bool read_from_set(reader_t *r, uint32_t v20);
	void dump();
};

#endif
