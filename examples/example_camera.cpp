#include "../valiant/valiant.hpp"

class Player : public valiant::Object, public valiant::SpriteRenderer {
   public:
    void start() override {
        sprite_renderer.sprite = "examples/assets/sprite.png";
    }
};

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

int main() {
    valiant::Renderer renderer;
    Player player;
    Camera camera;
    renderer.add_object(player);
    renderer.add_camera(camera);
    renderer.run();
}
