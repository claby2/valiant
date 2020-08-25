#ifndef VALIANT_RENDERER_HPP
#define VALIANT_RENDERER_HPP

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <vector>

#include "camera.hpp"
#include "error.hpp"
#include "object.hpp"
#include "sprite_renderer.hpp"
#include "time.hpp"

namespace valiant {
namespace {
const int DEFAULT_WINDOW_WIDTH = 640;
const int DEFAULT_WINDOW_HEIGHT = 480;
}  // namespace

enum class RenderMode { ENABLE, DISABLE };

class Renderer {
   public:
    Renderer(RenderMode render_mode = RenderMode::ENABLE)
        : camera_(nullptr),
          has_camera_(false),
          render_mode_(render_mode),
          renderer_(nullptr),
          window_(nullptr) {
        initialize_sdl();
    }

    ~Renderer() { close_sdl(); }

    template <typename T>
    inline void add_object(T& object) {
        objects_.push_back(&object);
    }

    inline void add_camera(Camera& camera) {
        has_camera_ = true;
        camera_ = &camera;
    }

    void run() {
        // Check if camera has been added
        Camera new_camera;
        if (has_camera_ == false) {
            // Create a camera as no camera has been added
            camera_ = &new_camera;
        }
        // Run awake methods
        for (auto object : objects_) {
            object->awake();
        }
        camera_->awake();
        // Run start methods
        for (auto object : objects_) {
            object->start();
        }
        camera_->start();
        if (render_mode_ == RenderMode::ENABLE) {
            SDL_Event event;
            bool quit = false;
            uint64_t start = 0;
            while (quit == false) {
                uint64_t last = start;
                start = SDL_GetPerformanceCounter();
                // Calculate delta time in seconds
                double delta = (((start - last) * 1000 /
                                 (double)SDL_GetPerformanceFrequency()) *
                                0.001);
                if (SDL_PollEvent(&event) != 0) {
                    if (event.type == SDL_QUIT) {
                        quit = true;
                    }
                }
                // Run update methods
                for (auto object : objects_) {
                    // Update event state for each object
                    object->input.event = event;
                    // Update time for each object
                    object->time.set(delta);
                    // Run update object method
                    object->update();
                }
                camera_->input.event = event;
                camera_->time.set(delta);
                camera_->update();
                // Rendering
                SDL_RenderClear(renderer_);
                for (auto object : objects_) {
                    if (SpriteRenderer* sprite_object =
                            dynamic_cast<SpriteRenderer*>(object)) {
                        // Object has sprite renderer component
                        SpriteRendererComponent sprite_renderer =
                            sprite_object->sprite_renderer;
                        if (sprite_renderer.sprite.texture == nullptr) {
                            sprite_object->sprite_renderer.sprite
                                .create_texture(renderer_);
                        }
                        SDL_Rect rect = {
                            object->transform.position.x,
                            object->transform.position.y,
                            (int)(sprite_renderer.sprite.surface->w /
                                  camera_->camera.size),
                            (int)(sprite_renderer.sprite.surface->h /
                                  camera_->camera.size)};
                        SDL_RendererFlip flip = (SDL_RendererFlip)(
                            (sprite_renderer.flip_x ? SDL_FLIP_HORIZONTAL : 0) |
                            (sprite_renderer.flip_y ? SDL_FLIP_VERTICAL : 0));
                        SDL_RenderCopyEx(renderer_,
                                         sprite_renderer.sprite.texture,
                                         nullptr, &rect, 0.0, nullptr, flip);
                    }
                }
                SDL_RenderPresent(renderer_);
            }
        }
    }

   private:
    std::vector<Object*> objects_;
    Camera* camera_;
    bool has_camera_;
    RenderMode render_mode_;
    SDL_Renderer* renderer_;
    SDL_Window* window_;

    void initialize_sdl() {
        if (render_mode_ == RenderMode::ENABLE) {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
            window_ = SDL_CreateWindow(
                "Valiant Engine", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH,
                DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            renderer_ =
                SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
            IMG_Init(IMG_INIT_PNG);
        }
    }

    void close_sdl() {
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        renderer_ = nullptr;
        window_ = nullptr;
        IMG_Quit();
        SDL_Quit();
    }
};
}  // namespace valiant

#endif
