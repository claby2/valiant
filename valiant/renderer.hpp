#ifndef VALIANT_RENDERER_HPP
#define VALIANT_RENDERER_HPP

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cstdint>
#include <string>
#include <vector>

#include "camera.hpp"
#include "collider.hpp"
#include "color.hpp"
#include "error.hpp"
#include "object.hpp"
#include "shape.hpp"
#include "sprite_renderer.hpp"
#include "time.hpp"

namespace valiant {
// Default dimensions represent window dimensions on start
const int DEFAULT_WINDOW_WIDTH = 640;
const int DEFAULT_WINDOW_HEIGHT = 480;
// Logical dimensions represent device independent resolution for rendering
const int LOGICAL_WINDOW_WIDTH = 1920;
const int LOGICAL_WINDOW_HEIGHT = 1080;

const Color DEFAULT_BACKGROUND_COLOR = {0, 0, 0, 255};

typedef enum : uint_fast8_t {
    ENABLE = (1 << 0),
    DISABLE = (1 << 1),
    FULLSCREEN = (1 << 2),
    VSYNC = (1 << 3)
} RenderFlags;

class ObjectManager {
   public:
    static ObjectData get_object_data(Object* object) {
        // Assume width and height as 0
        ObjectData object_data = {0, 0, object->transform.position};
        if (SpriteRenderer* sprite_renderer =
                dynamic_cast<SpriteRenderer*>(object)) {
            object_data.width = sprite_renderer->sprite_renderer.sprite.width;
            object_data.height = sprite_renderer->sprite_renderer.sprite.height;
        } else if (Rectangle* rectangle = dynamic_cast<Rectangle*>(object)) {
            object_data.width = rectangle->shape.width;
            object_data.height = rectangle->shape.height;
        }
        return object_data;
    }

    static SDL_Rect get_object_camera_position(ObjectData object,
                                               CameraData camera) {
        if (camera.size <= 0) {
            // Invalid camera size
            throw ValiantError("Invalid camera size: " +
                               std::to_string(camera.size));
        }
        int width = static_cast<int>(
            ((object.width / camera.size) * DEFAULT_CAMERA_SIZE));
        int height = static_cast<int>(
            ((object.height / camera.size) * DEFAULT_CAMERA_SIZE));
        int x = static_cast<int>((object.position.x / camera.size) +
                                 (LOGICAL_WINDOW_WIDTH / 2) -
                                 camera.position.x - (width / 2));
        int y = static_cast<int>((object.position.y / camera.size) +
                                 (LOGICAL_WINDOW_HEIGHT / 2) -
                                 camera.position.y - (height / 2));
        return {x, y, width, height};
    }
};

class CollisionManager : public ObjectManager {
   public:
    CollisionManager() {}

    CollisionManager(const CollisionManager& collision_manager)
        : collider_objects_(collision_manager.collider_objects_),
          collision_matrix_(collision_manager.collision_matrix_) {}

    void process_collisions(CameraData camera) {
        for (size_t i = 0; i < collider_objects_.size(); ++i) {
            CollisionData collision_1(collider_objects_[i], camera);
            for (size_t j = i + 1; j < collider_objects_.size(); ++j) {
                CollisionData collision_2(collider_objects_[j], camera);
                if ((!collision_1.collider->collider.enabled ||
                     !collision_2.collider->collider.enabled) ||
                    (!is_colliding(collision_1.rect, collision_2.rect))) {
                    if (collision_matrix_[i][j]) {
                        // Object at i and j were previously colliding. Execute
                        // exit collision method
                        collision_1.collider->on_collision_exit(
                            get_collision_from_object(collision_2.object));
                        collision_2.collider->on_collision_exit(
                            get_collision_from_object(collision_1.object));
                    }
                    collision_matrix_[i][j] = false;
                } else {
                    // A valid collision has been detected
                    if (collision_matrix_[i][j]) {
                        // Object at i and j previous collided before and
                        // are right colliding right now
                        collision_1.collider->on_collision_stay(
                            get_collision_from_object(collision_2.object));
                        collision_2.collider->on_collision_stay(
                            get_collision_from_object(collision_1.object));
                    } else {
                        // Object at i and j were not colliding previously,
                        // but are colliding right now
                        collision_1.collider->on_collision_enter(
                            get_collision_from_object(collision_2.object));
                        collision_2.collider->on_collision_enter(
                            get_collision_from_object(collision_1.object));
                        collision_matrix_[i][j] = true;
                    }
                }
            }
        }
    }

