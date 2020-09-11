#include <SDL2/SDL.h>

#include <catch2/catch.hpp>
#include <cstdint>

#include "../valiant/collider.hpp"
#include "../valiant/color.hpp"
#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"

void test_object_camera_position(const valiant::Renderer &renderer,
                                 valiant::ObjectData object,
                                 valiant::CameraData camera) {
    SDL_Rect rect =
        valiant::ObjectManager::get_object_camera_position(object, camera);
    int expected_x = (valiant::LOGICAL_WINDOW_WIDTH / 2) -
                     (object.width / (2 * camera.size));
    int expected_y = (valiant::LOGICAL_WINDOW_HEIGHT / 2) -
                     (object.height / (2 * camera.size));
    int expected_w = (object.width / camera.size);
    int expected_h = (object.height / camera.size);
    REQUIRE(rect.x == expected_x);
    REQUIRE(rect.y == expected_y);
    REQUIRE(rect.w == expected_w);
    REQUIRE(rect.h == expected_h);
}

TEST_CASE("Renderer default window dimensions") {
    valiant::Renderer renderer(valiant::DISABLE);
    int renderer_width = renderer.window_width();
    int renderer_height = renderer.window_height();
    REQUIRE(renderer_width == valiant::DEFAULT_WINDOW_WIDTH);
    REQUIRE(renderer_height == valiant::DEFAULT_WINDOW_HEIGHT);
}

TEST_CASE("Renderer background") {
    SECTION("Default background color") {
        valiant::Renderer renderer(valiant::DISABLE);
        valiant::Color background_color = renderer.background_color();
        valiant::Color expected_color = {0, 0, 0, 255};
        REQUIRE(background_color == expected_color);
    }
    SECTION("Set background color without color object") {
        valiant::Renderer renderer(valiant::DISABLE);
        valiant::Color expected_color = {255, 255, 255, 0};
        renderer.set_background_color(expected_color.r, expected_color.g,
                                      expected_color.b, expected_color.a);
        valiant::Color background_color = renderer.background_color();
        REQUIRE(background_color == expected_color);
    }
    SECTION("Set background color with color object") {
        valiant::Renderer renderer(valiant::DISABLE);
        valiant::Color expected_color = {255, 255, 255, 0};
        valiant::Color new_background_color(expected_color);
        renderer.set_background_color(new_background_color);
        valiant::Color background_color = renderer.background_color();
        REQUIRE(background_color == expected_color);
    }
}

TEST_CASE("Renderer invalid camera size exception") {
    valiant::Renderer renderer(valiant::DISABLE);
    valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
    valiant::CameraData invalid_camera_data_1 = {0., {0, 0, 0}};
    valiant::CameraData invalid_camera_data_2 = {-1., {0, 0, 0}};
    REQUIRE_THROWS_WITH(
        valiant::ObjectManager::get_object_camera_position(
            object_data, invalid_camera_data_1),
        "Invalid camera size: " + std::to_string(invalid_camera_data_1.size));
    REQUIRE_THROWS_WITH(
        valiant::ObjectManager::get_object_camera_position(
            object_data, invalid_camera_data_2),
        "Invalid camera size: " + std::to_string(invalid_camera_data_2.size));
}

TEST_CASE("Renderer camera positioning") {
    valiant::Renderer renderer(valiant::DISABLE);
    SECTION("Center positioning") {
        valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
        valiant::CameraData camera_data = {1., {0, 0, 0}};
        test_object_camera_position(renderer, object_data, camera_data);
    }
    SECTION("Increased camera size") {
        valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
        valiant::CameraData camera_data = {2., {0, 0, 0}};
        test_object_camera_position(renderer, object_data, camera_data);
    }
    SECTION("Decreased camera size") {
        valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
        valiant::CameraData camera_data = {0.5, {0, 0, 0}};
        test_object_camera_position(renderer, object_data, camera_data);
    }
}

