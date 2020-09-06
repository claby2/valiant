#include <catch2/catch.hpp>

#include "../valiant/collider.hpp"
#include "../valiant/renderer.hpp"

TEST_CASE("Collider default values") {
    class Player : public valiant::Object, public valiant::Collider {};
    Player player;
    bool collider_enabled = player.collider.enabled;
    REQUIRE(collider_enabled == true);
}
