#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <cstdint>

class window_t;

class opengl_renderer_t
{
	HWND  hWnd;
	HDC   hDC;
	HGLRC hRC;

	GLuint    texture_id;
	uint16_t *textureData;

public:
	bool create(window_t *window);
	void render_frame(uint16_t *frame);
	~opengl_renderer_t();
};



#endif
