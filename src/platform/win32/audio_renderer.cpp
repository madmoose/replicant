#include "audio_renderer.h"

#include <cstdio>

bool audio_renderer_t::init()
{
	mix_buffer_empty = true;
	is_started = false;
	memset(mix_buffer, 0, 4 * AUDIO_FRAME_SIZE);

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
	                         0,
	                         0,
	                         CALLBACK_NULL);

    if (r != MMSYSERR_NOERROR)
    	return false;

    waveOutPause(hWaveOut);

    return true;
}

void audio_renderer_t::start()
{
	if (!is_started)
		waveOutRestart(hWaveOut);
	is_started = true;
}

void audio_renderer_t::stop()
{
	if (is_started)
	{
		waveOutReset(hWaveOut);
		//waveOutPause(hWaveOut);
	}
	is_started = false;
}

void audio_renderer_t::mix_in_audio_frame(int16_t frame[AUDIO_FRAME_SIZE], int volume)
{
	mix_buffer_empty = false;

	/*
	if (pan > 100) pan = 100;
	if (pan < 0)   pan = 0;

	int pan_factor_l = pan;
	int pan_factor_r = 100 - pan;
	*/

	for (int i = 0; i != AUDIO_FRAME_SIZE; ++i)
	{
		mix_buffer[2 * i + 0] += frame[i] * 100 / volume / 2;
		mix_buffer[2 * i + 1] += frame[i] * 100 / volume / 2;
	}
}

bool audio_renderer_t::output_audio_frame()
{
	MMRESULT r;
	WAVEHDR *waveHdr;

	if (!waveHdrDeque.empty() && (r = waveOutUnprepareHeader(hWaveOut, *waveHdrDeque.begin(), sizeof(WAVEHDR))) == MMSYSERR_NOERROR)
	{
		waveHdr = *waveHdrDeque.begin();
		waveHdrDeque.pop_front();
	}
	else
	{
		waveHdr = new WAVEHDR;
		memset(waveHdr, 0, sizeof(WAVEHDR));

		int16_t *local_frame = new int16_t[2 * AUDIO_FRAME_SIZE];
		waveHdr->lpData = (LPSTR)local_frame;
	}

	memcpy((void*)waveHdr->lpData, mix_buffer, 4 * AUDIO_FRAME_SIZE);
	waveHdr->dwBufferLength = 4 * AUDIO_FRAME_SIZE;

	r = waveOutPrepareHeader(hWaveOut, waveHdr, sizeof(WAVEHDR));
	if (r != MMSYSERR_NOERROR)
		return false;

	r = waveOutWrite(hWaveOut, waveHdr, sizeof(WAVEHDR));
	if (r != MMSYSERR_NOERROR)
		return false;

	waveHdrDeque.push_back(waveHdr);

	if (!mix_buffer_empty)
		memset(mix_buffer, 0, 4 * AUDIO_FRAME_SIZE);
	mix_buffer_empty = true;

	start();

	return true;
}
