#include <SDL2/SDL.h>

#include <catch2/catch.hpp>

#include "../valiant/object.hpp"
#include "../valiant/renderer.hpp"

TEST_CASE("Renderer camera positioning") {
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    SECTION("Center positioning") {
        int object_width = 32;
        int object_height = 64;
        valiant::Vector3 object_position(0, 0, 0);
        float camera_size = 1.;
        valiant::Vector3 camera_position(0, 0, 0);
        SDL_Rect rect = renderer.get_object_camera_position(
            object_width, object_height, object_position, camera_size,
            camera_position);
        // Since object and camera position are zero, object camera position
        // should be at the center of the screen. The top left positioning
        // should be taken into account here.
        int expected_x = (renderer.window_width() / 2) - (object_width / 2);
        int expected_y = (renderer.window_height() / 2) - (object_height / 2);
        REQUIRE(rect.x == expected_x);
        REQUIRE(rect.y == expected_y);
        // Since camera size is 1, object dimensions should be equal
        REQUIRE(rect.w == object_width);
        REQUIRE(rect.h == object_height);
    }
    SECTION("Increased camera size") {
        int object_width = 32;
        int object_height = 64;
        valiant::Vector3 object_position(0, 0, 0);
        // Make camera size larger
        float camera_size = 2.;
        valiant::Vector3 camera_position(0, 0, 0);
        SDL_Rect rect = renderer.get_object_camera_position(
            object_width, object_height, object_position, camera_size,
            camera_position);
        // Positioning should be at center; however, due to the modified camera
        // size, dimensions should be halved. This means that the offset is the
        // dimension divided by 4.
        int expected_x = (renderer.window_width() / 2) - (object_width / 4);
        int expected_y = (renderer.window_height() / 2) - (object_height / 4);
        REQUIRE(rect.x == expected_x);
        REQUIRE(rect.y == expected_y);
        // Since camera size is 2, object dimensions should be halved
        int expected_w = object_width / 2;
        int expected_h = object_height / 2;
        REQUIRE(rect.w == expected_w);
        REQUIRE(rect.h == expected_h);
    }
    SECTION("Decreased camera size") {
        int object_width = 32;
        int object_height = 64;
        valiant::Vector3 object_position(0, 0, 0);
        // Make camera size smaller
        float camera_size = 0.5;
        valiant::Vector3 camera_position(0, 0, 0);
        SDL_Rect rect = renderer.get_object_camera_position(
            object_width, object_height, object_position, camera_size,
            camera_position);
        // Positioning should be at center; however, due to the modified camera
        // size, dimensions should be doubled. This means that the offset is
        // equal to the dimension itself.
        int expected_x = (renderer.window_width() / 2) - object_width;
        int expected_y = (renderer.window_height() / 2) - object_height;
        REQUIRE(rect.x == expected_x);
        REQUIRE(rect.y == expected_y);
        // Since camera size is 0.5, object dimensions should be doubled
        int expected_w = object_width * 2;
        int expected_h = object_height * 2;
        REQUIRE(rect.w == expected_w);
        REQUIRE(rect.h == expected_h);
    }
}
