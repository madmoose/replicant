#include "aud_decoder.h"
#include "aud_player.h"
#include "engine.h"
#include "platform/win32/platform.h"
#include "resources.h"
#include "tre.h"
#include "utils.h"

#include <cstdio>
#include <cstring>

void open_resource_files()
{
	/* hackety hack hack */
	resource_manager.set_base_path("C:\\WESTWOOD\\BLADE\\");

	resource_manager.open_resource_file("1.TLK");
	resource_manager.open_resource_file("2.TLK");
	resource_manager.open_resource_file("3.TLK");
	resource_manager.open_resource_file("A.TLK");
	resource_manager.open_resource_file("MODE.MIX");
	resource_manager.open_resource_file("MUSIC.MIX");
	resource_manager.open_resource_file("OUTTAKE1.MIX");
	resource_manager.open_resource_file("OUTTAKE2.MIX");
	resource_manager.open_resource_file("OUTTAKE3.MIX");
	resource_manager.open_resource_file("OUTTAKE4.MIX");
	resource_manager.open_resource_file("SFX.MIX");
	resource_manager.open_resource_file("SPCHSFX.TLK");
	resource_manager.open_resource_file("STARTUP.MIX");
	resource_manager.open_resource_file("VQA1.MIX");
	resource_manager.open_resource_file("VQA2.MIX");
	resource_manager.open_resource_file("VQA3.MIX");
}

int play_game()
{
	platform_t *platform = new platform_t;
	engine_t *engine     = new engine_t;

	if (!platform->init(engine))
	{
		puts("Failed to initialize platform");
		return 1;
	}

	engine->init(platform);
	if (!engine->run())
		return 1;

	delete engine;

	platform->shutdown();
	delete platform;

	return 0;
}

int play_aud(const char *aud_name)
{
	char name[13];

	if (valid_resource_name(aud_name))
		sprintf(name, "%s", aud_name);
	else if (valid_resource_basename(aud_name))
		sprintf(name, "%s.AUD", aud_name);
	else
	{
		printf("'%s' is not a valid resource name\n", aud_name);
		return 1;
	}
	strupcase(name);

	platform_t *platform = new platform_t;

	if (!platform->init_audio_only(0))
	{
		puts("Failed to initialize platform");
		return 1;
	}

	reader_t *r = resource_manager.get_resource_by_name(name);
	if (!r) return 1;

	aud_decoder_t *aud_decoder = new aud_decoder_t(r);

	r->seek_set(0);
	aud_player_t *aud_player = new aud_player_t(platform);
	if (!aud_player) return 1;

	aud_player->play_aud(r);
	uint32_t start_time = platform->get_time();
	while (!aud_player->is_done())
	{
		aud_player->output_frame();
		platform->output_audio_frame();
	}

	uint32_t length_in_ms = aud_decoder->get_length_in_ms() + 100;
	printf("Length: %.3fs\n", length_in_ms / 1000.0);
	while (start_time + length_in_ms > platform->get_time())
		platform->handle_input();

	delete platform;

	return 0;
}

int save_resource(const char *name)
{
	reader_t *r = resource_manager.get_resource_by_name(name);
	if (!r) return 1;

	uint8_t buf[4096];

	FILE *f = fopen(name, "wb");
	if (!f)
		return 1;

	while (r->remain())
	{
		size_t amount = std::min(r->remain(), sizeof(buf));

		r->read_bytes(buf, amount);

		fwrite(buf, amount, 1, f);
	}

	fclose(f);
	return 0;
}

struct wave_s
{
	uint32_t riff_tag;
	uint32_t riff_size;
	uint32_t wave_tag;
	uint32_t fmt_tag;
	uint32_t fmt_size;
	uint16_t format;
	uint16_t channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint32_t data_tag;
	uint32_t data_size;
};

int save_wav(const char *aud_name)
{
	char name[13];

	if (valid_resource_name(aud_name))
		sprintf(name, "%s", aud_name);
	else if (valid_resource_basename(aud_name))
		sprintf(name, "%s.AUD", aud_name);
	else
	{
		printf("'%s' is not a valid resource name\n", aud_name);
		return 1;
	}
	strupcase(name);

	reader_t *r = resource_manager.get_resource_by_name(name);
	if (!r) return 1;

	wave_s wave =
	{
		htobe32(0x52494646),      // RIFF
		htole32(-1),              // size
		htobe32(0x57415645),      // WAVE
		htobe32(0x666d7420),      // fmt_tab
		htole32(16),              // fmt_size
		htole16(1),               // format
		htole16(1),               // channels
		htole32(22050),           // sample_rate
		htole32(22050 * 2),       // byte_rate
		htole16(2),               // block_align
		htole16(16),              // bits_per_sample
		htobe32(0x64617461),      // data_tag
		htole32(-1)               // data_size
	};

	aud_decoder_t aud_decoder(r);

	wave.data_size = htole32(aud_decoder.get_size_in_samples() * 2);
	wave.riff_size = htole32(36 + wave.data_size);

	std::vector<int16_t> buffer;

	uint32_t remain = aud_decoder.get_size_in_samples();

	while (remain > 0)
	{
		int16_t *frame = aud_decoder.get_frame();

		for (size_t i = 0; i != 1470; ++i)
			frame[i] = htole16(frame[i]);
		buffer.insert(buffer.end(), frame, frame + 1470);

		remain -= std::min(remain, 1470u);
	}

	*strchr(name, '.') = 0;
	strcat(name, ".WAV");

	FILE *f = fopen(name, "wb");

	fwrite(&wave, sizeof(wave), 1, f);
	fwrite(&buffer[0], aud_decoder.get_size_in_samples() * 2, 1, f);

	fclose(f);
	return 0;
}

int dump_tre(const char *tre_name)
{
	tre_t tre;
	if (!tre.open(tre_name))
		return 1;

	tre.dump();

	return 0;
}

int hexdump(const char *name)
{
	reader_t *r = resource_manager.get_resource_by_name(name);
	if (!r) return 1;

	hexdump(r, r->remain());

	delete r;

	return 0;
}

int main(int argc, char **argv)
{
	open_resource_files();

	int r = 0;

	if (argc > 2 && strcmp(argv[1], "play_aud") == 0)
		r = play_aud(argv[2]);
	else if (argc > 2 && strcmp(argv[1], "save_wav") == 0)
		r = save_wav(argv[2]);
	else if (argc > 2 && strcmp(argv[1], "dump_tre") == 0)
		r = dump_tre(argv[2]);
	else if (argc > 2 && strcmp(argv[1], "save") == 0)
		r = save_resource(argv[2]);
	else if (argc > 2 && strcmp(argv[1], "hexdump") == 0)
		r = hexdump(argv[2]);
	else
		r = play_game();

	return r;
}
