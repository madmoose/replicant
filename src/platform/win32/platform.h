#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstddef>
#include <cstdint>

#define AUDIO_FRAME_SIZE 1470

class engine_t;
class window_t;
class opengl_renderer_t;
class audio_renderer_t;

typedef void audio_callback_fn_t(void *ctx, int16_t *data, size_t size);

class platform_t
{
public:
	engine_t           *engine;
	window_t           *window;
	opengl_renderer_t  *opengl_renderer;
	audio_renderer_t   *audio_renderer;

public:
	platform_t();
	~platform_t();

	bool init(engine_t *a_engine);
	bool init_audio_only(engine_t *a_engine);

	void shutdown();

	bool render_frame(uint16_t *frame);

	void set_audio_callback(audio_callback_fn_t *callback, void *ctx);
	void start_audio();
	void stop_audio();

	uint32_t get_time();

	bool handle_input();
};

#endif
