#ifndef VALIANT_OBJECT_HPP
#define VALIANT_OBJECT_HPP

#include <string>

#include "input.hpp"

namespace valiant {
struct Vector3 {
    int x;
    int y;
    int z;

    Vector3() : x(0), y(0), z(0){};
};

struct Transform {
    Vector3 position;
};

class Object {
   public:
    Transform transform;
    Input input;
    virtual void awake() {}
    virtual void start() {}
    virtual void update() {}
};
}  // namespace valiant

#endif