#include "vqa_decoder.h"

#include "decompress.h"
#include "reader.h"
#include "utils.h"

#include <cstdio>
#include <cstring>

#define kAESC 0x41455343
#define kCBFZ 0x4342465A
#define kCIND 0x43494E44
#define kCINF 0x43494E46
#define kCINH 0x43494E48
#define kCLIP 0x434C4950
#define kFINF 0x46494E46
#define kFORM 0x464f524d
#define kLIND 0x4C494E44
#define kLINF 0x4C494E46
#define kLINH 0x4C494E48
#define kLITE 0x4C495445
#define kLNID 0x4C4E4944
#define kLNIH 0x4C4E4948
#define kLNIN 0x4C4E494E
#define kLNIO 0x4C4E494F
#define kMFCD 0x4D464344
#define kMFCH 0x4D464348
#define kMFCI 0x4D464349
#define kMFCT 0x4D464354
#define kMSCH 0x4D534348
#define kMSCI 0x4D534349
#define kMSCT 0x4D534354
#define kSN2J 0x534e324a
#define kSND2 0x534e4432
#define kVIEW 0x56494557
#define kVPTR 0x56505452
#define kVQFL 0x5651464C
#define kVQFR 0x56514652
#define kVQHD 0x56514844
#define kWVQA 0x57565141
#define kZBUF 0x5A425546

vqa_decoder_t::vqa_decoder_t(reader_t *r)
	: r(r), codebook(0), cbfz(0), vptr(0),
	  cur_frame(-1),
	  cur_loop(-1),
	  loop_special(-1),
	  loop_default(-1),
	  has_view(false),
	  audio_frame(0)
{
}

vqa_decoder_t::~vqa_decoder_t()
{
}

struct iff_chunk_header_s
{
	iff_chunk_header_s()
		: id(0), size(0)
	{}

	uint32_t id;
	uint32_t size;
};

static inline uint32_t roundup(uint32_t v)
{
	return (v + 1) & ~1u;
}

static
bool read_iff_chunk_header(reader_t *r, iff_chunk_header_s *ts)
{
	if (r->remain() < 8)
		return false;

	r->read_be32(&ts->id);
	r->read_be32(&ts->size);
	return true;
}

const char *str_tag(uint32_t tag)
{
	static char s[5];

	sprintf(s, "%c%c%c%c",
		(tag >> 24) & 0xff,
		(tag >> 16) & 0xff,
		(tag >>  8) & 0xff,
		(tag >>  0) & 0xff);

	return s;
}

bool vqa_decoder_t::read_header()
{
	iff_chunk_header_s chd;
	uint32_t type;
	bool rc;

	read_iff_chunk_header(r, &chd);
	if (chd.id != kFORM || !chd.size)
		return false;

	r->read_be32(&type);

	if (type != kWVQA)
		return false;

	do {
		if (!read_iff_chunk_header(r, &chd))
			return false;

		//printf("\t%s\n", str_tag(chd.id));

		rc = false;
		switch (chd.id)
		{
			case kCINF: rc = read_cinf(chd.size); break;
			case kCLIP: rc = read_clip(chd.size); break;
			case kFINF: rc = read_finf(chd.size); break;
			case kLINF: rc = read_linf(chd.size); break;
			case kLNIN: rc = read_lnin(chd.size); break;
			case kMFCI: rc = read_mfci(chd.size); break;
			case kMSCI: rc = read_msci(chd.size); break;
			case kVQHD: rc = read_vqhd(chd.size); break;
			default:
				printf("Unhandled chunk %s\n", str_tag(chd.id));
				r->seek_cur(roundup(chd.size));
		}

		if (!rc)
			return false;

	} while (chd.id != kFINF);

	return true;
}

