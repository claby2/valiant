#include <catch2/catch.hpp>
#include <string>

#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"

TEST_CASE("Object creation") {
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
    valiant::Renderer renderer;
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
    REQUIRE(position.x == 0);
    REQUIRE(position.y == 0);
    REQUIRE(position.z == 0);
    valiant::Renderer renderer;
    renderer.add_object(player);
    renderer.run();
    position = player.transform.position;
    REQUIRE(position.x == 1);
    REQUIRE(position.y == 2);
    REQUIRE(position.z == 3);
}