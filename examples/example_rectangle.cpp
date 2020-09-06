#include "../valiant/valiant.hpp"

class RectangleObject1 : public valiant::Object, public valiant::Rectangle {
   public:
    void start() override {
        shape.width = 50;
        shape.height = 25;
        shape.color.r = 0;
        shape.color.g = 255;
        shape.color.b = 0;
        shape.color.a = 255;
    }
};

class RectangleObject2 : public valiant::Object, public valiant::Rectangle {
   public:
    void start() override {
        shape.width = 100;
        shape.height = 50;
        shape.color.r = 255;
        shape.color.g = 0;
        shape.color.b = 0;
        shape.color.a = 255;
        transform.position.x = 100;
    }
};

int main() {
    valiant::Renderer renderer;
    RectangleObject1 rectangle_object1;
    RectangleObject2 rectangle_object2;
    renderer.add_object(rectangle_object1);
    renderer.add_object(rectangle_object2);
    renderer.run();
}