bool vqa_decoder_t::read_vqhd(uint32_t size)
{
	if (size != 42)
		return false;

	r->read_le16(&header.version);
	r->read_le16(&header.flags);
	r->read_le16(&header.numFrames);
	r->read_le16(&header.width);
	r->read_le16(&header.height);
	r->read_byte(&header.blockW);
	r->read_byte(&header.blockH);
	r->read_byte(&header.frameRate);
	r->read_byte(&header.cbParts);
	r->read_le16(&header.colors);
	r->read_le16(&header.maxBlocks);
	r->read_le16(&header.offset_x);
	r->read_le16(&header.offset_y);
	r->read_le16(&header.maxVPTRSize);
	r->read_le16(&header.freq);
	r->read_byte(&header.channels);
	r->read_byte(&header.bits);
	r->read_le32(&header.unk3);
	r->read_le16(&header.unk4);
	r->read_le32(&header.maxCBFZSize);
	r->read_le32(&header.unk5);

	/*
		printf("1 %d\n", header.version);
		printf("2 %d\n", header.flags);
		printf("3 %d\n", header.numFrames);
		printf("4 %d\n", header.width);
		printf("5 %d\n", header.height);
		printf("6 %d\n", header.blockW);
		printf("7 %d\n", header.blockH);
		printf("8 %d\n", header.frameRate);
		printf("9 %d\n", header.cbParts);
		printf("0 %d\n", header.colors);
		printf("1 %d\n", header.maxBlocks);
		//printf("2 %x\n", header.unk1);
		printf("3 %d\n", header.maxVPTRSize);
		printf("4 %d\n", header.freq);
		printf("5 %d\n", header.channels);
		printf("6 %d\n", header.bits);
		printf("7 %d\n", header.unk3);
		printf("8 %d\n", header.unk4);
		printf("9 %d\n", header.maxCBFZSize);
		printf("0 %d\n", header.unk5);
	*/

	return true;
}

bool vqa_decoder_t::read_msci(uint32_t size)
{
	r->seek_cur(roundup(size));
	return true;
}

bool vqa_decoder_t::read_linf(uint32_t size)
{
	iff_chunk_header_s chd;

	read_iff_chunk_header(r, &chd);
	if (chd.id != kLINH || chd.size != 6)
		return false;

	r->read_le16(&loop_info.loop_count);
	r->read_le32(&loop_info.unk1);

	if (loop_info.unk1 != 2)
		return false;


	read_iff_chunk_header(r, &chd);
	if (chd.id != kLIND || chd.size != 4 * loop_info.loop_count)
		return false;

	loop_info.loops = new loop_range_t[loop_info.loop_count];
	for (int i = 0; i != loop_info.loop_count; ++i)
	{
		r->read_le16(&loop_info.loops[i].begin);
		r->read_le16(&loop_info.loops[i].end);

		//printf("LOOP %2d: %d %d\n", i, loop_info.loops[i].begin, loop_info.loops[i].end);
	}

	return true;
}

bool vqa_decoder_t::read_cinf(uint32_t size)
{
	iff_chunk_header_s chd;

	read_iff_chunk_header(r, &chd);
	if (chd.id != kCINH || chd.size != 8)
		return false;

	r->read_le16(&clip_info.clip_count);
	r->seek_cur(6);


	read_iff_chunk_header(r, &chd);
	if (chd.id != kCIND || chd.size != 6 * clip_info.clip_count)
		return false;

	r->seek_cur(chd.size);

	return true;
}

bool vqa_decoder_t::read_finf(uint32_t size)
{
	if (size != 4 * header.numFrames)
		return false;

	frame_info = new uint32_t[header.numFrames];

	for (uint32_t i = 0; i != header.numFrames; ++i)
		r->read_le32(&frame_info[i]);

	return true;
}

bool vqa_decoder_t::read_lnin(uint32_t size)
{
	r->seek_cur(roundup(size));
	//hexdump(r, roundup(size));
	return true;
}

bool vqa_decoder_t::read_clip(uint32_t size)
{
	//r->seek_cur(roundup(size));
	hexdump(r, roundup(size));
	return true;
}

bool vqa_decoder_t::read_mfci(uint32_t size)
{
	r->seek_cur(roundup(size));
	//hexdump(r, roundup(size));
	return true;
}

