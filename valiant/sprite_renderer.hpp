#ifndef VALIANT_SPRITE_RENDERER_HPP
#define VALIANT_SPRITE_RENDERER_HPP

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>

#include "error.hpp"

namespace valiant {
struct Sprite {
    std::string path;
    int width{0};
    int height{0};
    SDL_Surface* surface;
    SDL_Texture* texture;

    Sprite() : path(""), surface(nullptr), texture(nullptr) {}

    Sprite& operator=(const std::string& new_path) {
        path = new_path;
        surface = IMG_Load(path.c_str());
        if (!surface) {
            // Error encountered when loading image
            throw ValiantError(IMG_GetError());
        }
        width = surface->w;
        height = surface->h;
        // Reset texture
        texture = nullptr;
        return *this;
    }

    void create_texture(SDL_Renderer* renderer) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
};

struct SpriteRendererComponent {
    Sprite sprite;
    bool flip_x{false};
    bool flip_y{false};

    SpriteRendererComponent() : sprite() {}
};

class SpriteRenderer {
   public:
    SpriteRendererComponent sprite_renderer;
};
};  // namespace valiant
#endif
