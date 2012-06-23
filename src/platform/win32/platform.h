#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstdint>

#define AUDIO_FRAME_SIZE 1470

class engine_t;
class window_t;
class opengl_renderer_t;
class audio_renderer_t;

class platform_t
{
	engine_t           *engine;
	window_t           *window;
	opengl_renderer_t  *opengl_renderer;
	audio_renderer_t   *audio_renderer;

public:
	bool init(engine_t *a_engine);
	bool init_audio_only(engine_t *a_engine);

	void shutdown();

	bool render_frame(uint16_t *frame);

	void start_audio();
	void stop_audio();

	void mix_in_audio_frame(int16_t frame[AUDIO_FRAME_SIZE], int volume);
	bool output_audio_frame();

	uint32_t get_time();

	bool handle_input();
};

#endif
