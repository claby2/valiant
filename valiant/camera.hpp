#ifndef VALIANT_CAMERA_HPP
#define VALIANT_CAMERA_HPP

#include "object.hpp"

namespace valiant {
class CameraComponent {
   public:
    float size;
    CameraComponent(float new_size = 1.) : size(new_size) {}
};
class Camera {
   public:
    CameraComponent camera;
    Transform transform;
    Input input;
    Time time;
    virtual void awake() {}
    virtual void start() {}
    virtual void update() {}
};
}  // namespace valiant
#endif
