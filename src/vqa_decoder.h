#ifndef VQA_DECODER_H
#define VQA_DECODER_H

#include <cstddef>
#include <cstdint>

#include "aud_decoder.h"
#include "view.h"

class reader_t;

struct vqa_header_t
{
	uint16_t version;
	uint16_t flags;
	uint16_t numFrames;
	uint16_t width;
	uint16_t height;
	uint8_t  blockW;
	uint8_t  blockH;
	uint8_t  frameRate;
	uint8_t  cbParts;
	uint16_t colors;
	uint16_t maxBlocks;
	uint16_t offset_x;
	uint16_t offset_y;
	uint16_t maxVPTRSize;
	uint16_t freq;
	uint8_t  channels;
	uint8_t  bits;
	uint32_t unk3;
	uint16_t unk4;
	uint32_t maxCBFZSize;
	uint32_t unk5;
};

struct loop_range_t {
	uint16_t begin;
	uint16_t end;
};

struct loop_info_t
{
	uint16_t loop_count;
	uint32_t unk1;
	loop_range_t *loops;
};

struct clip_info_t
{
	uint16_t clip_count;
};

class vqa_decoder_t
{
	reader_t *r;

	vqa_header_t header;
	loop_info_t  loop_info;
	clip_info_t  clip_info;

	size_t   codebookSize;
	uint8_t *codebook;
	uint8_t *cbfz;

	size_t   vptrSize;
	uint8_t *vptr;

	uint32_t *frame_info;

	int       cur_frame;

	int       cur_loop;
	int       loop_special;
	int       loop_default;

	//uint16_t *zbuf;
	//uint16_t *frame;
	bool     has_view;
	view_t   view;

	ima_adpcm_ws_decoder_t ima_adpcm_ws_decoder;
	int16_t *audio_frame;

	bool read_vqhd(uint32_t size);
	bool read_msci(uint32_t size);
	bool read_mfci(uint32_t size);
	bool read_linf(uint32_t size);
	bool read_cinf(uint32_t size);
	bool read_finf(uint32_t size);
	bool read_lnin(uint32_t size);
	bool read_clip(uint32_t size);

	bool read_sn2j(uint32_t size);
	bool read_snd2(uint32_t size);
	bool read_vqfr(uint32_t size);
	bool read_vptr(uint32_t size);
	bool read_vqfl(uint32_t size);
	bool read_cbfz(uint32_t size);
	bool read_zbuf(uint32_t size);
	bool read_view(uint32_t size);
	bool read_aesc(uint32_t size);
	bool read_lite(uint32_t size);

public:
	vqa_decoder_t(reader_t *r);
	~vqa_decoder_t();

	bool read_header();
	bool read_frame();

	void vptr_write_block(uint16_t *frame, unsigned int dst_block, unsigned int src_block, int count, bool alpha = false) const;

	void set_loop_special(int loop, bool wait);
	void set_loop_default(int loop);

	bool seek_to_frame(int frame);
	bool decode_frame(uint16_t *frame);
	int16_t *get_audio_frame();

	bool get_view(view_t *view);
};

#endif
