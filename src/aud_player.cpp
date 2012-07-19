#include "aud_player.h"

#include "aud_decoder.h"
#include "name.h"
#include "platform.h"
#include "reader.h"

#include "resources.h"

#include <cstdio>

void audio_callback(void *ctx, int16_t *data, size_t len)
{
	aud_player_t *aud_player = (aud_player_t*)ctx;
	aud_player->output_frame(data, len);
}

aud_player_t::aud_player_t(platform_t *platform)
	: platform(platform)
{
#ifdef _WIN32
	InitializeCriticalSection(&cs);
#endif
	platform->set_audio_callback(audio_callback, this);
}

void aud_player_t::play_aud(const name_t &name)
{
	play_aud(name, 100);
}

void aud_player_t::play_aud(const name_t &name, int volume, uint8_t flags)
{
	reader_t *r;
	uint32_t hash = name.hash();

	if (cache.find(hash) == cache.end())
	{
		r = resource_manager.get_resource_by_name(name);

		size_t size = r->remain();
		uint8_t *p = new uint8_t[size];
		r->read_bytes(p, size);

		cache[hash] = std::make_pair(p, size);
	}

	std::pair<uint8_t *, size_t> mem = cache[hash];
	r = make_memory_reader(mem.first, mem.second);

	channel_t channel;

	channel.name    = name;
	channel.volume  = volume;
	channel.decoder = new aud_decoder_t(r);
	channel.loop    = flags & AUD_LOOP;

#ifdef _WIN32
	EnterCriticalSection(&cs);
#endif

	channels.push_back(channel);

#ifdef _WIN32
	LeaveCriticalSection(&cs);
#endif
}

void aud_player_t::stop()
{
#ifdef _WIN32
	EnterCriticalSection(&cs);
#endif

	for (size_t i = 0; i != channels.size(); ++i)
		delete channels[i].decoder;
	channels.clear();

#ifdef _WIN32
	LeaveCriticalSection(&cs);
#endif

	for (auto i : cache)
		delete[] i.second.first;

	cache.clear();
}

void aud_player_t::output_frame(int16_t *data, size_t len)
{
#ifdef _WIN32
	EnterCriticalSection(&cs);
#endif

	channels_t::iterator it = channels.begin();

	while (it != channels.end())
	{
		int16_t local_data[1470];
		size_t  size = it->decoder->decode(local_data, len, it->loop);

		if (!size)
		{
			delete it->decoder;
			it = channels.erase(it);
		}
		else
		{
			int16_t *p = data;
			for (size_t i = 0; i != size; ++i)
			{
				*p++ += local_data[i] * it->volume / 100;
				*p++ += local_data[i] * it->volume / 100;
			}
	 		++it;
		}
 	}

#ifdef _WIN32
	LeaveCriticalSection(&cs);
#endif
}
