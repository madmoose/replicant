#include "aud_player.h"

#include "aud_decoder.h"
#include "platform.h"
#include "reader.h"

aud_player_t::aud_player_t(platform_t *platform)
	: platform(platform)
{
}

void aud_player_t::play_aud(reader_t *r)
{
	aud_decoder_t *decoder = new aud_decoder_t(r);

	decoders.push_back(decoder);
}

void aud_player_t::stop()
{
	for (size_t i = 0; i != decoders.size(); ++i)
	{
		delete decoders[i];
		decoders[i] = 0;
	}
	decoders.clear();
}

bool aud_player_t::is_done()
{
	return decoders.empty();
}

void aud_player_t::output_frame()
{
	int16_t *local_frame;

	decoders_t::iterator it = decoders.begin();

	while (it != decoders.end())
	{
		local_frame = (*it)->get_frame();
		if (!local_frame)
		{
			delete *it;
			it = decoders.erase(it);
		}
		else
		{
			platform->mix_in_audio_frame(local_frame, 100);
	 		++it;
		}
 	}
}
