#ifndef VALIANT_COLLIDER_HPP
#define VALIANT_COLLIDER_HPP

#include <string>

#include "object.hpp"

namespace valiant {
struct Collision {
    Transform transform;
    std::string tag;

    bool operator==(const Collision &collision) const {
        return (transform == collision.transform && tag == collision.tag);
    }
};

struct ColliderComponent {
    bool enabled;

    ColliderComponent() : enabled(true) {}
};

class Collider {
   public:
    ColliderComponent collider;

    virtual void on_collision_enter(const Collision &collision) {}
    virtual void on_collision_stay(const Collision &collision) {}
    virtual void on_collision_exit(const Collision &collision) {}
};
}  // namespace valiant
#endif
