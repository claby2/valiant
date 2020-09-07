#ifndef VALIANT_OBJECT_HPP
#define VALIANT_OBJECT_HPP

#include <string>

#include "input.hpp"
#include "time.hpp"

namespace valiant {
struct Vector3 {
    float x;
    float y;
    float z;

    Vector3(float new_x = 0, float new_y = 0, float new_z = 0)
        : x(new_x), y(new_y), z(new_z){};

    bool operator==(const Vector3& vector3) const {
        return (x == vector3.x && y == vector3.y && z == vector3.z);
    }
};

struct Transform {
    Vector3 position;

    bool operator==(const Transform& transform) const {
        return (position == transform.position);
    }
};

class Object {
   public:
    Transform transform;
    Input input;
    Time time;
    std::string tag = "untagged";
    virtual void awake() {}
    virtual void start() {}
    virtual void update() {}
};
}  // namespace valiant

#endif