TEST_CASE("Renderer collision processing") {
    SECTION("Collider methods") {
        class ColliderObject : public valiant::Collider {
           public:
            bool has_collided{false};
            bool has_stay{false};
            bool has_exit{false};

            void on_collision_enter(
                const valiant::Collision &collision) override {
                has_collided = true;
            }

            void on_collision_stay(
                const valiant::Collision &collision) override {
                has_stay = true;
            }

            void on_collision_exit(
                const valiant::Collision &collision) override {
                has_exit = true;
            }
        };
        class Player : public valiant::Object,
                       public valiant::Rectangle,
                       public ColliderObject {
           public:
            void start() override {
                transform.position.x = 0;
                transform.position.y = 0;
                transform.position.z = 0;
                shape.width = 50;
                shape.height = 50;
            }
        };
        class Enemy : public valiant::Object,
                      public valiant::Rectangle,
                      public ColliderObject {
           public:
            void start() override {
                transform.position.x = 100;
                transform.position.y = 100;
                transform.position.z = 100;
                shape.width = 50;
                shape.height = 50;
            }

            void move_towards() { transform.position = {0, 0, 0}; }

            void move_away() { transform.position = {100, 100, 100}; }
        };
        valiant::CollisionManager collision_manager;
        valiant::Renderer renderer(valiant::DISABLE);
        valiant::CameraData camera_data = {1., {0, 0, 0}};
        Player player;
        Enemy enemy;
        renderer.add_object(player);
        renderer.add_object(enemy);
        renderer.run();
        collision_manager.fill_collider_objects(renderer.get_objects());
        // At this point the player and enemy object should not be colliding
        collision_manager.process_collisions(camera_data);
        REQUIRE(player.has_collided == false);
        REQUIRE(enemy.has_collided == false);
        // Make enemy collide with player by manually changing position
        enemy.move_towards();
        collision_manager.process_collisions(camera_data);
        REQUIRE(player.has_collided == true);
        REQUIRE(enemy.has_collided == true);
        REQUIRE(player.has_stay == false);
        REQUIRE(enemy.has_stay == false);
        REQUIRE(player.has_exit == false);
        REQUIRE(enemy.has_exit == false);
        // Process collisions once again to register trigger on collision stay
        // method
        collision_manager.process_collisions(camera_data);
        REQUIRE(player.has_stay == true);
        REQUIRE(player.has_stay == true);
        REQUIRE(player.has_exit == false);
        REQUIRE(enemy.has_exit == false);
        // Make enemy move away from player to remove collision
        enemy.move_away();
        collision_manager.process_collisions(camera_data);
        REQUIRE(player.has_exit == true);
        REQUIRE(enemy.has_exit == true);
    }
    SECTION("Object tagging") {
        class Player : public valiant::Object,
                       public valiant::Rectangle,
                       public valiant::Collider {
           public:
            bool collided{false};

            void start() override {
                transform.position.x = 0;
                transform.position.y = 0;
                transform.position.z = 0;
                shape.width = 50;
                shape.height = 50;
            }

            void on_collision_enter(
                const valiant::Collision &collision) override {
                if (collision.tag == "tagged") {
                    collided = true;
                } else {
                    collided = false;
                }
            }
        };
        class Object : public valiant::Object,
                       public valiant::Rectangle,
                       public valiant::Collider {
           public:
            void start() override {
                transform.position.x = 0;
                transform.position.y = 0;
                transform.position.z = 0;
                shape.width = 50;
                shape.height = 50;
            }

            void add_tag() { tag = "tagged"; }
        };
        valiant::CollisionManager collision_manager;
        valiant::Renderer renderer(valiant::DISABLE);
        valiant::CameraData camera_data = {1., {0, 0, 0}};
        Player player;
        Object object;
        renderer.add_object(player);
        renderer.add_object(object);
        renderer.run();
        collision_manager.fill_collider_objects(renderer.get_objects());
        // Set the object's tag
        object.add_tag();
        collision_manager.process_collisions(camera_data);
        REQUIRE(player.collided == true);
    }
}

TEST_CASE("Renderer is colliding method") {
    SECTION("Test intersecting") {
        SDL_Rect rect_1 = {0, 0, 50, 50};
        SDL_Rect rect_2 = {49, 49, 50, 50};
        bool is_colliding =
            valiant::CollisionManager::is_colliding(rect_1, rect_2);
        REQUIRE(is_colliding == true);
    }
    SECTION("Test non-intersecting") {
        SDL_Rect rect_1 = {0, 0, 50, 50};
        SDL_Rect rect_2 = {50, 50, 50, 50};
        bool is_colliding =
            valiant::CollisionManager::is_colliding(rect_1, rect_2);
        REQUIRE(is_colliding == false);
    }
}

TEST_CASE("Renderer get collision from object") {
    valiant::Object object;
    valiant::Collision collision_1 =
        valiant::CollisionManager::get_collision_from_object(&object);
    object.transform.position = {1, 1, 1};
    object.tag = "tagged";
    valiant::Collision collision_2 =
        valiant::CollisionManager::get_collision_from_object(&object);
    // Test collision before changes (default values)
    valiant::Collision expected_collision_1 = {{{0, 0, 0}}, "untagged"};
    REQUIRE(collision_1 == expected_collision_1);
    // Test collision after changes
    valiant::Collision expected_collision_2 = {{{1, 1, 1}}, "tagged"};
    REQUIRE(collision_2 == expected_collision_2);
}

TEST_CASE("Renderer collider object registration") {
    class ObjectWithCollider : public valiant::Object,
                               public valiant::Collider {};
    class ObjectWithoutCollider : public valiant::Object {};
    valiant::CollisionManager collision_manager;
    valiant::Renderer renderer(valiant::DISABLE);
    ObjectWithCollider object_with_collider;
    ObjectWithoutCollider object_without_collider;
    renderer.add_object(object_with_collider);
    renderer.add_object(object_without_collider);
    collision_manager.fill_collider_objects(renderer.get_objects());
    // Only one object added has a collider
    size_t collider_size = collision_manager.collider_objects_size();
    REQUIRE(collider_size == 1);
}

TEST_CASE("Renderer get window flags") {
    uint_fast8_t input_window_flags = valiant::FULLSCREEN;
    uint32_t output_window_flags =
        valiant::Renderer::get_window_flags(input_window_flags);
    bool has_fullscreen =
        static_cast<bool>(output_window_flags & SDL_WINDOW_FULLSCREEN);
    REQUIRE(has_fullscreen == true);
}

TEST_CASE("Renderer get renderer flags") {
    uint_fast8_t input_renderer_flags = valiant::VSYNC;
    uint32_t output_renderer_flags =
        valiant::Renderer::get_renderer_flags(input_renderer_flags);
    bool has_vsync =
        static_cast<bool>(output_renderer_flags & SDL_RENDERER_PRESENTVSYNC);
    REQUIRE(has_vsync == true);
}
