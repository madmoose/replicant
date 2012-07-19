#ifndef AUDIO_RENDERER_H
#define AUDIO_RENDERER_H

#include <windows.h>

#include <deque>
#include <cstdint>

#define AUDIO_FRAME_SIZE 1470
                      // 1470 = 22050 / 15 FPS

class platform_t;

typedef void audio_callback_fn_t(void *ctx, int16_t *data, size_t size);

class audio_renderer_t
{
	platform_t *platform;

	CRITICAL_SECTION CriticalSection;
	HANDLE hThread;
	HANDLE hSemaphore;
	HANDLE hStopEvent;

	HWAVEOUT hWaveOut;
	WAVEHDR  waveHdr[2];
	int      nextWaveHdr;

	bool    is_started;

	audio_callback_fn_t *audio_callback_fn;
	void                *audio_callback_ctx;

	friend DWORD WINAPI audio_thread_runner(LPVOID lpParam);
	friend void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

	void audio_thread_runner();

public:
	audio_renderer_t();
	~audio_renderer_t();

	bool init(platform_t *platform);
	void set_audio_callback(audio_callback_fn_t *callback, void *ctx);

	void start();
	void stop();
};

#endif
