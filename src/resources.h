#ifndef RESOURCES_H
#define RESOURCES_H

#include "reader.h"

#include <cstdint>
#include <vector>

enum resource_file_type_e
{
	RT_MIX,
	RT_TLK
};

class resource_file_t
{
private:
	typedef struct {
		uint32_t id;
		uint32_t offset;
		uint32_t length;
	} entry_t;

	static
	bool entries_id_lt(const entry_t &a, const entry_t &b)
	{
		return a.id < b.id;
	}

	char                *name;
	reader_t            *r;
	resource_file_type_e type;
	uint16_t             entry_count;
	uint32_t             size;
	std::vector<entry_t> entries;
	size_t               data_offset;

	void read();
	int index_for_id(uint32_t id);

public:
	resource_file_t(reader_t *r, resource_file_type_e type);
	const char *get_name() { return r->get_name(); }
	resource_file_type_e get_type() { return type; }

	reader_t *reader_for_id(uint32_t id, const char *name);
};

class resource_manager_t
{
private:
	std::vector<resource_file_t*> resource_files;

public:
	bool open_resource_file(const char *name);
	reader_t *get_resource_by_name(const char *name);
	reader_t *get_speech_resource(int actor_id, int sentence_id);
};

extern resource_manager_t resource_manager;

#endif
