#include "../valiant/valiant.hpp"

class Player : public valiant::Object,
               public valiant::Rectangle,
               public valiant::Collider {
   public:
    Player() : move_speed_(300), w_(false), a_(false), s_(false), d_(false) {}
    void start() override {
        shape.width = 50;
        shape.height = 50;
        shape.color.r = 255;
        shape.color.g = 0;
        shape.color.b = 255;
        shape.color.a = 255;
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

        move();
    }

    void on_collision_stay(const valiant::Collision &collision) override {
        shape.color.g = 255;
    }

    void on_collision_exit(const valiant::Collision &collision) override {
        shape.color.g = 0;
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

class RectangleObject : public valiant::Object,
                        public valiant::Rectangle,
                        public valiant::Collider {
   public:
    void start() override {
        shape.width = 100;
        shape.height = 100;
        shape.color.r = 255;
        shape.color.g = 0;
        shape.color.b = 0;
        shape.color.a = 255;
        transform.position.x = -75;
    }
};

int main() {
    valiant::Renderer renderer(valiant::ENABLE | valiant::VSYNC);
    Player player;
    RectangleObject rectangle_object;
    renderer.add_object(player);
    renderer.add_object(rectangle_object);
    renderer.run();
}
