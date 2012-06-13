#ifndef TRE_H
#define TRE_H

#include <cstdint>

class tre_t
{
	uint32_t  count;
	uint32_t *indices;
	uint32_t *offsets;
	char     *strings;

public:
	bool     open(const char *tre_name);

	uint32_t string_count() { return count; }

	char *get_string(uint32_t id);

	void dump();
};

#endif
