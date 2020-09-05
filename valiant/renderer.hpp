#ifndef VALIANT_RENDERER_HPP
#define VALIANT_RENDERER_HPP

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cstdint>
#include <string>
#include <vector>

#include "camera.hpp"
#include "color.hpp"
#include "error.hpp"
#include "object.hpp"
#include "shape.hpp"
#include "sprite_renderer.hpp"
#include "time.hpp"

namespace valiant {
namespace {
const int DEFAULT_WINDOW_WIDTH = 640;
const int DEFAULT_WINDOW_HEIGHT = 480;
}  // namespace

enum class RenderMode { ENABLE, DISABLE };

struct ObjectData {
    int width;
    int height;
    Vector3 position;
};

struct CameraData {
    float size;
    Vector3 position;
};

class Renderer {
   public:
    Renderer(RenderMode render_mode = RenderMode::ENABLE)
        : background_color_({0, 0, 0, 255}),
          camera_(nullptr),
          window_width_(DEFAULT_WINDOW_WIDTH),
          window_height_(DEFAULT_WINDOW_HEIGHT),
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

    inline int window_width() const { return window_width_; }

    inline int window_height() const { return window_height_; }

    void set_background_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        background_color_.r = r;
        background_color_.g = g;
        background_color_.b = b;
        background_color_.a = a;
    }

    void set_background_color(Color color) { background_color_ = color; }

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
                SDL_GetWindowSize(window_, &window_width_, &window_height_);
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
                SDL_SetRenderDrawColor(renderer_, background_color_.r,
                                       background_color_.g, background_color_.b,
                                       background_color_.a);
                SDL_RenderClear(renderer_);
                // Get camera data
                CameraData camera_data = {camera_->camera.size,
                                          camera_->transform.position};
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
                        ObjectData object_data = {sprite_renderer.sprite.width,
                                                  sprite_renderer.sprite.height,
                                                  object->transform.position};
                        SDL_Rect rect = get_object_camera_position(object_data,
                                                                   camera_data);
                        SDL_RendererFlip flip = (SDL_RendererFlip)(
                            (sprite_renderer.flip_x ? SDL_FLIP_HORIZONTAL : 0) |
                            (sprite_renderer.flip_y ? SDL_FLIP_VERTICAL : 0));
                        SDL_RenderCopyEx(renderer_,
                                         sprite_renderer.sprite.texture,
                                         nullptr, &rect, 0.0, nullptr, flip);
                    } else if (Rectangle* rectangle_object =
                                   dynamic_cast<Rectangle*>(object)) {
                        // Object has rectangle component
                        Shape shape = rectangle_object->shape;
                        ObjectData object_data = {shape.width, shape.height,
                                                  object->transform.position};
                        SDL_Rect rect = get_object_camera_position(object_data,
                                                                   camera_data);
                        SDL_SetRenderDrawColor(renderer_, shape.color.r,
                                               shape.color.g, shape.color.b,
                                               shape.color.a);
                        if (shape.fill == true) {
                            SDL_RenderFillRect(renderer_, &rect);
                        }
                        SDL_RenderDrawRect(renderer_, &rect);
                    }
                }
                SDL_RenderPresent(renderer_);
            }
        }
    }

    SDL_Rect get_object_camera_position(ObjectData object,
                                        CameraData camera) const {
        if (camera.size <= 0) {
            // Invalid camera size
            throw ValiantError("Invalid camera size: " +
                               std::to_string(camera.size));
        }
        int width = static_cast<int>((object.width / camera.size) *
                                     DEFAULT_CAMERA_SIZE);
        int height = static_cast<int>((object.height / camera.size) *
                                      DEFAULT_CAMERA_SIZE);
        int x = (((object.position.x / camera.size) + (window_width_ / 2)) -
                 camera.position.x) -
                (width / 2);
        int y = (((object.position.y / camera.size) + (window_height_ / 2)) -
                 camera.position.y) -
                (height / 2);
        return {x, y, width, height};
    }

   private:
    std::vector<Object*> objects_;
    Color background_color_;
    Camera* camera_;
    int window_width_;
    int window_height_;
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
                SDL_WINDOWPOS_UNDEFINED, window_width_, window_height_,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
