#ifndef NAME_H
#define NAME_H

#include <cassert>
#include <cstring>
#include <cstdint>
#include <ctype.h>

class name_t
{
	char _name[13];
public:
	name_t()
	{
		_name[0] = '\0';
	}
	name_t(const char *name)
	{
		assert(name);
		assert(strlen(name) < 13);
		strncpy(_name, name, 13);
		for (char *p = _name; *p; ++p)
			*p = toupper(*p);
	}
	operator const char*() const { return _name; }

	uint32_t hash() const;
};

#endif
