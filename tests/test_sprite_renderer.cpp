#include <catch2/catch.hpp>
#include <string>

#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"
#include "../valiant/sprite_renderer.hpp"

TEST_CASE("Sprite renderer properties") {
    std::string fake_sprite_path = "this is a fake sprite path";
    class SpriteObject : public valiant::Object,
                         public valiant::SpriteRenderer {
       public:
        explicit SpriteObject(const std::string &fake_sprite_path)
            : fake_sprite_path_(fake_sprite_path) {}
        void start() override {
            sprite_renderer.flip_x = true;
            sprite_renderer.flip_y = true;
        }
        inline void set_sprite() { sprite_renderer.sprite = fake_sprite_path_; }

       private:
        std::string fake_sprite_path_;
    };
    SpriteObject sprite_object(fake_sprite_path);
    valiant::Renderer renderer;
    // Test sprite renderer component default values
    valiant::SpriteRendererComponent sprite_renderer =
        sprite_object.sprite_renderer;
    REQUIRE(sprite_renderer.sprite.path == "");
    REQUIRE(sprite_renderer.flip_x == false);
    REQUIRE(sprite_renderer.flip_y == false);
    renderer.add_object(sprite_object);
    renderer.run();
    sprite_renderer = sprite_object.sprite_renderer;
    REQUIRE_THROWS_WITH(sprite_object.set_sprite(),
                        Catch::Contains(fake_sprite_path));
    REQUIRE(sprite_renderer.flip_x == true);
    REQUIRE(sprite_renderer.flip_y == true);
}

TEST_CASE("Sprite renderer successful sprite") {
    std::string sprite_path = "examples/assets/sprite.png";
    class SpriteObject : public valiant::Object,
                         public valiant::SpriteRenderer {
       public:
        explicit SpriteObject(const std::string &sprite_path)
            : sprite_path_(sprite_path) {}
        inline void set_sprite() { sprite_renderer.sprite = sprite_path_; }

       private:
        std::string sprite_path_;
    };
    SpriteObject sprite_object(sprite_path);
    valiant::Renderer renderer;
    renderer.add_object(sprite_object);
    renderer.run();
    REQUIRE_NOTHROW(sprite_object.set_sprite());
    REQUIRE(sprite_object.sprite_renderer.sprite.path == sprite_path);
}