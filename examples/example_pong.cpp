#include <cstdlib>
#include <ctime>
#include <string>

#include "../valiant/valiant.hpp"

class Paddle : public valiant::Object,
               public valiant::Rectangle,
               public valiant::Collider {
   public:
    bool up{false};
    bool down{false};
    Paddle(int x, const std::string &paddle_tag, const std::string &up_key,
           const std::string &down_key)
        : move_speed_(1000),
          tag_(paddle_tag),
          up_key_(up_key),
          down_key_(down_key) {
        transform.position.x = x;
    }

    void start() override {
        tag = tag_;
        shape.width = 40;
        shape.height = 250;
        shape.color = {255, 255, 255};
    }

    void update() override {
        if (input.get_key(up_key_)) {
            up = true;
        }
        if (input.get_key(down_key_)) {
            down = true;
        }

        if (input.get_key_up(up_key_)) {
            up = false;
        }
        if (input.get_key_up(down_key_)) {
            down = false;
        }

        move();
    }

   private:
    const float move_speed_;
    std::string tag_;
    std::string up_key_;
    std::string down_key_;

    void move() {
        float delta = move_speed_ * time.delta_time;
        if (up) {
            transform.position.y -= delta;
        }
        if (down) {
            transform.position.y += delta;
        }
    }
};

class Ball : public valiant::Object,
             public valiant::Rectangle,
             public valiant::Collider {
   public:
    Ball(Paddle &paddle_1, Paddle &paddle_2)
        : speed_(1000),
          paddle_1_(paddle_1),
          paddle_2_(paddle_2),
          start_(true),
          velocity_x_(0),
          velocity_y_(0) {}

    void start() override {
        transform.position = {0, 0, 0};
        shape.width = 50;
        shape.height = 50;
        shape.color = {255, 255, 255};
    }

    void update() override {
        if (start_) {
            reset();
        }
        check_wall();
        check_boundary();
        move();
    }

    void on_collision_enter(const valiant::Collision &collision) override {
        if (collision.tag == "paddle_1" || collision.tag == "paddle_2") {
            velocity_x_ = -1 * velocity_x_;
            if ((collision.tag == "paddle_1" && paddle_1_.up) ||
                (collision.tag == "paddle_2" && paddle_2_.up)) {
                velocity_y_ -= speed_;
            } else if ((collision.tag == "paddle_1" && paddle_1_.down) ||
                       (collision.tag == "paddle_2" && paddle_2_.down)) {
                velocity_y_ += speed_;
            }
        }
    }

   private:
    const float speed_;
    Paddle &paddle_1_;
    Paddle &paddle_2_;
    bool start_;
    float velocity_x_;
    float velocity_y_;

    void move() {
        float delta_x = velocity_x_ * time.delta_time;
        float delta_y = velocity_y_ * time.delta_time;
        transform.position.x += delta_x;
        transform.position.y += delta_y;
    }

    void reset() {
        // Reset ball position
        transform.position = {0, 0, 0};
        velocity_x_ = ((rand() % 2) ? 1 : -1) * speed_;
        velocity_y_ = 0;
        start_ = false;
    }

    void check_wall() {
        // Check if the ball has touched the top or bottom wall
        if ((transform.position.y + (shape.height / 2)) >=
                (valiant::LOGICAL_WINDOW_HEIGHT / 2) ||
            (transform.position.y - (shape.height / 2)) <=
                (-1 * valiant::LOGICAL_WINDOW_HEIGHT) / 2) {
            // Top or bottom wall was hit
            velocity_y_ = -1 * velocity_y_;
        }
    }

    void check_boundary() {
        // Check if the ball has touched the left or right boundary
        if ((transform.position.x - (shape.width / 2)) >=
                (valiant::LOGICAL_WINDOW_WIDTH / 2) ||
            (transform.position.x + (shape.width / 2)) <=
                (-1 * valiant::LOGICAL_WINDOW_WIDTH) / 2) {
            // Left or right boundary was crossed
            start_ = true;
        }
    }
};

int main() {
    std::srand(std::time(0));
    valiant::Renderer renderer(valiant::ENABLE | valiant::VSYNC);
    Paddle paddle_1(-750, "paddle_1", "W", "S");
    Paddle paddle_2(750, "paddle_2", "Up", "Down");
    Ball ball(paddle_1, paddle_2);
    renderer.add_object(paddle_1);
    renderer.add_object(paddle_2);
    renderer.add_object(ball);
    renderer.run();
}
