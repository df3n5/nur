#ifndef  NXTEXTURELOADER_H
#define  NXTEXTURELOADER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

#include <nxCore/nxLog.h>
#include <nxCore/nxTypes.h>

#define NX_MAX_TEXTURES 255

GLuint nxTextureLoader_loadImageFromFilename(const char* filename);
void nxTextureLoader_shutdown();

#endif   // NXTEXTURELOADER_H
