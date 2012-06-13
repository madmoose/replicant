#ifndef ENDIAN_H
#define ENDIAN_H

#include <cstdint>

#define BIG_ENDIAN 0

inline
uint16_t swap16(uint16_t a)
{
	return ((a & 0x00ff) <<  8)
	     | ((a & 0xff00) >>  8);
}

inline
uint32_t swap32(uint32_t a)
{
	return ((a & 0x000000ff) << 24)
	     | ((a & 0x0000ff00) <<  8)
	     | ((a & 0x00ff0000) >>  8)
	     | ((a & 0xff000000) >> 24);
}

#if BIG_ENDIAN
inline uint16_t letoh16(uint16_t a) { return swap16(a); }
inline uint16_t htole16(uint16_t a) { return swap16(a); }

inline uint32_t letoh32(uint32_t a) { return swap32(a); }
inline uint32_t htole32(uint32_t a) { return swap32(a); }

inline uint16_t betoh16(uint16_t a) { return a; }
inline uint16_t htobe16(uint16_t a) { return a; }

inline uint32_t betoh32(uint32_t a) { return a; }
inline uint32_t htobe32(uint32_t a) { return a; }

#else

inline uint16_t letoh16(uint16_t a) { return a; }
inline uint16_t htole16(uint16_t a) { return a; }

inline uint32_t letoh32(uint32_t a) { return a; }
inline uint32_t htole32(uint32_t a) { return a; }

inline uint16_t betoh16(uint16_t a) { return swap16(a); }
inline uint16_t htobe16(uint16_t a) { return swap16(a); }

inline uint32_t betoh32(uint32_t a) { return swap32(a); }
inline uint32_t htobe32(uint32_t a) { return swap32(a); }
#endif

#endif
