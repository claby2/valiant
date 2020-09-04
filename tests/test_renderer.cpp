#include <SDL2/SDL.h>

#include <catch2/catch.hpp>

#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"

void test_object_camera_position(valiant::Renderer &renderer,
                                 valiant::ObjectData object,
                                 valiant::CameraData camera) {
    SDL_Rect rect = renderer.get_object_camera_position(object, camera);
    int expected_x =
        (renderer.window_width() / 2) - (object.width / (2 * camera.size));
    int expected_y =
        (renderer.window_height() / 2) - (object.height / (2 * camera.size));
    int expected_w = (object.width / camera.size);
    int expected_h = (object.height / camera.size);
    REQUIRE(rect.x == expected_x);
    REQUIRE(rect.y == expected_y);
    REQUIRE(rect.w == expected_w);
    REQUIRE(rect.h == expected_h);
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
