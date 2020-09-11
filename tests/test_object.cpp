#include <catch2/catch.hpp>
#include <string>

#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"
#include "../valiant/shape.hpp"
#include "../valiant/sprite_renderer.hpp"

TEST_CASE("Object tagging") {
    class Player : public valiant::Object {
       public:
        void start() override { tag = "player"; }
    };
    Player player;
    valiant::Renderer renderer(valiant::DISABLE);
    std::string default_player_tag = player.tag;
    renderer.add_object(player);
    renderer.run();
    std::string new_player_tag = player.tag;
    // Test default tag
    REQUIRE(default_player_tag == "untagged");
    // Test new tag value after start method execution
    REQUIRE(new_player_tag == "player");
}
TEST_CASE("Object method execution") {
    class Player : public valiant::Object {
       public:
        Player() : awake_count_(0), start_count_(0) {}
        void awake() override { awake_count_ += 1; }
        void start() override { start_count_ += 1; }
        inline int awake_count() const { return awake_count_; }
        inline int start_count() const { return start_count_; }

       private:
        int awake_count_;
        int start_count_;
    };
    class Enemy : public valiant::Object {
       public:
        Enemy() : name_("fake_name1") {}
        void awake() override { name_ = "fake_name2"; }
        void start() override {
            if (name_ == "fake_name2") {
                // Awake has run before start
                name_ = "real_name";
            }
        }
        inline std::string name() const { return name_; }

       private:
        std::string name_;
    };
    Player player;
    Enemy enemy;
    valiant::Renderer renderer(valiant::DISABLE);
    renderer.add_object(player);
    renderer.add_object(enemy);
    renderer.run();
    int player_awake_count = player.awake_count();
    int player_start_count = player.start_count();
    std::string enemy_name = enemy.name();
    REQUIRE(player_awake_count == 1);
    REQUIRE(player_start_count == 1);
    REQUIRE(enemy_name == "real_name");
}

TEST_CASE("Object transform") {
    class Player : public valiant::Object {
        void start() override {
            transform.position.x = 1;
            transform.position.y = 2;
            transform.position.z = 3;
        }
    };
    Player player;
    valiant::Vector3 position = player.transform.position;
    // Test default position values
    valiant::Vector3 expected_position = {0, 0, 0};
    REQUIRE(position == expected_position);
    valiant::Renderer renderer(valiant::DISABLE);
    renderer.add_object(player);
    renderer.run();
    position = player.transform.position;
    // Test position values after start method has been run
    expected_position = {1, 2, 3};
    REQUIRE(position == expected_position);
}

TEST_CASE("Object get object data") {
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
            valiant::ObjectManager::get_object_data(&sprite_object);
        valiant::ObjectData rectangle_object_data =
            valiant::ObjectManager::get_object_data(&rectangle_object);
        valiant::ObjectData expected_object_data = {0, 0, {0, 0, 0}};
        REQUIRE(sprite_object_data == expected_object_data);
        REQUIRE(rectangle_object_data == expected_object_data);
    }
    SECTION("Changed data") {
        valiant::Renderer renderer(valiant::DISABLE);
        SpriteObject sprite_object;
        RectangleObject rectangle_object;
        renderer.add_object(sprite_object);
        renderer.add_object(rectangle_object);
        renderer.run();
        valiant::ObjectData sprite_object_data =
            valiant::ObjectManager::get_object_data(&sprite_object);
        valiant::ObjectData rectangle_object_data =
            valiant::ObjectManager::get_object_data(&rectangle_object);
        valiant::ObjectData expected_object_data = {1, 1, {1, 1, 1}};
        REQUIRE(sprite_object_data == expected_object_data);
        REQUIRE(rectangle_object_data == expected_object_data);
    }
}
