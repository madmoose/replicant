#ifndef AUD_PLAYER_H
#define AUD_PLAYER_H

#include <vector>

class aud_decoder_t;
class platform_t;
class reader_t;

class aud_player_t
{
	platform_t *platform;
	typedef std::vector<aud_decoder_t*> decoders_t;
	decoders_t decoders;

public:
	aud_player_t(platform_t *platform);

	void play_aud(reader_t *r);
	void stop();
	bool is_done();
	void output_frame();
};

#endif
