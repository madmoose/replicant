#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <cstdint>

class reader_t;

void hexdump(void *data, size_t size);
void hexdump(reader_t *r, size_t size);
int popcount(uint32_t n);

void save_raw(const char *filename, uint16_t *frame);
void save_rgb555_as_png(const char *filename, uint32_t w, uint32_t h, void *data);

float hexfloat(unsigned int a);

void draw_point(uint16_t *frame, int x, int y, uint16_t color);
void draw_line(uint16_t *frame, int x0, int y0, int x1, int y1, uint16_t color);

char *strupcase(char *s);
bool strendswith(const char *s1, const char *s2);

#ifdef __WIN32__
char *strdup(const char *s);
#endif

#endif
