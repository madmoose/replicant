#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <cstddef>
#include <cstdint>

uint32_t decompress_lcw(uint8_t *inbuf, size_t in_len, uint8_t *outbuf, size_t out_len);

#endif
