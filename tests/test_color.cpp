#include <catch2/catch.hpp>

#include "../valiant/color.hpp"

TEST_CASE("Color default values") {
    valiant::Color color;
    REQUIRE(color.r == 255);
    REQUIRE(color.g == 255);
    REQUIRE(color.b == 255);
    REQUIRE(color.a == 255);
}

TEST_CASE("Color equal operator") {
    valiant::Color color_1(0, 0, 0, 0);
    valiant::Color color_2;
    color_2 = color_1;
    REQUIRE(color_2.r == 0);
    REQUIRE(color_2.g == 0);
    REQUIRE(color_2.b == 0);
    REQUIRE(color_2.a == 0);
}
