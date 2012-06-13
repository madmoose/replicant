#ifndef AUD_DECODER_H
#define AUD_DECODER_H

#include <cstdint>
#include <cstddef>

#include <deque>

class ima_adpcm_ws_decoder_t
{
	int _step_index;
	int _predictor;

public:
	ima_adpcm_ws_decoder_t()
		: _step_index(0), _predictor(0)
	{}

	void set_parameters(int a_step_index, int a_predictor)
	{
		_step_index = a_step_index;
		_predictor = a_predictor;
	}

	void decode(uint8_t *in, size_t size, int16_t *out);
};

class reader_t;

class aud_decoder_t
{
	reader_t *r;
	ima_adpcm_ws_decoder_t ima_adpcm_ws_decoder;

	struct
	{
		uint16_t sample_rate;
		uint32_t size;
		uint32_t out_size;
		uint8_t  flags;
		uint8_t  type;
	} header;

	std::deque<int16_t> buffer;
	int16_t frame[1470];
public:
	aud_decoder_t(reader_t *r);
	int16_t *get_frame();
};

#endif
