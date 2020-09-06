#include <SDL2/SDL.h>

#include <catch2/catch.hpp>
#include <cstdint>

#include "../valiant/color.hpp"
#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"

void test_object_camera_position(const valiant::Renderer &renderer,
                                 valiant::ObjectData object,
                                 valiant::CameraData camera) {
    SDL_Rect rect = renderer.get_object_camera_position(object, camera);
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
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    int renderer_width = renderer.window_width();
    int renderer_height = renderer.window_height();
    REQUIRE(renderer_width == valiant::DEFAULT_WINDOW_WIDTH);
    REQUIRE(renderer_height == valiant::DEFAULT_WINDOW_HEIGHT);
}

TEST_CASE("Renderer background") {
    SECTION("Default background color") {
        valiant::Renderer renderer(valiant::RenderMode::DISABLE);
        valiant::Color background_color = renderer.background_color();
        REQUIRE(background_color.r == 0);
        REQUIRE(background_color.g == 0);
        REQUIRE(background_color.b == 0);
        REQUIRE(background_color.a == 255);
    }
    SECTION("Set background color without color object") {
        valiant::Renderer renderer(valiant::RenderMode::DISABLE);
        renderer.set_background_color(255, 255, 255, 0);
        valiant::Color background_color = renderer.background_color();
        REQUIRE(background_color.r == 255);
        REQUIRE(background_color.g == 255);
        REQUIRE(background_color.b == 255);
        REQUIRE(background_color.a == 0);
    }
    SECTION("Set background color with color object") {
        valiant::Renderer renderer(valiant::RenderMode::DISABLE);
        valiant::Color new_background_color(255, 255, 255, 0);
        renderer.set_background_color(new_background_color);
        valiant::Color background_color = renderer.background_color();
        REQUIRE(background_color.r == 255);
        REQUIRE(background_color.g == 255);
        REQUIRE(background_color.b == 255);
        REQUIRE(background_color.a == 0);
    }
}

TEST_CASE("Renderer invalid camera size exception") {
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
    valiant::CameraData invalid_camera_data_1 = {0., {0, 0, 0}};
    valiant::CameraData invalid_camera_data_2 = {-1., {0, 0, 0}};
    REQUIRE_THROWS_WITH(
        renderer.get_object_camera_position(object_data, invalid_camera_data_1),
        "Invalid camera size: " + std::to_string(invalid_camera_data_1.size));
    REQUIRE_THROWS_WITH(
        renderer.get_object_camera_position(object_data, invalid_camera_data_2),
        "Invalid camera size: " + std::to_string(invalid_camera_data_2.size));
}

TEST_CASE("Renderer camera positioning") {
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
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
    class Player : public valiant::Object,
                   public valiant::Rectangle,
                   public valiant::Collider {
       public:
        bool has_collided{false};
        bool has_stay{false};
        bool has_exit{false};

        void start() override {
            transform.position.x = 0;
            transform.position.y = 0;
            transform.position.z = 0;
            shape.width = 50;
            shape.height = 50;
        }

        void on_collision_enter(const valiant::Collision &collision) override {
            has_collided = true;
        }

        void on_collision_stay(const valiant::Collision &collision) override {
            has_stay = true;
        }

        void on_collision_exit(const valiant::Collision &collision) override {
            has_exit = true;
        }
    };
    class Enemy : public valiant::Object,
                  public valiant::Rectangle,
                  public valiant::Collider {
       public:
        bool has_collided{false};
        bool has_stay{false};
        bool has_exit{false};

        void start() override {
            transform.position.x = 100;
            transform.position.y = 100;
            transform.position.z = 100;
            shape.width = 50;
            shape.height = 50;
        }

        void move_towards() { transform.position = {0, 0, 0}; }

        void move_away() { transform.position = {100, 100, 100}; }

        void on_collision_enter(const valiant::Collision &collision) override {
            has_collided = true;
        }

        void on_collision_stay(const valiant::Collision &collision) override {
            has_stay = true;
        }

        void on_collision_exit(const valiant::Collision &collision) override {
            has_exit = true;
        }
    };
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    valiant::CameraData camera_data = {1., {0, 0, 0}};
    Player player;
    Enemy enemy;
    renderer.add_object(player);
    renderer.add_object(enemy);
    renderer.run();
    // At this point the player and enemy object should not be colliding
    renderer.process_collisions(camera_data);
    REQUIRE(player.has_collided == false);
    REQUIRE(enemy.has_collided == false);
    // Make enemy collide with player by manually changing position
    enemy.move_towards();
    renderer.process_collisions(camera_data);
    REQUIRE(player.has_collided == true);
    REQUIRE(enemy.has_collided == true);
    REQUIRE(player.has_stay == false);
    REQUIRE(enemy.has_stay == false);
    REQUIRE(player.has_exit == false);
    REQUIRE(enemy.has_exit == false);
    // Process collisions once again to register trigger on collision stay
    // method
    renderer.process_collisions(camera_data);
    REQUIRE(player.has_stay == true);
    REQUIRE(player.has_stay == true);
    REQUIRE(player.has_exit == false);
    REQUIRE(enemy.has_exit == false);
    // Make enemy move away from player to remove collision
    enemy.move_away();
    renderer.process_collisions(camera_data);
    REQUIRE(player.has_exit == true);
    REQUIRE(enemy.has_exit == true);
}
TEST_CASE("Renderer is colliding method") {
    SECTION("Test intersecting") {
        SDL_Rect rect_1 = {0, 0, 50, 50};
        SDL_Rect rect_2 = {49, 49, 50, 50};
        bool is_colliding = valiant::Renderer::is_colliding(rect_1, rect_2);
        REQUIRE(is_colliding == true);
    }
    SECTION("Test non-intersecting") {
        SDL_Rect rect_1 = {0, 0, 50, 50};
        SDL_Rect rect_2 = {50, 50, 50, 50};
        bool is_colliding = valiant::Renderer::is_colliding(rect_1, rect_2);
        REQUIRE(is_colliding == false);
    }
}

