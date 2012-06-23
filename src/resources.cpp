#include "resources.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>

#include "utils.h"

static bool valid_name_char(char c)
{
	return ('0' <= c && c <= '9')
	    || ('a' <= c && c <= 'z')
	    || ('A' <= c && c <= 'Z')
	    || c == '_' || c == '-';
}

bool valid_resource_name(const char *name)
{
	if (strlen(name) > 12)
		return false;

	const char *p;
	int basename_length = 0;
	for (p = name; *p && *p != '.'; ++p)
	{
		if (!valid_name_char(*p))
			return false;
		++basename_length;
	}

	if (*p != '.' || basename_length > 8)
		return false;

	int extension_length = 0;
	for (++p; *p && *p != '.'; ++p)
	{
		if (!valid_name_char(*p))
			return false;
		++extension_length;
	}

	if (*p || extension_length > 3)
		return false;

	return true;
}

bool valid_resource_basename(const char *name)
{
	if (strlen(name) > 8)
		return false;

	for (const char *p = name; *p; ++p)
		if (!valid_name_char(*p))
			return false;

	return true;
}

const char *get_resource_extension(const char *name)
{
	if (!valid_resource_name(name))
		return 0;

	const char *dot = strchr(name, '.');
	if (!dot)
		return 0;

	return dot + 1;
}

/*
 * Ugh, global variable...
 */

resource_manager_t resource_manager;

resource_file_t::resource_file_t(reader_t *r, resource_file_type_e type)
	: r(r), type(type)
{
	read();
}

void resource_file_t::read()
{
	assert(r);

	r->read_le16(&entry_count);
	r->read_le32(&size);

	entries.resize(entry_count);

	for (uint32_t i = 0; i != entry_count; ++i)
	{
		r->read_le32(&entries[i].id);
		r->read_le32(&entries[i].offset);
		r->read_le32(&entries[i].length);
	}

	std::sort(entries.begin(), entries.end(), entries_id_lt);

	//for (uint32_t i = 0; i != entry_count; ++i)
	//	printf("%08x %08x %08x\n", entries[i].id, entries[i].offset, entries[i].length);

	data_offset = r->tell();
}

int resource_file_t::index_for_id(uint32_t id)
{
	int lo = 0, hi = entry_count;

	while (lo < hi)
	{
		int mid = lo + (hi - lo) / 2;

		if (id > entries[mid].id)
			lo = mid + 1;
		else if (id < entries[mid].id)
			hi = mid;
		else
			return mid;
	}
	return -1;
}

reader_t *resource_file_t::reader_for_id(uint32_t id, const char *name)
{
	int i = index_for_id(id);

	if (i == -1)
		return NULL;

	return make_sub_reader(r, name, data_offset + entries[i].offset, entries[i].length);
}

#include "Shlwapi.h"

void resource_manager_t::set_base_path(const char *path)
{
	base_path = strdup(path);

	if (!PathIsDirectoryA(base_path))
	{
		printf("'%s' is not a valid base path\n", base_path);
		exit(1);
	}
}

bool resource_manager_t::open_resource_file(const char *name)
{
	char full_name[MAX_PATH];

	strcpy(full_name, base_path);

	PathAppendA(full_name, name);

	reader_t *r = make_file_reader(full_name);

	resource_file_type_e type;

	if (strendswith(name, ".MIX"))
		type = RT_MIX;
	else if (strendswith(name, ".TLK"))
		type = RT_TLK;
	else
		return false;

	if (!r)
	{
		printf("Unable to open resource file %s\n", name);
		return false;
	}

	resource_file_t *rf = new resource_file_t(r, type);

	if (!rf)
		return false;

	resource_files.push_back(rf);
	return true;
}

#define ROL(n) ((n << 1) | ((n >> 31) & 1))

static
uint32_t mix_id_for_name(const char *aResourceName)
{
	char buffer[12] = { 0 };
	int i;

	for (i = 0; aResourceName[i] && i < 12; ++i)
		buffer[i] = (char)toupper(aResourceName[i]);

	uint32_t id = 0;
	for (i = 0; i < 12 && buffer[i]; i += 4)
	{
		uint32_t t = (uint32_t)buffer[i + 3] << 24
		           | (uint32_t)buffer[i + 2] << 16
		           | (uint32_t)buffer[i + 1] <<  8
		           | (uint32_t)buffer[i + 0];

		id = ROL(id) + t;
	}

	return id;
}

reader_t *resource_manager_t::get_resource_by_name(const char *name)
{
	if (!valid_resource_name(name))
	{
		printf("'%s' is not a valid resource name\n", name);
		return 0;
	}

	size_t i = 0;
	reader_t *r = 0;
	uint32_t id = mix_id_for_name(name);

	for (; i != resource_files.size(); ++i)
	{
		if (resource_files[i]->get_type() != RT_MIX)
			continue;

		r = resource_files[i]->reader_for_id(id, name);
		if (r)
			break;
	}

	if (r)
	{
		size_t size = r->remain();
		printf("Located resource %-12s in %s, size %d (0x%08x)\n\n", name, resource_files[i]->get_name(), size, size);
	}
	else
		printf("Failed to locate resource '%s'\n", name);

	return r;
}

reader_t *resource_manager_t::get_speech_resource(int actor_id, int sentence_id)
{
	if (actor_id < 0 || actor_id > 99)
		return 0;
	if (sentence_id < 0 || sentence_id > 9999)
		return 0;

	reader_t *r = 0;
	uint32_t id = actor_id * 10000 + sentence_id;

	size_t i = 0;
	for (;i != resource_files.size(); ++i)
	{
		if (resource_files[i]->get_type() != RT_TLK)
			continue;

		r = resource_files[i]->reader_for_id(id, "");
		if (r)
			break;
	}

	if (r)
	{
		size_t size = r->remain();
		printf("Located resource %02d-%04d.AUD in %s, size %d (0x%08x)\n\n", actor_id, sentence_id, resource_files[i]->get_name(), size, size);
	}
	else
		printf("Failed to locate resource '%02d-%04d.AUD'\n", actor_id, sentence_id);

	return r;
}
