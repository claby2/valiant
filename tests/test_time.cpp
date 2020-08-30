#include <catch2/catch.hpp>

#include "../valiant/time.hpp"

TEST_CASE("Time default values") {
    valiant::Time time;
    double delta_time = time.delta_time;
    REQUIRE(delta_time == 0.);
    time.set(1.5);
    delta_time = time.delta_time;
    REQUIRE(delta_time == 1.5);
}
