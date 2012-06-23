#include "utils.h"

#include "reader.h"

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "lodepng.h"

void hexdump(void *data, size_t size)
{
	unsigned char *p, *end;
	int c, c2;

	p = (uint8_t*)data;
	end = p + size;

	do {
		printf("%08x: ", (unsigned int)(p - (unsigned char*)data));
		for (c = 0; c != 16 && p + c != end; ++c)
		{
			if (c == 8)
				putchar(' ');

			printf("%02x ", p[c]);
		}
		for (c2 = c; c2 != 16; ++c2)
		{
			if (c2 == 8)
				putchar(' ');

			printf("   ");
		}

		for (c = 0; c != 16 && p + c != end; ++c)
			putchar(isprint(p[c]) ? p[c] : '.');

		putchar('\n');

		p += c;
	} while (p != end);
}

void hexdump(reader_t *r, size_t size)
{
	uint8_t *data = new uint8_t[size];
	r->read_bytes(data, size);
	hexdump(data, size);
	delete[] data;
}

int popcount(uint32_t n)
{
    n = n - ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    return (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void save_raw(const char *filename, uint16_t *frame)
{
	FILE *f = fopen(filename, "wb");
	fwrite(frame, 2 * 640 * 480, 1, f);
	fclose(f);
}

void save_rgb555_as_png(const char *filename, uint32_t w, uint32_t h, void *data)
{
	uint8_t  *buf = new uint8_t[4 * w * h];
	uint8_t  *dst = buf;
	uint16_t *src = (uint16_t*)data;
	uint32_t j;

	for (j = 0; j != w*h; ++j)
	{
		uint16_t rgb555 = letoh16(*src++);
		uint8_t r, g, b, a;

		r = (rgb555 >> 10) & 0x1f;
		g = (rgb555 >>  5) & 0x1f;
		b = (rgb555 >>  0) & 0x1f;
		a = 255;

		r = (r << 3) | (r >> 2);
		g = (g << 3) | (g >> 2);
		b = (b << 3) | (b >> 2);

		*dst++ = r;
		*dst++ = g;
		*dst++ = b;
		*dst++ = a;
	}

	LodePNG_encode_file(filename, buf, w, h, 6, 8);
	delete[] buf;
}

float hexfloat(unsigned int a)
{
	union {
		float f;
		unsigned int i;
	} u;

	u.i = a;
	return u.f;
}

/* crude drawing functions for debug... */

void draw_point(uint16_t *frame, int x, int y, uint16_t color)
{
	if (x < 0 || x >= 640) return;
	if (y < 0 || y >= 480) return;

	frame[640 * y + x] = color;
}

void draw_line(uint16_t *frame, int x0, int y0, int x1, int y1, uint16_t color)
{
	bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int delta_x = x1 - x0;
	int delta_y = std::abs(y1 - y0);
	double error = 0.0;
	double delta_err = (double)delta_y / delta_x;
	int ystep = (y0 < y1) ? 1 : -1;
	int y = y0;
	for (int x = x0; x != x1; ++x)
	{
		if (steep)
			draw_point(frame, y, x, color);
		else
			draw_point(frame, x, y, color);

		error += delta_err;
		if (error > 0.5)
		{
			y += ystep;
			error -= 1.0;
		}
	}
}

#include <string.h>

char upcase(char c)
{
	if (c >= 'a' && c <= 'z')
		c += ('A' - 'a');

	return c;
}

char *strupcase(char *s)
{
	for (char *p = s; *p; ++p)
		*p = upcase(*p);
	return s;
}

int strcasecmp(const char *s1, const char *s2)
{
	while (*s1 && *s2)
	{
		if (upcase(*s1) != upcase(*s2))
			break;
		++s1;
		++s2;
	}

	return *s1 - *s2;
}

bool strendswith(const char *s1, const char *s2)
{
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);

	if (len1 < len2)
		return false;

	return strcasecmp(s1 + len1 - len2, s2) == 0;
}

#ifdef __WIN32__
char *strdup(const char *s)
{
	char *r = (char*)malloc(strlen(s)+1);
	strcpy(r, s);
	return r;
}
#endif
