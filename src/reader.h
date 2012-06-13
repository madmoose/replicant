#ifndef READER_H
#define READER_H

#include <sys/types.h>

#include "endian.h"

class reader_t {
public:
	virtual const char *get_name() = 0;

	virtual bool     is_open()              = 0;
	virtual size_t   tell()                 = 0;
	virtual size_t   remain()               = 0;
	virtual void     seek_set(size_t pos)   = 0;
	virtual void     seek_cur(off_t offset) = 0;
	virtual void     seek_end(off_t offset) = 0;
	virtual void     read_bytes(void *ptr, size_t size) = 0;

	void read_byte(uint8_t   *p)  { uint8_t  v; read_bytes(&v, 1); *p = v; };
	void read_le16(uint16_t  *p)  { uint16_t v; read_bytes(&v, 2); *p = letoh16(v); };
	void read_be16(uint16_t  *p)  { uint16_t v; read_bytes(&v, 2); *p = betoh16(v); };
	void read_le32(uint32_t  *p)  { uint32_t v; read_bytes(&v, 4); *p = letoh32(v); };
	void read_be32(uint32_t  *p)  { uint32_t v; read_bytes(&v, 4); *p = betoh32(v); };
	void read_float(float *p)
	{
		union {
			uint32_t v;
			float    f;
		} u;
		read_le32(&u.v);
		*p = u.f;
	};
	void read_string(char *s, size_t size)
	{
		read_bytes(s, size);
		s[size-1] = '\0';
	}
};

reader_t *make_memory_reader(uint8_t *begin, size_t size);
reader_t *make_file_reader(const char *filename);
reader_t *make_sub_reader(reader_t *r, const char *name, size_t start, size_t size);

#endif
