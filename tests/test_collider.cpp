#include <catch2/catch.hpp>

#include "../valiant/collider.hpp"
#include "../valiant/renderer.hpp"

TEST_CASE("Collider default values") {
    class Player : public valiant::Object, public valiant::Collider {};
    Player player;
    bool collider_enabled = player.collider.enabled;
    REQUIRE(collider_enabled == true);
}

TEST_CASE("Collider collision equal to operator") {
    valiant::Collision collision_1 = {{{0, 0, 0}}, "tag"};
    valiant::Collision collision_2 = {{{0, 0, 0}}, "tag"};
    REQUIRE(collision_1 == collision_2);
}
