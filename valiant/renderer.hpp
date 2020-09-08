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

enum class RenderMode { ENABLE, DISABLE };

struct ObjectData {
    int width;
    int height;
    Vector3 position;

    bool operator==(const ObjectData& object_data) const {
        return (width == object_data.width && height == object_data.height &&
                position == object_data.position);
    }
};

struct CameraData {
    float size;
    Vector3 position;

    bool operator==(const CameraData& camera_data) const {
        return (size == camera_data.size && position == camera_data.position);
    }
};

class Renderer {
   public:
    explicit Renderer(RenderMode render_mode = RenderMode::ENABLE)
        : background_color_(DEFAULT_BACKGROUND_COLOR),
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

    Renderer(const Renderer& renderer)
        : objects_(renderer.objects_),
          collider_objects_(renderer.collider_objects_),
          collision_matrix_(renderer.collision_matrix_),
          background_color_(renderer.background_color_),
          camera_(renderer.camera_),
          window_width_(renderer.window_width_),
          window_height_(renderer.window_height_),
          has_camera_(renderer.has_camera_),
          render_mode_(renderer.render_mode_),
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

    void set_background_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        background_color_.r = r;
        background_color_.g = g;
        background_color_.b = b;
        background_color_.a = a;
    }

    void set_background_color(Color color) { background_color_ = color; }

    void run() {
        fill_collider_objects();
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
        if (render_mode_ == RenderMode::ENABLE) {
            SDL_Event event;
            bool quit = false;
            uint64_t start = 0;
            while (!quit) {
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
                        SDL_Rect rect = get_object_camera_position(
                            get_object_data(object), camera_data);
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
                        SDL_Rect rect = get_object_camera_position(
                            get_object_data(object), camera_data);
                        SDL_SetRenderDrawColor(renderer_, shape.color.r,
                                               shape.color.g, shape.color.b,
                                               shape.color.a);
                        if (shape.fill == true) {
                            SDL_RenderFillRect(renderer_, &rect);
                        }
                        SDL_RenderDrawRect(renderer_, &rect);
                    }
                }
                process_collisions(camera_data);
                SDL_RenderPresent(renderer_);
            }
        }
    }

    static auto get_object_camera_position(ObjectData object, CameraData camera)
        -> SDL_Rect {
        if (camera.size <= 0) {
            // Invalid camera size
            throw ValiantError("Invalid camera size: " +
                               std::to_string(camera.size));
        }
        int width = static_cast<int>(
            ((object.width / camera.size) * DEFAULT_CAMERA_SIZE));
        int height = static_cast<int>(
            ((object.height / camera.size) * DEFAULT_CAMERA_SIZE));
        int x = static_cast<int>(((object.position.x / camera.size) +
                                  (LOGICAL_WINDOW_WIDTH / 2)) -
                                 camera.position.x) -
                (width / 2);
        int y = static_cast<int>(((object.position.y / camera.size) +
                                  (LOGICAL_WINDOW_HEIGHT / 2)) -
                                 camera.position.y) -
                (height / 2);
        return {x, y, width, height};
    }

    void process_collisions(CameraData camera) {
        for (size_t i = 0; i < collider_objects_.size(); ++i) {
            Object* object_1 = collider_objects_[i];
            Collider* collider_1 = dynamic_cast<Collider*>(object_1);
            SDL_Rect object_1_rect =
                get_object_camera_position(get_object_data(object_1), camera);
            for (size_t j = i + 1; j < collider_objects_.size(); ++j) {
                Object* object_2 = collider_objects_[j];
                Collider* collider_2 = dynamic_cast<Collider*>(object_2);
                SDL_Rect object_2_rect = get_object_camera_position(
                    get_object_data(object_2), camera);
                if ((!collider_1->collider.enabled ||
                     !collider_2->collider.enabled) ||
                    (!is_colliding(object_1_rect, object_2_rect))) {
                    if (collision_matrix_[i][j]) {
                        // Object at i and j were previously colliding. Execute
                        // exit collision method
                        collider_1->on_collision_exit(
                            get_collision_from_object(object_1));
                        collider_2->on_collision_exit(
                            get_collision_from_object(object_2));
                    }
                    collision_matrix_[i][j] = false;
                } else {
                    if (collision_matrix_[i][j]) {
                        // Object at i and j previous collided before and
                        // are right colliding right now
                        collider_1->on_collision_stay(
                            get_collision_from_object(object_1));
                        collider_2->on_collision_stay(
                            get_collision_from_object(object_2));
                    } else {
                        // Object at i and j were not colliding previously,
                        // but are colliding right now
                        collider_1->on_collision_enter(
                            get_collision_from_object(object_1));
                        collider_2->on_collision_enter(
                            get_collision_from_object(object_2));
                        collision_matrix_[i][j] = true;
                    }
                }
            }
        }
    }

    static bool is_colliding(SDL_Rect& object_1, SDL_Rect& object_2) {
        return SDL_HasIntersection(&object_1, &object_2);
    }

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

    static inline Collision get_collision_from_object(Object* object) {
        return {object->transform, object->tag};
    }

    void fill_collider_objects() {
        for (auto object : objects_) {
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
    std::vector<Object*> objects_;
    std::vector<Object*> collider_objects_;
    std::vector<std::vector<bool>> collision_matrix_;
    Color background_color_;
    Camera* camera_{nullptr};
    int window_width_;
    int window_height_;
    bool has_camera_{false};
    RenderMode render_mode_;
    SDL_Renderer* renderer_;
    SDL_Window* window_;

    void initialize_sdl() {
        if (render_mode_ == RenderMode::ENABLE) {
            SDL_Init(SDL_INIT_VIDEO);
            IMG_Init(IMG_INIT_PNG);
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
            window_ = SDL_CreateWindow(
                "Valiant Engine", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, window_width_, window_height_,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            renderer_ =
                SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
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
