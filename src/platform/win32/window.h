#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

class engine_t;

class window_t
{
	HWND hWnd;

public:
	bool create(engine_t *engine);

	HWND get_hwnd() { return hWnd; }
};

#endif