bool vqa_decoder_t::read_frame()
{
	if (loop_special >= 0)
	{
		cur_loop = loop_special;
		loop_special = -1;

		cur_frame = loop_info.loops[cur_loop].begin;
		seek_to_frame(cur_frame);
	}
	else if (cur_loop == -1 && loop_default >= 0)
	{
		cur_loop = loop_default;
		cur_frame = loop_info.loops[cur_loop].begin;
		seek_to_frame(cur_frame);
	}
	else if (cur_loop >= -1 && cur_frame == loop_info.loops[cur_loop].end - 1)
	{
		if (loop_default == -1)
			return false;

		cur_loop = loop_default;
		cur_frame = loop_info.loops[cur_loop].begin;
		seek_to_frame(cur_frame);
	}
	else
		++cur_frame;

	if (cur_frame >= header.numFrames)
		return false;


	iff_chunk_header_s chd;

	//hexdump(r, 8);
	//return false;

	has_view = false;

	if (r->remain() < 8)
		return false;

	do {
		if (!read_iff_chunk_header(r, &chd))
			return false;

		//printf("%s ", str_tag(chd.id)); fflush(0);

		bool rc = false;
		switch (chd.id)
		{
			case kAESC: rc = read_aesc(chd.size); break;
			case kLITE: rc = read_lite(chd.size); break;
			case kSN2J: rc = read_sn2j(chd.size); break;
			case kSND2: rc = read_snd2(chd.size); break;
			case kVIEW: rc = read_view(chd.size); break;
			case kVQFL: rc = read_vqfl(chd.size); break;
			case kVQFR: rc = read_vqfr(chd.size); break;
			case kZBUF: rc = read_zbuf(chd.size); break;
			default:
				r->seek_cur(roundup(chd.size));
		}

		if (!rc)
		{
			printf("Error handling chunk %s\n", str_tag(chd.id));
			return false;
		}
	} while (chd.id != kVQFR);

	return true;
}


bool vqa_decoder_t::read_sn2j(uint32_t size)
{
	if (size != 6)
		return false;

	uint16_t step_index;
	uint32_t predictor;

	r->read_le16(&step_index);
	r->read_le32(&predictor);

	ima_adpcm_ws_decoder.set_parameters(step_index >> 5, predictor);

	return true;
}

bool vqa_decoder_t::read_snd2(uint32_t size)
{
	if (size != 735)
	{
		printf("audio frame size: %d\n", size);
		return false;
	}

	if (!audio_frame)
		audio_frame = new int16_t[2 * size];
	memset(audio_frame, 0, 4 * size);

	uint8_t in_frame[roundup(size)];
	r->read_bytes(in_frame, roundup(size));

	ima_adpcm_ws_decoder.decode(in_frame, size, audio_frame);

	return true;
}

bool vqa_decoder_t::read_vqfr(uint32_t size)
{
	iff_chunk_header_s chd;

	while (size >= 8)
	{
		if (!read_iff_chunk_header(r, &chd))
			return false;
		size -= roundup(chd.size) + 8;

		//printf("%s ", str_tag(chd.id)); fflush(0);

		bool rc = false;
		switch (chd.id)
		{
			case kCBFZ: rc = read_cbfz(chd.size); break;
			case kVPTR: rc = read_vptr(chd.size); break;
			default:
				r->seek_cur(roundup(chd.size));
		}

		if (!rc)
		{
			printf("VQFR: error handling chunk %s\n", str_tag(chd.id));
			return false;
		}
	}

	return true;
}

bool vqa_decoder_t::read_vqfl(uint32_t size)
{
	iff_chunk_header_s chd;

	while (size >= 8)
	{
		if (!read_iff_chunk_header(r, &chd))
			return false;
		size -= roundup(chd.size) + 8;

		bool rc = false;
		switch (chd.id)
		{
			case kCBFZ: rc = read_cbfz(chd.size); break;
			default:
				r->seek_cur(roundup(chd.size));
		}

		if (!rc)
		{
			printf("VQFL: error handling chunk %s\n", str_tag(chd.id));
			return false;
		}
	}

	return true;
}

bool vqa_decoder_t::read_cbfz(uint32_t size)
{
	if (size > header.maxCBFZSize)
		return false;

	if (!codebook)
	{
		codebookSize = 2 * header.maxBlocks * header.blockW * header.blockH;
		codebook = new uint8_t[codebookSize];
	}
	if (!cbfz)
		cbfz = new uint8_t[roundup(header.maxCBFZSize)];

	r->read_bytes(cbfz, roundup(size));

	decompress_lcw(cbfz, size, codebook, codebookSize);

	return true;
}

bool vqa_decoder_t::read_zbuf(uint32_t size)
{
	r->seek_cur(roundup(size));
	//hexdump(r, roundup(size));
	return true;
}

#include "view.h"

bool vqa_decoder_t::read_view(uint32_t size)
{
	if (size != 56)
		return false;

	view.read(r);
	has_view = true;

	return true;
}