TEST_CASE("Renderer get object data") {
    class SpriteObject : public valiant::Object,
                         public valiant::SpriteRenderer {
       public:
        void start() override {
            sprite_renderer.sprite.width = 1;
            sprite_renderer.sprite.height = 1;
            transform.position = {1, 1, 1};
        }
    };
    class RectangleObject : public valiant::Object, public valiant::Rectangle {
       public:
        void start() override {
            shape.width = 1;
            shape.height = 1;
            transform.position = {1, 1, 1};
        }
    };
    SECTION("Original data") {
        SpriteObject sprite_object;
        RectangleObject rectangle_object;
        valiant::ObjectData sprite_object_data =
            valiant::Renderer::get_object_data(&sprite_object);
        valiant::ObjectData rectangle_object_data =
            valiant::Renderer::get_object_data(&rectangle_object);
        REQUIRE(sprite_object_data.width == 0);
        REQUIRE(sprite_object_data.height == 0);
        REQUIRE(sprite_object_data.position.x == 0);
        REQUIRE(sprite_object_data.position.y == 0);
        REQUIRE(sprite_object_data.position.z == 0);
        REQUIRE(rectangle_object_data.width == 0);
        REQUIRE(rectangle_object_data.height == 0);
        REQUIRE(rectangle_object_data.position.x == 0);
        REQUIRE(rectangle_object_data.position.y == 0);
        REQUIRE(rectangle_object_data.position.z == 0);
    }
    SECTION("Changed data") {
        valiant::Renderer renderer(valiant::RenderMode::DISABLE);
        SpriteObject sprite_object;
        RectangleObject rectangle_object;
        renderer.add_object(sprite_object);
        renderer.add_object(rectangle_object);
        renderer.run();
        valiant::ObjectData sprite_object_data =
            valiant::Renderer::get_object_data(&sprite_object);
        valiant::ObjectData rectangle_object_data =
            valiant::Renderer::get_object_data(&rectangle_object);
        REQUIRE(sprite_object_data.width == 1);
        REQUIRE(sprite_object_data.height == 1);
        REQUIRE(sprite_object_data.position.x == 1);
        REQUIRE(sprite_object_data.position.y == 1);
        REQUIRE(sprite_object_data.position.z == 1);
        REQUIRE(rectangle_object_data.width == 1);
        REQUIRE(rectangle_object_data.height == 1);
        REQUIRE(rectangle_object_data.position.x == 1);
        REQUIRE(rectangle_object_data.position.y == 1);
        REQUIRE(rectangle_object_data.position.z == 1);
    }
}

TEST_CASE("Renderer get collision from object") {
    valiant::Object object;
    valiant::Collision collision_1 =
        valiant::Renderer::get_collision_from_object(&object);
    object.transform.position = {1, 1, 1};
    object.tag = "tagged";
    valiant::Collision collision_2 =
        valiant::Renderer::get_collision_from_object(&object);
    // Test collision before changes (default values)
    REQUIRE(collision_1.transform.position.x == 0);
    REQUIRE(collision_1.transform.position.y == 0);
    REQUIRE(collision_1.transform.position.z == 0);
    REQUIRE(collision_1.tag == "untagged");
    // Test collision after changes
    REQUIRE(collision_2.transform.position.x == 1);
    REQUIRE(collision_2.transform.position.y == 1);
    REQUIRE(collision_2.transform.position.z == 1);
    REQUIRE(collision_2.tag == "tagged");
}

TEST_CASE("Renderer collider object registration") {
    class ObjectWithCollider : public valiant::Object,
                               public valiant::Collider {};
    class ObjectWithoutCollider : public valiant::Object {};
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    ObjectWithCollider object_with_collider;
    ObjectWithoutCollider object_without_collider;
    renderer.add_object(object_with_collider);
    renderer.add_object(object_without_collider);
    renderer.fill_collider_objects();
    // Only one object added has a collider
    size_t collider_size = renderer.collider_objects_size();
    REQUIRE(collider_size == 1);
}
