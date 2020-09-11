#include <catch2/catch.hpp>

#include "../valiant/color.hpp"
#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"
#include "../valiant/shape.hpp"

TEST_CASE("Shape default values") {
    valiant::Shape shape;
    REQUIRE(shape.width == 0);
    REQUIRE(shape.height == 0);
    REQUIRE(shape.color.r == 255);
    REQUIRE(shape.color.g == 255);
    REQUIRE(shape.color.b == 255);
    REQUIRE(shape.color.a == 255);
    REQUIRE(shape.fill == true);
}

TEST_CASE("Shape constructor") {
    valiant::Shape shape(1, 1);
    REQUIRE(shape.width == 1);
    REQUIRE(shape.height == 1);
}
TEST_CASE("Shape rectangle properties") {
    class ShapeObject : public valiant::Object, public valiant::Rectangle {
       public:
        void start() override {
            shape.width = 1;
            shape.height = 1;
            shape.fill = false;
        }
    };
    ShapeObject shape_object;
    valiant::Renderer renderer(valiant::DISABLE);
    renderer.add_object(shape_object);
    renderer.run();
    valiant::Shape shape = shape_object.shape;
    // Assert that start method in ShapeObject is executed in renderer run
    REQUIRE(shape.width == 1);
    REQUIRE(shape.height == 1);
    REQUIRE(shape.fill == false);
}