    static bool is_colliding(SDL_Rect& object_1, SDL_Rect& object_2) {
        return SDL_HasIntersection(&object_1, &object_2);
    }

    static inline Collision get_collision_from_object(Object* object) {
        return {object->transform, object->tag};
    }

    void fill_collider_objects(const std::vector<Object*>& objects) {
        for (auto object : objects) {
            Collider* collider_object = dynamic_cast<Collider*>(object);
            if (collider_object) {
                collider_objects_.push_back(object);
            }
        }
        // Initialize collision matrix
        for (size_t i = 0; i < collider_objects_.size(); ++i) {
            std::vector<bool> row(collider_objects_.size(), false);
            collision_matrix_.push_back(row);
        }
    }

    inline size_t collider_objects_size() const {
        return collider_objects_.size();
    }

   private:
    std::vector<Object*> collider_objects_;
    std::vector<std::vector<bool>> collision_matrix_;

    struct CollisionData {
        Object* object;
        Collider* collider;
        SDL_Rect rect;

        CollisionData(Object* new_object, CameraData camera)
            : object(new_object),
              collider(dynamic_cast<Collider*>(new_object)),
              rect(get_object_camera_position(get_object_data(new_object),
                                              camera)) {}
    };
};

class Renderer : public ObjectManager {
   public:
    explicit Renderer(uint_fast8_t flags = ENABLE)
        : flags_(flags),
          background_color_(DEFAULT_BACKGROUND_COLOR),
          camera_(nullptr),
          window_width_(DEFAULT_WINDOW_WIDTH),
          window_height_(DEFAULT_WINDOW_HEIGHT),
          has_camera_(false),
          renderer_(nullptr),
          window_(nullptr) {
        initialize_sdl();
    }

    ~Renderer() { close_sdl(); }

    Renderer(const Renderer& renderer)
        : flags_(renderer.flags_),
          collision_manager_(renderer.collision_manager_),
          objects_(renderer.objects_),
          background_color_(renderer.background_color_),
          camera_(renderer.camera_),
          window_width_(renderer.window_width_),
          window_height_(renderer.window_height_),
          has_camera_(renderer.has_camera_),
          renderer_(renderer.renderer_),
          window_(renderer.window_) {}

    template <typename T>
    inline void add_object(T& object) {
        objects_.push_back(&object);
    }

    inline void add_camera(Camera& camera) {
        has_camera_ = true;
        camera_ = &camera;
    }

    auto window_width() const -> int { return window_width_; }

    auto window_height() const -> int { return window_height_; }

    auto background_color() const -> Color { return background_color_; }

    std::vector<Object*> get_objects() { return objects_; }

    void set_background_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        background_color_.r = r;
        background_color_.g = g;
        background_color_.b = b;
        background_color_.a = a;
    }

    void set_background_color(Color color) { background_color_ = color; }

    void set_window_size(int width, int height) {
        if (width <= 0 || height <= 0) {
            throw ValiantError("Window size must be > 0");
        }
        window_width_ = width;
        window_height_ = height;
        SDL_SetWindowSize(window_, window_width_, window_height_);
    }

    static uint32_t get_window_flags(uint_fast8_t flags) {
        uint32_t window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
        if (flags & FULLSCREEN) {
            window_flags |= SDL_WINDOW_FULLSCREEN;
        }
        return window_flags;
    }

    static uint32_t get_renderer_flags(uint_fast8_t flags) {
        uint32_t renderer_flags = SDL_RENDERER_ACCELERATED;
        if (flags & VSYNC) {
            renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
        }
        return renderer_flags;
    }

