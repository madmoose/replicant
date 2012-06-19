#include "tre.h"

#include "reader.h"
#include "resources.h"

#include <cstring>
#include <cstdio>

bool tre_t::open(const char *tre_name)
{
	char resource_name[13];
	if (strlen(tre_name) > 8)
		return false;

	strcpy(resource_name, tre_name);
	strcat(resource_name, ".TRE");

	reader_t *r = resource_manager.get_resource_by_name(resource_name);
	if (!r)
		return false;

	r->read_le32(&count);

	indices = new uint32_t[count];
	offsets = new uint32_t[count + 1];

	for (uint32_t i = 0; i != count; ++i)
		r->read_le32(&indices[i]);

	for (uint32_t i = 0; i != count + 1; ++i)
		r->read_le32(&offsets[i]);

	uint32_t strings_start = r->tell() - 4;
	uint32_t strings_size  = r->remain();

	strings = new char[strings_size];
	r->read_bytes(strings, strings_size);

	for (uint32_t i = 0; i != count + 1; ++i)
		offsets[i] -= strings_start;

	delete r;
	return true;
}

char *tre_t::get_string(uint32_t id)
{
	for (uint32_t i = 0; i != count; ++i)
		if (indices[i] == id)
			return &strings[offsets[i]];

	return 0;
}

void tre_t::dump()
{
	for (uint32_t i = 0; i != count; ++i)
		printf("%4d: %s\n", indices[i], get_string(indices[i]));
}
