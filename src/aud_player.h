#ifndef AUD_PLAYER_H
#define AUD_PLAYER_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <cstdint>
#include <map>
#include <vector>

#include "name.h"

class aud_decoder_t;
class name_t;
class platform_t;
class reader_t;

#define AUD_LOOP 1
#define AUD_OVERRIDE_VOLUME 2

class aud_player_t
{
	platform_t *platform;

#ifdef _WIN32
	CRITICAL_SECTION cs;
#endif

	struct channel_t
	{
		name_t         name;
		int            volume;
		bool           loop;
		aud_decoder_t *decoder;
	};

	typedef std::vector<channel_t> channels_t;
	channels_t channels;

	std::map<uint32_t, std::pair<uint8_t*, size_t>> cache;

	friend void audio_callback(void *ctx, int16_t *data, size_t len);
	void output_frame(int16_t *data, size_t len);

public:
	aud_player_t(platform_t *platform);

	void play_aud(const name_t &name);
	void play_aud(const name_t &name, int volume, uint8_t flags = 0);
	void stop();
};

#endif
