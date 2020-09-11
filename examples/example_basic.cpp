#include "../valiant/valiant.hpp"

class Camera : public valiant::Camera {
   public:
    void update() override {
        if (input.get_key("J")) {
            camera.size += 0.1;
        }
        if (input.get_key("K")) {
            camera.size -= camera.size > 0.1 ? 0.1 : 0;
        }
    }
};

class Player : public valiant::Object, public valiant::SpriteRenderer {
   public:
    Player() : move_speed_(150), w_(false), a_(false), s_(false), d_(false) {}
    void start() override {
        sprite_renderer.sprite = "examples/assets/sprite.png";
    }
    void update() override {
        if (input.get_key("W")) {
            w_ = true;
        }
        if (input.get_key("A")) {
            a_ = true;
        }
        if (input.get_key("S")) {
            s_ = true;
        }
        if (input.get_key("D")) {
            d_ = true;
        }

        if (input.get_key_up("W")) {
            w_ = false;
        }
        if (input.get_key_up("A")) {
            a_ = false;
        }
        if (input.get_key_up("S")) {
            s_ = false;
        }
        if (input.get_key_up("D")) {
            d_ = false;
        }

        if (input.get_key_down("R")) {
            sprite_renderer.flip_x = !sprite_renderer.flip_x;
        }

        move();
    }

   private:
    const float move_speed_;
    bool w_;
    bool a_;
    bool s_;
    bool d_;

    void move() {
        float delta = move_speed_ * time.delta_time;
        if (w_) {
            transform.position.y -= delta;
        }
        if (a_) {
            transform.position.x -= delta;
        }
        if (s_) {
            transform.position.y += delta;
        }
        if (d_) {
            transform.position.x += delta;
        }
    }
};

int main() {
    valiant::Renderer renderer(valiant::ENABLE | valiant::VSYNC);
    Player player;
    Camera camera;
    renderer.add_object(player);
    renderer.add_camera(camera);
    renderer.run();
}
