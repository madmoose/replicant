#include "lite.h"

#include "reader.h"
#include "utils.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>

#define DUMP_LITE 1

bool lite_t::read_from_set(reader_t *r, uint32_t v20)
{
	r->read_float(&color_r);
	r->read_float(&color_g);
	r->read_float(&color_b);
	r->read_le32(&lite_count);

	for (uint32_t i = 0; i != lite_count; ++i)
	{
		r->read_le32(&lite_kind);

		r->read_le32(&lite_size);
		lite_size -= 32;

		r->read_string(lite_name, 20);

		r->read_le32(&lite_flags);

		if (lite_size != (unsigned)(popcount(lite_flags) * 4 * (v20 - 1) + 76))
		{
			printf("%d %d\n", lite_size, popcount(lite_flags) * 4 * (v20 - 1) + 76);
			exit(0);
		}

		uint8_t *lite_stuff = new uint8_t[lite_size];
		r->read_bytes(lite_stuff, lite_size);
		delete[] lite_stuff;
	}

	return true;
}

void lite_t::dump()
{
	printf("\nrgb: %f %f %f\n", color_r, color_g, color_b);
	printf("lite_count: %d\n", lite_count);

	for (uint32_t i = 0; i != lite_count; ++i)
		printf("%d %-20s %08x %4d\n", lite_kind, lite_name, lite_flags, lite_size);
}
