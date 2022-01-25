//
// Created by caiiiycuk on 25.06.2021.
//

#ifndef VANGERS_XBMP_H
#define VANGERS_XBMP_H

#include <SDL.h>
#include <renderer/compositor/AbstractCompositor.h>

renderer::compositor::Texture BMP_CreateTexture(const char *file, renderer::compositor::AbstractCompositor *renderer);

#endif // VANGERS_XBMP_H
