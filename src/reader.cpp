#include "reader.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static
char *strdup(const char *s)
{
	size_t len = strlen(s) + 1;
	char *s2 = (char*)malloc(len);
	memcpy(s2, s, len);
	return s2;
}

class memory_reader_t : reader_t {
private:
	uint8_t *begin;
	uint8_t *position;
	uint8_t *end;

	memory_reader_t(uint8_t *b, uint8_t *e)
		: begin(b), position(b), end(e) {}

	friend reader_t *make_memory_reader(uint8_t *begin, size_t size);

public:
	const char *get_name() { return 0; }

	bool     is_open()                { return true; }
	size_t   tell()                   { return position - begin; }
	size_t   remain()                 { return end - position; }
	void     seek_set(size_t new_pos) { position = begin + new_pos; }
	void     seek_cur(off_t offset)   { position += offset; }
	void     seek_end(off_t offset)   { position = end + offset; }
	void     read_bytes(void *ptr, size_t size)
	{
		assert(size <= remain());
		memcpy(ptr, position, size);
		position += size;
	}
};

class file_reader_t : reader_t {
private:
	char *name;
	FILE *f;
	size_t position;
	size_t size;

	file_reader_t(const char *filename)
	{
		name = strdup(filename);
		f = fopen(filename, "rb");
		if (!f)
			return;

		position = 0;
		fseek(f, 0, SEEK_END);
		size = tell();
		fseek(f, 0, SEEK_SET);
	}
	~file_reader_t()
	{
		fclose(f);
		free(name);
	}

	friend reader_t *make_file_reader(const char *filename);

public:
	const char *get_name() { return name; }

	bool     is_open()              { return !!f; }
	size_t   tell()                 { return ftell(f); }
	size_t   remain()               { return size - tell(); }
	void     seek_set(size_t new_position)
	{
		if (position == new_position)
			return;

		if (new_position > size)
			new_position = size;

		position = new_position;
		fseek(f, position, SEEK_SET);
	}
	void     seek_cur(off_t offset) { position += offset;        seek_set(position); }
	void     seek_end(off_t offset) { position  = size + offset; seek_set(position);}
	void     read_bytes(void *ptr, size_t size)
	{
		//assert(size <= remain());
		fread(ptr, size, 1, f);
	}
};

class sub_reader_t : reader_t {
private:
	char *name;
	reader_t *r;
	size_t start;
	off_t  position;
	size_t size;

	sub_reader_t(reader_t *r, const char *a_name, size_t start, size_t size)
		: r(r), start(start), position(0), size(size)
	{
		name = strdup(a_name);
		r->seek_set(start);
	}
	~sub_reader_t()
	{
		free(name);
	}

	friend reader_t *make_sub_reader(reader_t *t, const char *name, size_t start, size_t size);

	void clamp_position()
	{
		if (position < 0)
			position = 0;
		else if (position > (off_t)size)
			position = (off_t)size;
	}

public:
	const char *get_name() { return name; }

	bool     is_open()                { return r && r->is_open(); }
	size_t   tell()                   { return position; }
	size_t   remain()                 { return size - position; }
	void     seek_set(size_t new_pos) { position = new_pos; clamp_position(); }
	void     seek_cur(off_t offset)   { seek_set(position + offset); }
	void     seek_end(off_t offset)   { seek_set(size + offset); }
	void     read_bytes(void *ptr, size_t size)
	{
		assert(size <= remain());

		r->seek_set(start + position);

		r->read_bytes(ptr, size);
		seek_cur(size);
	}
};

reader_t *make_memory_reader(uint8_t *begin, size_t size)
{
	return new memory_reader_t(begin, begin + size);
}

reader_t *make_file_reader(const char *filename)
{
	reader_t *r = new file_reader_t(filename);
	if (r->is_open())
		return r;

	delete r;
	return 0;
}

reader_t *make_sub_reader(reader_t *r, const char *name, size_t start, size_t size)
{
	return new sub_reader_t(r, name, start, size);
}
