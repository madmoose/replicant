#ifndef AUDIO_RENDERER_H
#define AUDIO_RENDERER_H

#include <windows.h>

#include <deque>
#include <cstdint>

#define AUDIO_FRAME_SIZE 1470
                      // 1470 = 22050 / 15 FPS

class audio_renderer_t
{
	HWAVEOUT hWaveOut;

	std::deque<WAVEHDR*> waveHdrDeque;

	bool    mix_buffer_empty;
	int16_t mix_buffer[2 * AUDIO_FRAME_SIZE];
	bool    is_started;
public:
	bool init();

	void start();
	void stop();

	void mix_in_audio_frame(int16_t frame[AUDIO_FRAME_SIZE], int volume);
	bool output_audio_frame();
};

#endif
