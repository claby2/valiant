#ifndef VALIANT_SPRITE_RENDERER_HPP
#define VALIANT_SPRITE_RENDERER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>

#include "error.hpp"

namespace valiant {
struct Sprite {
    std::string path;
    SDL_Surface* surface;

    void operator=(const std::string& new_path) {
        path = new_path;
        surface = IMG_Load(path.c_str());
        if (!surface) {
            // Error encountered when loading image
            throw ValiantError(IMG_GetError());
        }
    }
    Sprite() : path(""), surface(nullptr) {}
};

struct SpriteRendererComponent {
    Sprite sprite;
    bool flip_x;
    bool flip_y;

    SpriteRendererComponent() : sprite(), flip_x(false), flip_y(false) {}
};

class SpriteRenderer {
   public:
    SpriteRendererComponent sprite_renderer;
};
};  // namespace valiant
#endif