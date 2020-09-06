#include <SDL2/SDL.h>

#include <catch2/catch.hpp>

#include "../valiant/color.hpp"
#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"

void test_object_camera_position(const valiant::Renderer &renderer,
                                 valiant::ObjectData object,
                                 valiant::CameraData camera) {
    SDL_Rect rect = renderer.get_object_camera_position(object, camera);
    int expected_x = (valiant::LOGICAL_WINDOW_WIDTH / 2) -
                     (object.width / (2 * camera.size));
    int expected_y = (valiant::LOGICAL_WINDOW_HEIGHT / 2) -
                     (object.height / (2 * camera.size));
    int expected_w = (object.width / camera.size);
    int expected_h = (object.height / camera.size);
    REQUIRE(rect.x == expected_x);
    REQUIRE(rect.y == expected_y);
    REQUIRE(rect.w == expected_w);
    REQUIRE(rect.h == expected_h);
}

TEST_CASE("Renderer default window dimensions") {
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    int renderer_width = renderer.window_width();
    int renderer_height = renderer.window_height();
    REQUIRE(renderer_width == valiant::DEFAULT_WINDOW_WIDTH);
    REQUIRE(renderer_height == valiant::DEFAULT_WINDOW_HEIGHT);
}

TEST_CASE("Renderer background") {
    SECTION("Default background color") {
        valiant::Renderer renderer(valiant::RenderMode::DISABLE);
        valiant::Color background_color = renderer.background_color();
        REQUIRE(background_color.r == 0);
        REQUIRE(background_color.g == 0);
        REQUIRE(background_color.b == 0);
        REQUIRE(background_color.a == 255);
    }
    SECTION("Set background color without color object") {
        valiant::Renderer renderer(valiant::RenderMode::DISABLE);
        renderer.set_background_color(255, 255, 255, 0);
        valiant::Color background_color = renderer.background_color();
        REQUIRE(background_color.r == 255);
        REQUIRE(background_color.g == 255);
        REQUIRE(background_color.b == 255);
        REQUIRE(background_color.a == 0);
    }
    SECTION("Set background color with color object") {
        valiant::Renderer renderer(valiant::RenderMode::DISABLE);
        valiant::Color new_background_color(255, 255, 255, 0);
        renderer.set_background_color(new_background_color);
        valiant::Color background_color = renderer.background_color();
        REQUIRE(background_color.r == 255);
        REQUIRE(background_color.g == 255);
        REQUIRE(background_color.b == 255);
        REQUIRE(background_color.a == 0);
    }
}

TEST_CASE("Renderer invalid camera size exception") {
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
    valiant::CameraData invalid_camera_data_1 = {0., {0, 0, 0}};
    valiant::CameraData invalid_camera_data_2 = {-1., {0, 0, 0}};
    REQUIRE_THROWS_WITH(
        renderer.get_object_camera_position(object_data, invalid_camera_data_1),
        "Invalid camera size: " + std::to_string(invalid_camera_data_1.size));
    REQUIRE_THROWS_WITH(
        renderer.get_object_camera_position(object_data, invalid_camera_data_2),
        "Invalid camera size: " + std::to_string(invalid_camera_data_2.size));
}

TEST_CASE("Renderer camera positioning") {
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    SECTION("Center positioning") {
        valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
        valiant::CameraData camera_data = {1., {0, 0, 0}};
        test_object_camera_position(renderer, object_data, camera_data);
    }
    SECTION("Increased camera size") {
        valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
        valiant::CameraData camera_data = {2., {0, 0, 0}};
        test_object_camera_position(renderer, object_data, camera_data);
    }
    SECTION("Decreased camera size") {
        valiant::ObjectData object_data = {32, 64, {0, 0, 0}};
        valiant::CameraData camera_data = {0.5, {0, 0, 0}};
        test_object_camera_position(renderer, object_data, camera_data);
    }
}
