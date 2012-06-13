#include "opengl_renderer.h"

#include "window.h"

#include <cstdio>

#define GL_BGRA 0x80E1
#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366

#define glError() { \
	GLenum err = glGetError(); \
	if (err != GL_NO_ERROR) { \
		while (err != GL_NO_ERROR) { \
			fprintf(stderr, "glError: %s caught at %s:%u\n", (char *)gluErrorString(err), __FILE__, __LINE__); \
			err = glGetError(); \
		} \
		exit(1); \
	} \
}

bool opengl_renderer_t::create(window_t *window)
{
	hWnd = window->get_hwnd();

	// set up opengl context
	PIXELFORMATDESCRIPTOR pfd;
	int format;

	// get the device context (DC)
	hDC = GetDC(hWnd);
	if (!hDC)
		return false;

	// set the pixel format for the DC
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize      = sizeof(pfd);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, format, &pfd);

	// create and enable the render context (RC)
	hRC = wglCreateContext(hDC);
	if (!hRC)
		return false;

	wglMakeCurrent(hDC, hRC);
	glError();

	glGenTextures(1, &texture_id);
	glError();

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glError();

	glTexImage2D(GL_TEXTURE_2D,       // target
	             0,                   // level
	             GL_RGB5_A1,          // internalFormat
	             1024,                // width
	              512,                // height
	             0,                   // border
	             GL_BGRA,             // format
	             GL_UNSIGNED_SHORT_1_5_5_5_REV,    // type
	             0);                  // data
	glError();

	return true;
}

void opengl_renderer_t::render_frame(uint16_t *frame)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glError();

	glTexSubImage2D(GL_TEXTURE_2D,       // target
	                0,                   // level
	                0, 0,                // x y offset
	                640, 480,            // width height
	                GL_BGRA,             // format
	                GL_UNSIGNED_SHORT_1_5_5_5_REV,    // type
	                frame); // data
	glError();

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glError();

	glViewport(0, 0, 640, 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 640, 480, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glError();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glError();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float tx = 640.0 / 1024.0;
	float ty = 480.0 /  512.0;

	float vw = 640.0;
	float wh = 480.0;

	glBegin(GL_QUADS);
		glTexCoord2d( 0,  0);
		glVertex2f(   0,  0);

		glTexCoord2d(tx,  0);
		glVertex2f(  vw,  0);

		glTexCoord2d(tx, ty);
		glVertex2f(  vw, wh);

		glTexCoord2d( 0, ty);
		glVertex2f(   0, wh);
 	glEnd();
	glError();

 	SwapBuffers(hDC);
}

opengl_renderer_t::~opengl_renderer_t()
{
	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(hRC);
}
