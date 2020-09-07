#include <catch2/catch.hpp>

#include "../valiant/color.hpp"

TEST_CASE("Color default values") {
    valiant::Color color;
    valiant::Color expected_color = {255, 255, 255, 255};
    REQUIRE(color == expected_color);
}

TEST_CASE("Color equal to operator") {
    valiant::Color color_1(0, 0, 0, 0);
    valiant::Color color_2;
    color_2 = color_1;
    REQUIRE(color_1 == color_2);
}