bool vqa_decoder_t::read_aesc(uint32_t size)
{
	r->seek_cur(roundup(size));
	//hexdump(r, roundup(size));
	return true;
}

bool vqa_decoder_t::read_lite(uint32_t size)
{
	r->seek_cur(roundup(size));
	//hexdump(r, roundup(size));
	return true;
}

bool vqa_decoder_t::read_vptr(uint32_t size)
{
	if (size > header.maxVPTRSize)
		return false;

	if (!vptr)
		vptr = new uint8_t[roundup(header.maxVPTRSize)];

	vptrSize = size;
	r->read_bytes(vptr, roundup(size));

	return true;
}

void vqa_decoder_t::vptr_write_block(uint16_t *frame, unsigned int dst_block, unsigned int src_block, int count, bool alpha) const
{
	uint16_t frame_width  = header.width;
	uint32_t frame_stride = 640;
	uint16_t block_width  = header.blockW;
	uint16_t block_height = header.blockH;

	const uint8_t *const block_src =
		&codebook[2 * src_block * block_width * block_height];

	uint8_t blocks_per_line = frame_width / block_width;

	do
	{
		uint32_t frame_x = dst_block % blocks_per_line * block_width;
		uint32_t frame_y = dst_block / blocks_per_line * block_height;
		//printf("%2dx%2d\n", frame_x, frame_y);

		uint32_t dst_offset = frame_x + frame_y * frame_stride;

		const uint8_t *__restrict__ src = block_src;
		uint16_t      *__restrict__ dst = frame + dst_offset;

		unsigned int block_y;
		for (block_y = 0; block_y != block_height; ++block_y)
		{
			unsigned int block_x;
			for (block_x = 0; block_x != block_width; ++block_x)
			{
				uint16_t rgb555 = src[0] | (src[1] << 8);
				src += 2;

				if (!(alpha && (rgb555 & 0x8000)))
					*dst = rgb555;
				++dst;
			}
			dst += frame_stride - block_width;
		}

		++dst_block;
	}
	while (--count);
}

void vqa_decoder_t::set_loop_special(int loop, bool wait)
{
	loop_special = loop;
	if (!wait)
		cur_loop = -1;
}

void vqa_decoder_t::set_loop_default(int loop)
{
	loop_default = loop;
}

bool vqa_decoder_t::seek_to_frame(int frame)
{
	if (frame < 0 || frame >= header.numFrames)
		return false;

	r->seek_set(2 * (frame_info[frame] & 0x1fffffff));
	return true;
}

bool vqa_decoder_t::decode_frame(uint16_t *frame)
{
	uint8_t *src = vptr;
	uint8_t *end = vptr + vptrSize;

	uint16_t count, src_block, dst_block = 0;
	(void)src_block;

	while (end - src >= 2)
	{
		uint16_t command = src[0] | (src[1] << 8);
		uint8_t  prefix = command >> 13;
		src += 2;

		switch (prefix)
		{
		case 0:
			count = command & 0x1fff;
			dst_block += count;
			break;
		case 1:
			count = 2 * (((command >> 8) & 0x1f) + 1);
			src_block = command & 0x00ff;

			vptr_write_block(frame, dst_block, src_block, count);
			dst_block += count;
			break;
		case 2:
			count = 2 * (((command >> 8) & 0x1f) + 1);
			src_block = command & 0x00ff;

			vptr_write_block(frame, dst_block, src_block, 1);
			++dst_block;

			for (int i = 0; i < count; ++i)
			{
				src_block = *src++;
				vptr_write_block(frame, dst_block, src_block, 1);
				++dst_block;
			}
			break;
		case 3:
		case 4:
			count = 1;
			src_block = command & 0x1fff;

			vptr_write_block(frame, dst_block, src_block, count, prefix == 4);
			++dst_block;
			break;
		case 5:
		case 6:
			count = *src++;
			src_block = command & 0x1fff;

			vptr_write_block(frame, dst_block, src_block, count, prefix == 6);
			dst_block += count;
			break;
		default:
			printf("Undefined case %d\n", command >> 13);
		}
	}

	return true;
}

int16_t *vqa_decoder_t::get_audio_frame()
{
	return audio_frame;
}

bool vqa_decoder_t::get_view(view_t *a_view)
{
	if (!has_view)
		return false;

	*a_view = view;
	return true;
}
