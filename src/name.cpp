#include "name.h"

#define ROL(n) ((n << 1) | ((n >> 31) & 1))

uint32_t name_t::hash() const
{
	uint32_t id = 0;
	for (int i = 0; i < 12 && _name[i]; i += 4)
	{
		uint32_t t = (uint32_t)_name[i + 3] << 24
		           | (uint32_t)_name[i + 2] << 16
		           | (uint32_t)_name[i + 1] <<  8
		           | (uint32_t)_name[i + 0];

		id = ROL(id) + t;
	}

	return id;
}
