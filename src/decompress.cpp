#include "decompress.h"

#include <cstring>
#include <cstdio>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

uint32_t decompress_lcw(uint8_t *inbuf, size_t in_len, uint8_t *outbuf, size_t out_len)
{
	int version = 1;
	unsigned int count, i, color, pos, relpos;

	uint8_t *src = inbuf;
	uint8_t *dst = outbuf;
	uint8_t *out_end = outbuf + out_len;
	uint32_t out_remain;

	if (src[0] == 0)
	{
		version = 2;
		++src;
	}

	while (src < inbuf + in_len && dst < out_end && src[0] != 0x80)
	{
		out_remain = out_end - dst;

		if (src[0] == 0xff)      // 0b11111111
		{
			count = src[1] | (src[2] << 8);
			pos   = src[3] | (src[4] << 8);
			src += 5;
			count = MIN(count, out_remain);

			if (version == 1)
			{
				for (i = 0; i < count; i++)
					dst[i] = outbuf[i + pos];
			}
			else
			{
				for (i = 0; i < count; i++)
					dst[i] = *(dst + i - pos);
			}
		}
		else if (src[0] == 0xfe) // 0b11111110
		{
			count = src[1] | (src[2] << 8);
			color = src[3];
			src += 4;
			count = MIN(count, out_remain);

			memset(dst, color, count);
		}
		else if (src[0] >= 0xc0)  // 0b11??????
		{
			count = (src[0] & 0x3f) + 3;
			pos   = src[1] | (src[2] << 8);
			src += 3;
			count = MIN(count, out_remain);

			if (version == 1)
			{
				for (i = 0; i < count; i++)
					dst[i] = outbuf[i + pos];
			}
			else
			{
				for (i = 0; i < count; i++)
					dst[i] = *(dst + i - pos);
			}
		}
		else if (src[0] >= 0x80)  // 0b10??????
		{
			count = src[0] & 0x3f;
			++src;
			count = MIN(count, out_remain);

			memcpy(dst, src, count);
			src += count;
		}
		else                     // 0b0???????
		{
			count  = ((src[0] & 0x70) >> 4) + 3;
			relpos = ((src[0] & 0x0f) << 8) | src[1];
			src += 2;
			count = MIN(count, out_remain);

			for (i = 0; i < count; i++)
			{
				dst[i] = *(dst + i - relpos);
			}
		}

		dst += count;
	}

	return dst - outbuf;
}
