#include "window.h"

#include "engine.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool window_t::create(engine_t *engine)
{
	WNDCLASS wc;

	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra  = 0;
	wc.cbWndExtra  = 0;
	wc.hInstance   = 0;
	wc.hIcon       = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor     = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ReplicantWindow";
	if (!RegisterClass(&wc))
		return false;

	// create main window
	hWnd = CreateWindow(
		"ReplicantWindow", "Replicant",
		WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
		0, 0, 640, 480,
		NULL, NULL, wc.hInstance, NULL);
	if (!hWnd)
		return false;

	SetProp(hWnd, "engine", (HANDLE)engine);

	return true;
}

#include <cstdio>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	engine_t *engine = (engine_t*)GetProp(hWnd, "engine");

	switch (message)
	{
		case WM_CREATE:
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					if (engine)
						engine->set_skip();
					return 0;
			}
			return 0;
		case WM_QUIT:
			return wParam;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
