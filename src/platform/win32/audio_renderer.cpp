#include "audio_renderer.h"

#include <cassert>
#include <cstdio>

#include "platform.h"
#include "window.h"

audio_renderer_t::audio_renderer_t()
	: hThread(0),
	  hSemaphore(0),
	  hStopEvent(0),
	  hWaveOut(0),
	  nextWaveHdr(0),
	  is_started(false),
	  audio_callback_fn(0)
{
	for (int i = 0; i != 2; ++i)
		waveHdr[i].lpData = 0;
}

audio_renderer_t::~audio_renderer_t()
{
	stop();
	CloseHandle(hSemaphore);
	CloseHandle(hStopEvent);

	waveOutReset(hWaveOut);
	waveOutClose(hWaveOut);

	for (int i = 0; i != 2; ++i)
		free(waveHdr[i].lpData);
}

DWORD WINAPI audio_thread_runner(LPVOID lpParam)
{
	audio_renderer_t *audio_renderer = (audio_renderer_t*)lpParam;
	audio_renderer->audio_thread_runner();

	return 0;
}

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	if (uMsg != WOM_DONE)
		return;

	audio_renderer_t *audio_renderer = (audio_renderer_t*)dwInstance;

	ReleaseSemaphore(audio_renderer->hSemaphore, 1, 0);
}

bool audio_renderer_t::init(platform_t *a_platform)
{
	platform = a_platform;

	hSemaphore = CreateSemaphore(NULL, 2, 2, NULL);
	hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	assert(hSemaphore != NULL);
	assert(hStopEvent != NULL);

	WAVEFORMATEX waveFormat;
	memset(&waveFormat, 0, sizeof(WAVEFORMATEX));

	waveFormat.wFormatTag      = WAVE_FORMAT_PCM;
	waveFormat.nChannels       = 2;
	waveFormat.nSamplesPerSec  = 22050;
	waveFormat.wBitsPerSample  = 16;
	waveFormat.nBlockAlign     = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;

	waveFormat.cbSize = 0;

	MMRESULT r = waveOutOpen(&hWaveOut,
	                         WAVE_MAPPER,
	                         &waveFormat,
	                         (DWORD)waveOutProc,
	                         (DWORD)this,
	                         CALLBACK_FUNCTION);

	assert(r == MMSYSERR_NOERROR);

	for (int i = 0; i != 2; ++i)
	{
		memset(&waveHdr[i], 0, sizeof(WAVEHDR));

		waveHdr[i].dwBufferLength = 4 * AUDIO_FRAME_SIZE;
		waveHdr[i].lpData = (LPSTR)malloc(waveHdr[i].dwBufferLength);

		assert(waveHdr[i].lpData);
	}

	return true;
}

void audio_renderer_t::start()
{
	if (is_started)
		return;
	is_started = true;

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)::audio_thread_runner, this, 0, 0);
	SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
}

void audio_renderer_t::stop()
{
	if (!is_started)
		return;
	is_started = false;

	SetEvent(hStopEvent);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	hThread = 0;
}

void audio_renderer_t::audio_thread_runner()
{
	MMRESULT r;
	HANDLE handles[2] = { hStopEvent, hSemaphore };

	for (;;)
	{
		DWORD res = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (res == WAIT_OBJECT_0) // hStopEvent
			return;

		r = waveOutUnprepareHeader(hWaveOut, &waveHdr[nextWaveHdr], sizeof(WAVEHDR));
		assert(r == MMSYSERR_NOERROR);

		memset(waveHdr[nextWaveHdr].lpData, 0, 4 * AUDIO_FRAME_SIZE);

		if (audio_callback_fn)
			audio_callback_fn(audio_callback_ctx, (int16_t*)waveHdr[nextWaveHdr].lpData, AUDIO_FRAME_SIZE);

		waveHdr[nextWaveHdr].dwFlags = 0;
		r = waveOutPrepareHeader(hWaveOut, &waveHdr[nextWaveHdr], sizeof(WAVEHDR));
		assert(r == MMSYSERR_NOERROR);

		r = waveOutWrite(hWaveOut, &waveHdr[nextWaveHdr], sizeof(WAVEHDR));
		assert(r == MMSYSERR_NOERROR);
		nextWaveHdr = (nextWaveHdr + 1) % 2;
	}
}

void audio_renderer_t::set_audio_callback(audio_callback_fn_t *callback, void *ctx)
{
	audio_callback_fn  = callback;
	audio_callback_ctx = ctx;
}
