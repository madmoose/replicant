#include "platform.h"

#include "window.h"
#include "opengl_renderer.h"
#include "audio_renderer.h"
#include "msg_loop.h"

platform_t::platform_t()
	: engine(0),
	  window(0),
	  opengl_renderer(0),
	  audio_renderer(0)
{}

platform_t::~platform_t()
{
	delete audio_renderer;
	delete opengl_renderer;
	delete window;
}

bool platform_t::init(engine_t *a_engine)
{
	engine = a_engine;

	window = new window_t;
	if (!window->create(engine))
		return false;

	opengl_renderer = new opengl_renderer_t;
	if (!opengl_renderer->create(window))
		return false;

	audio_renderer = new audio_renderer_t;
	if (!audio_renderer->init(this))
		return false;

	return true;
}

bool platform_t::init_audio_only(engine_t *a_engine)
{
	engine = a_engine;

	window = 0;
	opengl_renderer = 0;

	audio_renderer = new audio_renderer_t;
	if (!audio_renderer->init(this))
		return false;

	return true;
}

void platform_t::shutdown()
{
	delete opengl_renderer;
	opengl_renderer = 0;

	delete window;
	window = 0;

	engine = 0;
}

bool platform_t::render_frame(uint16_t *frame)
{
	opengl_renderer->render_frame(frame);
	return true;
}

void platform_t::set_audio_callback(audio_callback_fn_t *callback, void *ctx)
{
	if (audio_renderer)
		audio_renderer->set_audio_callback(callback, ctx);
}

void platform_t::start_audio()
{
	audio_renderer->start();
}

void platform_t::stop_audio()
{
	audio_renderer->stop();
}

uint32_t platform_t::get_time()
{
	return timeGetTime();
}

bool platform_t::handle_input()
{
	win32_msg_loop();
	return true;
}
