#include <iostream>

#include "../valiant/valiant.hpp"

class Player : public valiant::Object, public valiant::SpriteRenderer {
   public:
    Player() : move_speed_(10) {}
    void start() override {
        sprite_renderer.sprite = "examples/assets/sprite.png";
    }
    void update() override {
        if (input.get_key("W")) {
            transform.position.y -= move_speed_;
        }
        if (input.get_key("A")) {
            transform.position.x -= move_speed_;
        }
        if (input.get_key("S")) {
            transform.position.y += move_speed_;
        }
        if (input.get_key("D")) {
            transform.position.x += move_speed_;
        }
        if (input.get_key_down("R")) {
            sprite_renderer.flip_x = !sprite_renderer.flip_x;
        }
    }

   private:
    const int move_speed_;
};

int main() {
    valiant::Renderer renderer(true);
    Player player;
    renderer.add_object(player);
    renderer.run();
}