#include "msg_loop.h"

#include "window.h"

void win32_msg_loop()
{
	MSG msg;
	memset(&msg, 0, sizeof(msg));

	if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE | PM_NOYIELD))
		Sleep(1);

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
		exit(0);
}
