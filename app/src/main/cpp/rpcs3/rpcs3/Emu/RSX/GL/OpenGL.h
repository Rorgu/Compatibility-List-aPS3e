#pragma once
#ifndef _WIN32
#ifndef __ANDROID__
#include <GL/glew.h>
#endif
#endif

#ifdef _WIN32
#include <Windows.h>
#include "GL/gl.h"
#include <glext.h>
typedef BOOL (WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);

#define OPENGL_PROC(p, n) extern p gl##n
#define WGL_PROC(p, n) extern p wgl##n
#define OPENGL_PROC2(p, n, tn) OPENGL_PROC(p, n)
	#include "GLProcTable.h"
#undef OPENGL_PROC
#undef WGL_PROC
#undef OPENGL_PROC2

#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif defined(__ANDROID__)
#include <GLES3/gl32.h>
#define USE_GLES

#define _GL_DOUBLE 0x140A
#define _GL_UNSIGNED_BYTE_3_3_2 0x8032
#define _GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
#define _GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
#define _GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define _GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#define _GL_UNSIGNED_INT_8_8_8_8 0x8035
#define _GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define _GL_UNSIGNED_INT_10_10_10_2 0x8036
#define _GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define _GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define _GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define _GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#define _GL_R16 0x822A
#define _GL_RG16 0x822C
#define _GL_MIRROR_CLAMP_EXT 0x8742
//#define _GL_MIRROR_CLAMP_TO_EDGE 0x8743
#define _GL_MIRROR_CLAMP_TO_BORDER_EXT 0x8912
#define _GL_BGR 0x80E0
#define _GL_BGRA 0x80E1
#define _GL_TEXTURE_1D 0x0DE0

#else
#include <GL/gl.h>
#ifdef HAVE_X11
#include <GL/glxew.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#endif
#endif

#ifndef GL_TEXTURE_BUFFER_BINDING
//During spec release, this enum was removed during upgrade from ARB equivalent
//See https://www.khronos.org/bugzilla/show_bug.cgi?id=844
#define GL_TEXTURE_BUFFER_BINDING 0x8C2A
#endif

namespace gl
{
	void init();
	void set_swapinterval(int interval);
}