    void run() {
        collision_manager_.fill_collider_objects(objects_);
        // Check if camera has been added
        Camera new_camera;
        if (!has_camera_) {
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
        if (flags_ & ENABLE) {
            SDL_Event event;
            bool quit = false;
            uint64_t start = SDL_GetPerformanceCounter();
            while (!quit) {
                SDL_GetWindowSize(window_, &window_width_, &window_height_);
                uint64_t last = start;
                start = SDL_GetPerformanceCounter();
                // Calculate delta time in seconds
                double delta =
                    (start - last) /
                    static_cast<double>(SDL_GetPerformanceFrequency());
                if (SDL_PollEvent(&event) != 0) {
                    if (event.type == SDL_QUIT) {
                        quit = true;
                    }
                }
                // Run update methods
                for (auto object : objects_) {
                    object->input.event = event;
                    object->time.set(delta);
                    object->update();
                }
                camera_->input.event = event;
                camera_->time.set(delta);
                camera_->update();
                // Get camera data
                CameraData camera_data = {camera_->camera.size,
                                          camera_->transform.position};
                // Rendering
                SDL_SetRenderDrawColor(renderer_, background_color_.r,
                                       background_color_.g, background_color_.b,
                                       background_color_.a);
                SDL_RenderClear(renderer_);
                render(camera_data);
                collision_manager_.process_collisions(camera_data);
                SDL_RenderPresent(renderer_);
            }
        }
    }

   private:
    uint_fast8_t flags_;
    CollisionManager collision_manager_;
    std::vector<Object*> objects_;
    Color background_color_;
    Camera* camera_{nullptr};
    int window_width_;
    int window_height_;
    bool has_camera_{false};
    SDL_Renderer* renderer_;
    SDL_Window* window_;

    void render(CameraData camera_data) {
        for (auto object : objects_) {
            if (SpriteRenderer* sprite_object =
                    dynamic_cast<SpriteRenderer*>(object)) {
                // Object has sprite renderer component
                SpriteRendererComponent sprite_renderer =
                    sprite_object->sprite_renderer;
                if (sprite_renderer.sprite.texture == nullptr) {
                    // Texture has not been created yet
                    sprite_object->sprite_renderer.sprite.create_texture(
                        renderer_);
                }
                SDL_Rect rect = get_object_camera_position(
                    get_object_data(object), camera_data);
                SDL_RendererFlip flip = (SDL_RendererFlip)(
                    (sprite_renderer.flip_x ? SDL_FLIP_HORIZONTAL : 0) |
                    (sprite_renderer.flip_y ? SDL_FLIP_VERTICAL : 0));
                SDL_RenderCopyEx(renderer_, sprite_renderer.sprite.texture,
                                 nullptr, &rect, 0.0, nullptr, flip);
            } else if (Rectangle* rectangle_object =
                           dynamic_cast<Rectangle*>(object)) {
                // Object has rectangle component
                Shape shape = rectangle_object->shape;
                SDL_Rect rect = get_object_camera_position(
                    get_object_data(object), camera_data);
                SDL_SetRenderDrawColor(renderer_, shape.color.r, shape.color.g,
                                       shape.color.b, shape.color.a);
                if (shape.fill == true) {
                    SDL_RenderFillRect(renderer_, &rect);
                }
                SDL_RenderDrawRect(renderer_, &rect);
            }
        }
    }

    void initialize_sdl() {
        if (flags_ & ENABLE) {
            SDL_Init(SDL_INIT_VIDEO);
            IMG_Init(IMG_INIT_PNG);
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
            window_ =
                SDL_CreateWindow("Valiant Engine", SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED, window_width_,
                                 window_height_, get_window_flags(flags_));
            renderer_ =
                SDL_CreateRenderer(window_, -1, get_renderer_flags(flags_));
            SDL_RenderSetLogicalSize(renderer_, LOGICAL_WINDOW_WIDTH,
                                     LOGICAL_WINDOW_HEIGHT);
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
